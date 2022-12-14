#ifndef CONFIG_H
#define CONFIG_H

#include "core.h"

#include <string>
#include <vector>
#include <map>

enum class MaterialType { DIFFUSE, SPECULAR };

struct Config {
    struct LightConfig {
        float position[3];
        float size[2];
        float radiance[3];
    };

    struct CamConfig {
        float position[3];
        float look_at[3];
        float ref_up[3];
        float vertical_fov;
        float focal_length;
    };

    struct MaterialConfig {
        float color[3];
        MaterialType type;
        std::string name;
    };

    struct ObjConfig {
        std::string obj_file_path;
        std::string material_name;
        float translate[3];
        float scale;
        bool has_bvh;
    };

    //   RenderConfig render_config;
    int spp;
    int max_depth;
    int image_resolution[2];
    CamConfig cam_config;
    LightConfig light_config;
    std::vector<MaterialConfig> materials;
    std::vector<ObjConfig> objects;
};

#endif  // CONFIG_H