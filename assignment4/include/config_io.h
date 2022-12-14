#ifndef CONFIG_IO_H_
#define CONFIG_IO_H_

#define JSON_USE_IMPLICIT_CONVERSIONS 0

#include "config.h"
#include <nlohmann/json.hpp>

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Config::LightConfig, position, size, radiance)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Config::CamConfig, position, look_at, ref_up, vertical_fov, focal_length)

// add your own bsdf name if needed
NLOHMANN_JSON_SERIALIZE_ENUM(MaterialType, {{MaterialType::DIFFUSE, "diffuse"}, {MaterialType::SPECULAR, "specular"}})

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Config::MaterialConfig, color, type, name)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Config::ObjConfig, obj_file_path, material_name, translate, scale, has_bvh)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Config, spp, max_depth, image_resolution, cam_config, light_config, materials, objects)

#endif  // CONFIG_IO_H_