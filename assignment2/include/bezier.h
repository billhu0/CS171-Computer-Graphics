#ifndef BEZIER_H_
#define BEZIER_H_
#include "defines.h"
#include "object.h"
#include <vector>

// Bezier Curve

Vertex bezier_curve_evaluate(const std::vector<vec3>& control_points, float t);

class BezierCurve {
   private:
    std::vector<vec3> m_control_points;

   public:
    explicit BezierCurve(int m);
    explicit BezierCurve(const std::vector<vec3>& control_points);

    void setControlPoint(int i, vec3& point);
    static Vertex evaluate(const std::vector<vec3>& control_points, float t);
    [[nodiscard]] Vertex evaluate(float t) const;
    [[nodiscard]] Object generateObject() const;
};

// Bezier Surface

Vertex bezier_surface_evaluate(const std::vector<vec3>& control_points, float u, float v);

class BezierSurface {
   private:
    std::vector<std::vector<vec3>> m_control_points;

   public:
    BezierSurface(int m, int n);
    explicit BezierSurface(const std::vector<std::vector<vec3>>& control_points);

    void setControlPoint(int i, int j, const glm::vec3& point);
    [[nodiscard]] Vertex evaluate(float u, float v) const;
    [[nodiscard]] Object generateObject() const;


    // Adaptive mesh
   private:
    std::vector<std::vector<vec3>> m_sample_points_u;
    std::vector<std::vector<vec3>> m_sample_points_v;
    std::vector<float> m_partition_u;
    std::vector<float> m_partition_v;
    void adaptive_division();
    void get_partition();
    int max_num_points_u(const float l, const float r);
    int max_num_points_v(const float l, const float r);
   
   public:
    [[nodiscard]] Object generateObjectAdaptive();
};

std::vector<BezierSurface> read(const std::string& path);
#endif