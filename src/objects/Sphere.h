#ifndef __SPHERE_H__
#define __SPHERE_H__

class Sphere {
    public:
        Sphere() {};
        ~Sphere() {};

        Sphere (Sphere &t) {
            tmin = t.tmin;
            tmax = t.tmax;
            radius = t.radius;
        }

        // Calculates the intersection times of the ray with the sphere
        bool calcTimes(glm::vec4 origin, glm::vec4 direction) {
            float a = pow(direction.x, 2) + pow(direction.y, 2) + pow(direction.z, 2);
            float b = 2 * (origin.x * direction.x + origin.y * direction.y + origin.z * direction.z);
            float c = pow(origin.x, 2) + pow(origin.y, 2) + pow(origin.z, 2) - pow(radius, 2);

            float discriminant = pow(b, 2) - 4 * a * c;
            if (discriminant < 0) {
                return false;
            }

            tmin = (-b + sqrt(discriminant)) / (2 * a);
            tmax = (-b - sqrt(discriminant)) / (2 * a);

            if (tmin > tmax) {
                std::swap(tmin, tmax);
            }

            return true;
        }

        // Returns the time of intersection with the sphere
        float getTime() {
            if (tmin > 0 && tmax > 0 && tmin <= tmax) {
                return tmin;
            } else if (tmin < 0 && tmax > 0) {
                return tmax;
            } else {
                return std::numeric_limits<float>::max();
            }
        }

        // Returns the normal vector at the intersection point
        glm::vec4 getNormal(glm::vec4 intersectionPoint) {
            glm::vec4 normal = glm::vec4(intersectionPoint.x, intersectionPoint.y, intersectionPoint.z, 0.0f);
            return normal;
        }

    private:
        float tmin;
        float tmax;
        float radius = 1.0f;
};

#endif