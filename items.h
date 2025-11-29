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

class Item {
public:
	int id;
	bool isPlaceable;
	bool isUsable;
	bool isTool;
	bool isBreakable;
	bool isFlat;
	int isLuminous;

	Item() {
		id = 0;
		isPlaceable = 0;
		isUsable = 0;
		isTool = 0;
		isBreakable = 0;
		isFlat = 0;
		isLuminous = 0;
	}
	~Item() {
		id = 0;
		isPlaceable = 0;
		isUsable = 0;
		isTool = 0;
		isBreakable = 0;
		isFlat = 0;
		isLuminous = 0;
	}
	Item(int item_id, bool placeable, bool useable, bool toolable, bool breakable) {
		id = item_id;
		isPlaceable = placeable;
		isUsable = useable;
		isTool = toolable;
		isBreakable = breakable;
		isFlat = 0;
		isLuminous = 0;
	}

	Item(int item_id, bool placeable, bool useable, bool toolable, bool breakable, bool flatable) {
		id = item_id;
		isPlaceable = placeable;
		isUsable = useable;
		isTool = toolable;
		isBreakable = breakable;
		isFlat = flatable;
		isLuminous = 0;
	}

	Item(int item_id, bool placeable, bool useable, bool toolable, bool breakable, bool flatable, int luminable) {
		id = item_id;
		isPlaceable = placeable;
		isUsable = useable;
		isTool = toolable;
		isBreakable = breakable;
		isFlat = flatable;
		isLuminous = luminable;
	}

	void assignLuminousId(unsigned int& id) {
		isLuminous = ++id;
	}

	void assignLight(PointLight* pLight, vec3 position) {		
		if (isLuminous) {
			pLight[pointLightCount] = PointLight(1.0f, 0.9f, 0.2f,
				1.0f, 0.2f,
				position.x + 0.5, position.y + 1.25, position.z + 0.5,
				0.2, 0.1f, 0.05f);
			assignLuminousId(pointLightCount);
		}
	}

	void deassignLight(PointLight* pLight, vec3 position) {
		pLight[pointLightCount] = PointLight();
		if (isLuminous) {
			pointLightCount--;
		}
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
		isFlat = item.isFlat;
		isLuminous = item.isLuminous;
	}
};

Item AIR				(0,  0, 0, 0, 1);
Item DIAMOND_ORE		(1,  1, 0, 0, 1);
Item GRASS_BLOCK		(2,  1, 0, 0, 1);
Item IRON_ORE			(3,  1, 0, 0, 1);
Item STONE_BLOCK		(4,  1, 0, 0, 1);
Item DIRT_BLOCK			(5,  1, 0, 0, 1);
Item OAK_WOOD			(6,  1, 0, 0, 1);
Item CLOUD				(7,  1, 0, 0, 1);
Item OAK_LEAVES			(8,  1, 0, 0, 1);
Item OAK_PLANK			(9,  1, 0, 0, 1);
Item POPPY				(10, 1, 0, 0, 1, 1);
Item BLUE_ORCHID		(11, 1, 0, 0, 1, 1);
Item GRASS				(12, 1, 0, 0, 1, 1);

Item CRAFTING_TABLE		(13, 1, 1, 0, 1);
Item BEDROCK			(14, 1, 0, 0, 0);
Item TORCH				(15, 1, 0, 0, 1, 1, 1);

Item WOODEN_PICKAXE		(16, 1, 0, 1, 1);
Item STICK				(17, 1, 0, 1, 1);

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
	TORCH,
	WOODEN_PICKAXE,
	STICK
};

void getUVs(Item blockType, float* attrs) {
	attrs[0] = 1, attrs[1] = 0, attrs[2] = 0, attrs[3] = 0, attrs[4] = 0, attrs[5] = 0, attrs[6] = 1.0f;

	if (blockType == AIR) {
		attrs[0] = 1; attrs[1] = 0;
	}

	else if (blockType == DIAMOND_ORE) {
		attrs[0] = 4, attrs[1] = 4;
	}

	else if (blockType == GRASS_BLOCK) {
		attrs[0] = 3, attrs[1] = 2; attrs[2] = -3; attrs[3] = -2; attrs[4] = -3; attrs[5] = 1;
	}

	else if (blockType == IRON_ORE) {
		attrs[0] = 1; attrs[1] = 3;
	}

	else if (blockType == STONE_BLOCK) {
		attrs[0] = 0; attrs[1] = 1;
	}

	else if (blockType == DIRT_BLOCK) {
		attrs[0] = 0; attrs[1] = 3;
	}

	else if (blockType == OAK_WOOD) {
		attrs[0] = 0, attrs[1] = 2; attrs[2] = 2; attrs[3] = 1; attrs[4] = 2; attrs[5] = 1;
	}

	else if (blockType == CLOUD) {
		attrs[0] = 2, attrs[1] = 1, attrs[6] = 0.75f;
	}

	else if (blockType == OAK_PLANK) {
		attrs[0] = 3, attrs[1] = 3;
	}

	else if (blockType == OAK_LEAVES) {
		attrs[0] = 4, attrs[1] = 3;
	}

	else if (blockType == GRASS) {
		attrs[0] = 1, attrs[1] = 2;
	}

	else if (blockType == POPPY) {
		attrs[0] = 2, attrs[1] = 2;
	}

	else if (blockType == BLUE_ORCHID) {
		attrs[0] = 1, attrs[1] = 4;
	}

	else if (blockType == BEDROCK) {
		attrs[0] = 0, attrs[1] = 4;
	}

	else if (blockType == CRAFTING_TABLE) {
		attrs[0] = 2, attrs[1] = 4; attrs[2] = 1; attrs[3] = 0; attrs[4] = 1; attrs[5] = -1;
	}

	else if (blockType == TORCH) {
		attrs[0] = 1, attrs[1] = 1; attrs[2] = 3; attrs[3] = 1; attrs[4] = 3; attrs[5] = 1;
	}
}

//void getUVs(Item blockType, float* attrs) {	
//	attrs[0] = 1, attrs[1] = 0, attrs[2] = 0, attrs[3] = 0, attrs[4] = 0, attrs[5] = 0, attrs[6] = 1.0f;
//
//	if (blockType == AIR) {
//		attrs[0] = 3; attrs[1] = 3;
//	}
//
//	else if (blockType == DIAMOND_ORE) {
//		attrs[0] = 0, attrs[1] = 1;
//	}
//
//	else if (blockType == GRASS_BLOCK) {
//		attrs[0] = 0, attrs[1] = 2; attrs[2] = 2; attrs[3] = 0; attrs[4] = 2; attrs[5] = -2;
//	}
//
//	else if (blockType == IRON_ORE) {
//		attrs[0] = 1; attrs[1] = 1;
//	}
//
//	else if (blockType == STONE_BLOCK) {
//		attrs[0] = 1; attrs[1] = 2;
//	}
//
//	else if (blockType == DIRT_BLOCK) {
//		attrs[0] = 2; attrs[1] = 0;
//	}
//
//	else if (blockType == OAK_WOOD) {
//		attrs[0] = 2, attrs[1] = 1; attrs[2] = -2; attrs[3] = 2; attrs[4] = -2; attrs[5] = 2;
//	}
//
//	else if (blockType == CLOUD) {
//		attrs[0] = 1, attrs[1] = 0, attrs[6] = 0.75f;
//	}
//
//	else if (blockType == OAK_PLANK) {
//		attrs[0] = 3, attrs[1] = 0;
//	}
//
//	else if (blockType == OAK_LEAVES) {
//		attrs[0] = 0, attrs[1] = 0;
//	}
//
//	else if (blockType == GRASS) {
//		attrs[0] = 3, attrs[1] = 1;
//	}
//
//	else if (blockType == POPPY) {
//		attrs[0] = 3, attrs[1] = 2;
//	}
//
//	else if (blockType == BLUE_ORCHID) {
//		attrs[0] = 0, attrs[1] = 4;
//	}
//
//	else if (blockType == BEDROCK) {
//		attrs[0] = 4, attrs[1] = 0;
//	}
//
//	else if (blockType == CRAFTING_TABLE) {
//		attrs[0] = 2, attrs[1] = 3; attrs[2] = -1; attrs[3] = 0; attrs[4] = 1; attrs[5] = -3;
//	}
//
//	else if (blockType == TORCH) {
//		attrs[0] = 4, attrs[1] = 1; attrs[2] = 0; attrs[3] = 1; attrs[4] = 0; attrs[5] = 1;
//	}
//}