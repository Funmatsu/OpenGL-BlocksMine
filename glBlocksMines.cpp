#define GLEW_STATIC
//(DONE!!!) TO DO : Implement spectator mode kind of face culling;;;; Block right next to block of Air can be rendered. 

#include "libraries.h"

using namespace std;
using namespace glm;

#define BLOCK_TEX           0
#define MAIN_INV_TEX        1
#define SLOT_TEX            2
#define LARGE_INV_TEX       3
#define TOOLS_TEX           4
#define CRAFT_GUI_TEX       5
#define TOP_TEX             6

Window mainWindow;
GLfloat deltaTime = 2.0f;
GLfloat lastTime = 0.0f;

vector<GL_Texture*> Textures;

int renderDistance = 10;

DirectionalLight mainLight, auxLight;
PointLight pointLights[MAX_POINT_LIGHTS];
unsigned int pointLightCount = 0;

#include "varDef.h"

Sky sky;

void addTextures();
void renderWorld();
void directionalShadowPass(DirectionalLight* light, mat4 model);

int main()
{
    mainWindow = Window(WIDTH, HEIGHT);
    mainWindow.initialize(); 

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glfwSwapInterval(0);

    createShaders();
    addTextures();

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CCW);

    int spawn = 1;
    camera.setCameraPos(vec3(0.0f, CHUNK_SIZE * CHUNK_SIZE / 2, 0.0f));

    Crosshair crosshair;
    crosshair.defineCrosshairGeometry();
    inventory.defineMainInventoryGeometry();
    inventory.defineHotbarGeometry();    

    initChunksNoise();

    Block currentBlock;

    mat4 model(1.0f), projection(1.0f), view(1.0f);
    projection = perspective(radians(45.0f), (float(mainWindow.getBufferWidth()) / float(mainWindow.getBufferHeight())), 0.01f, 500.0f);

    glm::mat4 ortho = glm::ortho(0.0f, float(WIDTH), 0.0f, float(HEIGHT));

    mat4 modelCurSlots[4][9];
    mat4 modelCraftInvSlots[2][2], modelbigCraftInvSlots[3][3];;
    mat4 modelCraftedInvSlot(1.0f), modelbigCraftedInvSlot(1.0f);
    mat4 modelCurSlotsMain[9];
    mat4 modelCurSlot(1.0f);
    mat4 modelCur(1.0f);

    for (int j = 0; j < 9; j++) {
        modelCurSlotsMain[j] = mat4(1.0f);
        modelCurSlotsMain[j] = translate(modelCurSlotsMain[j], vec3((centerX - 40.0f + j * 137) / WIDTH, 2 * (centerY + offsetY + 70.0f) / HEIGHT, 0.0f));
    }
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 9; j++) {
            modelCurSlots[i][j] = mat4(1.0f);
            modelCurSlots[i][j] = translate(modelCurSlots[i][j], vec3((centerX + 15.0f + (float)j * 123) / WIDTH, 2 * (centerY + offsetY + 323.0f + (3 - i) * 10 + 85.0f - (i == 3 ? 0 : 10)) / HEIGHT, 0.0f));
        }
    }
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            modelCraftInvSlots[i][j] = mat4(1.0f);
            modelCraftInvSlots[i][j] = translate(modelCraftInvSlots[i][j], vec3((centerX + 18.0f + (float)j * 123) / WIDTH, 2 * (centerY + offsetY + 323.0f + (3 - i) * 100 + 80.0f) / HEIGHT, 0.0f));
        }
    }
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            modelbigCraftInvSlots[i][j] = mat4(1.0f);
            modelbigCraftInvSlots[i][j] = translate(modelbigCraftInvSlots[i][j], vec3((centerX / 2 + 72 +  (float)j * 123) / WIDTH, 2 * (centerY + offsetY + 270.0f + (4 - i) * 100 + 80.0f) / HEIGHT, 0.0f));
        }
    }
    modelCraftedInvSlot = translate(modelCraftedInvSlot, vec3((centerX + 18.0f + 300.0f) / WIDTH, 2 * (centerY + offsetY + 323.0f + 150 + 80.0f) / HEIGHT, 0.0f));
    modelbigCraftedInvSlot = translate(modelCraftedInvSlot, vec3((-150.0f) / WIDTH, (10.0f) / HEIGHT, 0.0f));
    modelCurSlot = rotate(modelCurSlot, radians(-30.0f), vec3(1.0f, 0.0f, 0.0f));
    int firstChunk = 0;
    int renderX = 1, renderY = 1;
    
    Block craftedItem(vec3(0.0f), AIR, {}, {});
    int offs = 1, ctrl = 0;
    bool jumping = false, ctrlJump = false, flying = false;
    float jumpCount = 0;
    float lastXChange = 0.0f, lastYChange = 0.0f;

    float time = 300.0f, lowTime = 10.0f, maxTime = 1000.0;
    bool night = false;
    sky.buildSky();

    std::vector<std::thread> workers;
    for (int i = 0; i < 1; ++i) {
        workers.emplace_back(chunkWorker); // worker thread is somewhere in threading.h
    }

    mainLight = DirectionalLight(1920, 1059,
        1.0f, 1.0f, 1.0f,
        0.5f, time / maxTime,
        0.0f, -CHUNK_SIZE * CHUNK_SIZE, 1.0f);

    auxLight = DirectionalLight(1920, 1059,
        1.0f, 1.0f, 1.0f,
        0.5f, time / maxTime,
        0.0f, CHUNK_SIZE * CHUNK_SIZE, CHUNK_SIZE);

    while (!mainWindow.getShouldClose()) {
        mainLight.setShadowPos(vec2(camera.getCameraPos().x, camera.getCameraPos().z));
        int count = 0, count2 = 0;
        for (int i = -renderX; i < renderX; i++) {
            for (int j = -renderY; j < renderY; j += 1) {
                //if (count >= CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE ) break; // Just in case to limit the number of chunks generated per frame
                ivec2 chunkPos = ivec2(floorDiv(camera.getCameraPos().x, CHUNK_SIZE) + i, floorDiv(camera.getCameraPos().z, CHUNK_SIZE) + j);
                if (chunkCoords.count(chunkPos) <= 0) {
                    chunkCoords.insert({ chunkPos });
                    {
                        chunkGenRunning = true;
                        std::lock_guard<std::mutex> lock(queueMutex);
                        chunkRequestQueue.push(chunkPos);
                        queueCV.notify_one();
                    }
                } 
            }
 /*         count2++; if (count2 >= CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE) break;*/
        } {
            //std::lock_guard<std::mutex> lock(resultMutex);
            while (!chunkResultQueue.empty()) {
                //count++;
                //if (count > 2) break;
                std::lock_guard<std::mutex> lock(resultMutex);
                Chunk* chunk = new Chunk();
                *chunk = chunkResultQueue.front();   // Using a refference here instead of a copy is actually cracked! massive speed up
                world.addChunk(chunk, ivec2(chunk->coords));
                chunkResultQueue.pop();
            }
        }

        if (renderX < renderDistance) {
            renderX++;
            renderY++;
        }
        
        Textures[BLOCK_TEX]->useTexture();
        shaders[0]->useShader();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwPollEvents();

        sky.applySky(view, projection);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        directionalShadowPass(&mainLight, model);
        Textures[BLOCK_TEX]->useTexture();

        mainLight.getShadowMap()->read(GL_TEXTURE1);
        glUniform1i(glGetUniformLocation(shaders[0]->getShaderId(), "directionalShadowMap"), 1);
        
        Textures[TOP_TEX]->useNextTexture();
        glUniform1i(glGetUniformLocation(shaders[0]->getShaderId(), "topTexture"), 2);

        renderWorld();
        BlockData lookBlock;

        view = camera.calcViewMatrix();

        glUniformMatrix4fv(shaders[0]->getModelLocation(), 1, GL_FALSE, value_ptr(model));
        glUniformMatrix4fv(shaders[0]->getViewLocation(), 1, GL_FALSE, value_ptr(view));
        glUniformMatrix4fv(shaders[0]->getProjectionLocation(), 1, GL_FALSE, value_ptr(projection));
        glUniform3f(shaders[0]->getColorMaskLocation(), 1.0f, 1.0f, 1.0f);

        int sensitivity = 1.01f;
        camera.keyControl(mainWindow.getKeys(), deltaTime);
        if (inventory.inventoryOn || inventory.craftingInventoryOn) {
            glfwSetInputMode(mainWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            mainWindow.setMouseMoved();
            //camera.mouseControl(lastXChange, lastYChange);
        }
        else {
            glfwSetInputMode(mainWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
            //lastXChange = mainWindow.getXChange(); lastYChange = mainWindow.getYChange();
        }

        shaders[0]->setDirectionalLight(&mainLight); //replaced : mainLight.useLight(shaders[0]->getAmbientIntensityLocation(), shaders[0]->getAmbientColorLocation(), shaders[0]->getDiffuseIntensityLocation(), shaders[0]->getDirectionLocation());
        shaders[0]->setPointLights(pointLights, pointLightCount);

        if (mainWindow.getKeyPressed() >= GLFW_KEY_1 && mainWindow.getKeyPressed() <= GLFW_KEY_9) {
            slot = mainWindow.getKeyPressed() - GLFW_KEY_1; inv_change = true;
        }
        
        float lastPress = 0.0, maxDelay = 0.2 + glfwGetTime();
        if (mainWindow.getKeys()[GLFW_KEY_P]) {
            //double now_ = glfwGetTime();
            //while (!(now_ - lastPress > maxDelay)) { now_ = glfwGetTime(); }
            if (mainWindow.getKeys()[GLFW_KEY_I]) {
                inventory.inf_blocks = true;
            }
            else if (mainWindow.getKeys()[GLFW_KEY_N]) {
                inventory.inf_blocks = false;
            }

            if (mainWindow.getKeyPressed() > GLFW_KEY_1 && mainWindow.getKeyPressed() <= GLFW_KEY_9) world.addBlocklook_at(items[mainWindow.getKeyPressed() - GLFW_KEY_1]);
            else world.addBlocklook_at(inventory.inv_slots[3][slot]);
            if (mainWindow.getKeys()[GLFW_KEY_9]) world.addBlocklook_at(items[CRAFTING_TABLE.id]);
            if (mainWindow.getKeys()[GLFW_KEY_1]) world.addBlocklook_at(items[TORCH.id]);
        }
        if (mainWindow.getKeys()[GLFW_KEY_T] || mainWindow.leftClickButtonPressed()) {
            //double now_ = glfwGetTime();
            //while (!(now_ - lastPress > maxDelay)) { now_ = glfwGetTime(); }
            {
                std::lock_guard<std::mutex> lock(breakReqMutex);
                breakReqQueue.push(vec3(1.0f));
                if (!blockBreakingOut) {
                    blockBreakingOut = true;
                }
            }
            {
                //std::lock_guard<std::mutex> lock(breakResMutex);
                if (!breakResQueue.empty() && blockBreakingOut) {
                    breakResQueue.pop();
                }
            }
        }
        if (mainWindow.rightClickButtonPressed()) {
            lookBlock = world.getBlockAt(lookingAtBlock());
            if (!recipe.itemUsable(lookBlock.blockType)) {
                if (inventory.inv_slots[3][slot] != AIR && recipe.itemPlaceable(inventory.inv_slots[3][slot])) {
                    {
                        std::lock_guard<std::mutex> lock(placeReqMutex);
                        placeReqQueue.push(vec3(1.0f));
                        blockPlacingOut = true;
                    }
                    {
                        //  std::lock_guard<std::mutex> lock(placeResMutex);
                        if (!placeResQueue.empty()) {
                            placeResQueue.pop();
                        }
                    }
                    double now_ = glfwGetTime();
                    while (!(now_ - lastPress > maxDelay)) { now_ = glfwGetTime(); }
                }
                if (!inventory.inf_blocks) {
                    inventory.deassignInvSlot(slot, 3);
                }
            }
            else {
                inventory.craftingInventoryOn = true;
            }
        }

        if (mainWindow.getKeys()[GLFW_KEY_Q]) {
            inventory.inv_slots[3][slot] = AIR;
            inventory.currInvSlot[3][slot].clearMesh();
            inv_change = true;
        }
        
        shaders[13]->useShader();
        view = camera.calcViewMatrix();
        glUniformMatrix4fv(shaders[13]->getModelLocation(), 1, GL_FALSE, value_ptr(model));
        glUniformMatrix4fv(shaders[13]->getViewLocation(), 1, GL_FALSE, value_ptr(view));
        glUniformMatrix4fv(shaders[13]->getProjectionLocation(), 1, GL_FALSE, value_ptr(projection));

        //For block highlighting
        //cout << camera.getCameraPos().x << " " << camera.getCameraPos().z << endl;
        vec3 position = lookingAtBlock();
        if (position.y >= 0) {
            Mesh cloudMesh = world.createVertsOnlyMesh(position, 0.005f, CLOUD);
            cloudMesh.renderMesh();
        }

        glDisable(GL_DEPTH_TEST); // so crosshair draws on top
        shaders[1]->useShader();
        glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE);

        glUniformMatrix4fv(glGetUniformLocation(shaders[1]->getShaderId(), "ortho"), 1, GL_FALSE, glm::value_ptr(ortho));
        crosshair.drawCrosshair();

        shaders[2]->useShader();

        Textures[SLOT_TEX]->useTexture();
        inventory.defineHotbarSlotSelectorGeometry();
        inventory.drawHotbarSlotSelector();

        Textures[MAIN_INV_TEX]->useTexture();
        if (inv_change)
        {
            if (inventory.inv_slots[3][slot] != AIR) {
                currentBlock.blockMesh = world.createMeshCube(centerX + offsetX, centerY + offsetY, 0.0f, 400.0f, inventory.inv_slots[3][slot]);
                currentBlock.type = inventory.inv_slots[3][slot];
            }
            else {
                currentBlock.blockMesh.clearMesh();
            }

            for (int j = 0; j < (sizeof(inventory.inv_slots[0]) / sizeof(Item)); j++) {
                float itemHeight = 0.0f;
                if (inventory.inv_slots[3][j] == GRASS || inventory.inv_slots[3][j] == POPPY || inventory.inv_slots[3][j] == BLUE_ORCHID) {
                    itemHeight = -15.0f;
                }
                if (inventory.currInvSlot[3][j].verts.size() == 0 && inventory.inv_slots[3][j] != AIR) {
                    inventory.currInvSlot[3][j] = world.createMeshCube(centerX / 5, itemHeight, 0.0f, 35.0f, inventory.inv_slots[3][j]);
                }
            }

            if (inventory.inventoryOn) {
                for (int i = 0; i < (sizeof(inventory.craftInv) / sizeof(inventory.craftInv[0])); i++) {
                    for (int j = 0; j < (sizeof(inventory.craftInv[0]) / sizeof(Item)); j++) {
                        if (inventory.craftInvSlot[i][j].verts.size() == 0 && inventory.craftInv[i][j] != AIR) {
                            inventory.craftInvSlot[i][j] = world.createMeshCube(centerX / 5 + 270, centerY / 4 + 110, 0.0f, 35.0f, inventory.craftInv[i][j]);
                        }
                    }
                }
                if (recipe.getRecipe(inventory.craftInv) != AIR) {
                    craftedItem.blockMesh = world.createMeshCube(centerX / 5 + 315, centerY / 4 + 200.0f, 0.0f, 35.0f, recipe.getRecipe(inventory.craftInv));
                }
                craftedItem.type = recipe.getRecipe(inventory.craftInv);
            }
            else if (inventory.craftingInventoryOn) {
                for (int i = 0; i < (sizeof(inventory.bigCraftInv) / sizeof(inventory.bigCraftInv[0])); i++) {
                    for (int j = 0; j < (sizeof(inventory.bigCraftInv[0]) / sizeof(Item)); j++) {
                        if (inventory.bigCraftInvSlot[i][j].verts.size() == 0 && inventory.bigCraftInv[i][j] != AIR) {
                            inventory.bigCraftInvSlot[i][j] = world.createMeshCube(centerX / 5 + 270, centerY / 4 + 110, 0.0f, 35.0f, inventory.bigCraftInv[i][j]);
                        }
                    }
                }
                if (recipe.getRecipe(inventory.bigCraftInv) != AIR) {
                    craftedItem.blockMesh = world.createMeshCube(centerX / 5 + 315, centerY / 4 + 200.0f, 0.0f, 35.0f, recipe.getRecipe(inventory.bigCraftInv));
                }
                craftedItem.type = recipe.getRecipe(inventory.bigCraftInv);

            }

            inv_change = false;
        }

        shaders[2]->useShader();
        glUniformMatrix4fv(glGetUniformLocation(shaders[2]->getShaderId(), "ortho"), 1, GL_FALSE, glm::value_ptr(ortho));
        Textures[MAIN_INV_TEX]->useTexture();
        glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE);
        inventory.drawHotbar();
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        Textures[LARGE_INV_TEX]->useTexture();
        if (mainWindow.getKeys()[GLFW_KEY_C]) {
            bool blockAdded = false;
            if (inventory.inventoryOn) {
                for (int i = (sizeof(inventory.inv_slots) / sizeof(inventory.inv_slots[3])) - 1; i >= 0; i--) {
                    for (int j = 0; j < (sizeof(inventory.inv_slots[3]) / sizeof(Item)); j++) {
                        if (inventory.inv_slots[i][j] == AIR) {
                            inventory.inv_slots[i][j] = craftedItem.type;
                            for (int k = 0; k < sizeof(inventory.craftInv) / sizeof(inventory.craftInv[0]); k++) {
                                for (int l = 0; l < sizeof(inventory.craftInv[0]) / sizeof(Item); l++) {
                                    if (inventory.craftInv[k][l] != AIR) {
                                        inventory.craftInv[k][l] = AIR;
                                        inventory.craftInvSlot[k][l].clearMesh();
                                        inv_change = true;
                                    }
                                }
                            }
                            blockAdded = true;
                            break;
                        }
                    }
                    if (blockAdded) {
                        break;
                    }
                }
            }

            blockAdded = false;
            if (inventory.craftingInventoryOn) {
                for (int i = (sizeof(inventory.inv_slots) / sizeof(inventory.inv_slots[3])) - 1; i >= 0; i--) {
                    for (int j = 0; j < (sizeof(inventory.inv_slots[3]) / sizeof(Item)); j++) {
                        if (inventory.inv_slots[i][j] == AIR) {
                            inventory.inv_slots[i][j] = craftedItem.type;
                            for (int k = 0; k < sizeof(inventory.bigCraftInv) / sizeof(inventory.bigCraftInv[0]); k++) {
                                for (int l = 0; l < sizeof(inventory.bigCraftInv[0]) / sizeof(Item); l++) {
                                    if (inventory.bigCraftInv[k][l] != AIR) {
                                        inventory.bigCraftInv[k][l] = AIR;
                                        inventory.bigCraftInvSlot[k][l].clearMesh();
                                        inv_change = true;
                                    }
                                }
                            }
                            blockAdded = true;
                            break;
                        }
                    }
                    if (blockAdded) {
                        break;
                    }
                }
            }
            craftedItem.blockMesh.clearMesh();
        }

        if (mainWindow.getKeys()[GLFW_KEY_E]) {
            inventory.inventoryOn = true;
        }

        if (mainWindow.getKeys()[GLFW_KEY_RIGHT_SHIFT] && mainWindow.getKeys()[GLFW_KEY_E]) {
            for (int i = 0; i < (sizeof(inventory.inv_slots) / sizeof(inventory.inv_slots[3])); i++) {
                for (int j = 0; j < (sizeof(inventory.inv_slots[3]) / sizeof(Item)); j++) {
                    if (inventory.inv_slots[i][j] == AIR) {
                        for (int k = 0; k < sizeof(inventory.craftInv) / sizeof(inventory.craftInv[0]); k++) {
                            for (int l = 0; l < sizeof(inventory.craftInv[0]) / sizeof(Item); l++) {
                                if (inventory.craftInv[k][l] != AIR) {
                                    inventory.inv_slots[i][j] = inventory.craftInv[k][l];
                                    inventory.craftInv[k][l] = AIR;
                                    inventory.craftInvSlot[k][l] = Mesh();
                                    inv_change = true;
                                }
                            }
                        }
                    }
                }
            }
            inventory.inventoryOn = false;
            inventory.craftingInventoryOn = false;
            camera.mouseControl(lastXChange, lastYChange);
        }

        lastPress = 0.0, maxDelay = 0.2 + glfwGetTime();
        int keyPress = mainWindow.getKeyPressed();
        if (keyPress >= GLFW_KEY_RIGHT && keyPress <= GLFW_KEY_UP) {
            double now_ = glfwGetTime();
            while (!(now_ - lastPress > maxDelay)) { now_ = glfwGetTime(); }
            if (keyPress >= GLFW_KEY_DOWN && keyPress <= GLFW_KEY_UP) {
                if (slotY > -(keyPress - GLFW_KEY_DOWN) && slotY < 4 - (keyPress - GLFW_KEY_DOWN)) {
                    slotY += 2 * (keyPress - GLFW_KEY_DOWN) - 1; // down : slotY - 1, up : slotY + 1
                }
            }
            if (keyPress >= GLFW_KEY_RIGHT && keyPress <= GLFW_KEY_LEFT) {
                if (slotX >       keyPress - GLFW_KEY_RIGHT  - 1 && slotX < 8 + keyPress - GLFW_KEY_RIGHT) {
                    slotX -= 2 * (keyPress - GLFW_KEY_RIGHT) - 1;
                }
            }
        }

        if (inventory.inventoryOn) {
            Textures[LARGE_INV_TEX]->useTexture();
            shaders[2]->useShader();
            glUniformMatrix4fv(glGetUniformLocation(shaders[2]->getShaderId(), "ortho"), 1, GL_FALSE, glm::value_ptr(ortho));
            glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE);

            inventory.drawMainInventory();
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            Textures[SLOT_TEX]->useTexture();
            glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE);

            inventory.defineInvSlotSelectGeometry();
            inventory.drawInvSlotSelector();

            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            Textures[BLOCK_TEX]->useTexture();

            // Mesh for every inventory slot is being created here
            for (int i = 0; i < (sizeof(inventory.inv_slots) / sizeof(inventory.inv_slots[3])) - 1; i++) {
                for (int j = 0; j < (sizeof(inventory.inv_slots[3]) / sizeof(Item)); j++) {
                    float itemHeight = 0.0f;
                    if (!inventory.inv_slots[i][j].isFlat) {
                        itemHeight = 10.0f;
                    }
                    if (inventory.currInvSlot[i][j].verts.size() == 0 && inventory.inv_slots[i][j] != AIR) {
                        inventory.currInvSlot[i][j] = world.createMeshCube(centerX / 5, (3 - i) * 90 + 20 + itemHeight, 0.0f, 35.0f, inventory.inv_slots[i][j]);
                    }
                }
            }
            // Mesh for normal invertory is drawn here
            for (int i = 0; i < (sizeof(inventory.inv_slots) / sizeof(inventory.inv_slots[3])); i++) {
                for (int j = 0; j < (sizeof(inventory.inv_slots[3]) / sizeof(Item)); j++) {
                    Textures[BLOCK_TEX]->useTexture();
                    if (recipe.isTool(inventory.inv_slots[i][j])) {
                        Textures[TOOLS_TEX]->useTexture();
                    }
                    InventoryShaders[9 * i + j]->useShader();
                    glUniformMatrix4fv(glGetUniformLocation(InventoryShaders[9 * i + j]->getShaderId(), "ortho"), 1, GL_FALSE, glm::value_ptr(ortho));
                    glUniformMatrix4fv(InventoryShaders[9 * i + j]->getModelLocation(), 1, GL_FALSE, value_ptr(modelCurSlots[i][j]));

                    inventory.currInvSlot[i][j].renderMesh();
                    inv_change = true;
                }
            }

            if (mainWindow.getKeys()[GLFW_KEY_RIGHT_SHIFT] && mainWindow.getKeys()[GLFW_KEY_ENTER]) {
                if (mainWindow.getKeyPressed() >= GLFW_KEY_0 && mainWindow.getKeyPressed() <= GLFW_KEY_3) {
                    if (inventory.craftInv[(mainWindow.getKeyPressed() - GLFW_KEY_0) / 2][(mainWindow.getKeyPressed() - GLFW_KEY_0) % 2] == AIR) {
                        inventory.craftInv[(mainWindow.getKeyPressed() - GLFW_KEY_0) / 2][(mainWindow.getKeyPressed() - GLFW_KEY_0) % 2] = inventory.inv_slots[3 - (int)slotY][(int)slotX];
                        inventory.currInvSlot[3 - (int)slotY][(int)slotX].clearMesh();
                        inventory.inv_slots[3 - (int)slotY][(int)slotX] = AIR;
                    }
                }
                inv_change = true;
            }

            //Crafting inventory slots are being drawn here.
            for (int i = 0; i < (sizeof(inventory.craftInv) / sizeof(inventory.craftInv[0])); i++) {
                for (int j = 0; j < (sizeof(inventory.craftInv[0]) / sizeof(Item)); j++) {
                    Textures[BLOCK_TEX]->useTexture();
                    craftInvShaders[2 * i + j]->useShader();
                    glUniformMatrix4fv(glGetUniformLocation(craftInvShaders[2 * i + j]->getShaderId(), "ortho"), 1, GL_FALSE, glm::value_ptr(ortho));
                    glUniformMatrix4fv(craftInvShaders[2 * i + j]->getModelLocation(), 1, GL_FALSE, value_ptr(modelCraftInvSlots[i][j]));
                    if (recipe.isTool(inventory.craftInv[i][j])) {
                        Textures[TOOLS_TEX]->useTexture();
                    }
                    inventory.craftInvSlot[i][j].renderMesh();
                    inv_change = true;
                }
                craftInvShaders[4]->useShader();
                glUniformMatrix4fv(glGetUniformLocation(craftInvShaders[4]->getShaderId(), "ortho"), 1, GL_FALSE, glm::value_ptr(ortho));
                glUniformMatrix4fv(craftInvShaders[4]->getModelLocation(), 1, GL_FALSE, value_ptr(modelCraftedInvSlot));
                if (recipe.isTool(craftedItem.type)) {
                    Textures[TOOLS_TEX]->useTexture();
                }
                else {
                    Textures[BLOCK_TEX]->useTexture();
                }
                craftedItem.blockMesh.renderMesh();
            }
        }

        // crafting inventory GUI appears here
        if (inventory.craftingInventoryOn) {
            if (mainWindow.getKeys()[GLFW_KEY_RIGHT_SHIFT] && mainWindow.getKeys()[GLFW_KEY_ENTER]) {
                if (mainWindow.getKeyPressed() >= GLFW_KEY_0 && mainWindow.getKeyPressed() <= GLFW_KEY_8) {
                    if (inventory.bigCraftInv[(mainWindow.getKeyPressed() - GLFW_KEY_0) / 3][(mainWindow.getKeyPressed() - GLFW_KEY_0) % 3] == AIR) {
                        inventory.bigCraftInv[(mainWindow.getKeyPressed() - GLFW_KEY_0) / 3][(mainWindow.getKeyPressed() - GLFW_KEY_0) % 3] = inventory.inv_slots[3 - (int)slotY][(int)slotX];
                        inventory.currInvSlot[3 - (int)slotY][(int)slotX].clearMesh();
                        inventory.inv_slots[3 - (int)slotY][(int)slotX] = AIR;
                    }
                }
                inv_change = true;
            }

            Textures[CRAFT_GUI_TEX]->useTexture();
            shaders[2]->useShader();
            glUniformMatrix4fv(glGetUniformLocation(shaders[2]->getShaderId(), "ortho"), 1, GL_FALSE, glm::value_ptr(ortho));
            glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE);
            inventory.drawMainInventory();
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            Textures[SLOT_TEX]->useTexture();
            shaders[2]->useShader();
            glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE);
            inventory.defineCrafingInvSlotSelectorGeometry();
            inventory.drawInvSlotSelector();
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            Textures[BLOCK_TEX]->useTexture();

            for (int i = 0; i < (sizeof(inventory.inv_slots) / sizeof(inventory.inv_slots[3]) - 1); i++) {
                for (int j = 0; j < (sizeof(inventory.inv_slots[3]) / sizeof(Item)); j++) {
                    float itemHeight = 0.0f;
                    if (!inventory.inv_slots[i][j].isFlat) {
                        itemHeight = 10.0f;
                    }
                    if (inventory.currInvSlot[i][j].verts.size() == 0 && inventory.inv_slots[i][j] != AIR) {
                        inventory.currInvSlot[i][j] = world.createMeshCube(centerX / 5, (3 - i) * 90 + 20 + itemHeight, 0.0f, 35.0f, inventory.inv_slots[i][j]);
                    }
                }
            }

            for (int i = 0; i < (sizeof(inventory.inv_slots) / sizeof(inventory.inv_slots[3])); i++) {
                for (int j = 0; j < (sizeof(inventory.inv_slots[3]) / sizeof(Item)); j++) {
                    Textures[BLOCK_TEX]->useTexture();
                    InventoryShaders[9 * i + j]->useShader();
                    glUniformMatrix4fv(glGetUniformLocation(InventoryShaders[9 * i + j]->getShaderId(), "ortho"), 1, GL_FALSE, glm::value_ptr(ortho));
                    glUniformMatrix4fv(InventoryShaders[9 * i + j]->getModelLocation(), 1, GL_FALSE, value_ptr(modelCurSlots[i][j]));
                    if (recipe.isTool(inventory.inv_slots[i][j])) {
                        Textures[TOOLS_TEX]->useTexture();
                    }
                    inventory.currInvSlot[i][j].renderMesh();
                    inv_change = true;
                }
            }

            for (int i = 0; i < (sizeof(inventory.bigCraftInv) / sizeof(inventory.bigCraftInv[0])); i++) {
                for (int j = 0; j < (sizeof(inventory.bigCraftInv[0]) / sizeof(Item)); j++) {
                    Textures[BLOCK_TEX]->useTexture();
                    bigCraftInvShaders[3 * i + j]->useShader();
                    glUniformMatrix4fv(glGetUniformLocation(bigCraftInvShaders[3 * i + j]->getShaderId(), "ortho"), 1, GL_FALSE, glm::value_ptr(ortho));
                    glUniformMatrix4fv(bigCraftInvShaders[3 * i + j]->getModelLocation(), 1, GL_FALSE, value_ptr(modelbigCraftInvSlots[i][j]));
                    if (recipe.isTool(inventory.bigCraftInv[i][j])) {
                        Textures[TOOLS_TEX]->useTexture();
                    }
                    inventory.bigCraftInvSlot[i][j].renderMesh();
                    //cout << "printed!" << endl;
                    inv_change = true;
                }
            }
            bigCraftInvShaders[9]->useShader();
            glUniformMatrix4fv(glGetUniformLocation(bigCraftInvShaders[9]->getShaderId(), "ortho"), 1, GL_FALSE, glm::value_ptr(ortho));
            glUniformMatrix4fv(bigCraftInvShaders[9]->getModelLocation(), 1, GL_FALSE, value_ptr(modelbigCraftedInvSlot));
            if (recipe.isTool(craftedItem.type)) {
                Textures[TOOLS_TEX]->useTexture();
            }
            else {
                Textures[BLOCK_TEX]->useTexture();
            }
            craftedItem.blockMesh.renderMesh();
        }

        Textures[BLOCK_TEX]->useTexture();
        
        for (int i = 0; i < 9; i++) {
            shaders[4 + i]->useShader();
            glUniformMatrix4fv(glGetUniformLocation(shaders[4 + i]->getShaderId(), "ortho"), 1, GL_FALSE, glm::value_ptr(ortho));
            glUniformMatrix4fv(shaders[4 + i]->getModelLocation(), 1, GL_FALSE, value_ptr(modelCurSlotsMain[i]));
            if (recipe.isTool(inventory.inv_slots[3][i])) {
                Textures[TOOLS_TEX]->useTexture();
            }
            else {
                Textures[BLOCK_TEX]->useTexture();
            }
            inventory.currInvSlot[3][i].renderMesh();
            inv_change = true;
        }
   
        shaders[3]->useShader();

        glUniformMatrix4fv(glGetUniformLocation(shaders[3]->getShaderId(), "ortho"), 1, GL_FALSE, glm::value_ptr(ortho)); 

        glUniformMatrix4fv(shaders[3]->getModelLocation(), 1, GL_FALSE, value_ptr(modelCur));
        glEnable(GL_DEPTH_TEST);
        if (recipe.isTool(currentBlock.type)) {
            Textures[TOOLS_TEX]->useTexture();
        }
        else {
            Textures[BLOCK_TEX]->useTexture();
        }
        currentBlock.blockMesh.renderMesh();
        glDisable(GL_DEPTH_TEST);
        
        if (mainWindow.getShouldClose()) {
            chunkGenRunning = false;
            queueCV.notify_all(); // wake up sleeping threads

            for (auto& t : workers)
                t.join();

            chunkGenRunning = false;
            chunkGenRunning2 = false;
            chunkGenRunning3 = false;
            blockPlacing = false;
            blockBreaking = false;//chunkGenThread.join(); //chunkGenThread2.join(); //chunkGenThread3.join();
            blockBreakThread.join();
            blockPlaceThread.join();
        }

        //if (spawn > 0) {
        //    if (!blockExistsAt(ivec3(camera.getCameraPos().x, camera.getCameraPos().y - 2, camera.getCameraPos().z))) {
        //        camera.setCameraPos(vec3(camera.getCameraPos().x, camera.getCameraPos().y + 20, camera.getCameraPos().z));
        //        cout << "drain" << endl;
        //    }
        //    else spawn++;
        //}

        mainWindow.swapBuffers();
    }
}////

void addTextures() {
    /*Textures.push_back(new Texturegl("textures\\block_atlas_4.png"));*/
    Textures.push_back(new GL_Texture("textures\\block_atlas_32.png"));         //#define BLOCK_TEX           0
    Textures.push_back(new GL_Texture("textures\\clear_toolbar_2.png"));        //#define MAIN_INV_TEX        1
    Textures.push_back(new GL_Texture("textures\\clear_toolbar_3.png"));        //#define SLOT_TEX            2
    Textures.push_back(new GL_Texture("textures\\main_inventory.jpg"));         //#define LARGE_INV_TEX       3
    Textures.push_back(new GL_Texture("textures\\tools_atlas_3.png"));          //#define TOOLS_TEX           4
    Textures.push_back(new GL_Texture("textures\\crafting_table_gui.png"));     //#define CRAFT_GUI_TEX       5
    Textures.push_back(new GL_Texture("textures\\block_overlay_2.png"));        //#define TOP_TEX             6
    //Textures.push_back(new Texturegl("textures\\inventory_base.png"));

    for (int i = BLOCK_TEX; i < Textures.size(); i++) { Textures[i]->loadTexture(); }
}

void renderWorld() {
    for (auto& chunks : world.chunkData) {
        if ((chunks.second.coords.x >= camera.getCameraPos().x / CHUNK_SIZE - renderDistance * 1.5 && chunks.second.coords.x <= camera.getCameraPos().x / CHUNK_SIZE + renderDistance * 1.5) &&
            (chunks.second.coords.y >= camera.getCameraPos().z / CHUNK_SIZE - renderDistance * 1.5 && chunks.second.coords.y <= camera.getCameraPos().z / CHUNK_SIZE + renderDistance * 1.5)) {

            if (chunks.second.needUpdate) {
                chunks.second.mesh.createMesh(chunks.second.vertices, chunks.second.indices, chunks.second.vertices.size(), chunks.second.indices.size());
                chunks.second.needUpdate = false;
            }
            chunks.second.mesh.renderMesh();
        }
    }
}

void directionalShadowPass(DirectionalLight* light, mat4 model) {
    directionalShadowShader.useShader();
    directionalShadowShader.setDirectionalLightTransform(light->calcLightTransform());
    glUniformMatrix4fv(directionalShadowShader.getModelLocation(), 1, GL_FALSE, value_ptr(model));
    light->shadow_map->write();
    glClear(GL_DEPTH_BUFFER_BIT);
    renderWorld();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    shaders[0]->useShader();
    shaders[0]->setDirectionalLightTransform(light->directionalLightTransform);
}////