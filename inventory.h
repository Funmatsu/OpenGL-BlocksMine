#pragma once
#include "libraries.h"
bool inv_change;
int slot = 0;
float slotX = 0.0f, slotY = 0.0f;

class Inventory {
public:
	Mesh currInvSlot[4][9];
	Mesh craftInvSlot[2][2], bigCraftInvSlot[3][3];
	Item craftInv[2][2], bigCraftInv[3][3];
	Item inv_slots[4][9];

	bool inf_blocks = false, inventoryOn = false, craftingInventoryOn = false;

    void assignAvailableSlot(Item replaceItem) {
        bool blockBroken = false;
        for (int m = 0; m < 4; m++) {
            for (int l = 0; l < 9; l++) {
                if (inv_slots[3 - m][l] == AIR) {
                    inv_slots[3 - m][l] = replaceItem;
                    inv_change = true;
                    blockBroken = true;
                    break;
                }
            }
            if (blockBroken) {
                break;
            }
        }
    }
};

Inventory inventory;