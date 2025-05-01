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

    initScenegraph();
}

// Initialize the scenegraph by loading it from a file
void Controller::initScenegraph() {
    // Ask user to input the file name
    string filename;
    cout << "Enter the name of the scenegraph file (e.g., final-raytrace.txt): ";
    cin >> filename;

    // Construct full path
    string filepath = "scenegraphmodels/" + filename;
    ifstream inFile(filepath);

    if (!inFile.is_open()) {
        cerr << "Error opening scenegraph file: " << filepath << endl;
        exit(EXIT_FAILURE);
    }

    // Parse the scenegraph
    sgraph::ScenegraphImporter importer;
    IScenegraph *scenegraph = importer.parse(inFile);
    if (scenegraph == nullptr) {
        cerr << "Error parsing scenegraph from file: " << filepath << endl;
        exit(EXIT_FAILURE);
    }

    cout << "Scenegraph loaded from " << filepath << endl;
    model.setScenegraph(scenegraph);
}

Controller::~Controller() {}

void Controller::run() {

    // For debugging
    /*
    vector<string> meshNames = model.getMeshNames();
    cout << "Object Meshes in model: " << endl;
    for (const string& name : meshNames) {
        cout << name << endl;
    } */

    //cout <<"Initializing The View..." << endl;
    view.init(this, model);
    //cout <<"Initialized View" << endl;

    while (!view.shouldWindowClose()) {
        view.display(model);
    }
    
    view.closeWindow();
    exit(EXIT_SUCCESS);
}

void Controller::onkey(int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        //toggle between OpenGL and Ray Tracing
        if (key == GLFW_KEY_S) {
            model.toggleRenderMode();

            // If raytracing mode, save the scene as a png file
            if (model.getRenderMode() == RAYTRACE) {
                cout << "Raytracing..." << endl;
                view.raytrace(model);
                cout << "Raytracing Completed" << endl;
            }
        }
    } 
}

// Callback for mouse button events
void Controller::onMouseClick(int button, int action, int mods) {}

// Callback for mouse movement events
void Controller::onMouseMove(double x, double y) {}

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