#ifndef __TRIANGLE_H__
#define __TRIANGLE_H__

#include <glm/glm.hpp>

class Triangle {
    public:
        Triangle();

        Triangle(glm::vec4 v0_in, glm::vec4 v1_in, glm::vec4 v2_in) {
            v0 = v0_in;
            v1 = v1_in;
            v2 = v2_in;
            e1 = v1 - v0;
            e2 = v2 - v0;
            normal = getCrossProduct(e1, e2);
            t = std::numeric_limits<float>::max();
        }

        ~Triangle() {}

        bool calcTimes(glm::vec4 s, glm::vec4 v) {
            float a = glm::dot(v, normal);
            if (a == 0) {
                return false;
            }

            t = (-1.0f * (glm::dot(normal, s))) / glm::dot(normal, v);

            glm::vec4 intersectionPoint = s + t * v;
            if (!pointInsideTriangle(intersectionPoint)) {
                return false;
            }
            return true;
        }

        float getTime() {
            if (t > 0) {
                return t;
            } else {
                return std::numeric_limits<float>::max();
            }
        }

        glm::vec4 getNormal(glm::vec4 intersectionPoint) {
            return normal;
        }

        bool pointInsideTriangle(glm::vec4 intersectionPoint) {
            glm::vec4 e0 = v1 - v0;
            glm::vec4 e1 = v2 - v0;
            glm::vec4 e2 = v0 - v2;
            glm::vec4 c0 = intersectionPoint - v0;
            glm::vec4 c1 = intersectionPoint - v1;
            glm::vec4 c2 = intersectionPoint - v2;

            if ((glm::dot(normal, getCrossProduct(e0, c0)) > 0) &&
                (glm::dot(normal, getCrossProduct(e1, c1)) > 0) &&
                (glm::dot(normal, getCrossProduct(e2, c2)) > 0)) {
                return true;
            } else {
                return false;
            }
        }

        void applyTransform(glm::mat4 transform) {
            v0 = transform * v0;
            v1 = transform * v1;
            v2 = transform * v2;
            e1 = v1 - v0;
            e2 = v2 - v0;
            float normalX = (e1.y * e2.z) - (e1.z * e2.y);
            float normalY = (e1.z * e2.x) - (e1.x * e2.z);
            float normalZ = (e1.x * e2.y) - (e1.y * e2.x);
            normal = normalize(glm::vec4(normalX, normalY, normalZ, 0.0f));
        }
        
        void removeTransform(glm::mat4 inverseTransform) {
            v0 = inverseTransform * v0;
            v1 = inverseTransform * v1;
            v2 = inverseTransform * v2;
            e1 = v1 - v0;
            e2 = v2 - v0;
            float normalX = (e1.y * e2.z) - (e1.z * e2.y);
            float normalY = (e1.z * e2.x) - (e1.x * e2.z);
            float normalZ = (e1.x * e2.y) - (e1.y * e2.x);
            normal = normalize(glm::vec4(normalX, normalY, normalZ, 0.0f));
        }

    private:
        glm::vec4 v0; 
        glm::vec4 v1; 
        glm::vec4 v2;
        glm::vec4 e1;
        glm::vec4 e2;
        glm::vec4 normal;
        float t;
        glm::vec4 getCrossProduct(glm::vec4 vec1, glm::vec4 vec2) {
            return glm::vec4(vec1.y * vec2.z - vec1.z * vec2.y,
                             vec1.z * vec2.x - vec1.x * vec2.z,
                             vec1.x * vec2.y - vec1.y * vec2.x, 0.0f);
        }
};

#endif