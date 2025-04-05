#ifndef __MODEL_H__
#define __MODEL_H__

#include "PolygonMesh.h"
#include "VertexAttrib.h"
#include "sgraph/IScenegraph.h"
#include <map>
using namespace std;

#include "Material.h"

//getting ready to toggle between the two render modes
enum RenderMode {OPENGL, RAYTRACE};


class Model 
{
public:
    Model();
    ~Model();

    sgraph::IScenegraph *getScenegraph();
    void setScenegraph(sgraph::IScenegraph *scenegraph);

    const util::PolygonMesh<VertexAttrib>& getMesh(const string& name);
    const util::Material& getMaterial(const string& name);
    const glm::mat4 getTransform(const string& name);
    const glm::mat4 getAnimationTransform(const string& name);
    vector<string> getMeshNames();
    void addMesh(const string& name,util::PolygonMesh<VertexAttrib>& mesh,util::Material& mat,glm::mat4& transformation);
    void animateStep(float time);
    void adjustPropellerSpeed(float speedAdjustment);
    void startRollAnimation(); 

    void moveDrone(float direction);
    //rotateDrone is for the left/right arrow keys
    void rotateDrone(float angle);
    //tiltDrone is for the up/down arrow keys which rotate it up or down :D
    void tiltDrone(float angle);
    //resetting the drone's position
    void resetDrone();

    
    vector<string> getTextureNames();
    util::TextureImage* getTextureObject(const string& name);

    void toggleRenderMode();
    RenderMode getRenderMode() const;

private:
    sgraph::IScenegraph *scenegraph;

    map<string,util::PolygonMesh<VertexAttrib> > meshes;
    map<string,util::Material> materials;
    map<string,glm::mat4> transformations;
    map<string,glm::mat4> animation_transformations;


    //variables needed for the drone's animations
    float propellerSpeed;  
    float rollAngle = 0.0f;  //tracks roll animation progress
    bool isRolling = false;  
    float rollSpeed = glm::radians(45.0f);

    float droneMovePos = 0.0f;
    float droneLeftRightRot = 0.0f;
    float droneUpDownRot = 0.0f;

    RenderMode mode;
};
#endif