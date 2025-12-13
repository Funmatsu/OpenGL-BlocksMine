#include "shader.h"

using namespace std;

Shadergl::Shadergl() {
    shaderId = 0;
    uniformModel = 0;
    uniformProjection = 0;
    pointLightCount = 0;
}

void Shadergl::createShaderFromString(const char* vertexCode, const char* fragmentCode) {
    addShader(vertexCode, fragmentCode);
}

unsigned int Shadergl::compileShader(unsigned int type, const char* source) {
    unsigned int id = glCreateShader(type);
    const char* src = source;  // does same as &source[0] or points to beginning of the string
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        cout << (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment") << " shader says: Failed to compile shader; \n" << message << endl;
        glDeleteShader(id);
    }

    return id;
}

void Shadergl::addShader(const char* vertexCode, const char* fragmentCode) {
    unsigned int program = glCreateProgram();
    unsigned int vertShad = compileShader(GL_VERTEX_SHADER, vertexCode);
    unsigned int fragShad = compileShader(GL_FRAGMENT_SHADER, fragmentCode);

    glAttachShader(program, vertShad);
    glAttachShader(program, fragShad);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vertShad);
    glDeleteShader(fragShad);

    shaderId = program;
    uniformModel = glGetUniformLocation(shaderId, "model");
    uniformView = glGetUniformLocation(shaderId, "view");
    uniformProjection = glGetUniformLocation(shaderId, "projection");
    uniformDirectionalLight.uniformColor = glGetUniformLocation(shaderId, "directionalLight.base.color");
    uniformDirectionalLight.uniformAmbientIntensity = glGetUniformLocation(shaderId, "directionalLight.base.ambientIntensity");
    uniformDirectionalLight.uniformDiffuseIntensity = glGetUniformLocation(shaderId, "directionalLight.base.diffuseIntensity");
    uniformDirectionalLight.uniformDirection = glGetUniformLocation(shaderId, "directionalLight.direction");

    uniformPointLightCount = glGetUniformLocation(shaderId, "pointLightCount");
    for(int i = 0; i < MAX_POINT_LIGHTS; i++){
        char locBuff[200] = { '\0' };
        snprintf(locBuff, sizeof(locBuff), "pointLights[%d].base.color", i);
        uniformPointLight[i].uniformColor = glGetUniformLocation(shaderId, locBuff);

        snprintf(locBuff, sizeof(locBuff), "pointLights[%d].base.ambientintensity", i);
        uniformPointLight[i].uniformAmbientIntensity = glGetUniformLocation(shaderId, locBuff);

        snprintf(locBuff, sizeof(locBuff), "pointLights[%d].base.diffuseIntensity", i);
        uniformPointLight[i].uniformDiffuseIntensity = glGetUniformLocation(shaderId, locBuff);

        snprintf(locBuff, sizeof(locBuff), "pointLights[%d].position", i);
        uniformPointLight[i].uniformPosition = glGetUniformLocation(shaderId, locBuff);

        snprintf(locBuff, sizeof(locBuff), "pointLights[%d].constant", i);
        uniformPointLight[i].uniformConstant = glGetUniformLocation(shaderId, locBuff);

        snprintf(locBuff, sizeof(locBuff), "pointLights[%d].linear", i);
        uniformPointLight[i].uniformLinear = glGetUniformLocation(shaderId, locBuff);

        snprintf(locBuff, sizeof(locBuff), "pointLights[%d].exponent", i);
        uniformPointLight[i].uniformExponent = glGetUniformLocation(shaderId, locBuff);
    }

    uniformTexture = glGetUniformLocation(shaderId, "theTexture");
    uniformDirectionalLightTransform = glGetUniformLocation(shaderId, "directionalLightTransform");
    uniformDirectionalShadowMap = glGetUniformLocation(shaderId, "directionalShadowMap");
    uniformColorMask = glGetUniformLocation(shaderId, "colorMask");
}

string Shadergl::readShaderFiles(const char* fileLocation) {
    string shaderContent, line;
    ifstream ifs(fileLocation);
    if (!ifs.is_open()) {
        cout << "failed to open \"" << fileLocation << "\"" << endl;
    }
    stringstream ss;
    while (getline(ifs, line)) {
        ss << line << "\n";
    }
    shaderContent = ss.str();

    return shaderContent;
}

void Shadergl::createShaderFromFiles(const char* vertexFilePath, const char* fragmentFilePath) {
    string vertexCodeString, fragmentCodeString;
    vertexCodeString = readShaderFiles(vertexFilePath);
    fragmentCodeString = readShaderFiles(fragmentFilePath);
    addShader(vertexCodeString.c_str(), fragmentCodeString.c_str());
}
void Shadergl::useShader(){
    glUseProgram(shaderId);
}

void Shadergl::clearShader() {
    glDeleteProgram(shaderId);
    shaderId = 0;
}

unsigned int Shadergl::getModelLocation() {
    return uniformModel;
}
unsigned int Shadergl::getProjectionLocation() {
    return uniformProjection;
}
unsigned int Shadergl::getViewLocation() {
    return uniformView;
}
unsigned int Shadergl::getAmbientIntensityLocation() {
    return uniformDirectionalLight.uniformAmbientIntensity;
}
unsigned int Shadergl::getAmbientColorLocation() {
    return uniformDirectionalLight.uniformColor;
}
unsigned int Shadergl::getDiffuseIntensityLocation() {
    return uniformDirectionalLight.uniformDiffuseIntensity;
}
unsigned int Shadergl::getDirectionLocation() {
    return uniformDirectionalLight.uniformDirection;
}
unsigned int Shadergl::getColorMaskLocation() {
    return uniformColorMask;
}

void Shadergl::setDirectionalLight(DirectionalLight* dLight) {
    dLight->useLight(uniformDirectionalLight.uniformAmbientIntensity, uniformDirectionalLight.uniformColor, uniformDirectionalLight.uniformDiffuseIntensity, uniformDirectionalLight.uniformDirection);
}

void Shadergl::setPointLights(PointLight* pLight, unsigned int lightCount) {
    if (lightCount > MAX_POINT_LIGHTS) {
        lightCount = MAX_POINT_LIGHTS;
    }
    glUniform1i(uniformPointLightCount, lightCount);
    for (int i = 0; i < lightCount; i++) {
        pLight[i].useLight(uniformPointLight[i].uniformAmbientIntensity, uniformPointLight[i].uniformColor, uniformPointLight[i].uniformDiffuseIntensity
            , uniformPointLight[i].uniformPosition, uniformPointLight[i].uniformConstant, uniformPointLight[i].uniformLinear, uniformPointLight[i].uniformExponent);

    }
}

void Shadergl::setTexture(GLenum texture_unit) {
    glUniform1i(uniformTexture, texture_unit);
}

void Shadergl::setDirectionalShadowMap(GLenum texture_unit) {
    glUniform1i(uniformDirectionalShadowMap, texture_unit);
}

void Shadergl::setDirectionalLightTransform(mat4 lTransform) {
    glUniformMatrix4fv(uniformDirectionalLightTransform, 1, GL_FALSE, glm::value_ptr(lTransform));
    //cout << lTransform[0].x << " " << lTransform[0].y << " " << lTransform[0].z << endl;
    //cout << lTransform[1].x << " " << lTransform[1].y << " " << lTransform[1].z << endl;
    //cout << lTransform[2].x << " " << lTransform[2].y << " " << lTransform[2].z << endl;
    //cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;
}

Shadergl::~Shadergl() {
    clearShader();
}