#ifndef CS171_HW3_INCLUDE_GEOMETRY_H_
#define CS171_HW3_INCLUDE_GEOMETRY_H_

#include "core.h"
#include "ray.h"
#include "interaction.h"
#include "material.h"

class Geometry {
   public:
    Geometry() { material = nullptr; }
    virtual ~Geometry() = default;
    virtual bool intersect(Ray &ray, Interaction &interaction) const = 0;
    void setMaterial(const std::shared_ptr<Material> &mat) { material = mat; }

   protected:
    std::shared_ptr<Material> material;
};

class Triangle : public Geometry {
   public:
    Triangle(Vec3f v0, Vec3f v1, Vec3f v2);
    bool intersect(Ray &ray, Interaction &interaction) const override;

   protected:
    Vec3f v0, v1, v2;
    Vec3f normal;
};

class Rectangle : public Geometry {
   public:
    Rectangle(Vec3f position, Vec2f dimension, Vec3f normal, Vec3f tangent);
    bool intersect(Ray &ray, Interaction &interaction) const override;
    [[nodiscard]] Vec2f getSize() const;
    [[nodiscard]] Vec3f getNormal() const;
    [[nodiscard]] Vec3f getTangent() const;
    [[nodiscard]] Vec3f getPos() const;

   protected:
    Vec3f position;
    Vec2f size;
    Vec3f normal;
    Vec3f tangent;
};

class Ellipsoid : public Geometry {
   public:
    Ellipsoid(const Vec3f &p, const Vec3f &a, const Vec3f &b, const Vec3f &c);
    bool intersect(Ray &ray, Interaction &interaction) const override;

   protected:
    Vec3f p;
    Vec3f a;
    Vec3f b;
    Vec3f c;
};

#endif  // CS171_HW3_INCLUDE_GEOMETRY_H_
