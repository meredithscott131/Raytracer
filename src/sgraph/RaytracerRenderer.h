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
#include "../objects/Cone.h"
#include "../objects/Cylinder.h"
#include "../objects/AbstractRaytraceObject.h"
#include "../Ray.h"

#include <cmath>
#define M_PI 3.14159265358979323846


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
            RaytracerRenderer(stack<glm::mat4>& mv, Ray ray) : modelview(mv), s(glm::vec4(ray.origin, 1.0f)), v(glm::vec4(ray.direction, 0.0f)) {
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

                // Set up the raytrace object based on the type of leaf node

                bool objectKnown = true;

                if (leafNode->getInstanceOf() == "box") raytraceObject = &box;
                else if (leafNode->getInstanceOf() == "sphere") raytraceObject = &sphere;
                else if (leafNode->getInstanceOf() == "cone") raytraceObject = &cone;
                else if (leafNode->getInstanceOf() == "cylinder") raytraceObject = &cylinder;
                else objectKnown = false;

                bool hit;         // whether ray intersects with the object
                float time;       // time of intersection

                if (!objectKnown) {
                    hit = false;
                    time = std::numeric_limits<float>::infinity();
                } else {
                    hit = raytraceObject->didHit(transformedS, transformedV);
                    if (hit) {
                        time = raytraceObject->getTime();
                    }
                }

                if (hit) {
                    if (time < hitRecord.t) {
                        glm::vec4 localIntersection = transformedS + (time * transformedV);
                        glm::vec4 normal = raytraceObject->getNormal(localIntersection);
                
                        glm::vec2 texCoords = getTextureCoordinates(leafNode->getInstanceOf(), localIntersection);
                        
                        // Transform point and normal to world/view space
                        glm::vec4 worldIntersection = modelview.top() * localIntersection;
                        glm::vec4 worldNormal = modelview.top() * normal;
                
                        HitRecord updatedHitRecord(
                            time,
                            worldIntersection,
                            worldNormal,
                            leafNode->getMaterial()
                        );
                        updatedHitRecord.textureImage = leafNode->getTextureObject();
                        updatedHitRecord.textureCoordinates = texCoords;
                        hitRecord = updatedHitRecord;
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
                return hitRecord;
            }

            glm::vec2 getTextureCoordinates(const std::string& instanceOf, const glm::vec4& point) {
                if (instanceOf == "sphere") {
                    return sphere.getTextureCoordinates(point);
                }
                else if (instanceOf == "box") {
                    return box.getTextureCoordinates(point);
                }
                return glm::vec2(0.0f, 0.0f); 
            }
            

        private:
            stack<glm::mat4>& modelview; // the modelview matrix stack
            glm::vec4 s;                 // the camera position
            glm::vec4 v;                 // the ray direction

            AbstractRaytraceObject *raytraceObject;     // the abstract raytrace object
            Box box;                                    // the box object
            Sphere sphere;                              // the sphere object
            Cone cone;                                  // the cone object
            Cylinder cylinder;                          // the cylinder object

            // HitRecord object to store the hit information
            HitRecord hitRecord = HitRecord(std::numeric_limits<float>::infinity(),
                                    glm::vec4(0.0f), glm::vec4(0.0f), util::Material());
   };
}

#endif