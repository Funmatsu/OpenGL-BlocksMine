#pragma once

#include "Crosshair.h"
//#include "World.h"
#include "recipes.h"
#include "normals.h"

int slot = 0, invSize = 300, invYCompensation = invSize / 2;
float slotX = 0.0f, slotY = 0.0f;
extern int invSizeX = 310;
extern int invSizeY = 65;
extern int invHeight = 700;
extern int slotLength = 69;
int offsetX = 600, offsetY = -800;
int xdimens = 5, ydimens = 5;
extern int xdimensItem = 2, ydimensItem = 2;
vec2 hotbarSlotPosition = {-275, -700};

InventorySlot craftedItem, currentBlock;

class Inventory {
public:
    InventorySlot hotbarSlots[9],
                  mainInventorySlots[4][9],
                  mainCraftingSlots[2][2],
                  craftingTableSlots[3][3];
                  

	bool inf_blocks = false, mainInventoryOn = false, craftingTableInventoryOn = false;
    bool invChange() { return inv_change; }
    void invDidChange(bool change) { inv_change = change; }
    void assignAvailableSlot(Item replaceItem);
    void deassignInvSlot(int x, int y);

    void defineHotbarGeometry();
    void drawHotbar();
    void defineMainInventoryGeometry();
    void drawMainInventory();
    void defineInvSlotSelectGeometry();
    void drawInvSlotSelector();
    void defineCrafingInvSlotSelectorGeometry();
    void defineHotbarSlotSelectorGeometry();
    void drawHotbarSlotSelector();
    void defineMainInventorySlotsGeometry();
    void clearMainCraftingSlots();
    void clearCraftingTableSlots();
    void drawMainInventorySlots(mat4 ortho, mat4 itemView, mat4 itemProj);
    void updateCurrentBlock();

    void initInventorySlots();
    void updateInventory();
private:
    Mesh inventoryMesh;
    Mesh currInvSlotSelector, craftInvSlotSelector;
    Mesh hotbar, craftingInventory;
    std::vector<glm::vec2> inventoryVertices;
    vector<GLfloat> versInv;
    vector<unsigned int> indicesInv;

    std::vector<glm::vec2> inventoryVertices2;
    vector<GLfloat> versInv2;
    vector<unsigned int> indicesInv2;
    bool inv_change;
    
    vector<unsigned int> indicesCraftInvSlotSelector;
};

Inventory inventory;

void Inventory::assignAvailableSlot(Item replaceItem) {
    bool blockBroken = false;
    for (int m = 0; m < 4; m++) {
        for (int l = 0; l < 9; l++) {
            if (mainInventorySlots[3 - m][l].item == AIR) {
                if (m == 0) { hotbarSlots[l].item = replaceItem; }
                mainInventorySlots[3 - m][l].item = replaceItem;
                inv_change = true;
                return;
            }
        }
    }
}
void Inventory::deassignInvSlot(int x, int y) {
    if (mainInventorySlots[y][x].item != AIR) {
        mainInventorySlots[y][x].item = AIR;
        mainInventorySlots[y][x].mesh.clearMesh();
        inv_change = true;
    }
}

void Inventory::defineMainInventoryGeometry() {
    inventoryVertices = {
        { centerX - invSizeX, centerY - invSizeY - invHeight}, { centerX + invSizeX, centerY - invSizeY - invHeight},
        { centerX - invSizeX, centerY + invSizeY - invHeight}, { centerX + invSizeX, centerY + invSizeY - invHeight},

        { centerX - invSizeX + slotLength, centerY - invSizeY - invHeight}, { centerX - invSizeX + slotLength, centerY + invSizeY - invHeight}
    };

    versInv = {
        centerX - invSize, centerY - invSize - invYCompensation, -0.1,     0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,
        centerX - invSize, centerY + invSize + invYCompensation, -0.1,     0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,
        centerX + invSize, centerY + invSize + invYCompensation, -0.1,     1.0f, 1.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,
        centerX + invSize, centerY - invSize - invYCompensation, -0.1,     1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,
    };

    indicesInv = {
        0, 1, 2,
        2, 3, 0
    };

    inventoryMesh.createMesh(versInv, indicesInv, versInv.size(), indicesInv.size());
}

void Inventory::drawMainInventory() {
    inventoryMesh.renderMesh();
}

void Inventory::defineHotbarGeometry() {
    inventoryVertices2 = {
        { centerX - invSizeX, centerY - invSizeY - invHeight}, { centerX + invSizeX, centerY - invSizeY - invHeight},
        { centerX - invSizeX, centerY + invSizeY - invHeight}, { centerX + invSizeX, centerY + invSizeY - invHeight},

        { centerX - invSizeX + slotLength, centerY - invSizeY - invHeight}, { centerX - invSizeX + slotLength, centerY + invSizeY - invHeight}
    };
    versInv2 = {
        inventoryVertices2[0].x, inventoryVertices2[0].y, -0.1, 0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 0.0f,  1.0f, 1.0f, 1.0f,
        inventoryVertices2[1].x, inventoryVertices2[1].y, -0.1, 1.0f, 0.0f, 1.0f,   0.0f, 0.0f, 0.0f,  1.0f, 1.0f, 1.0f,
        inventoryVertices2[2].x, inventoryVertices2[2].y, -0.1, 0.0f, 1.0f, 1.0f,   0.0f, 0.0f, 0.0f,  1.0f, 1.0f, 1.0f,
        inventoryVertices2[3].x, inventoryVertices2[3].y, -0.1, 1.0f, 1.0f, 1.0f,   0.0f, 0.0f, 0.0f,  1.0f, 1.0f, 1.0f,
    };

    indicesInv2 = {
        0, 2, 1,
        1, 2, 3
    };
    hotbar.createMesh(versInv2, indicesInv2, versInv2.size(), indicesInv2.size());
}

void Inventory::drawHotbar() {
    hotbar.renderMesh();
}

void Inventory::defineInvSlotSelectGeometry() {
    vector<GLfloat> versCraftInvSlotSelector;
    versCraftInvSlotSelector = {
        inventoryVertices[0].x + int(slotX) * 62 + 28, inventoryVertices[0].y + 323.0f + 10.0f + 5.0f + int(slotY) * 100 + (((int)slotY == 0) ? 0 : 20), 0.0, 0.0f, 0.0f, 1.0f,     0.0f, 0.0f, 0.0f,      1.0f, 1.0f, 1.0f,
        inventoryVertices[4].x + int(slotX) * 62 + 28, inventoryVertices[4].y + 323.0f + 10.0f + 5.0f + int(slotY) * 100 + (((int)slotY == 0) ? 0 : 20), 0.0, 0.0f, 1.0f, 1.0f,     0.0f, 0.0f, 0.0f,      1.0f, 1.0f, 1.0f,
        inventoryVertices[2].x + int(slotX) * 62 + 28, inventoryVertices[2].y + 323.0f + +5.0f + int(slotY) * 100 + (((int)slotY == 0) ? 0 : 20), 0.0, 1.0f, 0.0f, 1.0f,            0.0f, 0.0f, 0.0f,      1.0f, 1.0f, 1.0f,
        inventoryVertices[5].x + int(slotX) * 62 + 28, inventoryVertices[5].y + 323.0f + +5.0f + int(slotY) * 100 + (((int)slotY == 0) ? 0 : 20), 0.0, 1.0f, 1.0f, 1.0f,            0.0f, 0.0f, 0.0f,      1.0f, 1.0f, 1.0f,
    };

    vector<unsigned int> indicesCraftInvSlotSelector;
    indicesCraftInvSlotSelector = {
        0, 2, 1,
        1, 2, 3
    };

    craftInvSlotSelector.createMesh(versCraftInvSlotSelector, indicesCraftInvSlotSelector, versCraftInvSlotSelector.size(), indicesCraftInvSlotSelector.size());
}

void Inventory::defineCrafingInvSlotSelectorGeometry() {
    vector<GLfloat> versCraftInvSlotSelector;
    versCraftInvSlotSelector = {
        inventoryVertices[0].x + (int)(slotX) * 62 + 28, inventoryVertices[0].y + 323.0f + 10.0f + 5.0f + round(slotY) * 100 + (((int)slotY == 0) ? 0 : 20), 0.0, 0.0f, 0.0f, 1.0f,      0.0f, 0.0f, 0.0f,       1.0f, 1.0f, 1.0f,
        inventoryVertices[4].x + (int)(slotX) * 62 + 28, inventoryVertices[4].y + 323.0f + 10.0f + 5.0f + round(slotY) * 100 + (((int)slotY == 0) ? 0 : 20), 0.0, 0.0f, 1.0f, 1.0f,      0.0f, 0.0f, 0.0f,       1.0f, 1.0f, 1.0f,
        inventoryVertices[2].x + (int)(slotX) * 62 + 28, inventoryVertices[2].y + 323.0f + +5.0f + round(slotY) * 100 + (((int)slotY == 0) ? 0 : 20), 0.0, 1.0f, 0.0f, 1.0f,      0.0f, 0.0f, 0.0f,       1.0f, 1.0f, 1.0f,
        inventoryVertices[5].x + (int)(slotX) * 62 + 28, inventoryVertices[5].y + 323.0f + +5.0f + round(slotY) * 100 + (((int)slotY == 0) ? 0 : 20), 0.0, 1.0f, 1.0f, 1.0f,      0.0f, 0.0f, 0.0f,       1.0f, 1.0f, 1.0f,
    };
    indicesCraftInvSlotSelector = {
        0, 2, 1,
        1, 2, 3
    };

    craftInvSlotSelector.createMesh(versCraftInvSlotSelector, indicesCraftInvSlotSelector, versCraftInvSlotSelector.size(), indicesCraftInvSlotSelector.size());
}

void Inventory::drawInvSlotSelector() {
    craftInvSlotSelector.renderMesh();
}

void Inventory::defineHotbarSlotSelectorGeometry() {
    vector<GLfloat> versCraftInvSlotSelector2 = {
        hotbarSlotPosition.x + centerX - 37 + (slot) * 68, hotbarSlotPosition.y + centerY - 68, 0.0,      0.0f, 0.0f, 1.0f,      0.0f, 0.0f, 0.0f,       1.0f, 1.0f, 1.0f,
        hotbarSlotPosition.x + centerX - 37 + (slot) * 68, hotbarSlotPosition.y + centerY + 68, 0.0,      0.0f, 1.0f, 1.0f,      0.0f, 0.0f, 0.0f,       1.0f, 1.0f, 1.0f,
        hotbarSlotPosition.x + centerX + 37 + (slot) * 68, hotbarSlotPosition.y + centerY + 68, 0.0,      1.0f, 1.0f, 1.0f,      0.0f, 0.0f, 0.0f,       1.0f, 1.0f, 1.0f,
        hotbarSlotPosition.x + centerX + 37 + (slot) * 68, hotbarSlotPosition.y + centerY - 68, 0.0,      1.0f, 0.0f, 1.0f,      0.0f, 0.0f, 0.0f,       1.0f, 1.0f, 1.0f,
    };
    vector<unsigned int> indicesCraftInvSlotSelector2 = {
        0, 1, 2,
        2, 3, 0
    };

    currInvSlotSelector.createMesh(versCraftInvSlotSelector2, indicesCraftInvSlotSelector2, versCraftInvSlotSelector2.size(), indicesCraftInvSlotSelector2.size());
}

void Inventory::drawHotbarSlotSelector() {
    currInvSlotSelector.renderMesh();
}

LightMesh createMeshCube(vec3 xyz, float scale, Item blockType) {
    if (blockType == AIR) {
        return LightMesh();
    }
    float xoffset = 0, yoffset = 1, xoffsetTop = 0, yoffsetTop = 0, xoffsetBottom = 0, yoffsetBottom = 0, transparency = 1.0f;

    float yexponent = 1.5f;
    vector<GLfloat> vertices;
    vector<GLfloat> globalUVs;
    vector<unsigned int> indices;
    vector<GLfloat> normals;
    if (!recipe.isTool(blockType)) {
        float UVs[7];
        getUVs(blockType, UVs);
        float xoffset = UVs[0],
            yoffset = UVs[1],
            xoffsetTop = UVs[2],
            yoffsetTop = UVs[3],
            xoffsetBottom = UVs[4],
            yoffsetBottom = UVs[5],
            transparency = UVs[6];

        float clipX = 0.0f, clipY = 1.0f;

        for (int i = 0; i < 6; i++) {
            unsigned base = i * 4;
            indices.push_back(base + 0); indices.push_back(base + 1); indices.push_back(base + 2);
            indices.push_back(base + 2); indices.push_back(base + 3); indices.push_back(base + 0);
        }
        for (int i = 0; i < 6; i++) {
            int offsetX = 0, offsetY = 0;
            if (i == 4) { offsetX = xoffsetBottom; offsetY = yoffsetBottom; }
            else if (i == 5) { offsetX = xoffsetTop;  offsetY = yoffsetTop; }
            if (i % 3 == 0) {
                globalUVs.push_back((clipX + xoffset + offsetX) / xdimens); globalUVs.push_back((clipX + yoffset + offsetY) / ydimens); globalUVs.push_back(transparency);
                globalUVs.push_back((clipX + xoffset + offsetX) / xdimens); globalUVs.push_back((clipY + yoffset + offsetY) / ydimens); globalUVs.push_back(transparency);
                globalUVs.push_back((clipY + xoffset + offsetX) / xdimens); globalUVs.push_back((clipY + yoffset + offsetY) / ydimens); globalUVs.push_back(transparency);
                globalUVs.push_back((clipY + xoffset + offsetX) / xdimens); globalUVs.push_back((clipX + yoffset + offsetY) / ydimens); globalUVs.push_back(transparency);
            }
            else {
                globalUVs.push_back((clipX + xoffset + offsetX) / xdimens); globalUVs.push_back((clipX + yoffset + offsetY) / ydimens); globalUVs.push_back(transparency);
                globalUVs.push_back((clipY + xoffset + offsetX) / xdimens); globalUVs.push_back((clipX + yoffset + offsetY) / ydimens); globalUVs.push_back(transparency);
                globalUVs.push_back((clipY + xoffset + offsetX) / xdimens); globalUVs.push_back((clipY + yoffset + offsetY) / ydimens); globalUVs.push_back(transparency);
                globalUVs.push_back((clipX + xoffset + offsetX) / xdimens); globalUVs.push_back((clipY + yoffset + offsetY) / ydimens); globalUVs.push_back(transparency);
            }
        }

        vertices = {
            -0.5f * scale + xyz.x,  -0.5f * scale * yexponent + xyz.y,  -0.5f * scale + xyz.z,
            -0.5f * scale + xyz.x,   0.5f * scale * yexponent + xyz.y,  -0.5f * scale + xyz.z,
            -0.5f * scale + xyz.x,   0.5f * scale * yexponent + xyz.y,   0.5f * scale + xyz.z,
            -0.5f * scale + xyz.x,  -0.5f * scale * yexponent + xyz.y,   0.5f * scale + xyz.z,
                                                                                     
             0.5f * scale + xyz.x,  -0.5f * scale * yexponent + xyz.y,  -0.5f * scale + xyz.z,
             0.5f * scale + xyz.x,  -0.5f * scale * yexponent + xyz.y,   0.5f * scale + xyz.z,
             0.5f * scale + xyz.x,   0.5f * scale * yexponent + xyz.y,   0.5f * scale + xyz.z,
             0.5f * scale + xyz.x,   0.5f * scale * yexponent + xyz.y,  -0.5f * scale + xyz.z,
                                                                                     
            -0.5f * scale + xyz.x,  -0.5f * scale * yexponent + xyz.y,  -0.5f * scale + xyz.z,
             0.5f * scale + xyz.x,  -0.5f * scale * yexponent + xyz.y,  -0.5f * scale + xyz.z,
             0.5f * scale + xyz.x,   0.5f * scale * yexponent + xyz.y,  -0.5f * scale + xyz.z,
            -0.5f * scale + xyz.x,   0.5f * scale * yexponent + xyz.y,  -0.5f * scale + xyz.z,
                                                                                     
            -0.5f * scale + xyz.x,  -0.5f * scale * yexponent + xyz.y,   0.5f * scale + xyz.z,
            -0.5f * scale + xyz.x,   0.5f * scale * yexponent + xyz.y,   0.5f * scale + xyz.z,
             0.5f * scale + xyz.x,   0.5f * scale * yexponent + xyz.y,   0.5f * scale + xyz.z,
             0.5f * scale + xyz.x,  -0.5f * scale * yexponent + xyz.y,   0.5f * scale + xyz.z,
                                                                                     
            -0.5f * scale + xyz.x,  -0.5f * scale * yexponent + xyz.y,  -0.5f * scale + xyz.z,
             0.5f * scale + xyz.x,  -0.5f * scale * yexponent + xyz.y,  -0.5f * scale + xyz.z,
             0.5f * scale + xyz.x,  -0.5f * scale * yexponent + xyz.y,   0.5f * scale + xyz.z,
            -0.5f * scale + xyz.x,  -0.5f * scale * yexponent + xyz.y,   0.5f * scale + xyz.z,
                                                                                     
            -0.5f * scale + xyz.x,   0.5f * scale * yexponent + xyz.y,  -0.5f * scale + xyz.z,
             0.5f * scale + xyz.x,   0.5f * scale * yexponent + xyz.y,  -0.5f * scale + xyz.z,
             0.5f * scale + xyz.x,   0.5f * scale * yexponent + xyz.y,   0.5f * scale + xyz.z,
            -0.5f * scale + xyz.x,   0.5f * scale * yexponent + xyz.y,   0.5f * scale + xyz.z,
        };

        normals = long_normals;

        if (blockType.isFlat()) {
            float UVs[7];
            getUVs(blockType, UVs);
            float xoffset = UVs[0],
                yoffset = UVs[1],
                xoffsetTop = UVs[2],
                yoffsetTop = UVs[3],
                xoffsetBottom = UVs[4],
                yoffsetBottom = UVs[5],
                transparency = UVs[6];

            indices = {
                0 , 1 , 2 ,
                2 , 3 , 0 ,
                4 , 5 , 6 ,
                6 , 7 , 4
            };

            vertices = {
                -0.5f * scale + xyz.x, -0.5f * scale * yexponent + xyz.y, -0.5f + xyz.z,
                -0.5f * scale + xyz.x,  0.5f * scale * yexponent + xyz.y, -0.5f + xyz.z,
                 0.5f * scale + xyz.x,  0.5f * scale * yexponent + xyz.y, -0.5f + xyz.z,
                 0.5f * scale + xyz.x, -0.5f * scale * yexponent + xyz.y, -0.5f + xyz.z,

                -0.5f * scale + xyz.x, -0.5f * scale * yexponent + xyz.y, -0.5f + xyz.z,
                 0.5f * scale + xyz.x, -0.5f * scale * yexponent + xyz.y, -0.5f + xyz.z,
                 0.5f * scale + xyz.x,  0.5f * scale * yexponent + xyz.y, -0.5f + xyz.z,
                -0.5f * scale + xyz.x,  0.5f * scale * yexponent + xyz.y, -0.5f + xyz.z,
            };

            globalUVs = {
                (clipX + xoffset) / xdimens,   (clipX + yoffset) / ydimens, transparency,
                (clipX + xoffset) / xdimens,   (clipY + yoffset) / ydimens, transparency,
                (clipY + xoffset) / xdimens,   (clipY + yoffset) / ydimens, transparency,
                (clipY + xoffset) / xdimens,   (clipX + yoffset) / ydimens, transparency,

                (clipX + xoffset) / xdimens,   (clipX + yoffset) / ydimens, transparency,
                (clipY + xoffset) / xdimens,   (clipX + yoffset) / ydimens, transparency,
                (clipY + xoffset) / xdimens,   (clipY + yoffset) / ydimens, transparency,
                (clipX + xoffset) / xdimens,   (clipY + yoffset) / ydimens, transparency,
            };

            normals = {
                0.0f, 0.0f,-1.0f,
                0.0f, 0.0f,-1.0f,
                0.0f, 0.0f,-1.0f,
                0.0f, 0.0f,-1.0f,

                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
            };
        }
    }
    else {
        if (blockType == WOODEN_PICKAXE) {
            xoffset = 0, yoffset = 1;
        }

        else if (blockType == STICK) {
            xoffset = 1, yoffset = 1;
        }

        indices = {
            0,  1,  2,
            2,  3,  0
        };
        float clipX = 0.0f, clipY = 1.0f;

        vertices = {
                -0.5f * scale + xyz.x, -0.5f * scale * yexponent + xyz.y, -0.5f + xyz.z,
                -0.5f * scale + xyz.x,  0.5f * scale * yexponent + xyz.y, -0.5f + xyz.z,
                 0.5f * scale + xyz.x,  0.5f * scale * yexponent + xyz.y, -0.5f + xyz.z,
                 0.5f * scale + xyz.x, -0.5f * scale * yexponent + xyz.y, -0.5f + xyz.z,
        };

        globalUVs = {
                (clipX + xoffset) / xdimensItem,   (clipX + yoffset) / ydimensItem, transparency,
                (clipX + xoffset) / xdimensItem,   (clipY + yoffset) / ydimensItem, transparency,
                (clipY + xoffset) / xdimensItem,   (clipY + yoffset) / ydimensItem, transparency,
                (clipY + xoffset) / xdimensItem,   (clipX + yoffset) / ydimensItem, transparency,
        };

        normals = {
                0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f,
        };
    }

    vector<GLfloat> colorMask;
    float tintr = 1.0f, tintg = 1.0f, tintb = 1.0f;
    if (blockType == GRASS) { tintr = 0.2f, tintg = 1.45f, tintb = 0.15f; }
    else if (blockType == GRASS_BLOCK) { tintr = 0.2f, tintg = 1.0f, tintb = 0.15f; }
    else if (blockType == OAK_LEAVES) { tintr = 0.2f, tintg = 1.0f, tintb = 0.2f; }

    for (int i = 0; i < vertices.size() / 3; i++) { colorMask.push_back(tintr); colorMask.push_back(tintg); colorMask.push_back(tintb); } // could be done directly in shader but its ok here as well

    vector<GLfloat> finalvertices;
    for (int i = 0; i < vertices.size(); i += 3) {
        finalvertices.push_back(vertices[i + 0]);
        finalvertices.push_back(vertices[i + 1]);
        finalvertices.push_back(vertices[i + 2]);

        finalvertices.push_back(globalUVs[i + 0]);
        finalvertices.push_back(globalUVs[i + 1]);
        finalvertices.push_back(globalUVs[i + 2]);

        finalvertices.push_back(normals[i + 0]);
        finalvertices.push_back(normals[i + 1]);
        finalvertices.push_back(normals[i + 2]);

        finalvertices.push_back(colorMask[i + 0]);
        finalvertices.push_back(colorMask[i + 1]);
        finalvertices.push_back(colorMask[i + 2]);
    }

    LightMesh cubeMesh;
    cubeMesh.createMesh(finalvertices, indices, finalvertices.size(), indices.size());
    return cubeMesh;
}

LightMesh createMeshQuad(float scale) {
    float yexponent = 1.5f;
    vector<GLfloat> vertices;
    vector<GLfloat> globalUVs;
    vector<unsigned int> indices;
    vector<GLfloat> normals;
    vector<GLfloat> colorMask;

    float clipX = 0.0f, clipY = 1.0f;

    indices = { 0, 1, 2,
                2, 3, 0 };

    globalUVs.push_back(clipX); globalUVs.push_back(clipX); globalUVs.push_back(1);
    globalUVs.push_back(clipY); globalUVs.push_back(clipX); globalUVs.push_back(1);
    globalUVs.push_back(clipY); globalUVs.push_back(clipY); globalUVs.push_back(1);
    globalUVs.push_back(clipX); globalUVs.push_back(clipY); globalUVs.push_back(1);

    vertices = {
        -0.5f * scale,  -0.5f * scale * yexponent,  1.0f,
        -0.5f * scale,   0.5f * scale * yexponent,  1.0f,
         0.5f * scale,   0.5f * scale * yexponent,  1.0f,
         0.5f * scale,  -0.5f * scale * yexponent,  1.0f,
    };

    normals = long_normals;    

    float tintr = 1.0f, tintg = 1.0f, tintb = 1.0f;
    for (int i = 0; i < vertices.size(); i++) { colorMask.push_back(1.0f); }

    vector<GLfloat> finalvertices;
    for (int i = 0; i < vertices.size(); i += 3) {
        finalvertices.push_back(vertices[i + 0]);
        finalvertices.push_back(vertices[i + 1]);
        finalvertices.push_back(vertices[i + 2]);

        finalvertices.push_back(globalUVs[i + 0]);
        finalvertices.push_back(globalUVs[i + 1]);
        finalvertices.push_back(globalUVs[i + 2]);

        finalvertices.push_back(normals[i + 0]);
        finalvertices.push_back(normals[i + 1]);
        finalvertices.push_back(normals[i + 2]);

        finalvertices.push_back(colorMask[i + 0]);
        finalvertices.push_back(colorMask[i + 1]);
        finalvertices.push_back(colorMask[i + 2]);
    }

    LightMesh cubeMesh;
    cubeMesh.createMesh(finalvertices, indices, finalvertices.size(), indices.size());
    return cubeMesh;
}

void Inventory::initInventorySlots() {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 9; j++) {
            int x = 273, x1 = 245, y = 700, y1 = (i == 3) ? 365 : 333;
            mainInventorySlots[i][j].quadMesh = createMeshQuad(50.0f);
            mainInventorySlots[i][j].model    = translate(mat4(1.0f), vec3(float(centerX - x1) + 61.5f * (float)j, centerY - y1 + 93 * (3 - i), 0.0f));
            hotbarSlots[j].quadMesh = createMeshQuad(50.0f);
            hotbarSlots[j].model = translate(mat4(1.0f), vec3(float(centerX - x) + 68.5f * (float)j, centerY - y, 0.0f));
        }
    }

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            int x = 60, y = 125;
            mainCraftingSlots[i][j].quadMesh = createMeshQuad(50.0f);
            mainCraftingSlots[i][j].model = translate(mat4(1.0f), vec3(float(centerX + x) + 61.5f * (float)j, centerY + y + 93 * (2 - i), 0.0f));
        }
    }

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            int x = 60, y = 125;
            craftingTableSlots[i][j].quadMesh = createMeshQuad(50.0f);
            craftingTableSlots[i][j].model = translate(mat4(1.0f), vec3(float(centerX + x) + 61.5f * (float)j, centerY + y + 93 * (2 - i), 0.0f));
        }
    }

    craftedItem.quadMesh = createMeshQuad(50.0f);

    currentBlock.quadMesh = createMeshQuad(700.0f);
    currentBlock.model    = translate(mat4(1.0f), vec3(centerX + 600, centerY - 650, 0));

    initItemTextures(); // creates the frame buffer, color and depth texture
}

void Inventory::updateCurrentBlock() {
    if (currentBlock.item == mainInventorySlots[3][slot].item || mainInventorySlots[3][slot].item == AIR) return;
    else if (mainInventorySlots[3][slot].item != AIR && inv_change) {
        currentBlock.mesh = createMeshCube(vec3(0.0f), 400.0f, mainInventorySlots[3][slot].item);
        currentBlock = mainInventorySlots[3][slot];
    }
    else {
        currentBlock.mesh.clearMesh();
    }
}

void Inventory::updateInventory() {
    if (mainInventorySlots[3][slot].item != AIR || currentBlock.item != mainInventorySlots[3][slot].item) {
        currentBlock.mesh = createMeshCube(vec3(0.0f), 400.0f, hotbarSlots[slot].item);
        currentBlock = hotbarSlots[slot];
    }
    else {
        currentBlock.mesh.clearMesh();
    }

    for (int j = 0; j < (sizeof(mainInventorySlots[0]) / sizeof(InventorySlot)); j++) {
        float itemHeight = 0.0f;
        if (hotbarSlots[j].item == GRASS || hotbarSlots[j].item == POPPY || hotbarSlots[j].item == BLUE_ORCHID) {
            itemHeight = -1.50f;
        }

        if (hotbarSlots[j].item != mainInventorySlots[3][j].item || hotbarSlots[j].item != AIR) {
            hotbarSlots[j].item = mainInventorySlots[3][j].item;
            hotbarSlots[j].mesh = createMeshCube(vec3(0.0f + itemHeight), 35.0f, hotbarSlots[j].item);
        }
    }

    if (mainInventoryOn) {
        for (int i = 0; i < (sizeof(mainCraftingSlots) / sizeof(mainCraftingSlots[0])); i++) {
            for (int j = 0; j < (sizeof(mainCraftingSlots[0]) / sizeof(InventorySlot)); j++) {
                if (mainCraftingSlots[i][j].item != AIR) {
                    mainCraftingSlots[i][j].mesh = createMeshCube(vec3(0.0f), 35.0f, mainCraftingSlots[i][j].item);
                }
            }
        }
        if (recipe.getRecipe(mainCraftingSlots) != AIR) {
            craftedItem.mesh = createMeshCube(vec3(0.0f), 35.0f, recipe.getRecipe(mainCraftingSlots));
            craftedItem.item = recipe.getRecipe(mainCraftingSlots);
            craftedItem.model = translate(mat4(1.0f), vec3(centerX + 250, centerY + 250, 0.0f));
        }
    }
    else if (craftingTableInventoryOn) {
        for (int i = 0; i < (sizeof(craftingTableSlots) / sizeof(craftingTableSlots[0])); i++) {
            for (int j = 0; j < (sizeof(craftingTableSlots[0]) / sizeof(InventorySlot)); j++) {
                if (craftingTableSlots[i][j].item != AIR) {
                    craftingTableSlots[i][j].mesh = createMeshCube(vec3(0.0f), 35.0f, craftingTableSlots[i][j].item);
                }
            }
        }
        if (recipe.getRecipe(craftingTableSlots) != AIR) {
            craftedItem.mesh = createMeshCube(vec3(0.0f), 35.0f, recipe.getRecipe(craftingTableSlots));
            craftedItem.item = recipe.getRecipe(craftingTableSlots);
            craftedItem.model = translate(mat4(1.0f), vec3(centerX + 250, centerY + 250, 0.0f));
        }
    }

    inv_change = false;
}

void Inventory::defineMainInventorySlotsGeometry() {
    for (int i = 0; i < (sizeof(mainInventorySlots) / sizeof(mainInventorySlots[3])); i++) {
        for (int j = 0; j < (sizeof(mainInventorySlots[3]) / sizeof(InventorySlot)); j++) {
            float itemHeight = 0.0f;
            if (!mainInventorySlots[i][j].item.isFlat()) {
                itemHeight = 10.0f;
            }
            if (mainInventorySlots[i][j].item != AIR && inv_change) {
                mainInventorySlots[i][j].mesh = createMeshCube(vec3(0.0f), 35.0f, mainInventorySlots[i][j].item);
            }
        }
    }
}

void Inventory::clearMainCraftingSlots() {
    for (int k = 0; k < sizeof(mainCraftingSlots) / sizeof(mainCraftingSlots[0]); k++) {
        for (int l = 0; l < sizeof(mainCraftingSlots[0]) / sizeof(InventorySlot); l++) {
            mainCraftingSlots[k][l].item = AIR;
            mainCraftingSlots[k][l].mesh.clearMesh();
            invDidChange(1);
        }
    }
}

void Inventory::clearCraftingTableSlots() {
    for (int k = 0; k < sizeof(craftingTableSlots) / sizeof(craftingTableSlots[0]); k++) {
        for (int l = 0; l < sizeof(craftingTableSlots[0]) / sizeof(InventorySlot); l++) {
            if (craftingTableSlots[k][l].item != AIR) {
                craftingTableSlots[k][l].item = AIR;
                craftingTableSlots[k][l].mesh.clearMesh();
                invDidChange(1);;
            }
        }
    }
}

void render3Din2D(mat4 itemModel, LightMesh object3D, mat4 quadModel, LightMesh quad2D, mat4 itemOrtho, mat4 itemView, mat4 itemProj, Item item) {
    glEnable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, itemFbo);
    glViewport(0, 0, 350, 350);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shaders[3]->useShader();
    glUniformMatrix4fv(shaders[3]->getModelLocation(), 1, GL_FALSE, value_ptr(itemModel));//<-
    glUniformMatrix4fv(shaders[3]->getViewLocation(), 1, GL_FALSE, value_ptr(itemView));
    glUniformMatrix4fv(shaders[3]->getProjectionLocation(), 1, GL_FALSE, value_ptr(itemProj));
    shaders[3]->setDirectionalLight(&auxLight);

    if (recipe.isTool(item)) {
        Textures[TOOLS_TEX]->useTexture();
    }
    else {
        Textures[BLOCK_TEX]->useTexture();
    }
    Textures[TOP_TEX]->useNextTexture();
    glUniform1i(glGetUniformLocation(shaders[3]->getShaderId(), "topTexture"), 2);

    object3D.renderMesh();//<-

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, mainWindow.getBufferWidth(), mainWindow.getBufferHeight());
    glDisable(GL_DEPTH_TEST);

    shaders[4]->useShader();
    glUniformMatrix4fv(shaders[4]->getOrthoLocation(), 1, GL_FALSE, glm::value_ptr(itemOrtho));
    glUniformMatrix4fv(shaders[4]->getModelLocation(), 1, GL_FALSE, glm::value_ptr(quadModel));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, itemColorTex);
    glUniform1i(glGetUniformLocation(shaders[4]->getShaderId(), "theTexture"), 0);

    quad2D.renderMesh();//<-
}

void Inventory::drawMainInventorySlots(mat4 ortho, mat4 itemView, mat4 itemProj) {
    for (int i = 0; i < (sizeof(mainInventorySlots) / sizeof(mainInventorySlots[3])); i++) {
        for (int j = 0; j < (sizeof(mainInventorySlots[3]) / sizeof(InventorySlot)); j++) {
            mat4 itemModel = scale(mat4(1.0f), vec3(0.1f, 0.12f, 0.1f)) *
                rotate(mat4(1.0f), radians(-90.0f), vec3(0, 0, 1)) * (
                    (!inventory.mainInventorySlots[i][j].item.isFlat()) ?
                    rotate(mat4(1.0f), radians(40.0f), vec3(1, 0, 0)) *
                    rotate(mat4(1.0f), radians(45.0f), vec3(0, 1, 0)) : mat4(1.0f));
                //rotate(mat4(1.0f), radians(inventory.mainInventorySlots[3][i].angle), vec3(0, 1, 0));
            inventory.mainInventorySlots[3][i].angle += 0.5f;
            render3Din2D(itemModel, inventory.mainInventorySlots[i][j].mesh, inventory.mainInventorySlots[i][j].model, inventory.mainInventorySlots[i][j].quadMesh, ortho, itemView, itemProj, inventory.mainInventorySlots[i][j].item);
        }
    }
}