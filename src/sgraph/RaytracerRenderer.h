#ifndef _RAYTRACERRENDERER_H_
#define _RAYTRACERRENDERER_H_

#include "SGNodeVisitor.h"
#include "GroupNode.h"
#include "LeafNode.h"
#include "TransformNode.h"
#include "RotateTransform.h"
#include "ScaleTransform.h"
#include "TranslateTransform.h"
#include <ShaderProgram.h>
#include <ShaderLocationsVault.h>
#include "ObjectInstance.h"
#include <stack>
#include <iostream>
#include "../objects/MeshObject.h"
#include "../HitRecord.h"
#include "../objects/Box.h"
#include "../objects/Sphere.h"

using namespace std;

namespace sgraph {
    /**
     * This visitor implements drawing the scene graph using Raytracing
     * 
     */
    class RaytracerRenderer: public SGNodeVisitor {
        public:
        /**
         * @brief Construct a new GLScenegraphRenderer object
         * 
         * @param mv a reference to modelview stack that will be used while rendering
         * @param os the map of ObjectInstance objects
         * @param shaderLocations the shader locations for the program used to render
         */
        RaytracerRenderer(stack<glm::mat4>& mv,map<string,util::ObjectInstance *>& os, util::ShaderLocationsVault& shaderLocations, glm::vec4 o, glm::vec4 d) 
            : modelview(mv)
            , objects(os)
            , origin(o),
            direction(d) {
            this->shaderLocations = shaderLocations;
            for (map<string,util::ObjectInstance *>::iterator it=objects.begin();it!=objects.end();it++) {
                //cout << "Mesh with name: "<< it->first << endl;
            }
        }

        void setTextureIds(map<string,unsigned int> & viewtextureIds){
            this->viewtextureIds = viewtextureIds;
        }

        /**
         * @brief Recur to the children for drawing
         * 
         * @param groupNode 
         */
        void visitGroupNode(GroupNode *groupNode) {
            for (int i=0;i<groupNode->getChildren().size();i=i+1) {
                groupNode->getChildren()[i]->accept(this);
            }
        }

        /**
         * @brief Draw the instance for the leaf, after passing the 
         * modelview and color to the shader
         * 
         * @param leafNode 
         */
        void visitLeafNode(LeafNode *leafNode) {
            glm::mat4 inverseTransform = glm::inverse(modelview.top());
            glm::vec4 transformedOrigin = inverseTransform * origin;
            glm::vec4 transformedDirection = inverseTransform * direction;

            bool hit;
            float newTime;
            if (leafNode->getInstanceOf() == "box") {
                hit = box.calcTimes(transformedOrigin, transformedDirection);
                if (hit) {
                    newTime = box.getTime();
                }
            } else if (leafNode->getInstanceOf() == "sphere") {
                hit = sphere.calcTimes(transformedOrigin, transformedDirection);
                if (hit) {
                    newTime = sphere.getTime();
                }
            } else {
                MeshObject m = meshObjects.at(leafNode->getInstanceOf());
                hit = m.calcTimes(transformedOrigin, transformedDirection);
                if (hit) {
                    newTime = m.getTime();
                }
            }

            if (hit) {
                if (newTime < hitRecordWithMinTime.t) {
                    // Calculate the intersection point and normal
                    glm::vec4 intersectionPoint = transformedOrigin + (newTime * transformedDirection);
                    glm::vec4 normal;
                    normal = getNormal(intersectionPoint, leafNode->getInstanceOf());

                    // Transform the intersection point and normal back to world coordinates
                    glm::mat4 mvt1 = modelview.top();
                    intersectionPoint = mvt1 * intersectionPoint;
                    glm::mat4 mvt2 = modelview.top();
                    normal = mvt2 * normal;

                    // Creating a new hit record with the new time and intersection point
                    HitRecord newHitRecord(newTime, intersectionPoint, normal, leafNode->getMaterial());
                    hitRecordWithMinTime = newHitRecord;
                }
            }
        }

        /**
         * @brief Multiply the transform to the modelview and recur to child
         * 
         * @param transformNode 
         */
        void visitTransformNode(TransformNode * transformNode) {
            modelview.push(modelview.top());
            modelview.top() = modelview.top() * transformNode->getTransform();
            if (transformNode->getChildren().size()>0) {
                transformNode->getChildren()[0]->accept(this);
            }
            modelview.pop();
        }

        /**
         * @brief For this visitor, only the transformation matrix is required.
         * Thus there is nothing special to be done for each type of transformation.
         * We delegate to visitTransformNode above
         * 
         * @param scaleNode 
         */
        void visitScaleTransform(ScaleTransform *scaleNode) {
            visitTransformNode(scaleNode);
        }

        /**
         * @brief For this visitor, only the transformation matrix is required.
         * Thus there is nothing special to be done for each type of transformation.
         * We delegate to visitTransformNode above
         * 
         * @param translateNode 
         */
        void visitTranslateTransform(TranslateTransform *translateNode) {
            visitTransformNode(translateNode);
        }

        void visitRotateTransform(RotateTransform *rotateNode) {
            visitTransformNode(rotateNode);
        }

        HitRecord& getHitRecord() {
            return hitRecordWithMinTime;
        }

        private:
        stack<glm::mat4>& modelview;    
        util::ShaderLocationsVault shaderLocations;
        map<string,util::ObjectInstance *> objects;
        map<string,unsigned int> viewtextureIds;
        glm::vec4 origin;
        glm::vec4 direction;
        Box box;
        Sphere sphere;
        float minTime = std::numeric_limits<float>::max();
        HitRecord hitRecordWithMinTime = HitRecord(minTime, glm::vec4(0.0f), glm::vec4(0.0f), util::Material());
                map<string, MeshObject> meshObjects;

        glm::vec4 getNormal(glm::vec4 intersectionPoint, string instanceName) {
            if (instanceName == "box") {
                return box.getNormal(intersectionPoint);
            } else if (instanceName == "sphere") {
                return sphere.getNormal(intersectionPoint);
            } else {
                // Default normal
                return glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            }
        }
   };
}

#endif