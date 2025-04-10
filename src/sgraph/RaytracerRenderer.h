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
         */
        RaytracerRenderer(stack<glm::mat4>& mv, glm::vec4 s, glm::vec4 v) : modelview(mv), s(s), v(v) {
            this->hitRecord = HitRecord(std::numeric_limits<float>::infinity(), glm::vec4(0.0f), glm::vec4(0.0f), util::Material());
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
         * @brief Updates hit record if the ray hits the object
         * 
         * @param leafNode 
         */
        void visitLeafNode(LeafNode *leafNode) {
            // Get the modelview matrix for the current node
            glm::mat4 inverseTransform = glm::inverse(modelview.top());
            glm::vec4 transformedS = inverseTransform * s;
            glm::vec4 transformedV = inverseTransform * v;

            bool hit;         // hit is true if the ray intersects with the object
            float time;       // the time of intersection

            if (leafNode->getInstanceOf() == "box") {
                hit = box.calcTimes(transformedS, transformedV);
                if (hit) {
                    time = box.getTime();
                }
            } else if (leafNode->getInstanceOf() == "sphere") {
                hit = sphere.calcTimes(transformedS, transformedV);
                if (hit) {
                    time = sphere.getTime();
                }
            } else {
                // Unknown object type, no hit
                hit = false;
                time = std::numeric_limits<float>::infinity();
            }

            if (hit) {
                if (time < hitRecord.t) {
                    // Calculate the intersection point and normal
                    glm::vec4 intersectionPoint = transformedS + (time * transformedV);
                    glm::vec4 normal;
                    normal = getNormal(intersectionPoint, leafNode->getInstanceOf());

                    // Transform the intersection point and normal back to world coordinates
                    glm::mat4 mv = modelview.top();
                    intersectionPoint = mv * intersectionPoint;
                    glm::mat4 updatedMV = modelview.top();
                    normal = updatedMV * normal;
                    
                    HitRecord updatedHitRecord(time, intersectionPoint, normal, leafNode->getMaterial());

                    hitRecord = updatedHitRecord;
                }
            } else {
                //cout << "NO HIT" << endl;
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
            return hitRecord;
        }

        private:
        stack<glm::mat4>& modelview; // the modelview matrix stack
        glm::vec4 s;                 // the camera position
        glm::vec4 v;                 // the ray direction

        Box box;                     // the box object
        Sphere sphere;               // the sphere object

        // HitRecord object to store the hit information
        HitRecord hitRecord = HitRecord(std::numeric_limits<float>::infinity(), glm::vec4(0.0f), glm::vec4(0.0f), util::Material());

        // Returns the normal vector at the intersection point based on the object type
        glm::vec4 getNormal(glm::vec4 intersectionPoint, string instanceName) {
            if (instanceName == "box") {
                return box.getNormal(intersectionPoint);
            } else if (instanceName == "sphere") {
                return sphere.getNormal(intersectionPoint);
            } else {
                return glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            }
        }
   };
}

#endif