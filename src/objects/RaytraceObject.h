#ifndef __RAYTRACE_OBJECT_H__
#define __RAYTRACE_OBJECT_H__

#include <glm/glm.hpp>

// Base class for all raytrace objects
class RaytraceObject {
    public:
        RaytraceObject() {};
        virtual ~RaytraceObject() {};

        // Check if the given ray intersects with the object
        virtual bool didHit(glm::vec4 s, glm::vec4 v) = 0;

        // Returns the time of intersection with the object
        virtual float getTime() = 0;

        // Returns the normal vector at the intersection point
        virtual glm::vec4 getNormal(glm::vec4 intersectionPoint) = 0;

    protected:
        float tMin;     // minimum time of intersection
        float tMax;     // maximum time of intersection
};

#endif