#include <glm/glm.hpp>
#include <vector>
#include "uniforms.h"

enum ShaderType {
    GREENE,
    GAS,
    SUN,
    LAND,
    BALL,
    NEON,
    STARS
};

class Model {
    public:
    glm::mat4 modelMatrix;
    std::vector<glm::vec3> vertices;
    Uniforms uniforms;
    ShaderType currentShader;
};