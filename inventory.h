#pragma once
#include "libraries.h"

class Inventory {
public:
	Mesh currInvSlot[4][9];
	Mesh craftInvSlot[2][2], bigCraftInvSlot[3][3];
	Item craftInv[2][2], bigCraftInv[3][3];
	Item inv_slots[4][9];

	bool inf_blocks = false, inventoryOn = false, craftingInventoryOn = false;
};

Inventory inventory;