#pragma once

static const char* vshader = "C:\\Users\\Honla\\Desktop\\OpenGL_Udemy\\shaders/vshader.txt";
static const char* fshader = "C:\\Users\\Honla\\Desktop\\OpenGL_Udemy\\shaders/fshader.txt";

static const char* vshader2 = "C:\\Users\\Honla\\Desktop\\OpenGL_Udemy\\shaders/vshader2.txt";
static const char* fshader2 = "C:\\Users\\Honla\\Desktop\\OpenGL_Udemy\\shaders/fshader2.txt";

static const char* vshaderInv = "C:\\Users\\Honla\\Desktop\\OpenGL_Udemy\\shaders/vshaderInv.txt";
static const char* fshaderInv = "C:\\Users\\Honla\\Desktop\\OpenGL_Udemy\\shaders/fshaderInv.txt";

static const char* vshaderInvBlock = "C:\\Users\\Honla\\Desktop\\OpenGL_Udemy\\shaders/vshaderInvBlock.txt";
static const char* fshaderInvBlock = "C:\\Users\\Honla\\Desktop\\OpenGL_Udemy\\shaders/fshaderInvBlock.txt";

static const char* vshaderInvSlot = "C:\\Users\\Honla\\Desktop\\OpenGL_Udemy\\shaders/vshaderInvSlot.txt";
static const char* fshaderInvSlot = "C:\\Users\\Honla\\Desktop\\OpenGL_Udemy\\shaders/fshaderInvSlot.txt";

static const char* vshaderLooking = "C:\\Users\\Honla\\Desktop\\OpenGL_Udemy\\shaders/vshaderLooking.txt";
static const char* fshaderLooking = "C:\\Users\\Honla\\Desktop\\OpenGL_Udemy\\shaders/fshaderLooking.txt";

static const char* shadowMapvshader = "C:\\Users\\Honla\\Desktop\\OpenGL_Udemy\\shaders/shadowMapvshader.txt";
static const char* shadowMapfshader = "C:\\Users\\Honla\\Desktop\\OpenGL_Udemy\\shaders/shadowMapfshader.txt";

static const char* vshaderProjectile = "C:\\Users\\Honla\\Desktop\\OpenGL_Udemy\\shaders/vshaderProjectile.txt";
static const char* fshaderProjectile = "C:\\Users\\Honla\\Desktop\\OpenGL_Udemy\\shaders/fshaderProjectile.txt";

vector<glShader*> shaders;
vector<glShader*> InventoryShaders;
vector<glShader*> craftInvShaders;
vector<glShader*> bigCraftInvShaders;
glShader* directionalShadowShader;
glShader* projectileShader;

void createShaders() {
    glShader* shader1 = new glShader();
    shader1->createShaderFromFiles(vshader, fshader);
    shaders.push_back(shader1);

    glShader* shader2 = new glShader();
    shader2->createShaderFromFiles(vshader2, fshader2);
    shaders.push_back(shader2);

    glShader* shaderInv = new glShader();
    shaderInv->createShaderFromFiles(vshaderInv, fshaderInv);
    shaders.push_back(shaderInv);

    glShader* shaderInvBlock = new glShader();
    shaderInvBlock->createShaderFromFiles(vshaderInvBlock, fshaderInvBlock);
    shaders.push_back(shaderInvBlock);

    for (int i = 0; i < 9; i++) {
        glShader* shaderInvSlot = new glShader();
        shaderInvSlot->createShaderFromFiles(vshaderInvSlot, fshaderInvSlot);
        shaders.push_back(shaderInvSlot);
    }

    glShader* shaderLooking = new glShader();
    shaderLooking->createShaderFromFiles(vshaderLooking, fshaderLooking);
    shaders.push_back(shaderLooking);

    for (int i = 0; i < 9 * 4; i++) {
        glShader* shaderInvSlot = new glShader();
        shaderInvSlot->createShaderFromFiles(vshaderInvSlot, fshaderInvSlot);
        InventoryShaders.push_back(shaderInvSlot);
    }

    for (int i = 0; i < 4; i++) {
        glShader* shaderInvSlot = new glShader();
        shaderInvSlot->createShaderFromFiles(vshaderInvSlot, fshaderInvSlot);
        craftInvShaders.push_back(shaderInvSlot);
    }

    glShader* craftedBlockShader = new glShader();
    craftedBlockShader->createShaderFromFiles(vshaderInvSlot, fshaderInvSlot);
    craftInvShaders.push_back(craftedBlockShader);

    for (int i = 0; i <= 10; i++) {
        glShader* shaderCraftInvSlot = new glShader();
        shaderCraftInvSlot->createShaderFromFiles(vshaderInvSlot, fshaderInvSlot);
        bigCraftInvShaders.push_back(shaderCraftInvSlot);
    }

    directionalShadowShader = new glShader();
    directionalShadowShader->createShaderFromFiles(shadowMapvshader, shadowMapfshader);

    projectileShader = new glShader();
    projectileShader->createShaderFromFiles(vshaderProjectile, fshaderProjectile);
}