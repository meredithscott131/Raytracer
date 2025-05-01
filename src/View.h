#ifndef __VIEW_H__
#define __VIEW_H__

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cstdio>
#include <ShaderProgram.h>
#include "sgraph/SGNodeVisitor.h"
#include "ObjectInstance.h"
#include "PolygonMesh.h"
#include "VertexAttrib.h"
#include "Callbacks.h"
#include "sgraph/IScenegraph.h"
#include "sgraph/ConsoleScenegraphRenderer.h"
#include <glm/gtc/quaternion.hpp>
#include "Model.h"

#include <stack>
#include "sgraph/RaytracerRenderer.h"
using namespace std;


class View
{
    // Represents the location of a light in the scene
    class LightLocation {
        public:
            int position, ambient, diffuse, specular, spotdirection;
            float spotcutoff;
            LightLocation() {
                position = ambient = diffuse = specular = spotdirection = -1;
                spotcutoff = -1.0f;
            }
    };

public:
    View();
    ~View();

    void init(Callbacks* callbacks, Model& model);
    void display(Model& model);
    bool shouldWindowClose();
    void closeWindow();
    GLFWwindow* getWindow() const { return window; }
    void updateProjection(int width, int height);
    void raytrace(Model& model);

private: 

    GLFWwindow* window;
    util::ShaderProgram program;
    util::ShaderLocationsVault shaderLocations;
    map<string,util::ObjectInstance *> objects;
    sgraph::SGNodeVisitor *renderer;
    glm::mat4 projection;
    stack<glm::mat4> modelview;
    glm::ivec2 window_dimensions;

    // Helper functions
    void initObjects(Model& model);                             // Initializes the scenegraph objects
    void initShaderVariables(vector<util::Light>& lights);      // Initializes the shader variables
    void setCamera(Model &model);                               // Sets the camera position based on the mode
    void drawScenegraphModel(Model& model);                     // Draws the model
    void applyScenegraphLighting();                             // Applies lighting to the scenegraph

    sgraph::IScenegraph *sg;                                    // The scenegraph to render
    glm::vec3 cameraPosition;                                   // The camera position
    glm::vec3 cameraTarget;                                     // The camera target
    vector<LightLocation> lightLocations;                       // The light locations
    


    /********************************************** Raytracing **********************************************/

    stack<glm::mat4> raytraceModelview;                         // The modelview matrix for raytracing
    sgraph::RaytracerRenderer *raytracerRenderer;               // The raytracer renderer

    // Apply lighting and shading to the hit record
    glm::vec3 shade(HitRecord& hitRecord,
        const glm::vec4& viewDir,
        const std::vector<util::Light>& lights,
        int bounces,
        float currentRefractiveIndex = 1.0f);
    glm::vec3 applyLighting(HitRecord &hitRecord,
        const util::Light &light,
        const glm::vec3 &n,
        const glm::vec3 &v
    );
    float calculateSpotlight(const util::Light &light, const glm::vec3 &l);

    // Apply reflection and refraction to the hit record
    glm::vec3 applyReflection(HitRecord &hitRecord,
        const glm::vec3 &n,
        const glm::vec3 &v,
        const glm::vec3 &baseColor,
        const std::vector<util::Light> &lights,
        int bounces
    );
    glm::vec3 applyRefraction(HitRecord& hitRecord,
        const glm::vec3& n,
        const glm::vec3& v,
        const std::vector<util::Light>& lights,
        int bounces,
        float currentRefractiveIndex
    );
};

#endif