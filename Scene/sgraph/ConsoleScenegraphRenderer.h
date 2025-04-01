#ifndef _CONSOLESCENEGRAPHRENDERER_H_
#define _CONSOLESCENEGRAPHRENDERER_H_

#include "SGNodeVisitor.h"
#include "GroupNode.h"
#include "LeafNode.h"
#include "TransformNode.h"
#include "RotateTransform.h"
#include "ScaleTransform.h"
#include "TranslateTransform.h"
#include <sstream>
#include <iostream>
using namespace std;

namespace sgraph {
    /**
     * This visitor implements writing the scenegraph hierarchy to the console
     * 
     */
    class ConsoleScenegraphRenderer: public SGNodeVisitor {
        public:
            
             // Constructor
            ConsoleScenegraphRenderer() {
                append("Scenegraph:");
                level++;
            }

            // Returns the formatted output
            string getOutput() {
                return output.str();
            }

            // Visit the leaf node
            void visitLeafNode(LeafNode *leafNode) override {
                append("- " + leafNode->getName());
            }

            // Visit the group node
            void visitGroupNode(GroupNode *groupNode) override {
                append("- " + groupNode->getName());
                visitParentSGNode(groupNode);
            }

            // Visit the scale transform node
            void visitScaleTransform(ScaleTransform *scaleNode) override {
                append("- scale-" + scaleNode->getName());
                visitParentSGNode(scaleNode);
            }

            // Visit the translate transform node
            void visitTranslateTransform(TranslateTransform *translateNode) override {
                append("- translate-" + translateNode->getName());
                visitParentSGNode(translateNode);
            }

            // Visit the rotate transform node
            void visitRotateTransform(RotateTransform *rotateNode) override {
                append("- rotate-" + rotateNode->getName());
                visitParentSGNode(rotateNode);
            }

            // Visit the transform node
            void visitTransformNode(TransformNode *transformNode) override {
                visitParentSGNode(transformNode);
            }

        private:
            int level = 0;              // Current level of the tree
            stringstream output;        // Output stream

            // Append the string to the output, considering the current level for indentation
            void append(const string& str) {
                output << string(level * 2, ' ') << str << endl;
            }

            // Visit the parent node, incrementing the level
            void visitParentSGNode(ParentSGNode *node) {
                level++;
                for (auto child : node->getChildren()) {
                    child->accept(this);
                }
                level--;
            }
    };
}

#endif