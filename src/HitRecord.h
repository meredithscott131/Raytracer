#ifndef HITRECORD_H
#define HITRECORD_H

#include <glm/glm.hpp>
#include "Material.h"

//Stores all information to determine closest object that was hit, along with information about that object
//in order to calculate shading.
class HitRecord {
public:
    //the ray parameter at the intersection
    float t;  
    //the coordinates of the intersection point                      
    glm::vec3 point;
    //surface normal at intersection                
    glm::vec3 normal;    
    //material at intersection          
    util::Material material;        
    bool hit;                       

    //i have no idea if we need to use textures here but if we do we'll uncomment these lines
    // glm::vec2 texCoords;
    // Texture* texture;

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
