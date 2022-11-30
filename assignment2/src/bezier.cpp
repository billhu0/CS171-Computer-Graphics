#include "bezier.h"
#include "utils.h"
#include <vector>
#include <fstream>

static constexpr const int DIM = 50;
static constexpr const float DIM_RECIPROCAL = 1.0f / (float)DIM;

// Bezier Curve

BezierCurve::BezierCurve(int m) { m_control_points.resize(m); }

BezierCurve::BezierCurve(const std::vector<vec3>& control_points) { m_control_points = control_points; }

void BezierCurve::setControlPoint(int i, vec3& point) { m_control_points[i] = point; }

/**
 * Evaluate the point at t given control points.
 * Evaluating points on Bezier curve with De Casteljau's Algorithm
 * @param control_points Given control points.
 * @param t A number between 0 and 1.
 * @return The point position and its tangent.
 */
Vertex bezier_curve_evaluate(const std::vector<vec3>& control_points, float t) {
    glm::vec3 tangent;
    int num_points = (int)control_points.size();   // number of control points
    std::vector<glm::vec3> temp = control_points;  // save inputs (std::vector copy constructor)

    for (int k = 1; k < num_points; k++) {
        for (int j = 0; j < num_points - k; j++) {
            if (k == num_points - 1) {
                tangent = glm::normalize(temp[j + 1] - temp[j]);  // by-product of position evaluation
            }
            temp[j] = (1 - t) * temp[j] + t * temp[j + 1];
        }
    }

    // note that we just store the 'tangent' into the 'normal' space. We don't mean tangent = normal!
    return {temp[0], tangent};
}

/**
 * Evaluate the point at t with the given control_points.
 * Evaluating points on Bezier curve with De Casteljau's Algorithm
 * We need to evaluate 2 things for a point: the point position and the tangent.
 */
Vertex BezierCurve::evaluate(const std::vector<vec3>& control_points, float t) {
    return bezier_curve_evaluate(control_points, t);
}

/**
 * Evaluate the point at t given the curve's control points.
 * @param t A number between 0 and 1.
 * @return The point position and its tangent.
 */
Vertex BezierCurve::evaluate(float t) const { return evaluate(m_control_points, t); }

/**
 * Generate an Object of the current Bezier curve.
 * We can only generate a line, not a surface. 'Normal' is not used.
 */
Object BezierCurve::generateObject() const {
    Object obj;
    for (int i = 0; i < DIM; i++) {
        obj.m_vertices.push_back(evaluate((float)i * DIM_RECIPROCAL));
    }
    return obj;
}

// Bezier Surface

/**
 * Constructor of BezierSurface. 
 * We only resize the control points matrix. Further setControlPoint initialization is needed.
 * @param m Number of control points in u direction.
 * @param n Number of control points in v direction.
 */
BezierSurface::BezierSurface(int m, int n) {
    // Note that m_control_points_m is a two-dimensional vector of vec3 (vector<vector<vec3>>)
    // In this constructor we only need to resize the vectors.
    m_control_points.resize(m);
    for (std::vector<glm::vec3>& sub_vec : m_control_points) {
        sub_vec.resize(n);
    }
}

/**
 * Constructor of BezierSurface. 
 * In this constructor the exact control points matrix is given.
 * So we don't need further setControlPoint initialization.
 */
BezierSurface::BezierSurface(const std::vector<std::vector<vec3>>& control_points) {
    m_control_points = control_points;
}

/**
 * Set the control point at (i, j) to the given point.
 */
void BezierSurface::setControlPoint(int i, int j, const vec3& point) { m_control_points[i][j] = point; }

/**
 * Evaluate the point at (u, v) given control points.
 * Evaluating points on Bezier surface with De Casteljau's Algorithm
 * @param control_points Given control points.
 * @param u A number between 0 and 1.
 * @param v A number between 0 and 1.
 * @return The point position and its tangent.
 */
Vertex bezier_surface_evaluate(const std::vector<std::vector<vec3>>& control_points, float u, float v){
    // get the dimension of the control points
    int m = (int)control_points.size();
    // As a check, if vector is empty, there must be an error (e.g. Read in failed), and we need to abort.
    if (m == 0) {
        std::cerr << "Error: Bezier Surface's control_points are empty (control_points size = 0)!\n";
        std::exit(EXIT_FAILURE);
    }
    int n = (int)control_points[0].size();

    glm::vec3 tangent_u, tangent_v;
    Vertex result_point{};

    // first, construct m intermediate Bezier curves based on the corresponding n control points
    // and evaluate m points at parameter v,
    // then, construct another Bezier curve based on the m points and evaluate the point at u
    std::vector<glm::vec3> temp_u;
    for (int i = 0; i < m; i++) {
        Vertex tmp = bezier_curve_evaluate(control_points[i], u);
        temp_u.push_back(tmp.position);
    }
    for (int i = 0; i < n; i++) {
        Vertex tmp = bezier_curve_evaluate(temp_u, v);
        tangent_v = tmp.normal;
    }

    // in the generated points, construct another curve and evaluate
    std::vector<glm::vec3> temp_v;
    for (int i = 0; i < n; i++) {
        std::vector<glm::vec3> v_control_points;
        for (int j = 0; j < m; j++) {
            v_control_points.push_back(control_points[j][i]);
        }
        Vertex tmp = bezier_curve_evaluate(v_control_points, v);
        temp_v.push_back(tmp.position);
    }
    for (int i = 0; i < m; i++) {
        Vertex tmp = bezier_curve_evaluate(temp_v, u);
        result_point.position = tmp.position;
        tangent_u = tmp.normal;
    }

    // To compute the normal, apply cross product of the tangents along 2 dimensions
    result_point.normal = glm::normalize(glm::cross(tangent_v, tangent_u));
    return result_point;
}

/**
 * Evaluate the point at (u, v) with the given control points
 * Construct the Bezier surface.
 * Given m*n control points to evaluate the point at (u, v),
 * we first construct m intermediate Bezier curves
 */
Vertex BezierSurface::evaluate(float u, float v) const { return bezier_surface_evaluate(m_control_points, u, v); }


/**
 * Generate an Object of the current Bezier surface
 */
Object BezierSurface::generateObject() const {
    Object obj;

    // The object's m_vertices are the point at (u, v) (with given control points)
    for (int u = 0; u <= DIM; u++) {
        for (int v = 0; v <= DIM; v++) {
            obj.m_vertices.push_back(evaluate(DIM_RECIPROCAL * (float)u, DIM_RECIPROCAL * (float)v));
        }
    }

    // indices
    for (int i = 0; i < DIM; i++) {
        for (int j = 0; j < DIM; j++) {
            GLuint point_id = i * (DIM + 1) + j;  // this is the index of the top left corner

            // Triangle 1
            obj.m_indices.push_back(point_id);  // top left corner
            obj.m_indices.push_back(point_id + 1);  // top right corner
            obj.m_indices.push_back(point_id + DIM + 2); // bottom right corner

            // Triangle 2
            obj.m_indices.push_back(point_id); // top left corner
            obj.m_indices.push_back(point_id + DIM + 1); // bottom left corner
            obj.m_indices.push_back(point_id + DIM + 2);  // bottom right corner
        }
    }

    return obj;
}



/**
 * Read in bzs file to generate a vector of Bezier surface
 */
std::vector<BezierSurface> read(const std::string& path) {
    std::ifstream file_stream(path);

    // read in b, p, m, n
    // b: number of bezier surfaces
    // p: number of control points
    // m & n: row and column numbers of Bezier surface's control points
    int b, p, m, n;
    file_stream >> b >> p >> m >> n;

    // read in indices and control points
    std::vector<std::vector<int>> indices;
    indices.resize(b);

    // Read in indices.
    for (int i = 0; i < b; i++) {
        // For each line, there are m*n indices.
        indices[i].resize(m * n);
        for (int j = 0; j < m * n; j++) {
            int index;
            file_stream >> index;
            indices[i][j] = index;
        }
    }

    // Read in control points' positions (coordinates)
    std::vector<glm::vec3> control_points;
    control_points.reserve(p);

    for (int i = 0; i < p; i++) {
        // For each line, there are 3 floats
        float x, y, z;
        file_stream >> x >> y >> z;
        control_points.emplace_back(x, y, z);
    }

    file_stream.close();

    // Read in complete. Then we need to construct surfaces.
    std::vector<BezierSurface> surfaces;
    surfaces.reserve(b);  // There are b surfaces.

    for (int i = 0; i < b; ++i) {
        BezierSurface surface(m, n);
        for (int j = 0; j < m; j++) {
            for (int k = 0; k < n; k++) {
                surface.setControlPoint(j, k, control_points[indices[i][j * n + k]]);
            }
        }
        surfaces.push_back(surface);
    }

    return surfaces;
}