#ifndef __BOX_H__
#define __BOX_H__

class Box {
    public:
        Box() {};
        ~Box() {};

        Box (Box &t) {
            vmin = t.vmin;
            vmax = t.vmax;
            originMin = t.originMin;
            originMax = t.originMax;
        }

        // Calculates the intersection times of the ray with the box
        bool calcTimes(glm::vec4 origin, glm::vec4 direction) {
            float t_min_x = (vmin.x - origin.x) / direction.x;
            float t_max_x = (vmax.x - origin.x) / direction.x;

            if (t_min_x > t_max_x) {
                std::swap(t_min_x, t_max_x);
            }

            float t_min_y = (vmin.y - origin.y) / direction.y;
            float t_max_y = (vmax.y - origin.y) / direction.y;

            if (t_min_y > t_max_y) {
                std::swap(t_min_y, t_max_y);
            }

            originMin = (t_min_x > t_min_y) ? t_min_x : t_min_y;
            originMax = (t_min_x < t_max_y) ? t_max_x : t_max_y;

            if ((t_min_x > t_max_y) || (t_min_y > t_max_x)) {
                return false;
            }

            float t_min_z = (vmin.z - origin.z) / direction.z;
            float t_max_z = (vmax.z - origin.z) / direction.z;

            if (t_min_z > t_max_z) {
                std::swap(t_min_z, t_max_z);
            }

            if ((originMin > t_max_z) || (t_min_z > originMax)) {
                return false;
            }

            originMin = (t_min_z > originMin) ? t_min_z : originMin;
            originMax = (t_max_z < originMax) ? t_max_z : originMax;

            return true;
        }

        // Returns the time of intersection with the box
        float getTime() {
            if (originMin > 0 && originMax > 0 && originMin <= originMax) {
                return originMin;
            } else {
                return std::numeric_limits<float>::max();
            }
        }

        // Returns the normal vector at the intersection point
        glm::vec4 getNormal(glm::vec4 intersectionPoint) {
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
        float originMin;
        float originMax;
};

#endif