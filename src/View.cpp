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
#include "sgraph/GLScenegraphLighter.h"
#include <Light.h>
#include "VertexAttrib.h"
#include "sgraph/LeafNode.h"
#include "HitRecord.h"
#include "sgraph/SGNodeVisitor.h"
#include "PPMImageWriter.h"


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

    window = glfwCreateWindow(1000, 1000, "Scene", NULL, NULL);

    if (!window) {
        fprintf(stderr, "Failed to create window\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwGetFramebufferSize(window, &window_dimensions[0], &window_dimensions[1]);

    glfwSetWindowUserPointer(window, callbacks);

    //using C++ functions as callbacks to a C-style library
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

    // create the shader program
    program.createProgram(string("shaders/phong-multiple.vert"),
                          string("shaders/phong-multiple.frag"));
    // assuming it got created, get all the shader variables that it uses
    // so we can initialize them at some point
    // enable the shader program
    
    program.enable();
    shaderLocations = program.getAllShaderVariables();

    // Initialize the objects
    initObjects(model);

	int window_width,window_height;
    glfwGetFramebufferSize(window,&window_width,&window_height);

    //prepare the projection matrix for perspective projection
	projection = glm::perspective(glm::radians(60.0f),(float)window_width/window_height,0.1f,10000.0f);
    glViewport(0, 0, window_width,window_height);

    //cout << "Projection Matrix Initialized" << endl;
    // projection = glm::ortho(-400.0f,400.0f,-400.0f,400.0f,0.1f,10000.0f);

    angleOfRotation = 0;
    cameraMode = GLOBAL;

    frames = 0;
    time = glfwGetTime();
    
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
        //cout << "Created SG Instance for: " << it->first << endl;
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
        //cout << "Created Mesh Instance for: " << name << endl; 
    }

    cout << "Code is getting to textures in view::initobjects" << endl;
    
    glEnable(GL_TEXTURE_2D);
    vector<string> textureNames = model.getTextureNames();
    for (string name:textureNames) {
        util::TextureImage *textureObject = model.getTextureObject(name);
        unsigned int textureId;
        glGenTextures(1,&textureId);
        glBindTexture(GL_TEXTURE_2D,textureId);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureObject->getWidth(),textureObject->getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE,textureObject->getImage());
        glGenerateMipmap(GL_TEXTURE_2D);

        // Assign texture ID directly to corresponding LeafNodes
        for (auto& pair : sg->getNodes()) {
            sgraph::LeafNode* leaf = dynamic_cast<sgraph::LeafNode*>(pair.second);
            if (leaf && leaf->getTextureName() == name) {
                leaf->setTextureID(textureId);
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

        /*
        cout << "View Init Light: " << name.str() << endl;
        cout << "View Init Ambient: " << loc.ambient << endl;
        cout << "View Init Diffuse: " << loc.diffuse << endl;
        cout << "View Init Specular: " << loc.specular << endl;
        cout << "View Init Position: " << loc.position << endl;
        cout << "View Init Position: " << loc.position << endl;
        cout << "View Init Spot Direction: " << loc.spotdirection << endl;
        cout << "View Init Spot Cutoff: " << loc.spotcutoff << endl;
        */

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
    program.enable();
    glClearColor(1,1,1,1);                                  //set the background color to be black
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // clear the background
    glEnable(GL_DEPTH_TEST);

    //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE); //outline mode

    glUniformMatrix4fv(shaderLocations.getLocation("projection"), 1,
                            false, glm::value_ptr(projection));

    while (!modelview.empty()) {
        modelview.pop();
    }
    
    setCamera(cameraMode, model);

    // Draw the model
    for (string name:model.getMeshNames()) {
        modelview.push(modelview.top());  // save the current modelview
        glm::mat4 transform =
            model.getAnimationTransform(name) * model.getTransform(name);
        modelview.top() = modelview.top() * transform;

        // The total transformation is whatever was passed to it, with its own
        // transformation
        glUniformMatrix4fv(shaderLocations.getLocation("modelview"), 1,
                              false, glm::value_ptr(modelview.top()));
        // set the color for all vertices to be drawn for this object
        glUniform4fv(shaderLocations.getLocation("vColor"), 1,
                        glm::value_ptr(model.getMaterial(name).getAmbient()));
        objects[name]->draw();
        modelview.pop();
    }

    // Apply lighting to the scenegraph
    stack<glm::mat4> lighter_mv;
    lighter_mv.push(modelview.top());

    vector<util::Light> lights;
    vector<string> coordinates;
    sgraph::GLScenegraphLighter *lighter = new sgraph::GLScenegraphLighter(lighter_mv, lights, coordinates);
    
    sg->getRoot()->accept(lighter);
    
    //cout << "Number of lights: " << lights.size() << endl;
    //cout << "Number of coordinates: " << coordinates.size() << endl;

    initShaderVariables(lights);

    for(int i = 0; i < lights.size(); i++) {
        glm::vec4 position = lights[i].getPosition();
        glUniform4fv(lightLocations[i].position, 1, glm::value_ptr(position));
    }

    glUniform1i(shaderLocations.getLocation("numLights"), lights.size());

    // pass the light properties to the shader
    for (int i = 0; i < lights.size(); i++) {
        /*
        glm::vec3 ambient = lights[i].getAmbient();
        glm::vec3 diffuse = lights[i].getDiffuse();
        glm::vec3 specular = lights[i].getSpecular();
        glm::vec3 position = lights[i].getPosition();
        glm::vec3 spotDirection = lights[i].getSpotDirection();
        float spotCutoff = lights[i].getSpotCutoff();
    
        std::cout << "Light " << i << " Position: " << position.x << ", " << position.y << ", " << position.z << std::endl;
        std::cout << "Ambient: " << ambient.r << ", " << ambient.g << ", " << ambient.b << std::endl;
        std::cout << "Diffuse: " << diffuse.r << ", " << diffuse.g << ", " << diffuse.b << std::endl;
        std::cout << "Specular: " << specular.r << ", " << specular.g << ", " << specular.b << std::endl;
        std::cout << "Spot Direction: " << spotDirection.x << ", " << spotDirection.y << ", " << spotDirection.z << std::endl;
        std::cout << "Spot Cutoff: " << spotCutoff << std::endl;
        */
        
        glUniform3fv(lightLocations[i].ambient, 1, glm::value_ptr(lights[i].getAmbient()));
        glUniform3fv(lightLocations[i].diffuse, 1, glm::value_ptr(lights[i].getDiffuse()));
        glUniform3fv(lightLocations[i].specular, 1,glm::value_ptr(lights[i].getSpecular()));
        glUniform4fv(lightLocations[i].spotdirection, 1,glm::value_ptr(lights[i].getSpotDirection()));
        glUniform1f(lightLocations[i].spotcutoff,lights[i].getSpotCutoff());
    }

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
    frames++;
    double currenttime = glfwGetTime();
    if ((currenttime-time)>1.0) {
        printf("Framerate: %2.0f\r",frames/(currenttime-time));
        frames = 0;
        time = currenttime;
    }
    lightLocations.clear();
}

// Raytrace the scene and save it to a PPM file
void View::raytrace(Model& model) {
    int width = window_dimensions[0];
    int height = window_dimensions[1];

    // Allocate image buffer
    GLubyte* image = new GLubyte[3 * width * height];

    //collect lights for raytracing
    vector<util::Light> raytraceLights;
    vector<string> coords;
    stack<glm::mat4> mvCopy;
    mvCopy.push(glm::lookAt(cameraPosition, cameraTarget, glm::vec3(0.0f, 1.0f, 0.0f)));
    sgraph::GLScenegraphLighter lighter(mvCopy, raytraceLights, coords);
    sg->getRoot()->accept(&lighter);

    // Loop through each pixel
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            
            float vx = -(width / 2.0f) + (float) x;
            float vy = -(height / 2.0f) + (float) y;
            float vz = -(0.5f * height) / tan(0.5f * glm::radians(60.0f));

            glm::vec4 origin(0.0f, 0.0f, 0.0f, 1.0f);
            glm::vec4 direction(vx, vy, vz, 0.0f);

            while (!raytraceModelview.empty()) raytraceModelview.pop();

            raytraceModelview.push(glm::mat4(1.0f));
            raytraceModelview.top() = raytraceModelview.top() * glm::lookAt(cameraPosition,cameraTarget,glm::vec3(0.0f,1.0f,0.0f));

            raytracerRenderer = new sgraph::RaytracerRenderer(raytraceModelview, origin, direction);
            sg->getRoot()->accept(raytracerRenderer);

            HitRecord& hitRecord = dynamic_cast<sgraph::RaytracerRenderer*>(raytracerRenderer)->getHitRecord();

            //camera -> hit
            glm::vec3 viewDir = glm::normalize(glm::vec3(-direction)); 

            int idx = 3 * ((height - 1 - y) * width + x);

            if (hitRecord.t < std::numeric_limits<float>::infinity()) {
<<<<<<< Updated upstream
                // hit, set pixel to white
=======
                glm::vec3 shadedColor = shade(hitRecord, glm::vec4(viewDir, 0.0f), raytraceLights);
                image[idx]     = shadedColor.r * 255;
                image[idx + 1] = shadedColor.g * 255;
                image[idx + 2] = shadedColor.b * 255;
            }
            else {
                // Not hit, set pixel to white
>>>>>>> Stashed changes
                image[idx]     = 255;
                image[idx + 1] = 255;
                image[idx + 2] = 255;
            } else {
                // Not hit, set pixel to black
                image[idx]     = 0;
                image[idx + 1] = 0;
                image[idx + 2] = 0;
            }                       
        }
    }

    // Save the image to a PPM file
    PPMImageWriter writer = PPMImageWriter(image, width, height);
    writer.save("images/raytraced_scene.ppm");
    delete[] image;
}

<<<<<<< Updated upstream
// // Returns RGB color values between 0 and 1
// glm::vec4 View::getColor(HitRecord hitRecord, vector<vector<util::Light>>& lights, glm::vec4 rayDirection, int reflectiveBounces, int refractiveBounces) {
//     float a = hitRecord.material.getAbsorption();
//     float r = hitRecord.material.getReflection();
//     float t = hitRecord.material.getTransparency();

//   
//     spdlog::debug("material absorption: (int)" + (int) a);
//     spdlog::debug("material reflection: (int)" + (int) r);
//     spdlog::debug("material transparency: (int)" + (int) t);

//     glm::vec4 absorption = glm::vec4(0,0,0,0);
//     if (a > 0) {
//         absorption = hitRecord.object_mat->getAbsorption() * getAbsorptionColor(hitRecord,sceneLightCollections,scenegraph);
//     }

//     glm::vec4 reflection = glm::vec4(0,0,0,0);
//     spdlog::debug("reflective bounces: " + reflectiveBounces);
//     if (r > 0) {
//         if (reflectiveBounces < 5)  {
//             reflection = r * getReflectionColor(hitRecord,sceneLightCollections,scenegraph,rayDirection,reflectiveBounces,refractiveBounces);
//         }
//         else {
//             reflection = glm::vec4(1,1,1,0);
//         }
//     }

// }

// glm::vec4 View::getAbsorptionColor(HitRecord hitRecord, vector<vector<util::Light>>& lights) {
//     // TODO
// }

// glm::vec4 View::getReflectionColor(HitRecord hitRecord, vector<vector<util::Light>>& lights, glm::vec4 rayDirection, int reflectiveBounces, int refractiveBounces) {
//     // TODO
// }

// glm::vec4 View::getTransparencyColor(HitRecord hitRecord, vector<vector<util::Light>>& lights, glm::vec4 rayDirection, int reflectiveBounces, int refractiveBounces) {
//     // TODO
// }

// bool View::isInShadow(HitRecord hitRecord, vector<vector<util::Light>>& lights) {
//     // TODO
// }
=======
// // Returns rgb value of the pixel at (x,y) in the image
// glm::vec3 View::shade(HitRecord& hitRecord) {
//     glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f);
//     if (hitRecord.hit) {
//         color = hitRecord.material.getAmbient() + 
//                 hitRecord.material.getDiffuse() + 
//                 hitRecord.material.getSpecular() * hitRecord.material.getShininess();
//     }
//     return color;
// }

//new shade method:
glm::vec3 View::shade(HitRecord& hitRecord, const glm::vec4& viewDir, const std::vector<util::Light>& lights)
{
    glm::vec3 N = glm::normalize(glm::vec3(hitRecord.normal));
    glm::vec3 V = glm::normalize(glm::vec3(viewDir));
    glm::vec3 resultColor(0.0f);

    glm::vec3 ambient = hitRecord.material.getAmbient();
    glm::vec3 diffuse = hitRecord.material.getDiffuse();
    glm::vec3 specular = hitRecord.material.getSpecular();
    float shininess = hitRecord.material.getShininess();

    for (const auto& light : lights)
    {
        glm::vec3 L = glm::normalize(glm::vec3(light.getPosition()) - glm::vec3(hitRecord.point));
        glm::vec3 H = glm::normalize(L + V);

        // Spotlight effect
        float spotFactor = 1.0f;
        if (light.getSpotCutoff() > 0) {
            glm::vec3 spotDir = glm::normalize(glm::vec3(light.getSpotDirection()));
            glm::vec3 lightDir = glm::normalize(-L);  // reverse direction for comparison
            float spotCos = glm::dot(spotDir, lightDir);
            if (spotCos < light.getSpotCutoff()) {
                spotFactor = 0.0f;
            }
        }

        float diff = glm::max(glm::dot(N, L), 0.0f);
        float spec = glm::pow(glm::max(glm::dot(N, H), 0.0f), shininess);

        glm::vec3 ambientTerm = ambient * light.getAmbient();
        glm::vec3 diffuseTerm = diffuse * light.getDiffuse() * diff;
        glm::vec3 specularTerm = specular * light.getSpecular() * spec;

        resultColor += spotFactor * (ambientTerm + diffuseTerm + specularTerm);
    }

    return glm::clamp(resultColor, 0.0f, 1.0f);
}
>>>>>>> Stashed changes


// Set the camera position given the current camera mode
void View::setCamera(TypeOfCamera mode, Model& model) {
    glm::mat4 viewTransform;
    glm::vec3 upVector(0.0f, 1.0f, 0.0f);
    
    if (cameraMode == GLOBAL)
    {
        cameraPosition = glm::vec3(0.0f, 0.0f, 200.0f);
        cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    }
    else if (cameraMode == CHOPPER)
    {
        static float angle = 0.0f;
        float radius = 300.0f;
        float height = 400.0f;

        cameraPosition = glm::vec3(radius * cos(glm::radians(angle)), height, radius * sin(glm::radians(angle)));
        cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        angle += 0.5f;
        if (angle >= 360.0f) angle -= 360.0f;

        viewTransform = glm::lookAt(cameraPosition, cameraTarget, upVector);
        modelview.push(viewTransform);
    }
    else if (cameraMode == DRONE)
    {
        //locating the drone position
        glm::mat4 droneTransform = model.getAnimationTransform("DroneBody"); // Get latest drone transform
        glm::vec3 dronePos = glm::vec3(droneTransform[3]);

        //compute forward direction of the drone
        glm::vec3 forward = glm::normalize(glm::vec3(droneTransform * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f)));

        
        //offset camera position slightly behind and above the drone
        cameraPosition = dronePos - forward + glm::vec3(0.0f, 10.0f, 0.0f);
        cameraTarget = dronePos + forward * 30.0f; 

        viewTransform = glm::lookAt(cameraPosition, cameraTarget, upVector);
        modelview.push(viewTransform);
    }

    viewTransform = glm::lookAt(cameraPosition, cameraTarget, upVector);
    modelview.push(viewTransform);
}

// Given a mouse position as coordinates,
// returns a normalized 3D vector representing the projected point on the trackball.
glm::vec3 View::convertToTrackball(double x, double y, float radius, int width, int height) {
    // Normalize x and y based on viewport size
    float normX = (2.0f * x - width) / width;
    float normY = (height - 2.0f * y) / height;

    // Calculate the square of the length of the vector
    float lengthSq = normX * normX + normY * normY;

    // If the length is greater than 1, project onto the sphere
    float normZ = (lengthSq < 1.0f) ? sqrt(1.0f - lengthSq) : 0.0f;

    return glm::normalize(glm::vec3(normX, normY, normZ));
}

// Update the projection matrix based on the new window size
void View::updateProjection(int width, int height)
{
    if (height == 0) height = 1;

    // Update the projection matrix
    projection = glm::perspective(glm::radians(60.0f),
                            (float)width / (float)height, 0.1f, 1000.0f);
}

// Set the camera position given the current camera mode
void View::changeCameraMode(TypeOfCamera mode)
{
    cameraMode = mode; 
}

void View::updateDroneTransform(const glm::mat4& transform) {
    droneTransform = transform;
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