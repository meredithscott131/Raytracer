#include "View.h"
#include <cstdio>
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <vector>
using namespace std;
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "sgraph/GLScenegraphRenderer.h"
#include "sgraph/ConsoleScenegraphRenderer.h"
#include "sgraph/ScenegraphLighter.h"
#include <Light.h>
#include "VertexAttrib.h"
#include "sgraph/LeafNode.h"
#include "HitRecord.h"
#include "sgraph/SGNodeVisitor.h"
#include "PPMImageWriter.h"
#include "Ray.h"


View::View() {}

View::~View(){}

void View::init(Callbacks* callbacks, Model& model) 
{
    // Initialize the scenegraph
    sg = model.getScenegraph();

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Create a window
    window = glfwCreateWindow(1000, 1000, "Ray Tracer", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create window\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwGetFramebufferSize(window, &window_dimensions[0], &window_dimensions[1]);
    glfwSetWindowUserPointer(window, callbacks);

    // Using C++ functions as callbacks to a C-style library
    glfwSetKeyCallback(window, 
    [](GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        reinterpret_cast<Callbacks*>(glfwGetWindowUserPointer(window))->onkey(key,scancode,action,mods);
    });

    glfwSetWindowSizeCallback(window, 
    [](GLFWwindow* window, int width,int height)
    {
        reinterpret_cast<Callbacks*>(glfwGetWindowUserPointer(window))->reshape(width,height);
    });

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    // Create the shader program
    program.createProgram(string("shaders/phong-multiple.vert"),
                          string("shaders/phong-multiple.frag"));

    // Enable the shader program
    program.enable();
    shaderLocations = program.getAllShaderVariables();

    // Initialize the objects
    initObjects(model);

	int window_width,window_height;
    glfwGetFramebufferSize(window,&window_width,&window_height);

    // Prepare the projection matrix for perspective projection
	projection = glm::perspective(glm::radians(60.0f),(float)window_width/window_height,0.1f,10000.0f);
    glViewport(0, 0, window_width,window_height);
    
    // Print the scenegraph to the console
    sgraph::ConsoleScenegraphRenderer* consoleRenderer = new sgraph::ConsoleScenegraphRenderer();
    sg->getRoot()->accept(consoleRenderer);
    //cout << consoleRenderer->getOutput() << endl;
}

// Initialize the objects
void View::initObjects(Model& model) {

    // Set the shader variables to vertex attributes
    map<string,string> shaderVarsToVertexAttribs;
    shaderVarsToVertexAttribs["vPosition"] = "position";
    shaderVarsToVertexAttribs["vNormal"] = "normal";
    shaderVarsToVertexAttribs["vTexCoord"] = "texcoord";

    // Get the list of meshes from the scenegraph
    map<string,util::PolygonMesh<VertexAttrib> > meshes = sg->getMeshes();
    for (typename map<string,util::PolygonMesh<VertexAttrib> >::iterator it=meshes.begin();
           it!=meshes.end();
           it++) {
        util::ObjectInstance * obj = new util::ObjectInstance(it->first);
        obj->initPolygonMesh(shaderLocations,shaderVarsToVertexAttribs,it->second);
        objects[it->first] = obj;
    }

    // Add the meshes to the scenegraph renderer
    renderer = new sgraph::GLScenegraphRenderer(modelview,objects,shaderLocations);

    // Get the list of meshes from the model
    vector<string> meshNames = model.getMeshNames();
    for (string name:meshNames) {
        util::ObjectInstance *obj =  new util::ObjectInstance("triangles");
        obj->initPolygonMesh<VertexAttrib>(
            program,                    // the shader program
            shaderLocations,            // the shader locations
            shaderVarsToVertexAttribs,  // the shader variable -> attrib map
            model.getMesh(name));       // the actual mesh object
        objects[name] = obj;
    }
    
    glEnable(GL_TEXTURE_2D);
    vector<string> textureNames = model.getTextureNames();

    // Initialize the textures
    for (string name:textureNames) {
        util::TextureImage *textureObject = model.getTextureObject(name);
        unsigned int textureId;
        glGenTextures(1,&textureId);
        glBindTexture(GL_TEXTURE_2D,textureId);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureObject->getWidth(),
                        textureObject->getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE,textureObject->getImage());
        glGenerateMipmap(GL_TEXTURE_2D);

        // Assign texture ID directly to corresponding LeafNodes
        for (auto& pair : sg->getNodes()) {
            sgraph::LeafNode* leaf = dynamic_cast<sgraph::LeafNode*>(pair.second);
            if (leaf && leaf->getTextureName() == name) {
                leaf->setTextureID(textureId);
                leaf->setTextureObject(textureObject);
            }
        }
    }
}

// Initialize the lights
void View::initShaderVariables(vector<util::Light>& lights) {
    for (int i=0;i<lights.size();i++) {
        LightLocation loc;
        stringstream name;
        name << "light[" << i << "]";

        loc.ambient = shaderLocations.getLocation(name.str()+".ambient");
        loc.diffuse = shaderLocations.getLocation(name.str()+".diffuse");
        loc.specular = shaderLocations.getLocation(name.str()+".specular");
        loc.position = shaderLocations.getLocation(name.str()+".position");
        loc.spotdirection = shaderLocations.getLocation(name.str()+".spotDirection");
        loc.spotcutoff = shaderLocations.getLocation(name.str()+".spotCutoff");
        lightLocations.push_back(loc);
    }
}

// Display the scene
void View::display(Model& model)
{
    program.enable();                                       // enable the program
    glClearColor(1,1,1,1);                                  // set the background color to be white
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // clear the background
    glEnable(GL_DEPTH_TEST);
    //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);               //outline mode

    glUniformMatrix4fv(shaderLocations.getLocation("projection"), 1,
                            false, glm::value_ptr(projection));

    while (!modelview.empty()) {
        modelview.pop();
    }
    
    // Set the camera position given the current camera mode
    setCamera(model);

    // Draw the model
    drawScenegraphModel(model);

    // Apply lighting to the scenegraph
    applyScenegraphLighting();

    // Draw the scenegraph
    sg->getRoot()->accept(renderer);

    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(shaderLocations.getLocation("image"), 0);

    modelview.pop();

    glFlush();

    program.disable();
    
    glfwSwapBuffers(window);
    glfwPollEvents();
    lightLocations.clear();
}

// Draws the model
void View::drawScenegraphModel(Model& model) {
    for (string name:model.getMeshNames()) {
        modelview.push(modelview.top());  // save the current modelview

        // The total transformation is whatever was passed to it, with its own transformation
        glUniformMatrix4fv(shaderLocations.getLocation("modelview"), 1,
                              false, glm::value_ptr(modelview.top()));

        // Set the color for all vertices to be drawn for this object
        glUniform4fv(shaderLocations.getLocation("vColor"), 1,
                        glm::value_ptr(model.getMaterial(name).getAmbient()));
        
        objects[name]->draw();
        modelview.pop();
    }
}

// Apply lighting to the scenegraph
void View::applyScenegraphLighting() {
    stack<glm::mat4> lighter_mv;
    lighter_mv.push(modelview.top());

    vector<util::Light> lights;
    vector<string> coordinates;
    sgraph::ScenegraphLighter *lighter = new sgraph::ScenegraphLighter(lighter_mv, lights, coordinates);
    sg->getRoot()->accept(lighter);

    initShaderVariables(lights);

    for(int i = 0; i < lights.size(); i++) {
        glm::vec4 position = lights[i].getPosition();
        glUniform4fv(lightLocations[i].position, 1, glm::value_ptr(position));
    }

    glUniform1i(shaderLocations.getLocation("numLights"), lights.size());

    // Pass the light properties to the shader
    for (int i = 0; i < lights.size(); i++) {
        glUniform3fv(lightLocations[i].ambient, 1, glm::value_ptr(lights[i].getAmbient()));
        glUniform3fv(lightLocations[i].diffuse, 1, glm::value_ptr(lights[i].getDiffuse()));
        glUniform3fv(lightLocations[i].specular, 1,glm::value_ptr(lights[i].getSpecular()));
        glUniform4fv(lightLocations[i].spotdirection, 1,glm::value_ptr(lights[i].getSpotDirection()));
        glUniform1f(lightLocations[i].spotcutoff,lights[i].getSpotCutoff());
    }
}

// Raytrace the scene and save it to a PPM file
void View::raytrace(Model& model) {
    // Dimensions for the image
    int width = window_dimensions[0];
    int height = window_dimensions[1];

    // Allocate image buffer
    GLubyte* image = new GLubyte[3 * width * height];

    // Collect lights for raytracing
    vector<util::Light> raytraceLights;
    vector<string> coords;
    stack<glm::mat4> mvCopy;
    mvCopy.push(glm::lookAt(cameraPosition, cameraTarget, glm::vec3(0.0f, 1.0f, 0.0f)));
    sgraph::ScenegraphLighter lighter(mvCopy, raytraceLights, coords);
    sg->getRoot()->accept(&lighter);

    // Loop through each pixel
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            
            // Calculate ray direction
            float vx = -(width / 2.0f) + (float) x;
            float vy = -(height / 2.0f) + (float) y;
            float vz = -(0.5f * height) / tan(0.5f * glm::radians(60.0f));

            glm::vec4 s(0.0f, 0.0f, 0.0f, 1.0f);    // camera position
            glm::vec4 v(vx, vy, vz, 0.0f);          // ray direction
            Ray ray(s, v);                          // create ray

            while (!raytraceModelview.empty()) raytraceModelview.pop();

            // Set up the raytracer modelview matrix
            raytraceModelview.push(glm::mat4(1.0f));
            raytraceModelview.top() = raytraceModelview.top() * glm::lookAt(cameraPosition,cameraTarget,glm::vec3(0.0f,1.0f,0.0f));

            // Raytrace the current pixel
            raytracerRenderer = new sgraph::RaytracerRenderer(raytraceModelview, ray);
            sg->getRoot()->accept(raytracerRenderer);
            HitRecord& hitRecord = dynamic_cast<sgraph::RaytracerRenderer*>(raytracerRenderer)->getHitRecord();

            // Normalize the ray direction
            glm::vec3 viewDir = glm::normalize(glm::vec3(-v)); 

            // Calculate pixel index in the image buffer
            int idx = 3 * (y * width + x);

            // Check if the ray hit an object
            if (hitRecord.t < std::numeric_limits<float>::infinity()) {
                // Hit, set pixel color based on shading
                glm::vec3 shadedColor = shade(hitRecord, glm::vec4(viewDir, 0.0f), raytraceLights, 5, 1.0f);
                image[idx]     = shadedColor.r * 255;
                image[idx + 1] = shadedColor.g * 255;
                image[idx + 2] = shadedColor.b * 255;
            }
            else {
                // No hit, set pixel to white
                image[idx]     = 255;
                image[idx + 1] = 255;
                image[idx + 2] = 255;
            }                   
        }
    }

    // Save the image to a PPM file
    PPMImageWriter writer = PPMImageWriter(image, width, height);
    writer.save("images/raytraced_scene.ppm");
    delete[] image;
}

// Shade the given hit record, considering lighting and reflection
glm::vec3 View::shade(HitRecord& hitRecord, const glm::vec4& viewDir, const std::vector<util::Light>& lights,
    int bounces, float currentRefractiveIndex) {

    glm::vec3 color(0.0f);
    glm::vec3 n = glm::normalize(glm::vec3(hitRecord.normal));
    glm::vec3 v = glm::normalize(glm::vec3(viewDir));

    // Applying each light to the current pixel
    for (const auto& light : lights) {
        color += applyLighting(hitRecord, light, n, v);
    }

    // If hit is reflective material, apply reflection to the current pixel
    if (hitRecord.material.getReflection() > 0.0f && bounces > 0) {
        color = applyReflection(hitRecord, n, v, color, lights, bounces);
    }

    // If hit has texture, apply it to the current pixel
    if (hitRecord.textureImage) {
        glm::vec4 texColor = hitRecord.textureImage->getColor(
            hitRecord.textureCoordinates.x,
            hitRecord.textureCoordinates.y
        );
        glm::vec3 texRGB = glm::vec3(texColor.r, texColor.g, texColor.b) / 255.0f;

        color *= texRGB;
    }

    //refraction
    /*
    if (hitRecord.material.getTransparency() > 0.0f && bounces > 0) {
        glm::vec3 refractedColor = applyRefraction(hitRecord, n, v, lights, bounces - 1, currentRefractiveIndex);
        float absorption = hitRecord.material.getAbsorption();
        float reflection = hitRecord.material.getReflection();
        float transparency = hitRecord.material.getTransparency();
        float total = absorption + reflection + transparency;

        //trying to figure out the blend to make refraction appear...
        if (total > 0.0f) {
            color = (absorption * color + reflection * color + transparency * refractedColor) / total;
        } else {
            color = refractedColor;
        }
            }
    */

    return glm::clamp(color, 0.0f, 1.0f);
}

// Applies lighting to the given hit record
glm::vec3 View::applyLighting(HitRecord& hitRecord, const util::Light& light, const glm::vec3& n, const glm::vec3& v) {
    // Normalizing the light direction
    glm::vec3 l = glm::normalize(glm::vec3(light.getPosition()) - glm::vec3(hitRecord.point));
    glm::vec3 h = glm::normalize(l + v);

    // Applying offset to shadow ray to avoid precision errors
    glm::vec3 offsetOrigin = glm::vec3(hitRecord.point) + 0.001f * n;

    // Ray tracing the shadow ray
    Ray shadowRay(glm::vec4(offsetOrigin, 1.0f), glm::vec4(l, 0.0f));
    sgraph::RaytracerRenderer* shadowRenderer = new sgraph::RaytracerRenderer(raytraceModelview, shadowRay);
    sg->getRoot()->accept(shadowRenderer);
    HitRecord& shadowHit = shadowRenderer->getHitRecord();

    float distToLight = glm::length(glm::vec3(light.getPosition()) - offsetOrigin);
    glm::vec3 result(0.0f);

    // If point is in light, apply lighting
    if (shadowHit.t >= distToLight) {
        float spotFactor = calculateSpotlight(light, l);
        float diff = glm::max(glm::dot(n, l), 0.0f);
        float spec = glm::pow(glm::max(glm::dot(n, h), 0.0f), hitRecord.material.getShininess());

        // Calculate the final color components
        glm::vec3 ambientTerm = glm::vec3(hitRecord.material.getAmbient()) * glm::vec3(light.getAmbient());
        glm::vec3 diffuseTerm = glm::vec3(hitRecord.material.getDiffuse()) * glm::vec3(light.getDiffuse()) * diff;
        glm::vec3 specularTerm = glm::vec3(hitRecord.material.getSpecular()) * glm::vec3(light.getSpecular()) * spec;

        result = spotFactor * (ambientTerm + diffuseTerm + specularTerm);
    } else {
        glm::vec3 shadowColor(0.2f, 0.2f, 0.3f);  // blue shadow tint
        float strength = 0.6f;                    // opacity

        glm::vec3 base = glm::vec3(hitRecord.material.getAmbient());
        result = base * shadowColor * strength +
                 base * (1.0f - strength);
    }

    delete shadowRenderer;
    return result;
}

// Calculates the spotlight factor of the given the light and its direction
float View::calculateSpotlight(const util::Light& light, const glm::vec3& l) {
    if (light.getSpotCutoff() <= 0) return 1.0f;

    glm::vec3 spotDir = glm::normalize(glm::vec3(light.getSpotDirection()));
    glm::vec3 lightDir = glm::normalize(-l);
    float spotCos = glm::dot(spotDir, lightDir);

    return (spotCos < light.getSpotCutoff()) ? 0.0f : 1.0f;
}

// Applies reflection to the given hit record
glm::vec3 View::applyReflection(HitRecord& hitRecord, const glm::vec3& n, const glm::vec3& v,
    const glm::vec3& baseColor, const std::vector<util::Light>& lights, int bounces) {

    float reflection = hitRecord.material.getReflection();
    float absorption = hitRecord.material.getAbsorption();

    // Reflection direction
    glm::vec3 r = glm::reflect(-v, n);
    // Offset the origin
    glm::vec3 reflectionOrigin = glm::vec3(hitRecord.point) + 0.001f * r;

    // Ray tracing the reflection ray
    Ray reflectionRay(glm::vec4(reflectionOrigin, 1.0f), glm::vec4(r, 0.0f));
    sgraph::RaytracerRenderer* reflectionRenderer = new sgraph::RaytracerRenderer(raytraceModelview, reflectionRay);
    sg->getRoot()->accept(reflectionRenderer);
    HitRecord& reflectionHit = reflectionRenderer->getHitRecord();

    glm::vec3 color;
    if (reflectionHit.t < std::numeric_limits<float>::infinity()) {
        glm::vec3 reflectionColor = shade(reflectionHit, glm::vec4(-r, 0.0f), lights, bounces - 1);
        color = absorption * baseColor + reflection * reflectionColor;
    } else {
        color = absorption * baseColor + reflection * glm::vec3(1.0f);
    }

    delete reflectionRenderer;
    return color;
}

// Applies refraction onto the objects
glm::vec3 View::applyRefraction(HitRecord& hitRecord, const glm::vec3& n, const glm::vec3& v,
    const std::vector<util::Light>& lights, int bounces, float currentRefractiveIndex) {

    float eta_i = currentRefractiveIndex;
    float eta_t = hitRecord.material.getRefractiveIndex();
    glm::vec3 normal = n;
    float cos_i = glm::dot(-v, normal);

    if (cos_i < 0) {
        cos_i = -cos_i;
        std::swap(eta_i, eta_t);
        normal = -normal;
    }

    float eta = eta_i / eta_t;
    float sin2_t = eta * eta * (1.0f - cos_i * cos_i);

    if (sin2_t > 1.0f) {
        return glm::vec3(1.0f);
    }

    float cos_t = sqrt(1.0f - sin2_t);
    glm::vec3 refractedDir = eta * v + (eta * cos_i - cos_t) * normal;
    refractedDir = glm::normalize(refractedDir);
    glm::vec3 entryOffset = 0.001f * refractedDir;
    glm::vec3 entryOrigin = glm::vec3(hitRecord.point) + entryOffset;

    Ray entryRay(glm::vec4(entryOrigin, 1.0f), glm::vec4(refractedDir, 0.0f));
    sgraph::RaytracerRenderer* firstRenderer = new sgraph::RaytracerRenderer(raytraceModelview, entryRay);
    sg->getRoot()->accept(firstRenderer);
    HitRecord& exitHit = firstRenderer->getHitRecord();

    glm::vec3 color;
    if (exitHit.t < std::numeric_limits<float>::infinity()) {
        glm::vec3 n2 = glm::normalize(glm::vec3(exitHit.normal));
        glm::vec3 incident = -refractedDir;
        float eta_exit = eta_t / eta_i;
        float cos_i2 = glm::dot(-incident, n2);

        if (cos_i2 < 0) {
            cos_i2 = -cos_i2;
            n2 = -n2;
        }

        float sin2_t2 = eta_exit * eta_exit * (1.0f - cos_i2 * cos_i2);

        if (sin2_t2 > 1.0f) {
            delete firstRenderer;
            return glm::vec3(1.0f);
        }

        float cos_t2 = sqrt(1.0f - sin2_t2);
        glm::vec3 refractedDir2 = eta_exit * incident + (eta_exit * cos_i2 - cos_t2) * n2;
        refractedDir2 = glm::normalize(refractedDir2);
        glm::vec3 exitOffset = 0.001f * refractedDir2;
        glm::vec3 exitOrigin = glm::vec3(exitHit.point) + exitOffset;

        Ray exitRay(glm::vec4(exitOrigin, 1.0f), glm::vec4(refractedDir2, 0.0f));
        sgraph::RaytracerRenderer* secondRenderer = new sgraph::RaytracerRenderer(raytraceModelview, exitRay);
        sg->getRoot()->accept(secondRenderer);
        HitRecord& finalHit = secondRenderer->getHitRecord();

        if (finalHit.t < std::numeric_limits<float>::infinity()) {
            color = shade(finalHit, glm::vec4(-refractedDir2, 0.0f), lights, bounces, eta_i);
        } else {
            color = glm::vec3(1.0f);
        }
        delete secondRenderer;
    } else {
        color = glm::vec3(1.0f);
    }

    delete firstRenderer;
    return color;
}

// Set the camera position given the current camera mode
void View::setCamera(Model& model) {
    glm::mat4 viewTransform;
    glm::vec3 upVector(0.0f, 1.0f, 0.0f);
    
    cameraPosition = glm::vec3(0.0f, 0.0f, 200.0f);
    cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);

    viewTransform = glm::lookAt(cameraPosition, cameraTarget, upVector);
    modelview.push(viewTransform);
}

// Update the projection matrix based on the new window size
void View::updateProjection(int width, int height)
{
    if (height == 0) height = 1;

    // Update the projection matrix
    projection = glm::perspective(glm::radians(60.0f),
                            (float)width / (float)height, 0.1f, 1000.0f);
}

bool View::shouldWindowClose() {
    return glfwWindowShouldClose(window);
}

void View::closeWindow() {
    for (map<string,util::ObjectInstance *>::iterator it=objects.begin();
           it!=objects.end();
           it++) {
          it->second->cleanup();
          delete it->second;
    } 
    glfwDestroyWindow(window);

    glfwTerminate();
}