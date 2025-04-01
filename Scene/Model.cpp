#include "Model.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

Model::Model() {

}

Model::~Model() {
    if (scenegraph) {
        delete scenegraph;
    }
    
    propellerSpeed = 500.0f;  
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

const util::Material& Model::getMaterial(const string& name) {
    return materials[name];
}

const glm::mat4 Model::getTransform(const string& name) {
    return glm::mat4(transformations[name]);
}

const glm::mat4 Model::getAnimationTransform(const string& name) {
    return glm::mat4(animation_transformations[name]);
}

void Model::addMesh(const string& name,util::PolygonMesh<VertexAttrib>& mesh,util::Material& mat,glm::mat4& transformation) {
    meshes[name] = mesh;
    materials[name] = mat;
    transformations[name] = transformation;
    animation_transformations[name] = glm::mat4(1.0f);

}

vector<string> Model::getMeshNames() {
    vector<string> names;

    for (auto const& mapentry:meshes) {
        names.push_back(mapentry.first);
    }
    return names;
}

//this works through the drone's animations, including the propellors and the barrel rolls
void Model::animateStep(float time) {
    static float totalRotation = glm::radians(0.0f);
    totalRotation += glm::radians(propellerSpeed * time); 

    glm::mat4 rollTransform = glm::rotate(glm::mat4(1.0f), rollAngle, glm::vec3(0.0f, 0.0f, 1.0f));

    //movement transformations
    glm::mat4 movementTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, droneMovePos));

    //left right rotations:
    glm::mat4 leftRightRotation = glm::rotate(glm::mat4(1.0f), droneLeftRightRot, glm::vec3(0.0f, 1.0f, 0.0f));

    //up down rotations:
    glm::mat4 upDownRotation = glm::rotate(glm::mat4(1.0f), droneUpDownRot, glm::vec3(1.0f, 0.0f, 0.0f));

    //combine all transformations
    glm::mat4 droneTransform = movementTransform * leftRightRotation * upDownRotation * rollTransform;

    //update propeller transformations
    animation_transformations["DronePropellorBlades"] =
        droneTransform * glm::translate(glm::mat4(1.0), glm::vec3(8.35f, 10.0f, 70.0f)) *
        glm::rotate(glm::mat4(1.0), totalRotation, glm::vec3(0.0f, 1.0f, 0.0f));

    animation_transformations["DronePropellorBlades2"] =
        droneTransform * glm::translate(glm::mat4(1.0), glm::vec3(-8.0f, 10.0f, 50.0f)) *
        glm::rotate(glm::mat4(1.0), totalRotation, glm::vec3(0.0f, 1.0f, 0.0f));


    animation_transformations["DroneBody"] = droneTransform;
    animation_transformations["DroneFront"] = droneTransform;
    animation_transformations["DronePropellorLegs"] = droneTransform;
    animation_transformations["DroneWheels"] = droneTransform;

    //handle roll animation
    if (isRolling) {
        rollAngle += rollSpeed * time * 5.0f;  // Adjust speed for smooth roll


        if (rollAngle >= glm::radians(360.0f)) {
            isRolling = false;
            rollAngle = 0.0f;  
        }
    }
}


//a method that changes the propellor speed of the drone when a key is pressed. 
void Model::adjustPropellerSpeed(float speedAdjustment) {
    propellerSpeed += speedAdjustment;

    //propellerSpeed = glm::clamp(propellerSpeed, 100.0f, 1000.0f); 
}



void Model::startRollAnimation() {
    if (!isRolling) {
        isRolling = true;
        rollAngle = 0.0f;
    }
}



void Model::moveDrone(float direction) {
    // Scale movement speed with propeller speed
    
    droneMovePos += direction;
}

void Model::rotateDrone(float angle) {
    droneLeftRightRot += angle;
}


void Model::tiltDrone(float angle) {
    droneUpDownRot += angle;
}


void Model::resetDrone() {
    //reset the transformation matrices for the entire drone
    animation_transformations["DroneBody"] = glm::mat4(1.0f);
    animation_transformations["DroneFront"] = glm::mat4(1.0f);
    animation_transformations["DronePropellorLegs"] = glm::mat4(1.0f);
    animation_transformations["DronePropellorBlades"] = glm::mat4(1.0f);
    animation_transformations["DronePropellorBlades2"] = glm::mat4(1.0f);
    animation_transformations["DroneWheels"] = glm::mat4(1.0f);

    //reset the drone's movement variables
    droneMovePos = 0.0f;       
    droneLeftRightRot = 0.0f;  
    droneUpDownRot = 0.0f;   

    //stopping any rolling animation
    isRolling = false;
    rollAngle = 0.0f;

    //resetting the propeller animation
    propellerSpeed = 500.0f;  

}



vector<string> Model::getTextureNames() {
    return scenegraph->getTextureNames();
}

util::TextureImage* Model::getTextureObject(const string& name) {
    return scenegraph->getTextureObject(name);
}