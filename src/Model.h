#ifndef __MODEL_H__
#define __MODEL_H__

#include "PolygonMesh.h"
#include "VertexAttrib.h"
#include "sgraph/IScenegraph.h"
#include <map>
using namespace std;

#include "Material.h"

// For switching between OpenGL and Raytracing modes
enum RenderMode {OPENGL, RAYTRACE};


class Model 
{
public:
    Model();
    ~Model();

    sgraph::IScenegraph *getScenegraph();
    void setScenegraph(sgraph::IScenegraph *scenegraph);

    const util::PolygonMesh<VertexAttrib>& getMesh(const string& name);
    vector<string> getMeshNames();

    const util::Material& getMaterial(const string& name);
    
    vector<string> getTextureNames();
    util::TextureImage* getTextureObject(const string& name);

    void toggleRenderMode();
    RenderMode getRenderMode() const;

private:
    sgraph::IScenegraph *scenegraph;

    map<string,util::PolygonMesh<VertexAttrib> > meshes;
    map<string,util::Material> materials;

    RenderMode mode;
};

#endif