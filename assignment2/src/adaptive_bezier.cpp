#include "bezier.h"
#include "utils.h"
#include <vector>

static constexpr const float tolerance = 0.002f;
static constexpr const int depth_limit = 20;
static constexpr const float sample_interval = 10.0f;

// Given 4 control points, unlike the normal division which uses a fixed interval,
// we adaptively add the halfway points to form a vector of points, stored in the argument 'result'.
void recursive_division(std::vector<vec3>& result, vec3 p1, vec3 p2, vec3 p3, vec3 p4, int limit = 0) {
    // prevent StackOverFlow by recursion limit
    if (limit > depth_limit) return;

    // calculate mid points
    vec3 p12   = (p1 + p2) / 2.0f;
    vec3 p23   = (p2 + p3) / 2.0f;
    vec3 p34   = (p3 + p4) / 2.0f;
    vec3 p123  = (p12 + p23) / 2.0f;
    vec3 p234  = (p23 + p34) / 2.0f;
    vec3 p1234 = (p123 + p234) / 2.0f;

    // try to approximate full curve by a straight line
    float d0 = glm::length(p4 - p1);
    float d1 = glm::length(glm::cross(p2 - p1, p4 - p1)) / d0;
    float d2 = glm::length(glm::cross(p3 - p1, p4 - p1)) / d0;

    // if the curve is flat enough, return the middle point
    if ((d1 + d2) * (d1 + d2) <= tolerance * d0 * d0) {
        result.push_back(p1234);
        return;
    }

    // otherwise, split the curve into two halves and recursively approximate them
    recursive_division(result, p1, p12, p123, p1234, limit + 1);
    recursive_division(result, p1234, p234, p34, p4, limit + 1);
}

// Do adaptive division along each row (u-direction) and each column(v-direction),
// and store the sampled points in m_sample_points_u/v.
void BezierSurface::adaptive_division() {
    // clear vector
    m_sample_points_u.clear();
    m_sample_points_v.clear();

    const int m = (int) m_control_points.size();
    const int n = (int) m_control_points[0].size();

    // do recursive division along u-direction
    for (int i = 0; i < m; i++) {
        vec3 &p1 = m_control_points[i][0],
             &p2 = m_control_points[i][1],
             &p3 = m_control_points[i][2],
             &p4 = m_control_points[i][3];
        std::vector<vec3> tmp_points;
        tmp_points.push_back(p1);  // add the start point
        recursive_division(tmp_points, p1, p2, p3, p4);  // adaptive division and add the halfway points
        tmp_points.push_back(p4);  // add the end point

        m_sample_points_u.push_back(tmp_points);
    }

    // do recursive division along v-direction
    for (int i = 0; i < n; i++) {
        vec3 &p1 = m_control_points[0][i],
             &p2 = m_control_points[1][i],
             &p3 = m_control_points[2][i],
             &p4 = m_control_points[3][i];

        std::vector<vec3> tmp_points;
        tmp_points.push_back(p1);  // add the start point
        recursive_division(tmp_points, p1, p2, p3, p4);  // adaptive division and add the halfway points
        tmp_points.push_back(p4);  // add the end point

        m_sample_points_v.push_back(tmp_points);
    }
}

// Calculate the max number of points along a slice of u-direction.
// We traverse each sampled point and check if it lies in [l, r] by
// calculating the (normal) bezier curve evaluation(l, r) and then get the dot product
// to judge if it lies in the interval.
int BezierSurface::max_num_points_u(const float l, const float r) {
    int max_num_points = 0;  // max number of points in interval [l, r] of all u-direction lines

    for (std::vector<vec3> &line_of_points : m_sample_points_u) {
        int num_points = 0;  // number of points in interval [l, r] in the given u-direction line

        // first we get the 3d coordinate the [l, r] interval corresponds to
        // by normal bezier curve evaluation function
        vec3 l_pos = bezier_curve_evaluate(line_of_points, l).position;
        vec3 r_pos = bezier_curve_evaluate(line_of_points, r).position;

        // then for each point on the line, calculate the relative distance and
        // check if the point belongs to this interval
        for (vec3 &point : line_of_points) {
            double dot_product = glm::dot(point - l_pos, r_pos - l_pos) / glm::length(r_pos - l_pos);
            // 'dot_product >= 0' means it lies in [l, +infinity],
            // 'dot_product < length(r-l)' means it lies in [-infinity, r]
            // only when both satisfied can we guarantee it lies in [l, r]
            if (dot_product >= 0 && dot_product < glm::length(r_pos - l_pos)) {
                num_points++;
            }
        }
        // check if we have found higher num_points
        max_num_points = std::max(max_num_points, num_points);
    }
    return max_num_points;
}

// Calculate the max number of points along a slice of v-direction.
// We traverse each sampled point and check if it lies in [l, r] by
// calculating the (normal) bezier curve evaluation(l, r) and then get the dot product
// to judge if it lies in the interval.
int BezierSurface::max_num_points_v(const float l, const float r) {
    int max_num_points = 0;  // max number of points in interval [l, r] of all v-direction lines

    for (int i = 0; i < (int) m_sample_points_v.size(); i++) {
        // We need to get a collection of all control points along the direction.
        // However the vector<vector<vec3>> control points is not stored in this order,
        // so we have to 'transpose' it.
        std::vector<vec3> v_control_points;
        for (std::vector<vec3> &line_of_points : m_control_points) {
            v_control_points.push_back(line_of_points[i]);
        }

        // The following is similar to what we did in u-direction.
        int num_points = 0;
        vec3 l_pos = bezier_curve_evaluate(v_control_points, l).position;
        vec3 r_pos = bezier_curve_evaluate(v_control_points, r).position;
        for (vec3 &point : m_sample_points_v[i]) {
            // calculate the relative distance
            double dist = glm::dot(point - l_pos, r_pos - l_pos) / glm::length(r_pos - l_pos);
            // 'dot_product >= 0' means it lies in [l, +infinity],
            // 'dot_product < length(r-l)' means it lies in [-infinity, r]
            // only when both satisfied can we guarantee it lies in [l, r]
            if (dist >= 0 && dist < glm::length(r_pos - l_pos)){
                num_points++;
            }
        }
        // check if we found a higher num_points
        max_num_points = std::max(max_num_points, num_points);
    }
    return max_num_points;
}

void BezierSurface::get_partition() {
    // clear vector
    m_partition_u.clear();
    m_partition_v.clear();

    // sample_interval = 10
    for (int i = 0; i < (int) sample_interval; i++) {
        // get the max number of points between interval, for example, [0.2, 0.3]
        int num_samples_u = max_num_points_u((float)i / sample_interval, (float)(i + 1) / sample_interval);

        // add the start point 0.2 to the partition
        m_partition_u.push_back((float)i / sample_interval);

        // Suppose num_samples_u = 25, meaning that we have 25 points in interval [0.2, 0.3],
        // then we would add [0.204, 0.208, 0.212, ..., 0.296] to the partition
        if (num_samples_u > 0) {
            float step = 1.0f / ((float)num_samples_u + 1);
            for (int j = 1; j <= num_samples_u; j++){
                m_partition_u.push_back(((float)i + (float)j * step) / sample_interval);
            }
        }
    }
    
    // the end point
    m_partition_u.push_back(1.0f);

    for (int i = 0; i < (int) sample_interval; i++) {
        // similar to partition_u
        // get the max number of points between interval
        int num_samples_v = max_num_points_v((float) i / sample_interval, (float) (i + 1) / sample_interval);

        // add the start point 0.2 to the partition
        m_partition_v.push_back((float) i / sample_interval);

        // add the halfway points to the partition
        if (num_samples_v > 0) {
            float step = 1.0f / ((float)num_samples_v + 1);
            for (int j = 1; j <= num_samples_v; j++){
                m_partition_v.push_back(((float)i + (float)j * step) / sample_interval);
            }
        }

        // note that we don't need to add the endpoint ([0.3] in the above example) here because
        // the endpoint is the start point of next interval and will be added
    }

    // the end point
    m_partition_v.push_back(1.0f);
}

Object BezierSurface::generateObjectAdaptive() {
    // first we do the adaptive division
    adaptive_division();

    // get the partition along u and v direction
    get_partition();

    // construct the object
    Object obj;

    // evaluate points according to the generated coefficients (partitions)
    for (float i : m_partition_u) {
        for (float j : m_partition_v) {
            obj.m_vertices.push_back(evaluate(i, j));
        }
    }

    // indices
    for (int i = 0; i < (int) m_partition_u.size() - 1; i++){
        for (int j = 0; j < (int) m_partition_v.size() - 1; j++){
            GLuint point_id = i * m_partition_v.size() + j; // this is the index of the top left corner

            // Triangle 1
            obj.m_indices.push_back(point_id); // top left corner
            obj.m_indices.push_back(point_id + 1);  // top right corner
            obj.m_indices.push_back(point_id + m_partition_v.size() + 1);  // bottom right corner

            // Triangle 2
            obj.m_indices.push_back(point_id); // top left corner
            obj.m_indices.push_back(point_id + m_partition_v.size()); // bottom left corner
            obj.m_indices.push_back(point_id + m_partition_v.size() + 1);  // bottom right corner
        }
    }
    return obj;
}
