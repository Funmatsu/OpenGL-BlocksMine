#define GLEW_STATIC
//(DONE!!!) TO DO : Implement spectator mode kind of face culling;;;; Block right next to block of Air can be rendered. 

#include "libraries.h"
DirectionalLight mainLight, auxLight;
PointLight pointLights[MAX_POINT_LIGHTS];
unsigned int pointLightCount = 0;

using namespace std;
using namespace glm;

#define BLOCK_TEX           0
#define MAIN_INV_TEX        1
#define SLOT_TEX            2
#define LARGE_INV_TEX       3
#define TOOLS_TEX           4
#define CRAFT_GUI_TEX       5
#define TOP_TEX             6
#define FACE_TEX            7

Window mainWindow;
GLfloat deltaTime = 2.0f;
GLfloat lastTime = 0.0f;

vector<GL_Texture*> Textures;

int renderDistance = 10;
#include "varDef.h"

Sky sky;

void addTextures();
void renderWorld();
void directionalShadowPass(DirectionalLight* light, mat4 model);
int ftoint(float num);

Projectile ball;

int main(){
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
    firstCamera.setPosition(vec3(0.0f, 3 * CHUNK_SIZE * CHUNK_SIZE / 4, 0.0f));
    
    Crosshair crosshair;
    crosshair.defineCrosshairGeometry();
    inventory.defineMainInventoryGeometry();
    inventory.defineHotbarGeometry();    

    initChunksNoise();
    dropped.push_back(Projectile());

    mat4 model(1.0f), projection(1.0f), view(1.0f);
    projection = perspective(radians(45.0f), (float(mainWindow.getBufferWidth()) / float(mainWindow.getBufferHeight())), 0.01f, 500.0f);

    glm::mat4 ortho = glm::ortho(0.0f, float(WIDTH), 0.0f, float(HEIGHT));

    mat4 modelCurSlots[4][9];
    mat4 modelmainCraftingSlotsSlots[2][2], modelbigmainCraftingSlotsSlots[3][3];;
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
            modelmainCraftingSlotsSlots[i][j] = mat4(1.0f);
            modelmainCraftingSlotsSlots[i][j] = translate(modelmainCraftingSlotsSlots[i][j], vec3((centerX + 18.0f + (float)j * 123) / WIDTH, 2 * (centerY + offsetY + 323.0f + (3 - i) * 100 + 80.0f) / HEIGHT, 0.0f));
        }
    }
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            modelbigmainCraftingSlotsSlots[i][j] = mat4(1.0f);
            modelbigmainCraftingSlotsSlots[i][j] = translate(modelbigmainCraftingSlotsSlots[i][j], vec3((centerX / 2 + 72 +  (float)j * 123) / WIDTH, 2 * (centerY + offsetY + 270.0f + (4 - i) * 100 + 80.0f) / HEIGHT, 0.0f));
        }
    }
    modelCraftedInvSlot = translate(modelCraftedInvSlot, vec3((centerX + 18.0f + 300.0f) / WIDTH, 2 * (centerY + offsetY + 323.0f + 150 + 80.0f) / HEIGHT, 0.0f));
    modelbigCraftedInvSlot = translate(modelCraftedInvSlot, vec3((-150.0f) / WIDTH, (10.0f) / HEIGHT, 0.0f));
    modelCurSlot = rotate(modelCurSlot, radians(-30.0f), vec3(1.0f, 0.0f, 0.0f));
    int firstChunk = 0;
    int renderX = 1, renderY = 1;
    
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

    //auxLight = DirectionalLight(1920, 1059,
    //    1.0f, 1.0f, 1.0f,
    //    0.5f, time / maxTime,
    //    0.0f, CHUNK_SIZE * CHUNK_SIZE, CHUNK_SIZE);

    float dt = 0.01;
    int person_view = 0;
    bool tp = 0;

    while (!mainWindow.getShouldClose()) {
        thirdCamera_back.setPosition(firstCamera.getPosition() + firstCamera.getFront() * vec3(-5));
//        activeCamera = camera1;
        //auto start = chrono::high_resolution_clock::now();
        mainLight.setShadowPos(vec2(firstCamera.getPosition().x, firstCamera.getPosition().z));
        activeCamera.setFront(firstCamera.getFront());
        int count = 0, count2 = 0;
        
        for (int i = -renderX; i < renderX; i++) {
            for (int j = -renderY; j < renderY; j += 1) {
                if (count >= CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE ) break; // Just in case to limit the number of chunks generated per frame
                ivec2 chunkPos = ivec2(floorDiv(firstCamera.getPosition().x, CHUNK_SIZE) + i, floorDiv(firstCamera.getPosition().z, CHUNK_SIZE) + j);
                if (chunkCoords.count(to(chunkPos)) <= 0) {
                    chunkCoords.insert(to(chunkPos));
                    {
                        std::lock_guard<std::mutex> lock(queueMutex);
                        chunkRequestQueue.push(chunkPos);
                        queueCV.notify_one();
                    }
                }
            }
                     count2++; if (count2 >= CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE) break;
        } {
            //std::lock_guard<std::mutex> lock(resultMutex);
            while (!chunkResultQueue.empty()) {
                count++;
                if (count > 2) break;
                std::lock_guard<std::mutex> lock(queueMutex);
                Chunk& ch = *move(chunkResultQueue.front()); // Using a refference here instead of a copy is actually cracked! massive speed up
                world.addChunk(ch, ch.coords);
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

        view = activeCamera.calcViewMatrix();

        glUniformMatrix4fv(shaders[0]->getModelLocation(), 1, GL_FALSE, value_ptr(model));
        glUniformMatrix4fv(shaders[0]->getViewLocation(), 1, GL_FALSE, value_ptr(view));
        glUniformMatrix4fv(shaders[0]->getProjectionLocation(), 1, GL_FALSE, value_ptr(projection));
        glUniform3f(shaders[0]->getColorMaskLocation(), 1.0f, 1.0f, 1.0f);

        int sensitivity = 1.01f;
        firstCamera.keyControl(mainWindow.getKeys(), deltaTime, dt);
        if (inventory.mainInventoryOn || inventory.craftingTableInventoryOn) {
            glfwSetInputMode(mainWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            mainWindow.setMouseMoved();
        }
        else {
            glfwSetInputMode(mainWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            firstCamera.mouseControl(pow( 1, person_view) * mainWindow.getXChange(), mainWindow.getYChange());
            thirdCamera_back.mouseControl(pow(-1, person_view) * mainWindow.getXChange(), mainWindow.getYChange());
        }

        shaders[0]->setDirectionalLight(&mainLight); //replaced : mainLight.useLight(shaders[0]->getAmbientIntensityLocation(), shaders[0]->getAmbientColorLocation(), shaders[0]->getDiffuseIntensityLocation(), shaders[0]->getDirectionLocation());
        shaders[0]->setPointLights(pointLights, pointLightCount);

        if (mainWindow.getKeyPressed() >= GLFW_KEY_1 && mainWindow.getKeyPressed() <= GLFW_KEY_9) {
            slot = mainWindow.getKeyPressed() - GLFW_KEY_1; inventory.invDidChange(1);;
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
            else world.addBlocklook_at(inventory.mainInventorySlots[3][slot].item);
            if (mainWindow.getKeys()[GLFW_KEY_9]) world.addBlocklook_at(items[CRAFTING_TABLE.id]);
            if (mainWindow.getKeys()[GLFW_KEY_1]) world.addBlocklook_at(items[TORCH.id]);
        }
        if (mainWindow.getKeys()[GLFW_KEY_T] || mainWindow.leftClickButtonPressed()) {
            //double now_ = glfwGetTime();
            //while (!(now_ - lastPress > maxDelay)) { now_ = glfwGetTime(); }
            vec3 dropPos = lookingAtBlock();
            std::lock_guard<std::mutex> lock(breakReqMutex);
            breakReqQueue.push(vec3(1.0f));
            if (!blockBreakingOut) {
                blockBreakingOut = true;
            }
        }
        if (mainWindow.rightClickButtonPressed()) {
            lookBlock = world.getBlockAt(lookingAtBlock());
            if (!recipe.itemUsable(lookBlock.blockType)) {
                if (mainWindow.getKeys()[GLFW_KEY_I]) {
                    inventory.inf_blocks = true;
                }
                else if (mainWindow.getKeys()[GLFW_KEY_N]) {
                    inventory.inf_blocks = false;
                }
                if (inventory.mainInventorySlots[3][slot].item != AIR && recipe.itemPlaceable(inventory.mainInventorySlots[3][slot].item)) {
                    {
                        std::lock_guard<std::mutex> lock(placeReqMutex);
                        placeReqQueue.push(vec3(1.0f));
                        blockPlacingOut = true;
                    }
                    double now_ = glfwGetTime();
                    while (!(now_ - lastPress > maxDelay)) { now_ = glfwGetTime(); }
                }
                if (!inventory.inf_blocks) {
                    inventory.deassignInvSlot(slot, 3);
                }
            }
            else {
                inventory.craftingTableInventoryOn = true;
            }
        }

        if (mainWindow.getKeys()[GLFW_KEY_Q]) {
            dropped.push_back(Projectile());
            int xdrop, ydrop;
            if (inventory.mainInventoryOn) {
                xdrop = (int)slotX, ydrop = 3 - (int)slotY;
            }
            else {
                xdrop = slot, ydrop = 3;
            }
            dropped.back().shoot(firstCamera.getPosition() + normalize(firstCamera.getFront()), vec3(firstCamera.getFront().x, 0.25, firstCamera.getFront().z), inventory.mainInventorySlots[ydrop][xdrop].item);

            if (inventory.mainInventoryOn) {
                inventory.mainInventorySlots[3 - (int)slotY][(int)slotX].item = AIR;
                inventory.mainInventorySlots[3 - (int)slotY][(int)slotX].mesh.clearMesh();
            }
            else {
                inventory.mainInventorySlots[3][slot].item = AIR;
                inventory.mainInventorySlots[3][slot].mesh.clearMesh();
            }
            inventory.invDidChange(1);;
        }
        
        shaders[13]->useShader();
        view = activeCamera.calcViewMatrix();
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
        shaders[0]->useShader();
        Textures[FACE_TEX]->useTexture();

        if (mainWindow.getKeys()[GLFW_KEY_F3]) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            person_view = (++person_view % 3);
        }

        vec3 headPos;
        headPos = firstCamera.getPosition();
        if (person_view == 0) {
            activeCamera = firstCamera;
        }
        else if (person_view == 1) {
            activeCamera = thirdCamera_back;
        }

        Textures[TOP_TEX]->unbindNextTexture();
        
        Mesh headMesh = world.createMeshCube(headPos, -2.0f, CLOUD);
        headMesh.renderMesh();

        Textures[TOP_TEX]->useNextTexture();
        glUniform1i(glGetUniformLocation(shaders[0]->getShaderId(), "topTexture"), 2);

        //auto end = chrono::high_resolution_clock::now();
        //projectileShader->useShader();
        //glUniformMatrix4fv(projectileShader->getModelLocation(), 1, GL_FALSE, value_ptr(model));
        //glUniformMatrix4fv(projectileShader->getViewLocation(), 1, GL_FALSE, value_ptr(view));
        //glUniformMatrix4fv(projectileShader->getProjectionLocation(), 1, GL_FALSE, value_ptr(projection));
        Textures[SLOT_TEX]->useTexture();
        
        if (mainWindow.getKeys()[GLFW_KEY_U]) {
            ball.shoot(firstCamera.getPosition(), firstCamera.getFront());
        }
        vec3 cameraPosition = firstCamera.getPosition();
        if (ball.shot) {
            ball.mesh = world.createMeshCube((ball.position), -4.0f, CLOUD);
            //ball.shot = 0;
        }

        for (int i = 0; i < dropped.size(); i++) {
            if (dropped[i].shot) {
                dropped[i].mesh = world.createProjectileMesh((dropped[i].position), -4.0f, dropped[i].item);
            }
        }

        if (ball.shot) {
            if (!blockExistsAt((vec3(ftoint(ball.position.x), ftoint(ball.position.y - 0.5), ftoint(ball.position.z))))) {
                tp = 1;
                ball.update();
                ball.draw();
            }
            else {
                if (tp == 1) {
                    firstCamera.setPosition(ball.position);
                    ball.velocity = vec3(0);
                    ball.initial_velocity = vec3(0);
                    tp = 0;
                }
            }
        }
        
        Textures[BLOCK_TEX]->useTexture();
        for (int i = 0; i < dropped.size(); i++) {
            if (dropped[i].shot) {
                if (!blockExistsAt((vec3(ftoint(dropped[i].position.x), dropped[i].position.y - 0.5, ftoint(dropped[i].position.z))))) {
                    dropped[i].update();
                }
                else {
                    dropped[i].shot = 0;
                }
                glUniformMatrix4fv(projectileShader->getModelLocation(), 1, GL_FALSE, value_ptr(dropped[i].model));
            }
            dropped[i].draw();

        }
        //Mesh headMesh1 = world.createMeshCube(headPos + vec3(0, -1, 0), 0.5f, CLOUD);
        //headMesh1.renderMesh();
        //Mesh headMesh2 = world.createMeshCube(headPos + vec3(0, -2, 0), 0.5f, CLOUD);
        //headMesh2.renderMesh();

        glDisable(GL_DEPTH_TEST); // so crosshair draws on top
        shaders[1]->useShader();
        glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE);

        glUniformMatrix4fv(shaders[1]->getOrthoLocation(), 1, GL_FALSE, glm::value_ptr(ortho));
        crosshair.drawCrosshair();

        shaders[2]->useShader();
        Textures[SLOT_TEX]->useTexture();
        inventory.defineHotbarSlotSelectorGeometry();
        inventory.drawHotbarSlotSelector();

        if (inventory.invChange()) { inventory.updateInventory(); }

        shaders[2]->useShader();
        glUniformMatrix4fv(shaders[2]->getOrthoLocation(), 1, GL_FALSE, glm::value_ptr(ortho));
        Textures[MAIN_INV_TEX]->useTexture();
        glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE);
        inventory.drawHotbar();
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        Textures[LARGE_INV_TEX]->useTexture();
        if (mainWindow.getKeys()[GLFW_KEY_C]) {
            if (inventory.mainInventoryOn) {
                for (int i = (sizeof(inventory.mainInventorySlots) / sizeof(inventory.mainInventorySlots[3])) - 1; i >= 0; i--) {
                    for (int j = 0; j < (sizeof(inventory.mainInventorySlots[3]) / sizeof(InventorySlot)); j++) {
                        if (inventory.mainInventorySlots[i][j].item == AIR) {
                            inventory.mainInventorySlots[i][j].item = craftedItem.item;
                            inventory.clearMainCraftingSlots();
                            goto invcleared;
                        }
                    }
                }
            }
            
            else if (inventory.craftingTableInventoryOn) {
                for (int i = (sizeof(inventory.mainInventorySlots) / sizeof(inventory.mainInventorySlots[3])) - 1; i >= 0; i--) {
                    for (int j = 0; j < (sizeof(inventory.mainInventorySlots[3]) / sizeof(InventorySlot)); j++) {
                        if (inventory.mainInventorySlots[i][j].item == AIR) {
                            inventory.mainInventorySlots[i][j].item = craftedItem.item;
                            inventory.clearCraftingTableSlots();
                            goto invcleared;
                        }
                    }
                }
            }
            invcleared:
            craftedItem.mesh.clearMesh();
        }

        if (mainWindow.getKeys()[GLFW_KEY_E]) {
            std::this_thread::sleep_for(chrono::milliseconds(50));
            inventory.mainInventoryOn = !inventory.mainInventoryOn;
        }

        if (mainWindow.getKeys()[GLFW_KEY_RIGHT_SHIFT] && mainWindow.getKeys()[GLFW_KEY_E]) {
            for (int i = 0; i < (sizeof(inventory.mainInventorySlots) / sizeof(inventory.mainInventorySlots[3])); i++) {
                for (int j = 0; j < (sizeof(inventory.mainInventorySlots[3]) / sizeof(InventorySlot)); j++) {
                    if (inventory.mainInventorySlots[i][j].item == AIR) {
                        for (int k = 0; k < sizeof(inventory.mainCraftingSlots) / sizeof(inventory.mainCraftingSlots[0]); k++) {
                            for (int l = 0; l < sizeof(inventory.mainCraftingSlots[0]) / sizeof(InventorySlot); l++) {
                                if (inventory.mainCraftingSlots[k][l].item != AIR) {
                                    inventory.mainInventorySlots[i][j] = inventory.mainCraftingSlots[k][l];
                                    inventory.mainCraftingSlots[k][l].item = AIR;
                                    inventory.mainCraftingSlots[k][l].mesh = Mesh();
                                    inventory.invDidChange(1);
                                    craftedItem.item = AIR;
                                    craftedItem.mesh.clearMesh();
                                    if (k == sizeof(inventory.mainCraftingSlots) / sizeof(inventory.mainCraftingSlots[0]) - 1 &&
                                        sizeof(inventory.mainCraftingSlots[0]) / sizeof(InventorySlot) - 1) goto done;
                                }
                            }
                        }
                    }
                }
            }
            done:
            inventory.mainInventoryOn = false;
            inventory.craftingTableInventoryOn = false;
            firstCamera.mouseControl(lastXChange, lastYChange);
        }

        int keyPress = mainWindow.getKeyPressed();
        if (keyPress >= GLFW_KEY_RIGHT && keyPress <= GLFW_KEY_UP) {
            this_thread::sleep_for(chrono::milliseconds(20));
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

        if (inventory.mainInventoryOn) {
            Textures[LARGE_INV_TEX]->useTexture();
            shaders[2]->useShader();
            glUniformMatrix4fv(shaders[2]->getOrthoLocation(), 1, GL_FALSE, glm::value_ptr(ortho));
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
            inventory.defineMainInventorySlotsGeometry();
            // Mesh for normal invertory is drawn here
            inventory.drawMainInventorySlots(ortho, modelCurSlots);

            if (mainWindow.getKeys()[GLFW_KEY_RIGHT_SHIFT] && mainWindow.getKeys()[GLFW_KEY_ENTER]) {
                if (mainWindow.getKeyPressed() >= GLFW_KEY_0 && mainWindow.getKeyPressed() <= GLFW_KEY_3) {
                    if (inventory.mainCraftingSlots[(mainWindow.getKeyPressed() - GLFW_KEY_0) / 2][(mainWindow.getKeyPressed() - GLFW_KEY_0) % 2].item == AIR) {
                        inventory.mainCraftingSlots[(mainWindow.getKeyPressed() - GLFW_KEY_0) / 2][(mainWindow.getKeyPressed() - GLFW_KEY_0) % 2].item = inventory.mainInventorySlots[3 - (int)slotY][(int)slotX].item;
                        inventory.mainInventorySlots[3 - (int)slotY][(int)slotX].mesh.clearMesh();
                        inventory.mainInventorySlots[3 - (int)slotY][(int)slotX].item = AIR;
                    }
                }
                inventory.invDidChange(1);;
            }

            //Crafting inventory slots are being drawn here.
            for (int i = 0; i < (sizeof(inventory.mainCraftingSlots) / sizeof(inventory.mainCraftingSlots[0])); i++) {
                for (int j = 0; j < (sizeof(inventory.mainCraftingSlots[0]) / sizeof(InventorySlot)); j++) {
                    Textures[BLOCK_TEX]->useTexture();
                    craftInvShaders[2 * i + j]->useShader();
                    glUniformMatrix4fv(craftInvShaders[2 * i + j]->getOrthoLocation(), 1, GL_FALSE, glm::value_ptr(ortho));
                    glUniformMatrix4fv(craftInvShaders[2 * i + j]->getModelLocation(), 1, GL_FALSE, value_ptr(modelmainCraftingSlotsSlots[i][j]));
                    if (recipe.isTool(inventory.mainCraftingSlots[i][j].item)) {
                        Textures[TOOLS_TEX]->useTexture();
                    }
                    inventory.mainCraftingSlots[i][j].mesh.renderMesh();
                    inventory.invDidChange(1);;
                }
                craftInvShaders[4]->useShader();
                glUniformMatrix4fv(glGetUniformLocation(craftInvShaders[4]->getShaderId(), "ortho"), 1, GL_FALSE, glm::value_ptr(ortho));
                glUniformMatrix4fv(craftInvShaders[4]->getModelLocation(), 1, GL_FALSE, value_ptr(modelCraftedInvSlot));
                if (recipe.isTool(craftedItem.item)) {
                    Textures[TOOLS_TEX]->useTexture();
                }
                else {
                    Textures[BLOCK_TEX]->useTexture();
                }
                craftedItem.mesh.renderMesh();
            }
        }

        // crafting inventory GUI appears here
        if (inventory.craftingTableInventoryOn) {
            if (mainWindow.getKeys()[GLFW_KEY_RIGHT_SHIFT] && mainWindow.getKeys()[GLFW_KEY_ENTER]) {
                if (mainWindow.getKeyPressed() >= GLFW_KEY_0 && mainWindow.getKeyPressed() <= GLFW_KEY_8) {
                    if (inventory.craftingTableSlots[(mainWindow.getKeyPressed() - GLFW_KEY_0) / 3][(mainWindow.getKeyPressed() - GLFW_KEY_0) % 3].item == AIR) { // eg pressing 4 gives [5/3][5%3] = [1][2]
                        inventory.craftingTableSlots[(mainWindow.getKeyPressed() - GLFW_KEY_0) / 3][(mainWindow.getKeyPressed() - GLFW_KEY_0) % 3].item = inventory.mainInventorySlots[3 - (int)slotY][(int)slotX].item;
                        inventory.mainInventorySlots[3 - (int)slotY][(int)slotX].mesh.clearMesh();
                        inventory.mainInventorySlots[3 - (int)slotY][(int)slotX].item = AIR;
                    }
                }
                inventory.invDidChange(1);;
            }

            Textures[CRAFT_GUI_TEX]->useTexture();
            shaders[2]->useShader();
            glUniformMatrix4fv(shaders[2]->getOrthoLocation(), 1, GL_FALSE, glm::value_ptr(ortho));
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

            inventory.defineMainInventorySlotsGeometry();
            inventory.drawMainInventorySlots(ortho, modelCurSlots);

            for (int i = 0; i < (sizeof(inventory.craftingTableSlots) / sizeof(inventory.craftingTableSlots[0])); i++) {
                for (int j = 0; j < (sizeof(inventory.craftingTableSlots[0]) / sizeof(InventorySlot)); j++) {
                    Textures[BLOCK_TEX]->useTexture();
                    bigCraftInvShaders[3 * i + j]->useShader();
                    glUniformMatrix4fv(bigCraftInvShaders[3 * i + j]->getOrthoLocation(), 1, GL_FALSE, glm::value_ptr(ortho));
                    glUniformMatrix4fv(bigCraftInvShaders[3 * i + j]->getModelLocation(), 1, GL_FALSE, value_ptr(modelbigmainCraftingSlotsSlots[i][j]));
                    if (recipe.isTool(inventory.craftingTableSlots[i][j].item)) {
                        Textures[TOOLS_TEX]->useTexture();
                    }
                    inventory.craftingTableSlots[i][j].mesh.renderMesh();
                    //cout << "printed!" << endl;
                    inventory.invDidChange(1);
                }
            }
            bigCraftInvShaders[9]->useShader();
            glUniformMatrix4fv(bigCraftInvShaders[9]->getOrthoLocation(), 1, GL_FALSE, glm::value_ptr(ortho));
            glUniformMatrix4fv(bigCraftInvShaders[9]->getModelLocation(), 1, GL_FALSE, value_ptr(modelbigCraftedInvSlot));
            if (recipe.isTool(craftedItem.item)) {
                Textures[TOOLS_TEX]->useTexture();
            }
            else {
                Textures[BLOCK_TEX]->useTexture();
            }
            craftedItem.mesh.renderMesh();
        }

        Textures[BLOCK_TEX]->useTexture();
        
        for (int i = 0; i < 9; i++) {
            shaders[4 + i]->useShader();
            glUniformMatrix4fv(shaders[4 + i]->getOrthoLocation(), 1, GL_FALSE, glm::value_ptr(ortho));
            glUniformMatrix4fv(shaders[4 + i]->getModelLocation(), 1, GL_FALSE, value_ptr(modelCurSlotsMain[i]));
            if (recipe.isTool(inventory.mainInventorySlots[3][i].item)) {
                Textures[TOOLS_TEX]->useTexture();
            }
            else {
                Textures[BLOCK_TEX]->useTexture();
            }
            inventory.mainInventorySlots[3][i].mesh.renderMesh();
            inventory.invDidChange(1);;
        }
   
        shaders[3]->useShader();

        glUniformMatrix4fv(glGetUniformLocation(shaders[3]->getShaderId(), "ortho"), 1, GL_FALSE, glm::value_ptr(ortho)); 

        glUniformMatrix4fv(shaders[3]->getModelLocation(), 1, GL_FALSE, value_ptr(modelCur));
        glEnable(GL_DEPTH_TEST);
        if (recipe.isTool(currentBlock.item)) {
            Textures[TOOLS_TEX]->useTexture();
        }
        else {
            Textures[BLOCK_TEX]->useTexture();
        }
        currentBlock.mesh.renderMesh();
        glDisable(GL_DEPTH_TEST);
        
        if (mainWindow.getShouldClose()) {
            chunkGenRunning = false;
            queueCV.notify_all(); // wake up sleeping threads

            for (auto& t : workers)
                t.join();

            chunkGenRunning = false;
            chunkGenRunning2 = false;
            chunkUpdateGenRunning = false;
            blockPlacing = false;
            blockBreaking = false;//chunkGenThread.join(); //chunkGenThread2.join(); //chunkGenThread3.join();
            blockBreakThread.join();
            blockPlaceThread.join();
            chunkUpdateThread.join();
        }

        if (spawn <= 255) {
            spawn++;
        }

        mainWindow.swapBuffers();
        //dt = std::chrono::duration<double>(end - start).count();
        vec3 cameraPosition2 = firstCamera.getPosition();
        bool onGround = blockExistsAt(ivec3(ftoint(cameraPosition2.x + (cameraPosition2.x > 0 ? 1.0 : 0.0)), ftoint(cameraPosition2.y - 2), ftoint(cameraPosition2.z + (cameraPosition2.z > 0 ? 1.0 : 0.0))));
        if (!onGround && spawn > 255) {
            firstCamera.initial_velocity.y += -0.5;
            //firstCamera.velocity_factor = vec3(1, 1, 1);
            //firstCamera.calculateCamPos(0.01); // 0.01 instead of dt
//            cout << "\x1b[2J\x1b[H"; // clears console
//            cout << firstCamera.getPosition().x << " " << firstCamera.getPosition().y << " " << firstCamera.getPosition().z << "\n" << firstCamera.getFront().x << " " << firstCamera.getFront().y << " " << firstCamera.getFront().z << endl;
//            cout << firstCamera.velocity.y << " " << dt << endl;
        }
        else if(onGround) {
            if (blockExistsAt(ivec3(ftoint(cameraPosition2.x - 1), ftoint(cameraPosition2.y - 2), ftoint(cameraPosition2.z - 1))) ||
                blockExistsAt(ivec3(ftoint(cameraPosition2.x + 1), ftoint(cameraPosition2.y - 2), ftoint(cameraPosition2.z - 1))) ||
                blockExistsAt(ivec3(ftoint(cameraPosition2.x + 1), ftoint(cameraPosition2.y - 2), ftoint(cameraPosition2.z + 1))) ||
                blockExistsAt(ivec3(ftoint(cameraPosition2.x - 1), ftoint(cameraPosition2.y - 2), ftoint(cameraPosition2.z + 1)))) {
                firstCamera.velocity = vec3(0);
                firstCamera.initial_velocity *= vec3(0, 1, 0);
                firstCamera.initial_velocity.y += 0.5;
                firstCamera.velocity_factor = vec3(1);
            }
            else {
                firstCamera.velocity = vec3(0);
                firstCamera.initial_velocity = vec3(0);
                firstCamera.velocity_factor = vec3(1);
            }
        }
    }
}////

void addTextures() {
    Textures.push_back(new GL_Texture("textures\\block_atlas_32.png"));         //#define BLOCK_TEX           0
    Textures.push_back(new GL_Texture("textures\\clear_toolbar_2.png"));        //#define MAIN_INV_TEX        1
    Textures.push_back(new GL_Texture("textures\\clear_toolbar_3.png"));        //#define SLOT_TEX            2
    Textures.push_back(new GL_Texture("textures\\main_inventory.jpg"));         //#define LARGE_INV_TEX       3
    Textures.push_back(new GL_Texture("textures\\tools_atlas_3.png"));          //#define TOOLS_TEX           4
    Textures.push_back(new GL_Texture("textures\\crafting_table_gui.png"));     //#define CRAFT_GUI_TEX       5
    Textures.push_back(new GL_Texture("textures\\block_overlay_2.png"));        //#define TOP_TEX             6
    Textures.push_back(new GL_Texture("textures\\steve_face.jpg"));             //#define FACE_TEX            7
    //Textures.push_back(new Texturegl("textures\\inventory_base.png"));

    for (int i = BLOCK_TEX; i < Textures.size(); i++) { Textures[i]->loadTexture(); }
}

void renderWorld() {
    for (auto& chunks : world.chunkData) {
        if ((chunks.second.coords.x >= firstCamera.getPosition().x / CHUNK_SIZE - renderDistance * 1.5 && chunks.second.coords.x <= firstCamera.getPosition().x / CHUNK_SIZE + renderDistance * 1.5) &&
            (chunks.second.coords.y >= firstCamera.getPosition().z / CHUNK_SIZE - renderDistance * 1.5 && chunks.second.coords.y <= firstCamera.getPosition().z / CHUNK_SIZE + renderDistance * 1.5)) {

            if (chunks.second.unloaded) {
                { chunkUpdateRequestQueue.push(chunks.second); /*std::lock_guard<std::mutex> lock(chunkUpdateRequestMutex); */ }
                chunks.second.needUpdate = true;
                chunks.second.unloaded = false;
            }
            if (chunks.second.needUpdate) {
                //cout << chunks.second.mesh.vertices.size() << endl;
                chunks.second.mesh.createMesh(chunks.second.mesh.vertices, chunks.second.mesh.indices, chunks.second.mesh.vertices.size(), chunks.second.mesh.indices.size());
                chunks.second.needUpdate = false;
            }
            chunks.second.mesh.renderMesh();
        }
        else {
            chunks.second.mesh.vertices.clear();
            chunks.second.mesh.indices.clear();
            chunks.second.mesh.clearMesh();
            chunks.second.unloaded = true;
        }
    }
}

void directionalShadowPass(DirectionalLight* light, mat4 model) {
    directionalShadowShader->useShader();
    directionalShadowShader->setDirectionalLightTransform(light->calcLightTransform());
    glUniformMatrix4fv(directionalShadowShader->getModelLocation(), 1, GL_FALSE, value_ptr(model));
    light->shadow_map->write();
    glClear(GL_DEPTH_BUFFER_BIT);
    renderWorld();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    shaders[0]->useShader();
    shaders[0]->setDirectionalLightTransform(light->directionalLightTransform);
}////

int ftoint(float num) {
    return num >= 0 ? num : num - 1;
}