#include <shader.h>
#include <utils.h>
#include <fstream>
#include <sstream>

Shader::Shader(const std::string &vsPath, const std::string &fsPath) { init(vsPath, fsPath); }

void Shader::init(const std::string &vsPath, const std::string &fsPath) {
    initWithCode(getCodeFromFile(vsPath), getCodeFromFile(fsPath));
}

void Shader::initWithCode(const std::string &vs, const std::string &fs) {
    // get the source code (char[]) of VertexShader and FragShader
    const GLchar *vShaderCode = vs.c_str();
    const GLchar *fShaderCode = fs.c_str();

    int success;  // Compilation succeeded flag
    char infoLog[512];  // If compilation fails, hold the log

    // Create Vertex Shader
    GLuint VertexShader = glCreateShader(GL_VERTEX_SHADER); // create shader
    glShaderSource(VertexShader, 1, &vShaderCode, nullptr); // attach shader source code to shader object
    glCompileShader(VertexShader);  // compile shader
    glGetShaderiv(VertexShader, GL_COMPILE_STATUS, &success);  // check if compilation succeeds
    if (!success) {
        glGetShaderInfoLog(VertexShader, 512, nullptr, infoLog);
        std::cerr << "Vertex Shader Compilation Failed\n" << infoLog << std::endl;
    }

    // Compile Fragment Shader
    GLuint FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(FragmentShader, 1, &fShaderCode, nullptr);
    glCompileShader(FragmentShader);
    glGetShaderiv(FragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(FragmentShader, 512, nullptr, infoLog);
        std::cerr << "Fragment Shader Compilation Failed\n" << infoLog << std::endl;
    }

    // Now 2 shaders have been compiled. We need to link them to form a Shader Program Object (着色器程序对象)
    id = glCreateProgram();  // Create Shader Program. 'id' is the id reference to the object.
    glAttachShader(id, VertexShader);
    glAttachShader(id, FragmentShader);  // attach compiled shader to this program object
    glLinkProgram(id);
    glGetProgramiv(id, GL_LINK_STATUS, &success);  // check if compilation succeeds
    if (!success) {
        glGetProgramInfoLog(id, 512, nullptr, infoLog);
        std::cerr << "Program Linking Failed\n" << infoLog << std::endl;
    }

    // Delete linked shaders since we don't need these anymore.
    glDeleteShader(VertexShader);
    glDeleteShader(FragmentShader);
}

std::string Shader::getCodeFromFile(const std::string &path) {
    std::string code;
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        file.open(getPath(path));
        std::stringstream stream;
        stream << file.rdbuf();
        file.close();
        code = stream.str();
    } catch (std::ifstream::failure &e) {
        LOG_ERR("File Error: " + std::string(e.what()));
    }
    return code;
}

void Shader::use() const { glUseProgram(id); }

GLint Shader::getUniformLocation(const std::string &name) const { return glGetUniformLocation(id, name.c_str()); }

void Shader::setInt(const std::string &name, GLint value) const { glUniform1i(getUniformLocation(name), value); }

void Shader::setFloat(const std::string &name, GLfloat value) const { glUniform1f(getUniformLocation(name), value); }

void Shader::setMat3(const std::string &name, const mat3 &value) const {
    glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setMat4(const std::string &name, const mat4 &value) const {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setVec3(const std::string &name, const vec3 &value) const {
    glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::setVec4(const std::string &name, const vec4 &value) const {
    glUniform4fv(getUniformLocation(name), 1, glm::value_ptr(value));
}