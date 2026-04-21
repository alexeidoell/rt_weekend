#pragma once
#include "common.h"
#include "vec3.h"

class perlin {
    public:
        perlin() {
            for (int i = 0; i < point_count; i++) {
                random_floats[i] = random_double();
            }

            perlin_generate_perm(p_x);
            perlin_generate_perm(p_y);
            perlin_generate_perm(p_z);
        }

        float noise(const point3& p) const {
            auto i = int(4*p.x()) & 255;
            auto j = int(4*p.y()) & 255;
            auto k = int(4*p.z()) & 255;

            return random_floats[p_x[i] ^ p_y[j] ^ p_z[k]];
        }
    private:
        static constexpr int point_count = 256;
        float random_floats[point_count];
        int p_x[point_count];
        int p_y[point_count];
        int p_z[point_count];

        static void perlin_generate_perm(int p[point_count]) {
            for (int i = 0; i < point_count; i++) {
                p[i] = i;
            }
            for (int i = point_count - 1; i > 0; i--) {
                int target = random_int(0, i);
                std::swap(p[i], p[target]);
            }
        }




    
};
