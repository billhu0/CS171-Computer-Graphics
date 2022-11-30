#ifndef CS171_HW3_INCLUDE_CAMERA_H_
#define CS171_HW3_INCLUDE_CAMERA_H_

#include "core.h"
#include "ray.h"
#include "image.h"

class Camera {
   public:
    Camera();

    Ray generateRay(const float dx, const float dy);
    void lookAt(const Vec3f &look_at, const Vec3f &ref_up = {0, 0, 1});

    void setPosition(const Vec3f &pos);
    [[nodiscard]] Vec3f getPosition() const;
    void setFov(float new_fov);
    [[nodiscard]] float getFov() const;
    void setImage(std::shared_ptr<ImageRGB> &img);
    [[nodiscard]] std::shared_ptr<ImageRGB> &getImage();
    static std::vector<Vec2f> getRaySamples(const float x, const float y, const int num_samples);

   private:
    Vec3f position;  // camera's position
    Vec3f forward;
    Vec3f up;
    Vec3f right;
    float fov;
    float focal_len;  // always 1 in this assignment

    std::shared_ptr<ImageRGB> image;
};

#endif  // CS171_HW3_INCLUDE_CAMERA_H_
