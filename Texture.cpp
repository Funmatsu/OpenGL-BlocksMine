#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"
#include "Texture.h"

using namespace std;

Texturegl::Texturegl() {
	textureId = 0;
	width = 0;
	height = 0;
	bitDepth = 0;
	fileLocation = ""; 
}

Texturegl::Texturegl(const char* fileLocationString) {
	textureId = 0;
	width = 0;
	height = 0;
	bitDepth = 0;
	fileLocation = fileLocationString;
}
void Texturegl::loadTexture(){
	stbi_set_flip_vertically_on_load(1);

	unsigned char* texData = stbi_load(fileLocation, &width, &height, &bitDepth, 4);
	if (!texData) {
		cout << "Failed to find \"" << fileLocation << "\"" << endl;
		return;
	}
	else {
		cout << "Found \"" << fileLocation << "\"" << endl;
	}

	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(texData);
}
void Texturegl::useTexture(){
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId);
}
void Texturegl::clearTexture(){
	glDeleteTextures(1, &textureId);
	textureId = 0;
	width = 0;
	height = 0;
	bitDepth = 0;
	fileLocation = "";
}

Texturegl::~Texturegl() {
	clearTexture();
}