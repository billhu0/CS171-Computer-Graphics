#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "texture.h"

void Texture::loadTextureFromFile(const std::string &path) {
    int w, h, ch;
    stbi_uc *img = stbi_load(path.c_str(), &w, &h, &ch, 0);

    // assert the image is successfully loaded
    if (img == nullptr) {
        std::cerr << "Texture image (" << path << ") loading failed! Check the file path!\n";
        std::exit(1);
    }

    // assert image color channel is 3
    if (ch != 3) {
        std::cerr << "Texture image (" << path << ") is not a 3-channel image!\n";
        std::exit(1);
    }

    // The image data is an array of 'unsigned char' (uint_8_t).
    tex_data.assign(img, img + w * h * ch);
    width = w;
    height = h;
    stbi_image_free(img);
}

Texture::Texture(const std::string &path, Texture::Type tp) {
    loadTextureFromFile(path);
    type = tp;
}

Vec3f Texture::getColor(const Vec2f &uv) const {
    int x = (int) (uv.x() * (float) width);
    int y = (int) (uv.y() * (float) height);
    const int pixel_id = (y * width + x) * 3;

    const float R = (float) tex_data[pixel_id] / 255.0f;
    const float G = (float) tex_data[pixel_id + 1] / 255.0f;
    const float B = (float) tex_data[pixel_id + 2] / 255.0f;

    return {R, G, B};
}

Vec3f Texture::getNormal(const Vec2f &uv) const {
    int x = (int) (uv.x() * (float) width);
    int y = (int) (uv.y() * (float) height);
    const int pixel_id = (y * width + x) * 3;

    // The RGB color of the pixel is a number between [0, 1].
    // To convert it to normal, map it to [-1, 1] by (RGB * 2 - 1)
    const float R = ((float) tex_data[pixel_id] / 255.0f) * 2.0f - 1.0f;
    const float G = ((float) tex_data[pixel_id + 1] / 255.0f) * 2.0f - 1.0f;
    const float B = ((float) tex_data[pixel_id + 2] / 255.0f) * 2.0f - 1.0f;

    return {R, G, B};
}
