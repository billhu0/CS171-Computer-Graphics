#ifndef CS171_HW3_INCLUDE_TEXTURE_H_
#define CS171_HW3_INCLUDE_TEXTURE_H_

#include <vector>
#include <string>
#include <iostream>

#include "core.h"

class Texture {
   public:
    enum Type { DIFF, NORM, DISP [[maybe_unused]], NONE };
    Type type = NONE;
    void loadTextureFromFile(const std::string &path);

    // Constructor
    Texture() = default;
    Texture(const std::string& path, Type tp);

    [[nodiscard]] Vec3f getColor(const Vec2f& uv) const;
    [[nodiscard]] Vec3f getNormal(const Vec2f& uv) const;

   private:
    std::vector<uint8_t> tex_data;
    int height{}, width{};  // texture image's width and height
};

#endif  // CS171_HW3_INCLUDE_TEXTURE_H_
