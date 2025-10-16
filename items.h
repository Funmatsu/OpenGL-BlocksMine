#pragma once
#pragma once

//#define AIR				  0
//#define DIAMOND_ORE       1
//#define GRASS_BLOCK       2
//#define IRON_ORE          3
//#define STONE_BLOCK       4
//#define DIRT_BLOCK        5
//#define OAK_WOOD          6
//#define CLOUD             7
//#define OAK_LEAVES        8
//#define OAK_PLANK         9
//#define POPPY             10
//#define BLUE_ORCHID       11
//#define GRASS             12
//
//#define CRAFTING_TABLE    21
////This is the limit for checking if a block is a tool or breakable; Dont change it's position!(Dont change the positions of crafting table and bedrock
//#define BEDROCK           100
//
//
//#define WOODEN_PICKAXE 101
//#define STICK 102

struct Item {
	int id;
	bool isPlaceable;
	bool isUsable;
	bool isTool;
	bool isBreakable;

	Item() {
		id = 0;
		isPlaceable = 0;
		isUsable = 0;
		isTool = 0;
		isBreakable = 0;
	}
	Item(int item_id, bool placeable, bool useable, bool toolable, bool breakable) {
		id = item_id;
		isPlaceable = placeable;
		isUsable = useable;
		isTool = toolable;
		isBreakable = breakable;
	}

	bool operator==(const Item& item) {
		return(id == item.id);
	}

	bool operator!=(const Item& item) {
		return(id != item.id);
	}

	Item(const Item& item) {
		id = item.id;
		isPlaceable = item.isPlaceable;
		isUsable = item.isUsable;
		isTool = item.isTool;
		isBreakable = item.isBreakable;
	}
};

Item AIR(0, 0, 0, 0, 1) ;
Item DIAMOND_ORE(1, 1, 0, 0, 1);
Item GRASS_BLOCK(2, 1, 0, 0, 1);
Item IRON_ORE(3, 1, 0, 0, 1);
Item STONE_BLOCK(4, 1, 0, 0, 1);
Item DIRT_BLOCK(5, 1, 0, 0, 1);
Item OAK_WOOD(6, 1, 0, 0, 1);
Item CLOUD(7, 1, 0, 0, 1);
Item OAK_LEAVES(8, 1, 0, 0, 1);
Item OAK_PLANK(9, 1, 0, 0, 1);
Item POPPY(10, 1, 0, 0, 1);
Item BLUE_ORCHID(11, 1, 0, 0, 1);
Item GRASS(12, 1, 0, 0, 1);

Item CRAFTING_TABLE(13, 1, 1, 0, 1);
Item BEDROCK(14, 1, 0, 0, 0);

Item WOODEN_PICKAXE(15, 1, 0, 1, 1);
Item STICK(16, 1, 0, 1, 1);

vector<Item> items{
	AIR,
	DIAMOND_ORE,
	GRASS_BLOCK,
	IRON_ORE,
	STONE_BLOCK,
	DIRT_BLOCK,
	OAK_WOOD,
	CLOUD,
	OAK_LEAVES,
	OAK_PLANK,
	POPPY,
	BLUE_ORCHID,
	GRASS,
	CRAFTING_TABLE,
	BEDROCK,
	WOODEN_PICKAXE,
	STICK
};
