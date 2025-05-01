#include "Model.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Model::Model() {
    scenegraph = NULL;

    // starting the Rendermode as OPENGL;
    mode = OPENGL;
}

Model::~Model() {
    if (scenegraph) {
        delete scenegraph;
    }
}

sgraph::IScenegraph *Model::getScenegraph() {
    return this->scenegraph;
}

void Model::setScenegraph(sgraph::IScenegraph *scenegraph) {
    this->scenegraph = scenegraph;
}

const util::PolygonMesh<VertexAttrib>& Model::getMesh(const string& name) {
    return meshes[name];
}

vector<string> Model::getMeshNames() {
    vector<string> names;

    for (auto const& mapentry:meshes) {
        names.push_back(mapentry.first);
    }
    return names;
}

const util::Material& Model::getMaterial(const string& name) {
    return materials[name];
}

vector<string> Model::getTextureNames() {
    return scenegraph->getTextureNames();
}

util::TextureImage* Model::getTextureObject(const string& name) {
    return scenegraph->getTextureObject(name);
}

void Model::toggleRenderMode(){
    if (mode == OPENGL) {
        mode = RAYTRACE;
        std::cout << "Switched to Ray Tracing mode\n";
    }
    else {
        mode = OPENGL;
        std::cout << "Switched to OpenGL mode\n";
    }
}

RenderMode Model::getRenderMode() const {
    return mode; 
}