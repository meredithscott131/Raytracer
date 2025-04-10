#ifndef __CYLINDER_H__
#define __CYLINDER_H__

#include "AbstractRaytraceObject.h"

class Cylinder : public AbstractRaytraceObject {
    public:
        Cylinder() {};
        ~Cylinder() {};

        bool didHit(glm::vec4 s, glm::vec4 v) override {
            float a = pow(v.x,2) + pow(v.z,2);
            float b = (2 * v.x * s.x) + (2 * v.z * s.z);
            float c = pow(s.x,2) + pow(s.z,2) - radius;

            float discriminant = pow(b,2) - (4 * a * c);
            if (discriminant < 0) {
                return false;
            }
        
            tMin = (-b + sqrt(discriminant)) / (2 * a);
            tMax = (-b - sqrt(discriminant)) / (2 * a);
        
            if (tMin > tMax) {
                std::swap(tMin,tMax);
            }
        
            float tMinY = (vMin.y - s.y) / v.y; 
            float tMaxY = (vMax.y - s.y) / v.y; 
            
            if (tMinY > tMaxY) {
                std::swap(tMinY, tMaxY); 
            }
            
            if ((tMin > tMaxY) || (tMax < tMinY)) {
                return false;
            }
        
            tMin = (tMinY > tMin) ? tMinY : tMin;
            tMax = (tMaxY < tMax) ? tMaxY : tMax;

            return true;
        }    

        float getTime() override {
            if (tMin > 0 && tMax > 0 && tMin <= tMax) {
                return tMin;
            }
            else if (tMin < 0 && tMax > 0) {
                return tMax;
            }
            else {
                return INFINITY; 
            }
        }

        glm::vec4 getNormal(glm::vec4 intersectionPoint) override {        
            glm::vec4 normal(0,0,0,0);
        
            bool onTopBottom;
        
            // Check if the intersection point is on the top or bottom of the cylinder
            if ((intersectionPoint.y > (vMax.y - 0.001f)) &&
            (intersectionPoint.y < (vMax.y + 0.001f))) {
                normal.y = 1;
                onTopBottom = true;
            }
            else if ((intersectionPoint.y > (vMin.y - 0.001f)) &&
            (intersectionPoint.y < (vMin.y + 0.001f))) {
                normal.y = -1;
                onTopBottom = true;
            }
            else {
                normal.y = 0.0f;
                onTopBottom = false;
            }
        
            if (onTopBottom) {
                if ((intersectionPoint.x < (vMax.x + 0.001f)) &&
                (intersectionPoint.x > (vMax.x - 0.001f))) {
                    normal.x = 1;
                }
                else if ((intersectionPoint.x > (vMin.x - 0.001f)) &&
                (intersectionPoint.x < (vMin.x + 0.001f))) {
                    normal.x = -1;
                }
                else {
                    normal.x = 0;
                }
        
                if ((intersectionPoint.z < (vMax.z + 0.001f) ) &&
                (intersectionPoint.z > (vMax.z - 0.001f))) {
                    normal.z = 1;
                }
                else if ((intersectionPoint.z > (vMin.z - 0.001f)) &&
                (intersectionPoint.z < (vMin.z + 0.001f))) {
                    normal.z = -1;
                }
                else {
                    normal.z = 0;
                }
            }
            else {
                normal.x = intersectionPoint.x;
                normal.z = intersectionPoint.z;
            }
        
            return normal;
        }

    private:
        glm::vec3 vMin = glm::vec3(-1.0f, 0.0f, -1.0f);
        glm::vec3 vMax = glm::vec3(1.0f, 1.0f, 1.0f);
        float radius = 1.0f;
};

#endif