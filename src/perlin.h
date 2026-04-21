#pragma once
#include "common.h"
#include "vec3.h"

class perlin { // this algorithm makes no sense
    public:
        perlin() {
            for (int i = 0; i < point_count; i++) {
                rand_vecs[i] = random_unit_vector();
            }

            perlin_generate_perm(p_x);
            perlin_generate_perm(p_y);
            perlin_generate_perm(p_z);
        }

        float noise(const point3& p) const {
            auto u = p.x() - std::floor(p.x());
            auto v = p.y() - std::floor(p.y());
            auto w = p.z() - std::floor(p.z());

            auto i = int(std::floor(p.x()));
            auto j = int(std::floor(p.y()));
            auto k = int(std::floor(p.z()));
            vec3 c[2][2][2];

            for (int di=0; di < 2; di++)
                for (int dj=0; dj < 2; dj++)
                    for (int dk=0; dk < 2; dk++)
                        c[di][dj][dk] = rand_vecs[
                            p_x[(i+di) & 255] ^
                                p_y[(j+dj) & 255] ^
                                p_z[(k+dk) & 255]
                        ];

            return perlin_interp(c, u, v, w);
        }

        float turb(const point3& p, int depth) const {
            float accum = 0.0;
            point3 temp_p = p;
            float weight = 1.0;

            for (int i = 0; i < depth; i++) {
                accum += weight * noise(temp_p);
                weight *= 0.5;
                temp_p *= 2;
            }

            return std::fabsf(accum);
        }

    private:
        static constexpr int point_count = 256;
        vec3 rand_vecs[point_count];
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

        static float perlin_interp(vec3 c[2][2][2], float u, float v, float w) {
            float uu = u*u*(3-2*u);
            float vv = v*v*(3-2*v);
            float ww = w*w*(3-2*w);
            float accum = 0.0;

            for (int i=0; i < 2; i++)
                for (int j=0; j < 2; j++)
                    for (int k=0; k < 2; k++) {
                        vec3 weight_v(u-i, v-j, w-k);
                        accum += (i*uu + (1-i)*(1-uu))
                            * (j*vv + (1-j)*(1-vv))
                            * (k*ww + (1-k)*(1-ww))
                            * dot(c[i][j][k], weight_v);
                    }
            return accum;
        }
};
