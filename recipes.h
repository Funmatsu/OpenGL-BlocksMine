#pragma once
#include "libraries.h"
#include "InventorySlot.h"


class Recipe {

private:
	vector<Item> recipes = {

							OAK_WOOD, AIR, AIR, AIR, OAK_PLANK,
							AIR, OAK_WOOD, AIR, AIR, OAK_PLANK,
							AIR, AIR, OAK_WOOD, AIR, OAK_PLANK,
							AIR, AIR, AIR, OAK_WOOD, OAK_PLANK,
							STONE_BLOCK, IRON_ORE, AIR, AIR, WOODEN_PICKAXE,
							OAK_PLANK, OAK_PLANK, STICK, AIR, WOODEN_PICKAXE,
							OAK_PLANK, AIR, OAK_PLANK, AIR, STICK,
							OAK_PLANK, OAK_PLANK, OAK_PLANK, OAK_PLANK, CRAFTING_TABLE

	};

	vector<Item> bigrecipes = { OAK_WOOD, AIR, AIR, AIR, AIR, AIR, AIR, AIR, AIR, OAK_PLANK,
							   OAK_PLANK, OAK_PLANK, OAK_PLANK, AIR, STICK, AIR, AIR, STICK, AIR, WOODEN_PICKAXE };


public:
	Item getRecipe(InventorySlot slots[3][3]);

	Item getRecipe(InventorySlot slots[2][2]);

	bool itemPlaceable(Item itemType);

	bool itemUsable(Item itemType);

	bool isTool(Item itemType);

	bool isBreakable(Item itemType);
};

Recipe recipe;

Item Recipe::getRecipe(InventorySlot slots[3][3]) {
	for (int i = 0; i < bigrecipes.size(); i += 10) {
		if (slots[0][0].item == bigrecipes[i + 0] && slots[0][1].item == bigrecipes[i + 1] && slots[0][2].item == bigrecipes[i + 2] &&
			slots[1][0].item == bigrecipes[i + 3] && slots[1][1].item == bigrecipes[i + 4] && slots[1][2].item == bigrecipes[i + 5] &&
			slots[2][0].item == bigrecipes[i + 6] && slots[2][1].item == bigrecipes[i + 7] && slots[2][2].item == bigrecipes[i + 8]) {
			return bigrecipes[i + 9];
		}
	}
	return AIR;
}

Item Recipe::getRecipe(InventorySlot slots[2][2]) {
	for (int i = 0; i < recipes.size(); i += 5) {
		if (slots[0][0].item == recipes[i + 0] && slots[0][1].item == recipes[i + 1] &&
			slots[1][0].item == recipes[i + 2] && slots[1][1].item == recipes[i + 3]
			) {
			return recipes[i + 4];
		}
	}
	return AIR;
}

//Item Recipe::getRecipe(Item slots[3][3]) {
//	for (int i = 0; i < bigrecipes.size(); i += 10) {
//		if (slots[0][0] == bigrecipes[i + 0] && slots[0][1] == bigrecipes[i + 1] && slots[0][2] == bigrecipes[i + 2] &&
//			slots[1][0] == bigrecipes[i + 3] && slots[1][1] == bigrecipes[i + 4] && slots[1][2] == bigrecipes[i + 5] &&
//			slots[2][0] == bigrecipes[i + 6] && slots[2][1] == bigrecipes[i + 7] && slots[2][2] == bigrecipes[i + 8]) {
//			return bigrecipes[i + 9];
//		}
//	}
//	return AIR;
//}
//
//Item Recipe::getRecipe(Item slots[2][2]) {
//	for (int i = 0; i < recipes.size(); i += 5) {
//		if (slots[0][0] == recipes[i + 0] && slots[0][1] == recipes[i + 1] &&
//			slots[1][0] == recipes[i + 2] && slots[1][1] == recipes[i + 3]
//			) {
//			return recipes[i + 4];
//		}
//	}
//	return AIR;
//}

bool Recipe::itemPlaceable(Item itemType) {
	return (itemType.isPlaceable);
}

bool Recipe::itemUsable(Item itemType) {
	return (itemType.isUsable);
}

bool Recipe::isTool(Item itemType) {
	return (itemType.isTool);
}

bool Recipe::isBreakable(Item itemType) {
	return (itemType.isBreakable);
}