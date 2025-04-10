#ifndef HITRECORD_H
#define HITRECORD_H

#include <glm/glm.hpp>
#include "Material.h"

/*
Stores all information to determine closest object that was hit, along with
information about that object in order to calculate shading.
*/
class HitRecord {
public:
    float t;                        // the ray parameter at the intersection                    
    glm::vec3 point;                // the coordinates of the intersection point                 
    glm::vec3 normal;               // surface normal at intersection              
    util::Material material;        // material at intersection       
    bool hit;                       // whether the ray hit an object                   

    HitRecord() {
        t = std::numeric_limits<float>::max();
        hit = false;
    }

    HitRecord(float _t, const glm::vec3& _point, const glm::vec3& _normal, const util::Material& _material) 
        : t(_t), point(_point), normal(glm::normalize(_normal)), material(_material), hit(true) {}

    void update(float _t, const glm::vec3& _point, const glm::vec3& _normal, const util::Material& _material) {
        t = _t;
        point = _point;
        normal = glm::normalize(_normal);
        material = _material;
        hit = true;
    }
};

#endif
