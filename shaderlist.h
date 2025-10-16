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

vector<Shadergl*> shaders;
vector<Shadergl*> InventoryShaders;
vector<Shadergl*> craftInvShaders;
vector<Shadergl*> bigCraftInvShaders;

void createShaders() {
    Shadergl* shader1 = new Shadergl();
    shader1->createShaderFromFiles(vshader, fshader);
    shaders.push_back(shader1);

    Shadergl* shader2 = new Shadergl();
    shader2->createShaderFromFiles(vshader2, fshader2);
    shaders.push_back(shader2);

    Shadergl* shaderInv = new Shadergl();
    shaderInv->createShaderFromFiles(vshaderInv, fshaderInv);
    shaders.push_back(shaderInv);

    Shadergl* shaderInvBlock = new Shadergl();
    shaderInvBlock->createShaderFromFiles(vshaderInvBlock, fshaderInvBlock);
    shaders.push_back(shaderInvBlock);

    for (int i = 0; i < 9; i++) {
        Shadergl* shaderInvSlot = new Shadergl();
        shaderInvSlot->createShaderFromFiles(vshaderInvSlot, fshaderInvSlot);
        shaders.push_back(shaderInvSlot);
    }

    Shadergl* shaderLooking = new Shadergl();
    shaderLooking->createShaderFromFiles(vshaderLooking, fshaderLooking);
    shaders.push_back(shaderLooking);

    for (int i = 0; i < 9 * 4; i++) {
        Shadergl* shaderInvSlot = new Shadergl();
        shaderInvSlot->createShaderFromFiles(vshaderInvSlot, fshaderInvSlot);
        InventoryShaders.push_back(shaderInvSlot);
    }

    for (int i = 0; i < 4; i++) {
        Shadergl* shaderInvSlot = new Shadergl();
        shaderInvSlot->createShaderFromFiles(vshaderInvSlot, fshaderInvSlot);
        craftInvShaders.push_back(shaderInvSlot);
    }

    Shadergl* craftedBlockShader = new Shadergl();
    craftedBlockShader->createShaderFromFiles(vshaderInvSlot, fshaderInvSlot);
    craftInvShaders.push_back(craftedBlockShader);

    for (int i = 0; i <= 10; i++) {
        Shadergl* shaderCraftInvSlot = new Shadergl();
        shaderCraftInvSlot->createShaderFromFiles(vshaderInvSlot, fshaderInvSlot);
        bigCraftInvShaders.push_back(shaderCraftInvSlot);
    }
}