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
    void drawMainInventorySlots(mat4 ortho, mat4 model[4][9]);

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

Mesh createMeshCube(vec3 xyz, float scale, Item blockType) {
    if (blockType == AIR) {
        return Mesh();
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
            globalUVs.push_back((clipX + xoffset + offsetX) / xdimens); globalUVs.push_back((clipX + yoffset + offsetY) / ydimens); globalUVs.push_back(transparency);
            globalUVs.push_back((clipX + xoffset + offsetX) / xdimens); globalUVs.push_back((clipY + yoffset + offsetY) / ydimens); globalUVs.push_back(transparency);
            globalUVs.push_back((clipY + xoffset + offsetX) / xdimens); globalUVs.push_back((clipY + yoffset + offsetY) / ydimens); globalUVs.push_back(transparency);
            globalUVs.push_back((clipY + xoffset + offsetX) / xdimens); globalUVs.push_back((clipX + yoffset + offsetY) / ydimens); globalUVs.push_back(transparency);
        }

        vertices = {
            0.0f * scale + xyz.x,  0.0f * scale * yexponent + xyz.y,  0.0f * scale / 100 + xyz.z,
            0.0f * scale + xyz.x,  1.0f * scale * yexponent + xyz.y,  0.0f * scale / 100 + xyz.z,
            0.0f * scale + xyz.x,  1.0f * scale * yexponent + xyz.y,  1.0f * scale / 100 + xyz.z,
            0.0f * scale + xyz.x,  0.0f * scale * yexponent + xyz.y,  1.0f * scale / 100 + xyz.z,

            1.0f * scale + xyz.x,  0.0f * scale * yexponent + xyz.y,  0.0f * scale / 100 + xyz.z,
            1.0f * scale + xyz.x,  1.0f * scale * yexponent + xyz.y,  0.0f * scale / 100 + xyz.z,
            1.0f * scale + xyz.x,  1.0f * scale * yexponent + xyz.y,  1.0f * scale / 100 + xyz.z,
            1.0f * scale + xyz.x,  0.0f * scale * yexponent + xyz.y,  1.0f * scale / 100 + xyz.z,

            0.0f * scale + xyz.x,  0.0f * scale * yexponent + xyz.y,  0.0f * scale / 100 + xyz.z,
            0.0f * scale + xyz.x,  1.0f * scale * yexponent + xyz.y,  0.0f * scale / 100 + xyz.z,
            1.0f * scale + xyz.x,  1.0f * scale * yexponent + xyz.y,  0.0f * scale / 100 + xyz.z,
            1.0f * scale + xyz.x,  0.0f * scale * yexponent + xyz.y,  0.0f * scale / 100 + xyz.z,

            0.0f * scale + xyz.x,  0.0f * scale * yexponent + xyz.y,  1.0f * scale / 100 + xyz.z,
            0.0f * scale + xyz.x,  1.0f * scale * yexponent + xyz.y,  1.0f * scale / 100 + xyz.z,
            1.0f * scale + xyz.x,  1.0f * scale * yexponent + xyz.y,  1.0f * scale / 100 + xyz.z,
            1.0f * scale + xyz.x,  0.0f * scale * yexponent + xyz.y,  1.0f * scale / 100 + xyz.z,

            0.0f * scale + xyz.x,  0.0f * scale * yexponent + xyz.y,  0.0f * scale / 100 + xyz.z,
            1.0f * scale + xyz.x,  0.0f * scale * yexponent + xyz.y,  0.0f * scale / 100 + xyz.z,
            1.0f * scale + xyz.x,  0.0f * scale * yexponent + xyz.y,  1.0f * scale / 100 + xyz.z,
            0.0f * scale + xyz.x,  0.0f * scale * yexponent + xyz.y,  1.0f * scale / 100 + xyz.z,

            0.0f * scale + xyz.x,  1.0f * scale * yexponent + xyz.y,  0.0f * scale / 100 + xyz.z,
            1.0f * scale + xyz.x,  1.0f * scale * yexponent + xyz.y,  0.0f * scale / 100 + xyz.z,
            1.0f * scale + xyz.x,  1.0f * scale * yexponent + xyz.y,  1.0f * scale / 100 + xyz.z,
            0.0f * scale + xyz.x,  1.0f * scale * yexponent + xyz.y,  1.0f * scale / 100 + xyz.z,
        };

        normals = long_normals;

        if (blockType.isFlat) {
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
                2 , 3 , 0
            };

            vertices = {
                0.0f * scale + xyz.x, 0.0f * scale * yexponent + xyz.y, 0.0f + xyz.z,
                0.0f * scale + xyz.x,  1.0f * scale * yexponent + xyz.y, 0.0f + xyz.z,
                1.0f * scale + xyz.x,   1.0f * scale * yexponent + xyz.y, 0.0f + xyz.z,
                1.0f * scale + xyz.x,  0.0f * scale * yexponent + xyz.y, 0.0f + xyz.z,
            };

            globalUVs = {
                (clipX + xoffset) / xdimens,   (clipX + yoffset) / ydimens, transparency,
                (clipX + xoffset) / xdimens,   (clipY + yoffset) / ydimens, transparency,
                (clipY + xoffset) / xdimens,   (clipY + yoffset) / ydimens, transparency,
                (clipY + xoffset) / xdimens,   (clipX + yoffset) / ydimens, transparency,
            };

            normals = {
                0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f,
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
                0.0f * scale + xyz.x, 0.0f * scale * yexponent + xyz.y, 0.0f + xyz.z,
                0.0f * scale + xyz.x, 1.0f * scale * yexponent + xyz.y, 0.0f + xyz.z,
                1.0f * scale + xyz.x, 1.0f * scale * yexponent + xyz.y, 0.0f + xyz.z,
                1.0f * scale + xyz.x, 0.0f * scale * yexponent + xyz.y, 0.0f + xyz.z,
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
    else if (blockType == OAK_LEAVES) { tintr = 0.2f, tintg = 1.0f, tintb = 0.2f; }

    for (int i = 0; i < vertices.size() / 3; i++) { colorMask.push_back(tintr); colorMask.push_back(tintg); colorMask.push_back(tintb); }

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

    Mesh cubeMesh;
    cubeMesh.createMesh(finalvertices, indices, finalvertices.size(), indices.size());
    return cubeMesh;
}

void Inventory::updateInventory() {
    if (mainInventorySlots[3][slot].item != AIR) {
        currentBlock.mesh = createMeshCube(vec3(centerX + offsetX, centerY + offsetY, 0.0f), 400.0f, mainInventorySlots[3][slot].item);
        currentBlock = mainInventorySlots[3][slot];
    }
    else {
        currentBlock.mesh.clearMesh();
    }

    for (int j = 0; j < (sizeof(mainInventorySlots[0]) / sizeof(InventorySlot)); j++) {
        float itemHeight = 0.0f;
        if (mainInventorySlots[3][j].item == GRASS || mainInventorySlots[3][j].item == POPPY || mainInventorySlots[3][j].item == BLUE_ORCHID) {
            itemHeight = -15.0f;
        }
        if (mainInventorySlots[3][j].mesh.vertices.size() == 0 && mainInventorySlots[3][j].item != AIR) {
            mainInventorySlots[3][j].mesh = createMeshCube(vec3(centerX / 5, itemHeight, 0.0f), 35.0f, mainInventorySlots[3][j].item);
        }
    }

    if (mainInventoryOn) {
        for (int i = 0; i < (sizeof(mainCraftingSlots) / sizeof(mainCraftingSlots[0])); i++) {
            for (int j = 0; j < (sizeof(mainCraftingSlots[0]) / sizeof(InventorySlot)); j++) {
                if (mainCraftingSlots[i][j].mesh.vertices.size() == 0 && mainCraftingSlots[i][j].item != AIR) {
                    mainCraftingSlots[i][j].mesh = createMeshCube(vec3(centerX / 5 + 270, centerY / 4 + 110, 0.0f), 35.0f, mainCraftingSlots[i][j].item);
                }
            }
        }
        if (recipe.getRecipe(mainCraftingSlots) != AIR) {
            craftedItem.mesh = createMeshCube(vec3(centerX / 5 + 315, centerY / 4 + 200.0f, 0.0f), 35.0f, recipe.getRecipe(mainCraftingSlots));
        }
        craftedItem.item = recipe.getRecipe(mainCraftingSlots);
    }
    else if (craftingTableInventoryOn) {
        for (int i = 0; i < (sizeof(craftingTableSlots) / sizeof(craftingTableSlots[0])); i++) {
            for (int j = 0; j < (sizeof(craftingTableSlots[0]) / sizeof(InventorySlot)); j++) {
                if (craftingTableSlots[i][j].mesh.vertices.size() == 0 && craftingTableSlots[i][j].item != AIR) {
                    craftingTableSlots[i][j].mesh = createMeshCube(vec3(centerX / 5 + 270, centerY / 4 + 110, 0.0f), 35.0f, craftingTableSlots[i][j].item);
                }
            }
        }
        if (recipe.getRecipe(craftingTableSlots) != AIR) {
            craftedItem.mesh = createMeshCube(vec3(centerX / 5 + 315, centerY / 4 + 200.0f, 0.0f), 35.0f, recipe.getRecipe(craftingTableSlots));
        }
        craftedItem.item = recipe.getRecipe(craftingTableSlots);

    }

    inv_change = false;
}

void Inventory::defineMainInventorySlotsGeometry() {
    for (int i = 0; i < (sizeof(mainInventorySlots) / sizeof(mainInventorySlots[3]) - 1); i++) {
        for (int j = 0; j < (sizeof(mainInventorySlots[3]) / sizeof(InventorySlot)); j++) {
            float itemHeight = 0.0f;
            if (!mainInventorySlots[i][j].item.isFlat) {
                itemHeight = 10.0f;
            }
            if (mainInventorySlots[i][j].mesh.vertices.size() == 0 && mainInventorySlots[i][j].item != AIR) {
                mainInventorySlots[i][j].mesh = createMeshCube(vec3(centerX / 5, (3 - i) * 90 + 20 + itemHeight, 0.0f), 35.0f, mainInventorySlots[i][j].item);
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

void Inventory::drawMainInventorySlots(mat4 ortho, mat4 model[4][9]) {
    for (int i = 0; i < (sizeof(inventory.mainInventorySlots) / sizeof(inventory.mainInventorySlots[3])); i++) {
        for (int j = 0; j < (sizeof(inventory.mainInventorySlots[3]) / sizeof(InventorySlot)); j++) {
            Textures[BLOCK_TEX]->useTexture();
            if (recipe.isTool(inventory.mainInventorySlots[i][j].item)) {
                Textures[TOOLS_TEX]->useTexture();
            }
            InventoryShaders[9 * i + j]->useShader();
            glUniformMatrix4fv(InventoryShaders[9 * i + j]->getOrthoLocation(), 1, GL_FALSE, value_ptr(ortho));
            glUniformMatrix4fv(InventoryShaders[9 * i + j]->getModelLocation(), 1, GL_FALSE, value_ptr(model[i][j]));

            inventory.mainInventorySlots[i][j].mesh.renderMesh();
            inventory.invDidChange(1);
        }
    }
}