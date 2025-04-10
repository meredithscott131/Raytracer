#ifndef __BOX_H__
#define __BOX_H__

#include "AbstractRaytraceObject.h"

// Box raytrace object
class Box : public AbstractRaytraceObject {
    public:
        Box() {};
        ~Box() {};

        bool didHit(glm::vec4 s, glm::vec4 v) override {
            // Calculate the minimum and maximum t values for the x dimension
            float txMin = (-0.5 - s.x) / v.x;
            float txMax = (0.5 - s.x) / v.x;
            if (txMin > txMax) {
                std::swap(txMin, txMax);
            }

            // Calculate the minimum and maximum t values for the y dimension
            float tyMin = (-0.5 - s.y) / v.y;
            float tyMax = (0.5 - s.y) / v.y;
            if (tyMin > tyMax) {
                std::swap(tyMin, tyMax);
            }

            // Calculate the minimum and maximum t values for the z dimension
            float tzMin = (-0.5 - s.z) / v.z;
            float tzMax = (0.5 - s.z) / v.z;
            if (tzMin > tzMax) {
                std::swap(tzMin, tzMax);
            }

            // Calculate the minimum and maximum t values for the intersection
            tMin = std::max(txMin, std::max(tyMin, tzMin));
            tMax = std::min(txMax, std::min(tyMax, tzMax));

            if (tMin > tMax) {
                return false;
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
            glm::vec4 normal(0, 0, 0, 0);

            if ((intersectionPoint.x < 0.5001f) && (intersectionPoint.x > 0.4999f)) {
                normal.x = 1.0f;
            } else if ((intersectionPoint.x > -0.5001f) && (intersectionPoint.x < -0.4999f)) {
                normal.x = -1.0f;
            } else {
                normal.x = 0.0f;
            }

            if ((intersectionPoint.y < 0.5001f) && (intersectionPoint.y > 0.4999f)) {
                normal.y = 1.0f;
            } else if ((intersectionPoint.y > -0.5001f) && (intersectionPoint.y < -0.4999f)) {
                normal.y = -1.0f;
            } else {
                normal.y = 0.0f;
            }

            if ((intersectionPoint.z < 0.5001f) && (intersectionPoint.z > 0.4999f)) {
                normal.z = 1.0f;
            } else if ((intersectionPoint.z > -0.5001f) && (intersectionPoint.z < -0.4999f)) {
                normal.z = -1.0f;
            } else {
                normal.z = 0.0f;
            }

            return normal;
        }

    private:
        glm::vec3 vmin = glm::vec3(-0.5f, -0.5f, -0.5f);
        glm::vec3 vmax = glm::vec3(0.5f, 0.5f, 0.5f);
};

#endif