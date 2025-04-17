#ifndef _LEAFNODE_H_
#define _LEAFNODE_H_

#include "AbstractSGNode.h"
#include "SGNodeVisitor.h"
#include "Material.h"
#include "glm/glm.hpp"
#include <map>
#include <stack>
#include <string>
#include <Light.h>
using namespace std;

namespace sgraph
{

/**
 * This node represents the leaf of a scene graph. It is the only type of node that has
 * actual geometry to render.
 * \author Amit Shesh
 */
class LeafNode: public AbstractSGNode
{
    /**
     * The name of the object instance that this leaf contains. All object instances are stored
     * in the scene graph itself, so that an instance can be reused in several leaves
     */
protected:
    string objInstanceName;
    /**
     * The material associated with the object instance at this leaf
     */
    util::Material material;

    string textureName;

    unsigned int textureID = 0;

    util::TextureImage* textureImage = nullptr;



public:
    LeafNode(const string& instanceOf,util::Material& material,const string& name,sgraph::IScenegraph *graph,vector<util::Light>& lights)
        :AbstractSGNode(name,graph,lights) {
        this->objInstanceName = instanceOf;
        this->material = material;
    }

    LeafNode(const string& instanceOf,const string& name,sgraph::IScenegraph *graph,vector<util::Light>& lights)
        :AbstractSGNode(name,graph,lights) {
        this->objInstanceName = instanceOf;
    }
	
	~LeafNode(){}



    /*
	 *Set the material of each vertex in this object
	 */
    void setMaterial(const util::Material& mat) {
        material = mat;
    }

    /*
     * gets the material
     */
    util::Material getMaterial()
    {
        return material;
    }

    /**
     * Get the name of the instance this leaf contains
     * 
     * @return string 
     */
    string getInstanceOf() {
        return this->objInstanceName;
    }

    /**
     * Get a copy of this node.
     * 
     * @return SGNode* 
     */

    SGNode *clone() {
        LeafNode *newclone = new LeafNode(this->objInstanceName,material,name,scenegraph,lights);
        return newclone;
    }

    /**
     * Visit this node.
     * 
     */
    void accept(SGNodeVisitor* visitor) {
      visitor->visitLeafNode(this);
    }



    void setTextureName(string textname) {
        textureName = textname;
    }

    string getTextureName() {
        return textureName;
    }

    void setTextureID(unsigned int id) { textureID = id; }
    unsigned int getTextureID() const { return textureID; }

    //raytracing textures:

    void setTextureObject(util::TextureImage* image) {
        textureImage = image;
    }
    
    util::TextureImage* getTextureObject() const {
        return textureImage;
    }

};
}
#endif
