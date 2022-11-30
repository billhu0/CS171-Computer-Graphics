#include "camera.h"

Camera::Camera() : position(0, -1, 0), fov(45), focal_len(1) { 
    lookAt({0, 0, 0}, {0, 0, 1}); 
}

Ray Camera::generateRay(const float dx, const float dy) {
    // TODO: Your code here
    // You need to generate ray according to screen coordinate (dx, dy)

    // Get image width, height and aspect ratio.
    const int width = image->getResolution().x();
    const int height = image->getResolution().y();
    const float aspect_ratio = image->getAspectRatio();  // width / height

    // Get the ratio of dx and dy. The ratio is in [-1, 1].
    // In detail: the leftmost pixel gets ratio_x -1, rightmost pixel gets ratio_y = 1,
    // the remaining pixels are linearly distributed in (-1, 1).
    const float ratio_x = dx * 2 / (float)width - 1;
    const float ratio_y = dy * 2 / (float)height - 1;

    // Consider field_of_view.
    const float half_up = std::tan(radians(fov / 2.0f)) * focal_len;
    const float half_right = half_up * aspect_ratio;

    // Calculate ray direction.
    const Vec3f screen_center = position + forward * focal_len;
    const Vec3f pixel_pos = screen_center + up * half_up * ratio_y + right * half_right * ratio_x;
    const Vec3f dir = (pixel_pos - position).normalized();  // ray direction

    // ray origin is the camera position.
    return Ray{position, dir};
}

void Camera::lookAt(const Vec3f &look_at, const Vec3f &ref_up) {
    // TODO: Your code here
    forward = (look_at - position).normalized();
    right = (forward.cross(ref_up)).normalized();
    up = (right.cross(forward)).normalized();
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

std::vector<Vec2f> Camera::getRaySamples(const float dx, const float dy, const int num_samples) {
    
    // calculate magic angle (26.6)
    const float magic_angle = std::atan(0.5f);
    Mat2f rotator;
    rotator << std::cos(magic_angle), -std::sin(magic_angle),
               std::sin(magic_angle), std::cos(magic_angle);

    // sample
    std::vector<Vec2f> samples;
    for (int i = 0; i < num_samples; i++) {
        for (int j = 0; j < num_samples; j++) {
            float x = ((float)i + 0.5f) / (float)num_samples - 0.5f;
            float y = ((float)j + 0.5f) / (float)num_samples - 0.5f;
            
            samples.push_back(rotator * Vec2f(x, y) + Vec2f(0.5f + dx, 0.5f + dy));
        }
    }
    
    return samples;
}
