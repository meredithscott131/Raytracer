#ifndef __MESHOBJECT_H__
#define __MESHOBJECT_H__

#include <glm/glm.hpp>

#include <vector>
#include <math.h>

#include "Triangle.h"

class MeshObject {
    public:
        MeshObject();
        ~MeshObject();

        void setTriangles(std::vector<Triangle>* in_triangles) {
            std::vector<Triangle> temp = *in_triangles;
            for (int i = 0; i < temp.size(); i++) {
                triangles.push_back(temp[i]);
            }
        }

        bool calcTimes(glm::vec4 s, glm::vec4 v) {
            t = std::numeric_limits<float>::max();
            for (int i = 0; i < triangles.size(); i++) {
                if (triangles[i].calcTimes(s, v)) {
                    float temp = triangles[i].getTime();
                    if (temp < t) {
                        t = temp;
                    }
                }
            }
            return !isinf(t);
        }

        float getTime() {
            return t;
        }

        void transformTriangles(glm::mat4 transform) {
            for (int i = 0; i < triangles.size(); i++) {
                triangles[i].applyTransform(transform);
            }
        }

        void inverseTransformTriangles(glm::mat4 inverseTransform) {
            for (int i = 0; i < triangles.size(); i++) {
                triangles[i].applyTransform(inverseTransform);
            }
        }

    private:
        std::vector<Triangle> triangles;
        float t;
};

#endif