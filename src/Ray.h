#ifndef _RAY_H_
#define _RAY_H_

#include <glm/glm.hpp>

// Represents a 3D ray, containing a starting 3D point and a direction as a 3D vector
class Ray {
public:
    glm::vec3 origin;
    glm::vec3 direction; 

    Ray(const glm::vec3& o, const glm::vec3& d)
        : origin(o), direction(glm::normalize(d)) {}

    glm::vec3 pointAt(float t) const {
        return origin + t * direction;
    }
};

#endif
