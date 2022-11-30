#ifndef CS171_HW2_BSPLINE_H
#define CS171_HW2_BSPLINE_H

#include "defines.h"
#include "object.h"
#include <vector>

static constexpr const int DIM = 50;
static constexpr const float DIM_RECIPROCAL = 1.0f / (float)DIM;

Vertex bspline_curve_evaluate(const std::vector<vec3>& control_points, int degree, float t) noexcept {
    // generate knots
    std::vector<float> knots;
    knots.resize(control_points.size() + degree + 1);
    for (int i = 0; i <= degree; i++){
        knots[i] = 0;
        knots[control_points.size() + degree - i] = 1;
    }
    float step = 1.0f / (float) ((int) control_points.size() - degree);
    for (int i = degree + 1; i <= (int) control_points.size() - 1; i++){
        knots[i] = knots[i - 1] + step;
    }

    // find k
    auto find_k = [&knots, &t] () -> int {
        for (int i = 0; i < (int) knots.size() - 1; i++) {
            if ((t == 1.0f && t == knots[i + 1]) || (knots[i] <= t && t < knots[i + 1])) {
                return i;
            }
        }
        return -1;
    };

    int k = find_k();
    if (k == -1) {  // assert that k is found. If not, there must be an error!
        std::cerr << "Bspline curve: cannot find k according to t!\n";
        std::exit(-1);
    }

    // related points is a slice of the original points (control_points[k - degree : k])
    std::vector<vec3> related_points;
    for (int i = k - degree; i <= k; i++) {
        related_points.push_back(control_points[i]);
    }

    vec3 final_normal;

    // core algorithm.
    // See report for formulas and details.
    for (int i = 1; i < degree + 1; i++) {
        for (int j = degree; j > i - 1; j--) {
            float a = (t - knots[k + j - degree]) / (knots[k + j - (i - 1)] - knots[k + j - degree]);
            related_points[j] = (1 - a) * related_points[j - 1] + a * related_points[j];
        }
        if (i == degree - 1) {
            final_normal = glm::normalize(related_points[degree] - related_points[degree - 1]);
        }
    }

    return {related_points[degree], final_normal};
}


class BSplineSurface {
   public:
    std::vector<std::vector<vec3>> m_control_points;
    std::vector<float> m_knots;
    int m_degree;

    // constructor
    BSplineSurface(const std::vector<std::vector<vec3>>& control_points, int degree) noexcept {
        m_degree = degree;
        m_control_points = control_points;
    }

    // evaluate Bspline surface at a given point
    [[nodiscard]] Vertex evaluate(float u, float v) const noexcept {
        int m = (int) m_control_points.size();
        if (m == 0) {  // assert that m is not 0 to avoid Segmentation Fault!
            std::cerr << "Bspline surface: no control points!\n";
            std::exit(-1);
        }
        int n = (int) m_control_points[0].size();

        // for each row (u direction) of control points, evaluate it and add to tmp_u
        std::vector<vec3> tmp_u;
        for (const std::vector<vec3>& ctrl_points: m_control_points) {
            tmp_u.push_back(bspline_curve_evaluate(ctrl_points, m_degree, u).position);
        }
        vec3 tangent_v = bspline_curve_evaluate(tmp_u, m_degree, v).normal;

        // for each column (v direction) of control points, evaluate it.
        // This time we cannot directly traverse the vector<vector<vec3>>,
        // instead, we 'transpose' the matrix and then evaluate.
        std::vector<vec3> tmp_v;
        for (int i = 0; i < n; i++){
            std::vector<vec3> v_control_points;
            for (const std::vector<vec3> & points : m_control_points) {
                v_control_points.push_back(points[i]);
            }
            tmp_v.push_back(bspline_curve_evaluate(v_control_points, m_degree, v).position);
        }

        Vertex tmp = bspline_curve_evaluate(tmp_v, m_degree, u);
        vec3 tangent_u = tmp.normal;

        // The position is self-evident.
        // The normal is the cross product of the tangent at u and v direction.
        return {tmp.position, glm::normalize(glm::cross(tangent_u, tangent_v))};
    }

    // generate object
    [[nodiscard]] Object generateObject() const noexcept {
        Object obj;

        // evaluate the surface at each point
        [this](Object& obj) -> void {
            for (int i = 0; i < DIM + 1; i++) {
                for (int j = 0; j < DIM + 1; j++) {
                    obj.m_vertices.push_back(evaluate((float) i * DIM_RECIPROCAL, (float) j * DIM_RECIPROCAL));
                }
            }
        }(obj);

        // generate mesh indices
        [](Object& obj) -> void {
            for (int i = 0; i < DIM; i++) {
                for (int j = 0; j < DIM; j++) {
                    GLuint point_id = i * (DIM + 1) + j;  // top left corner

                    // Triangle 1
                    obj.m_indices.push_back(point_id);  // top left
                    obj.m_indices.push_back(point_id + 1);  // top right
                    obj.m_indices.push_back(point_id + DIM + 2);  // bottom left

                    // Triangle 2
                    obj.m_indices.push_back(point_id);  // top left
                    obj.m_indices.push_back(point_id + DIM + 1);  // bottom left
                    obj.m_indices.push_back(point_id + DIM + 2);  // bottom right

                }
            }
        }(obj);

        return obj;
    }
};

#endif  // CS171_HW2_BSPLINE_H
