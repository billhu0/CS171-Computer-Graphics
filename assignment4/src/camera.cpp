#include "camera.h"
#include "utils.h"

Camera::Camera() : position(0, 0, 4), fov(45), focal_len(1) {
    lookAt({0, 0, 0}, {0, 1, 0});
}

Camera::Camera(const Config::CamConfig &config, std::shared_ptr<ImageRGB> &img)
    : position(config.position), fov(config.vertical_fov), focal_len(config.focal_length) {
    image = img;
    lookAt(Vec3f(config.look_at), Vec3f(config.ref_up));
}

Ray Camera::generateRay(float dx, float dy) {
    auto resolution = image->getResolution();
    dx = dx / static_cast<float>(resolution.x()) * 2 - 1;
    dy = dy / static_cast<float>(resolution.y()) * 2 - 1;
    return Ray{position, dx * right + dy * up + forward};
}

void Camera::lookAt(const Vec3f &look_at, const Vec3f &ref_up) {
    forward = (look_at - position).normalized();
    right = forward.cross(ref_up).normalized();
    up = right.cross(forward);
    float half_fov = utils::radians(fov / 2);
    float y_len = tanf(half_fov) * focal_len;
    up *= y_len;
    float xLen = y_len * image->getAspectRatio();
    right *= xLen;
}

void Camera::setPosition(const Vec3f &pos) {
    position = pos;
}

Vec3f Camera::getPosition() const {
    return position;
}

void Camera::setFov(float new_fov) {
    fov = new_fov;
}

float Camera::getFov() const {
    return fov;
}

void Camera::setImage(std::shared_ptr<ImageRGB> &img) {
    image = img;
}
std::shared_ptr<ImageRGB> &Camera::getImage() {
    return image;
}
