#include "Controller.h"
#include "sgraph/IScenegraph.h"
#include "sgraph/Scenegraph.h"
#include "sgraph/GroupNode.h"
#include "sgraph/LeafNode.h"
#include "sgraph/ScaleTransform.h"
#include "ObjImporter.h"
#include <ObjImporter.h>

using namespace sgraph;
#include <iostream>
using namespace std;

#include "sgraph/ScenegraphExporter.h"
#include "sgraph/ScenegraphImporter.h"
#include <glm/gtc/quaternion.hpp>

Controller::Controller(Model& m,View& v) {
    model = m;
    view = v;
    time = 0.0f;

    //createDrone();
    initScenegraph();
}

void Controller::initScenegraph() {
    //loading the scenegraph from the file
    cout << "Loading scenegraph from file" << endl;
    ifstream inFile("scenegraphmodels/Scene.txt");

    sgraph::ScenegraphImporter importer;
    IScenegraph *scenegraph = importer.parse(inFile);
    cout << "Scenegraph loaded from file" << endl;

    model.setScenegraph(scenegraph);
    cout << "Scenegraph set in model" << endl;
}

void Controller::createDrone() {
    util::PolygonMesh<VertexAttrib> mesh;
    glm::mat4 transform;
    util::Material mat;

    ifstream in;

     //the body of the drone is cyan
     in.open("models/DroneBody.obj");
     mesh = util::ObjImporter<VertexAttrib>::importFile(in, true);
     mat.setAmbient(0.0, 1.0, 1.0);  
     mat.setDiffuse(0.0, 1.0, 1.0);
     mat.setSpecular(0.8, 0.1, 0.1);
     transform = glm::scale(glm::mat4(1.0), glm::vec3(20.0f, 20.0f, 20.0f)) *
                 glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 0.5f, 3.0f));
     model.addMesh("DroneBody", mesh, mat, transform);
     in.close();
 
     //the front of the drone is a red cylinder
     in.open("models/DroneFront.obj");
     mesh = util::ObjImporter<VertexAttrib>::importFile(in, true);
     mat.setAmbient(1.0, 0.0, 0.0);  
     mat.setDiffuse(1.0, 0.0, 0.0);
     mat.setSpecular(0.8, 0.1, 0.1);
     transform = glm::scale(glm::mat4(1.0), glm::vec3(5.0f, 5.0f, 5.0f)) *
                 glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 2.0f, 10.0f));
     model.addMesh("DroneFront", mesh, mat, transform);
     in.close();
 
     //legs for the propellors are green
     in.open("models/DronePropellorLegs.obj");
     mesh = util::ObjImporter<VertexAttrib>::importFile(in, true);
     mat.setAmbient(0.0, 1.0, 0.0);
     mat.setDiffuse(0.0, 1.0, 0.0);
     mat.setSpecular(0.3, 1.0, 0.3);
     transform = glm::scale(glm::mat4(1.0), glm::vec3(20.0f, 20.0f, 20.0f)) *
                 glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 0.5f, 3.0f));
     model.addMesh("DronePropellorLegs", mesh, mat, transform);
     in.close();
 
     //the drone blades are white
     in.open("models/DronePropellorBlades.obj");
     mesh = util::ObjImporter<VertexAttrib>::importFile(in, true);
     mat.setAmbient(1.0, 1.0, 1.0);  
     mat.setDiffuse(1.0, 1.0, 1.0);
     mat.setSpecular(0.9, 0.9, 0.9);
     transform = glm::scale(glm::mat4(1.0), glm::vec3(10.0f, 10.0f, 10.0f));
     model.addMesh("DronePropellorBlades", mesh, mat, transform);
     in.close();
 
     //the drone blades are white
     in.open("models/DronePropellorBlades.obj");
     mesh = util::ObjImporter<VertexAttrib>::importFile(in, true);
     mat.setAmbient(1.0, 1.0, 1.0);  
     mat.setDiffuse(1.0, 1.0, 1.0);
     mat.setSpecular(0.9, 0.9, 0.9);
     transform = glm::scale(glm::mat4(1.0), glm::vec3(10.0f, 10.0f, 10.0f));
     model.addMesh("DronePropellorBlades2", mesh, mat, transform);
     in.close();
 
     //the wheels at the bottom are blue
     in.open("models/DroneWheels.obj");
     mesh = util::ObjImporter<VertexAttrib>::importFile(in, true);
     mat.setAmbient(0.0, 0.0, 1.0);
     mat.setDiffuse(0.0, 0.0, 1.0);
     mat.setSpecular(0.2, 0.2, 0.9);
     transform = glm::scale(glm::mat4(1.0), glm::vec3(20.0f, 20.0f, 20.0f)) *
                 glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 0.5f, 3.0f));
     model.addMesh("DroneWheels", mesh, mat, transform);
     in.close();
     
}

Controller::~Controller()
{
    
}

void Controller::run()
{
    vector<string> meshNames = model.getMeshNames();
    cout << "Object Meshes in model: " << endl;
    for (const string& name : meshNames) {
        cout << name << endl;
    }

    cout <<"about to initialize the view" << endl;
    view.init(this, model);
    cout <<"Initialized properly";

    float lastTime = glfwGetTime();

    while (!view.shouldWindowClose()) {
        float currentTime = glfwGetTime();
        float time = currentTime - lastTime;
        lastTime = currentTime;
        if (time == 0.0f) {
            model.animateStep(5.0f);
        }
        else {
            model.animateStep(time);
        }
        view.display(model);
    }
    
    view.closeWindow();
    exit(EXIT_SUCCESS);
}

void Controller::onkey(int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        // Resetting trackball rotation
        if (key == GLFW_KEY_R) {

            // Reset the rotation quaternion and matrix to the identity
            view.setRotationQuat(glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
            view.setRotationMatrix(glm::mat4(1.0f));

            // Redraw the scene
            view.display(model);
        }

        //drone animations:
        //increasing propeller speed
        if (key == GLFW_KEY_F) {
            model.adjustPropellerSpeed(50.0f);
            cout << "Propeller Speed Increased" << endl;
        }
        //decrease propeller speed
        else if (key == GLFW_KEY_S) {
            model.adjustPropellerSpeed(-50.0f);
            cout << "Propeller Speed Decreased" << endl;
        }
        //start roll animation
        else if (key == GLFW_KEY_J) {
            model.startRollAnimation();
            cout << "Drone Rolling" << endl;
        }

        else if (key == GLFW_KEY_EQUAL) {
            model.moveDrone(10.0f);
            cout <<"moving drone forward" << endl;
        }
        else if (key == GLFW_KEY_MINUS) {
            model.moveDrone(-10.0f);
            cout <<"moving drone backwards" << endl;
        }


        //rotate Left/Right
        else if (key == GLFW_KEY_LEFT) model.rotateDrone(glm::radians(5.0f));
        else if (key == GLFW_KEY_RIGHT) model.rotateDrone(glm::radians(-5.0f));

        //tilt Up/Down
        else if (key == GLFW_KEY_UP) model.tiltDrone(glm::radians(-5.0f));
        else if (key == GLFW_KEY_DOWN) model.tiltDrone(glm::radians(5.0f));

        // resetting the drone position
        if (key == GLFW_KEY_D) model.resetDrone();

        // Changing the camera mode
        if (key == GLFW_KEY_1) view.changeCameraMode(View::GLOBAL);
        else if (key == GLFW_KEY_2) view.changeCameraMode(View::CHOPPER);
        else if (key == GLFW_KEY_3) view.changeCameraMode(View::DRONE);


        //changing between render modes
        if (key == GLFW_KEY_S) {
            model.toggleRenderMode();
            std::cout << "Switched to "<< (model.getRenderMode() == OPENGL ? "OpenGL" : "Ray Tracing") <<" mode.\n";
        }
    } 


}

void Controller::reshape(int width, int height) 
{
    glViewport(0, 0, width, height);
    // Update the projection matrix with the new aspect ratio
    view.updateProjection(width, height);
}

void Controller::dispose()
{
    view.closeWindow();
}

void Controller::error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

// Callback for mouse button events
void Controller::onMouseClick(int button, int action, int mods) {
    int width, height;
    glfwGetWindowSize(view.getWindow(), &width, &height);

    // If the left mouse button is pressed, start dragging
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            isDragging = true;
            double x, y;
            glfwGetCursorPos(view.getWindow(), &x, &y);
            // Project the cursor position to the trackball
            lastTrackballPos = view.convertToTrackball(x, y, 1.0f, width, height);
        } else if (action == GLFW_RELEASE) {
            // Stop dragging on mouse release
            isDragging = false;
        }
    }
}

// Callback for mouse movement events
void Controller::onMouseMove(double x, double y) {
    if (!isDragging) return;

    int width, height;
    glfwGetWindowSize(view.getWindow(), &width, &height);

    // Project the cursor position to the trackball
    glm::vec3 currentPos = view.convertToTrackball(x, y, 1.0f, width, height);
    glm::vec3 axis = glm::cross(lastTrackballPos, currentPos);
    float angle = glm::length(axis);

    // Do nothing if the angle is too small
    if (angle > 0.00001f) {
        axis = glm::normalize(axis);

        // Update the rotation quaternion and matrix
        glm::quat rotationQuat = glm::angleAxis(angle, axis);
        view.setRotationQuat(rotationQuat * view.getRotationQuat());
        view.setRotationMatrix(glm::mat4_cast(view.getRotationQuat()));
    }

    lastTrackballPos = currentPos;
}