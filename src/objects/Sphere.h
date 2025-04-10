#ifndef __SPHERE_H__
#define __SPHERE_H__

#include "AbstractRaytraceObject.h"

// Sphere raytrace object
class Sphere : public AbstractRaytraceObject {
    public:
        Sphere() {};
        ~Sphere() {};

        bool didHit(glm::vec4 s, glm::vec4 v) override {
            float a = pow(v.x,2) + pow(v.y,2) + pow(v.z,2);
            float b = 2 * ((s.x * v.x) + (s.y * v.y) + (s.z * v.z));
            float c = pow(s.x,2) + pow(s.y,2) + pow(s.z,2) - pow(radius,2);
        
            float discriminant = pow(b,2) - (4 * a * c);
            if (discriminant < 0) {
                return false;
            }
        
            tMin = (-b + sqrt(discriminant)) / (2 * a);
            tMax = (-b - sqrt(discriminant)) / (2 * a);
        
            if (tMin > tMax) {
                std::swap(tMin,tMax);
            }
            return true;
        }
        
        float getTime() override {
            if (tMin > 0 && tMax > 0 && tMin <= tMax) {
                return tMin;
            } else if (tMin < 0 && tMax > 0) {
                return tMax;
            } else {
                return INFINITY;
            }
        }

        glm::vec4 getNormal(glm::vec4 intersectionPoint) override {
            glm::vec4 normal = glm::vec4(intersectionPoint.x, intersectionPoint.y, intersectionPoint.z, 0.0f);
            return normal;
        }

    private:
        float radius = 1.0f;
};

#endif