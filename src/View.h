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
    typedef enum {GLOBAL,CHOPPER,DRONE} TypeOfCamera;

    void init(Callbacks* callbacks, Model& model);

    
    void display(Model& model);
    bool shouldWindowClose();
    void closeWindow();
    GLFWwindow* getWindow() const { return window; }

    glm::vec3 convertToTrackball(double x, double y, float radius, int width, int height);
    void updateProjection(int width, int height);
    glm::mat4 getRotationMatrix() const { return rotationMatrix; }
    void setRotationMatrix(glm::mat4 rotationMatrix) { this->rotationMatrix = rotationMatrix; }
    glm::quat getRotationQuat() const { return rotationQuat; }
    void setRotationQuat(glm::quat rotationQuat) { this->rotationQuat = rotationQuat; }

    void changeCameraMode(TypeOfCamera mode);
    void updateDroneTransform(const glm::mat4& transform);

    void raytrace(Model& model);

private: 

    GLFWwindow* window;
    util::ShaderProgram program;
    util::ShaderLocationsVault shaderLocations;
    map<string,util::ObjectInstance *> objects;
    sgraph::SGNodeVisitor *renderer;
    int frames;
    double time;
    glm::mat4 projection;
    stack<glm::mat4> modelview;
    glm::ivec2 window_dimensions;

    sgraph::IScenegraph *sg;                                        // The scenegraph to render
    glm::mat4 rotationMatrix = glm::mat4(1.0f);                     // The rotation matrix
    glm::quat rotationQuat = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);     // The rotation quaternion
    TypeOfCamera cameraMode = GLOBAL;                               // The camera mode
    glm::vec3 initDronePosition = glm::vec3(0.0f, 10.0f, 50.0f);    // The initial drone position
    glm::vec3 curDronePosition = initDronePosition;                 // The current drone position
    glm::vec3 cameraPosition;
    glm::vec3 cameraTarget;

    int angleOfRotation;
    glm::mat4 droneTransform = glm::mat4(1.0f);

    void initObjects(Model& model);
    void initShaderVariables(vector<util::Light>& lights);
    void setCamera(TypeOfCamera mode, Model &model);

    vector<LightLocation> lightLocations;

    // Raytracing
    bool isInShadow(HitRecord hitRecord, vector<vector<util::Light>> &lights);
    stack<glm::mat4> raytraceModelview;
    sgraph::RaytracerRenderer *raytracerRenderer;
    glm::vec3 shade(HitRecord& hitRecord, const glm::vec4& viewDir, const std::vector<util::Light>& lights, int bounces);

};

#endif