#include "material.h"

#include <utility>

ConstColorMat::ConstColorMat()
    : diffuse_color({1, 1, 1}), specular_color({1, 1, 1}), ambient_color(1, 1, 1), shininess(16.f) {}

ConstColorMat::ConstColorMat(const Vec3f &color, float sh)
    : diffuse_color(color), specular_color(color), ambient_color(color), shininess(sh) {}

ConstColorMat::ConstColorMat(Vec3f diff, Vec3f spec, Vec3f ambient, float sh)
    : diffuse_color(std::move(diff)),
      specular_color(std::move(spec)),
      ambient_color(std::move(ambient)),
      shininess(sh) {}

PhongModel ConstColorMat::evaluate([[maybe_unused]] Interaction &interaction) const {
    PhongModel m;
    // TODO: Your code here.
    m.diffusion = diffuse_color;
    m.specular = specular_color;
    m.ambient = ambient_color;
    m.shininess = shininess;
    return m;
}

// Texture

TextureMat::TextureMat() : shininess(16.f) {}

void TextureMat::setImageTexture(const std::string &img_path) {
    img_texture = Texture(img_path, Texture::DIFF);
}

void TextureMat::setNormalTexture(const std::string &normal_path) {
    normal_texture = Texture(normal_path, Texture::NORM);
}

void TextureMat::setShininess(float sh) { shininess = sh; }

PhongModel TextureMat::evaluate(Interaction &interaction) const {
    PhongModel m;
    m.shininess = shininess;

    // get the pixel color at texture coordinate (u, v)
    Vec3f color = img_texture.getColor(interaction.uv);
    m.ambient = color;
    m.diffusion = color;
    m.specular = color;

    // if we have a normal texture, modify the normal
    if (normal_texture.type == Texture::NORM) {
        Vec3f normal = normal_texture.getNormal(interaction.uv);
        interaction.normal = Vec3f(
            normal.x() * interaction.T.x() + normal.y() * interaction.B.x() + normal.z() * interaction.N.x(),
            normal.x() * interaction.T.y() + normal.y() * interaction.B.y() + normal.z() * interaction.N.y(),
            normal.x() * interaction.T.z() + normal.y() * interaction.B.z() + normal.z() * interaction.N.z()
        ).normalized();
    }

    return m;
}
