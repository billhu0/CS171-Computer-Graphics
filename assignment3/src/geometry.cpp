#include "geometry.h"

#include <utility>

Triangle::Triangle(Vec3f v0, Vec3f v1, Vec3f v2) : v0(std::move(v0)), v1(std::move(v1)), v2(std::move(v2)) {
    normal = (v1 - v0).cross(v2 - v1).normalized();
}

bool Triangle::intersect(Ray &ray, Interaction &interaction) const {
    // TODO: Your code here.

    // variables
    Vec3f e1 = v1 - v0;
    Vec3f e2 = v2 - v0;
    Vec3f s = ray.origin - v0;
    Vec3f s1 = ray.direction.cross(e2);
    Vec3f s2 = s.cross(e1);

    // solution of the equation
    float t = s2.dot(e2) / s1.dot(e1);
    float b1 = s1.dot(s) / s1.dot(e1);
    float b2 = s2.dot(ray.direction) / s1.dot(e1);

    // check whether the solution is legal, in order to determine intersection
    if (t >= ray.t_min && t <= ray.t_max && b1 >= 0 && b2 >= 0 && b1 + b2 <= 1) {
        interaction.dist = t;
        interaction.pos = ray.origin + t * ray.direction;
        interaction.type = Interaction::GEOMETRY;
        interaction.normal = normal;
        if (material != nullptr){
            interaction.model = material->evaluate(interaction);
        }
        return true;
    }

    // Otherwise, no intersection.
    return false;
}

Rectangle::Rectangle(Vec3f position, Vec2f dimension, Vec3f normal, Vec3f tangent)
    : Geometry(),
      position(std::move(position)),
      size(std::move(dimension)),
      normal(std::move(normal)),
      tangent(std::move(tangent)) {


}

bool Rectangle::intersect(Ray &ray, Interaction &interaction) const {
    // TODO: Your code here

    // compute t
    float t = (position - ray.origin).dot(normal) / ray.direction.dot(normal);

    // compute the intersection point
    Vec3f p = ray.origin + t * ray.direction;

    // check whether the intersection point is within t_min and t_max
    if (t >= ray.t_min && t <= ray.t_max) {
        // dot product between P-P0 and tangent
        const float dot1 = (p - position).dot(tangent);
        // dot product between P-P0 and cotangent
        const float dot2 = (p - position).dot(normal.cross(tangent));

        // check whether the intersection point is inside the rectangle
        if (std::abs(dot1) <= size.x() / 2 && std::abs(dot2) <= size.y() / 2) {
            // intersection found
            interaction.dist = t;
            interaction.type = Interaction::GEOMETRY;
            interaction.pos = p;
            interaction.normal = normal;

            // For texture mapping use.
            const float u = std::clamp((dot1 / (size.x() / 2) + 1.0f) / 2.0f, 0.0f, 1.0f);
            const float v = 1 - std::clamp((dot2 / (size.y() / 2) + 1.0f) / 2.0f, 0.0f, 1.0f);
            interaction.uv = Vec2f(u, v);

            interaction.T = tangent;
            interaction.B = normal.cross(tangent);
            interaction.N = normal;

            // For 'rectangle' of the light, the 'material' is nullptr. So we do a nullptr check here.
            if (material != nullptr){
                interaction.model = material->evaluate(interaction);
            }

            return true;
        }
    }

    // Otherwise, no intersection.
    return false;
}
Vec2f Rectangle::getSize() const { return size; }
Vec3f Rectangle::getNormal() const { return normal; }
Vec3f Rectangle::getTangent() const { return tangent; }
Vec3f Rectangle::getPos() const { return position; }

Ellipsoid::Ellipsoid(const Vec3f &p, const Vec3f &a, const Vec3f &b, const Vec3f &c) : p(p), a(a), b(b), c(c) {}

bool Ellipsoid::intersect(Ray &ray, Interaction &interaction) const {
    // TODO: Your code here.

    // Transform the ellipsoid to a sphere.
    // Calculate transformation matrix M and M_inverse.

    Eigen::Matrix4f T;
    T << 1, 0, 0, p.x(),
         0, 1, 0, p.y(),
         0, 0, 1, p.z(),
         0, 0, 0, 1;

    Eigen::Matrix4f R;
    const Vec3f normed_a = a.normalized();
    const Vec3f normed_b = b.normalized();
    const Vec3f normed_c = c.normalized();
    R << normed_a.x(), normed_b.x(), normed_c.x(), 0,
         normed_a.y(), normed_b.y(), normed_c.y(), 0,
         normed_a.z(), normed_b.z(), normed_c.z(), 0,
         0, 0, 0, 1;

    Eigen::Matrix4f S;
    S << a.norm(), 0, 0, 0,
         0, b.norm(), 0, 0,
         0, 0, c.norm(), 0,
         0, 0, 0, 1;

    Eigen::Matrix4f M = T * R * S;
    Eigen::Matrix4f M_inv = M.inverse();

    // By multipling the matrix 'M_inv', we transform the ray corresponding to the ellipsoid
    // to the ray in unit sphere.
    Ray ray_trans{
        (M_inv * Vec4f(ray.origin.x(), ray.origin.y(), ray.origin.z(), 1)).head(3),
        (M_inv * Vec4f(ray.direction.x(), ray.direction.y(), ray.direction.z(), 0)).head(3)
    };  // transformed new ray

    // Do intersect with the transformed ray.

    // variables
    Vec3f o = (M_inv * Vec4f(p.x(), p.y(), p.z(), 1)).head(3);  // new center
    Vec3f w = ray_trans.origin - o;

    // check if discriminant (b^2 - 4ac >= 0)
    const float quad_a = ray_trans.direction.dot(ray_trans.direction);
    const float quad_b = 2 * ray_trans.direction.dot(w);
    const float quad_c = w.dot(w) - 1;
    const float discriminant = quad_b * quad_b - 4 * quad_a * quad_c;

    // If no intersection, return false
    if (discriminant < 0) {
        return false;
    }

    // If intersection happens, compute the intersection point.
    // Get the two solutions of the equation, and take the smaller one.
    float t1 = (float) (-quad_b + std::sqrt(discriminant)) / (2.0f * quad_a);
    float t2 = (float) (-quad_b - std::sqrt(discriminant)) / (2.0f * quad_a);
    float t = std::min(t1, t2);

    // check if the solution is within t_min and t_max
    if (t >= ray_trans.t_min && t <= ray_trans.t_max) {

        // Calculate position and normal in the unit sphere.
        Vec3f pos_sphere = ray_trans.origin + t * ray_trans.direction;
        Vec3f normal_sphere = (pos_sphere - o);

        // Transform the intersection point and normal back, by multiplying matrix M.
        Vec4f pos_ellipsoid = M * Vec4f(pos_sphere.x(), pos_sphere.y(), pos_sphere.z(), 1);
        Vec4f normal_ellipsoid = (M * Vec4f(normal_sphere.x(), normal_sphere.y(), normal_sphere.z(), 0)).normalized();

        // Set the interaction
        interaction.pos = pos_ellipsoid.head(3);
        interaction.normal = normal_ellipsoid.head(3);
        interaction.dist = t;
        interaction.type = Interaction::GEOMETRY;
        if (material != nullptr){
            interaction.model = material->evaluate(interaction);
        }
        return true;
    }

    return false;
}
