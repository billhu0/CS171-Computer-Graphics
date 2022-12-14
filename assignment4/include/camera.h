#ifndef CAMERA_H_
#define CAMERA_H_

#include "config.h"
#include "core.h"
#include "image.h"
#include "ray.h"

class Camera {
   public:
    Camera();
    explicit Camera(const Config::CamConfig &config, std::shared_ptr<ImageRGB> &img);

    Ray generateRay(float x, float y);
    void lookAt(const Vec3f &look_at, const Vec3f &ref_up = {0, 1, 0});

    void setPosition(const Vec3f &pos);
    [[nodiscard]] Vec3f getPosition() const;
    void setFov(float new_fov);
    [[nodiscard]] float getFov() const;
    void setImage(std::shared_ptr<ImageRGB> &img);
    [[nodiscard]] std::shared_ptr<ImageRGB> &getImage();

   private:
    Vec3f position;
    Vec3f forward;
    Vec3f up;
    Vec3f right;
    float focal_len;
    float fov;

    std::shared_ptr<ImageRGB> image;
};

#endif  // CAMERA_H_
