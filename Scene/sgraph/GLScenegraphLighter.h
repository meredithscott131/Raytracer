#ifndef _GLSCENEGRAPHLIGHTER_H_
#define _GLSCENEGRAPHLIGHTER_H_

#include "SGNodeVisitor.h"
#include "GroupNode.h"
#include "LeafNode.h"
#include "Light.h"
#include "TransformNode.h"
#include "RotateTransform.h"
#include "ScaleTransform.h"
#include "TranslateTransform.h"
#include <ShaderProgram.h>
#include <ShaderLocationsVault.h>
#include "ObjectInstance.h"
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stack>
#include <iostream>
using namespace std;

namespace sgraph {
    /**
     * This visitor implements drawing the scene graph using OpenGL
     * 
     */
    class GLScenegraphLighter: public SGNodeVisitor {

        class LightLocation {
        public:
            int ambient,diffuse,specular,position;
            LightLocation()
            {
            ambient = diffuse = specular = position = -1;
            }

        };

        public:
        /**
         * @brief Construct a new GLScenegraphRenderer object
         * 
         * @param mv a reference to modelview stack that will be used while rendering
         */
        GLScenegraphLighter(stack<glm::mat4>& mv,vector<util::Light>& lights, vector<string>& lightCoordinateSystems) 
            : modelview(mv)
            , _lights(lights)
            , _lightCoordinateSystems(lightCoordinateSystems){
        }

        /**
         * @brief Recur to the children for drawing
         * 
         * @param groupNode 
         */
        void visitGroupNode(GroupNode *groupNode) {
            //cout << "visitGroupNode()" << endl;
            //cout << "lights = " << groupNode->getLights().size() << endl

            std::vector<util::Light> nodeLights = groupNode->getLights();
            pushLights(nodeLights);
            
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
            //cout << "VisitLeafNode()" << endl;
            //cout << "lights = " << leafNode->getLights().size() << endl;
            
            std::vector<util::Light> nodeLights = leafNode->getLights();
            pushLights(nodeLights);
        }

        /**
         * @brief Multiply the transform to the modelview and recur to child
         * 
         * @param transformNode 
         */
        void visitTransformNode(TransformNode * transformNode) {
            //cout << "lights = " << transformNode->getLights().size() << endl;
            modelview.push(modelview.top());
            modelview.top() = modelview.top() * transformNode->getTransform();
            std::vector<util::Light> nodeLights = transformNode->getLights();
            pushLights(nodeLights);
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
            //cout << "visitTranslateNode()" << endl;
            visitTransformNode(translateNode);
        }

        void visitRotateTransform(RotateTransform *rotateNode) {
            visitTransformNode(rotateNode);
        }

        private:
        stack<glm::mat4>& modelview;    
        
        vector<util::Light>& _lights;
        vector<string>& _lightCoordinateSystems; 

        void pushLights(vector<util::Light>& nodeLights) {
            for (int i = 0; i < nodeLights.size(); i++) {
                
                glm::vec3 temp = nodeLights[i].getPosition();
                glm::vec4 pos1 = glm::vec4(temp.x,temp.y,temp.z,1.0); 
                glm::vec4 pos = modelview.top() * pos1;
                nodeLights[i].setPosition(pos.x,pos.y,pos.z);
                
                glm::vec3 temp2 = nodeLights[i].getSpotDirection();
                glm::vec4 pos2 = glm::vec4(temp2.x,temp2.y,temp2.z,1.0); 
                glm::vec4 pos3 = modelview.top() * pos2;
                nodeLights[i].setSpotDirection(pos3.x,pos3.y,pos3.z);

                _lights.push_back(nodeLights[i]);
                _lightCoordinateSystems.push_back("world");
            }
        }

   };
}

#endif