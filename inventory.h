#pragma once
#include "libraries.h"
#include "Crosshair.h"
bool inv_change;
int slot = 0, invSize = 300, invYCompensation = invSize / 2;
float slotX = 0.0f, slotY = 0.0f;
extern int invSizeX = 310;
extern int invSizeY = 65;
extern int invHeight = 700;
extern int slotLength = 69;
int offsetX = 600, offsetY = -800;
vec2 hotbarSlotPosition = {-275, -700};

class Inventory {
public:
	Mesh currInvSlot[4][9];
	Mesh craftInvSlot[2][2], bigCraftInvSlot[3][3];
	Item craftInv[2][2], bigCraftInv[3][3];
	Item inv_slots[4][9];

	bool inf_blocks = false, inventoryOn = false, craftingInventoryOn = false;

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

private:
    Mesh inventoryMesh;
    Mesh currInvSlotSelector, craftInvSlotSelector;
    Mesh hotbar, craftingInventory;
    std::vector<glm::vec2> inventoryVertices;
    vector<GLfloat> versInv;
    vector<unsigned int> indsInv;

    std::vector<glm::vec2> inventoryVertices2;
    vector<GLfloat> versInv2;
    vector<unsigned int> indsInv2;

    vector<GLfloat> versCraftInvSlotSelector;
    vector<unsigned int> indsCraftInvSlotSelector;
};

Inventory inventory;

void Inventory::assignAvailableSlot(Item replaceItem) {
    bool blockBroken = false;
    for (int m = 0; m < 4; m++) {
        for (int l = 0; l < 9; l++) {
            if (inv_slots[3 - m][l] == AIR) {
                inv_slots[3 - m][l] = replaceItem;
                inv_change = true;
                return;
            }
        }
    }
}
void Inventory::deassignInvSlot(int x, int y) {
    if (inv_slots[y][x] != AIR) {
        inv_slots[y][x] = AIR;
        currInvSlot[y][x].clearMesh();
        inv_change = true;
    }
}

void Inventory::defineMainInventoryGeometry() {
    //for (int i = 0; i < 48; i++)
    //    versCraftInvSlotSelector.push_back(0.0f);
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

    indsInv = {
        0, 1, 2,
        2, 3, 0
    };

    inventoryMesh.createMesh(versInv, indsInv, versInv.size(), indsInv.size());
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

    indsInv2 = {
        0, 2, 1,
        1, 2, 3
    };
    hotbar.createMesh(versInv2, indsInv2, versInv2.size(), indsInv2.size());
}

void Inventory::drawHotbar() {
    hotbar.renderMesh();
}

void Inventory::defineInvSlotSelectGeometry() {
    versCraftInvSlotSelector = {
        inventoryVertices[0].x + int(slotX) * 62 + 28, inventoryVertices[0].y + 323.0f + 10.0f + 5.0f + int(slotY) * 100 + (((int)slotY == 0) ? 0 : 20), 0.0, 0.0f, 0.0f, 1.0f,     0.0f, 0.0f, 0.0f,      1.0f, 1.0f, 1.0f,
        inventoryVertices[4].x + int(slotX) * 62 + 28, inventoryVertices[4].y + 323.0f + 10.0f + 5.0f + int(slotY) * 100 + (((int)slotY == 0) ? 0 : 20), 0.0, 0.0f, 1.0f, 1.0f,     0.0f, 0.0f, 0.0f,      1.0f, 1.0f, 1.0f,
        inventoryVertices[2].x + int(slotX) * 62 + 28, inventoryVertices[2].y + 323.0f + +5.0f + int(slotY) * 100 + (((int)slotY == 0) ? 0 : 20), 0.0, 1.0f, 0.0f, 1.0f,            0.0f, 0.0f, 0.0f,      1.0f, 1.0f, 1.0f,
        inventoryVertices[5].x + int(slotX) * 62 + 28, inventoryVertices[5].y + 323.0f + +5.0f + int(slotY) * 100 + (((int)slotY == 0) ? 0 : 20), 0.0, 1.0f, 1.0f, 1.0f,            0.0f, 0.0f, 0.0f,      1.0f, 1.0f, 1.0f,
    };

    indsCraftInvSlotSelector = {
        0, 2, 1,
        1, 2, 3
    };

    craftInvSlotSelector.createMesh(versCraftInvSlotSelector, indsCraftInvSlotSelector, versCraftInvSlotSelector.size(), indsCraftInvSlotSelector.size());
}

void Inventory::defineCrafingInvSlotSelectorGeometry() {
    versCraftInvSlotSelector = {
        inventoryVertices[0].x + (int)(slotX) * 62 + 28, inventoryVertices[0].y + 323.0f + 10.0f + 5.0f + round(slotY) * 100 + (((int)slotY == 0) ? 0 : 20), 0.0, 0.0f, 0.0f, 1.0f,      0.0f, 0.0f, 0.0f,       1.0f, 1.0f, 1.0f,
        inventoryVertices[4].x + (int)(slotX) * 62 + 28, inventoryVertices[4].y + 323.0f + 10.0f + 5.0f + round(slotY) * 100 + (((int)slotY == 0) ? 0 : 20), 0.0, 0.0f, 1.0f, 1.0f,      0.0f, 0.0f, 0.0f,       1.0f, 1.0f, 1.0f,
        inventoryVertices[2].x + (int)(slotX) * 62 + 28, inventoryVertices[2].y + 323.0f + +5.0f + round(slotY) * 100 + (((int)slotY == 0) ? 0 : 20), 0.0, 1.0f, 0.0f, 1.0f,      0.0f, 0.0f, 0.0f,       1.0f, 1.0f, 1.0f,
        inventoryVertices[5].x + (int)(slotX) * 62 + 28, inventoryVertices[5].y + 323.0f + +5.0f + round(slotY) * 100 + (((int)slotY == 0) ? 0 : 20), 0.0, 1.0f, 1.0f, 1.0f,      0.0f, 0.0f, 0.0f,       1.0f, 1.0f, 1.0f,
    };
    indsCraftInvSlotSelector = {
        0, 2, 1,
        1, 2, 3
    };

    craftInvSlotSelector.createMesh(versCraftInvSlotSelector, indsCraftInvSlotSelector, versCraftInvSlotSelector.size(), indsCraftInvSlotSelector.size());
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
    vector<unsigned int> indsCraftInvSlotSelector2 = {
        0, 1, 2,
        2, 3, 0
    };

    currInvSlotSelector.createMesh(versCraftInvSlotSelector2, indsCraftInvSlotSelector2, versCraftInvSlotSelector2.size(), indsCraftInvSlotSelector2.size());
}

void Inventory::drawHotbarSlotSelector() {
    currInvSlotSelector.renderMesh();
}