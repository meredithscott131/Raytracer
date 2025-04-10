#ifndef __CONE_H__
#define __CONE_H__

#include "AbstractRaytraceObject.h"

class Cone : public AbstractRaytraceObject {
    public:
        Cone() {};
        ~Cone() {};

        bool didHit(glm::vec4 s, glm::vec4 v) override {
            float a = pow(v.z,2) + pow(v.x,2) - pow(v.y,2);
            float b = (2 * v.z * s.z) + (2 * v.x * s.x) + (-2 * v.y * s.y) + (2 * v.y);
            float c = pow(s.z,2) + pow(s.x,2) - pow(s.y,2) + (2 * s.y) - 1.0f;
        
            float discriminant = pow(b,2) - (4 * a * c);
            if (discriminant < 0) {
                return false;
            }

            tMin = (-b + sqrt(discriminant)) / (2 * a);
            tMax = (-b - sqrt(discriminant)) / (2 * a);

            float tMinY = s.y + (tMin * v.y);
            float tMaxY = s.y + (tMax * v.y);

            if (tMinY < 0 || tMinY > 1) { 
                tMin = INFINITY;
            }

            if (tMaxY < 0 || tMaxY > 1) { 
                tMax = INFINITY;
            }

            if (tMin > tMax) {
                std::swap(tMin,tMax);
            }

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
        
            bool onBase = false;
            bool onCurve = false;
        
            // Check if the intersection point is on the base or the curve of the cone
            if ((intersectionPoint.y > (0.999f)) &&
            (intersectionPoint.y < (1.001f))) {
                normal.y = 1;
            }
            else if ((intersectionPoint.y > (-0.001f)) &&
            (intersectionPoint.y < (0.001f))) {
                normal.y = -1;
                onBase = true;
            }
            else {
                onCurve = true;
            }
        
            if (onBase) {
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
        
            if (onCurve) {
                float cx = 2 * intersectionPoint.x;
                float cy = 2 - (2 * intersectionPoint.y);
                float cz = 2 * intersectionPoint.z;
                float magnitude = sqrt(pow(cx,2) + pow(cy,2) + pow(cz,2));
                glm::vec3 normalTemp = glm::vec3(cx,cy,cz) * (1.0f / magnitude);
                normal = glm::vec4(normalTemp.x, normalTemp.y, normalTemp.z, 0);
            }
        
            return normal;
        }

        private:
            glm::vec3 vMin = glm::vec3(-1.0f,0.0f,-1.0f);
            glm::vec3 vMax = glm::vec3(1.0f,1.0f,1.0f);
};

#endif