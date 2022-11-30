#ifndef CS171_HW3_INCLUDE_IMAGE_H_
#define CS171_HW3_INCLUDE_IMAGE_H_

#include <vector>
#include <string>

#include "core.h"
#include "utils.h"

class ImageRGB {
   public:
    ImageRGB() = delete;
    ImageRGB(int width, int height);
    [[nodiscard]] float getAspectRatio() const;
    [[nodiscard]] Vec2i getResolution() const;
    void setPixel(int x, int y, const Vec3f &value);
    void writeImgToFile(const std::string &file_name);

   private:
    std::vector<Vec3f> data;
    Vec2i resolution;
};

#endif  // CS171_HW3_INCLUDE_IMAGE_H_
