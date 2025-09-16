#include "shader.h"

using namespace std;

Shadergl::Shadergl() {
	shaderId = 0;
	uniformModel = 0;
	uniformProjection = 0;
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
Shadergl::~Shadergl() {
    clearShader();
}