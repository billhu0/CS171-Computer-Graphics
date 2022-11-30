#ifndef CS171_HW3_INCLUDE_MATERIAL_H_
#define CS171_HW3_INCLUDE_MATERIAL_H_

#include "interaction.h"
#include "texture.h"

class Material {
   public:
    Material() = default;
    virtual ~Material() = default;
    [[nodiscard]] virtual PhongModel evaluate(Interaction &interaction) const = 0;
};

class ConstColorMat : public Material {
   public:
    ConstColorMat();
    explicit ConstColorMat(const Vec3f &color, float sh = 16.f);
    ConstColorMat(Vec3f diff, Vec3f spec, Vec3f ambient, float sh = 16.f);
    [[nodiscard]] PhongModel evaluate(Interaction &interaction) const override;

   private:
    Vec3f diffuse_color;
    Vec3f specular_color;
    Vec3f ambient_color;
    float shininess;
};

class TextureMat : public Material {
    // TODO: Bonus - texture material
   public:
    TextureMat();
    // TextureMat(const std::string &path, Texture::Type type, float sh);
    // TextureMat(const std::string &img_path, const std::string &normal_path, float sh);
    [[nodiscard]] PhongModel evaluate(Interaction &interaction) const override;

    void setImageTexture(const std::string &img_path);
    void setNormalTexture(const std::string &normal_path);
    void setShininess(float sh);

   private:
    Texture img_texture;
    Texture normal_texture;

    float shininess;
};

#endif  // CS171_HW3_INCLUDE_MATERIAL_H_
