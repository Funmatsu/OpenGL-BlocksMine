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
#define BREAK_STAGE_TEX     8
#define TEXT_TEX            9

Window mainWindow;
GLfloat deltaTime = 2.0f;
GLfloat lastTime = 0.0f;

int renderDistance = 8;
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

    createShaders();
    addTextures();

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CCW);

    int spawn = 1;
    firstCamera.setPosition(vec3(0.0f, 3 * CHUNK_SIZE * CHUNK_SIZE / 3, 0.0f));
    
    Crosshair crosshair;
    crosshair.defineCrosshairGeometry();
    inventory.defineMainInventoryGeometry();
    inventory.defineHotbarGeometry();   
    LightMesh headMesh = world.createMeshCube(vec3(0), -2.5f, CLOUD);
    LightMesh lookingMesh = world.createVertsOnlyMesh(vec3(0), 1.01f, CLOUD);
    LightMesh compassMesh = createCompassVertsOnlyMesh(vec3(0));

    initChunksNoise();
    dropped.push_back(Projectile());

    mat4 model(1.0f), projection(1.0f), view(1.0f);
    float projAngle = 45.0;
    projection = perspective(radians(projAngle), (float(mainWindow.getBufferWidth()) / float(mainWindow.getBufferHeight())), 0.01f, 1000.0f);

    glm::mat4 ortho = glm::ortho(0.0f, float(WIDTH), 0.0f, float(HEIGHT));
    mat4 itemProj = perspective(radians(1.0f), 1.0f, 0.01f, 1500.0f),
         itemView = lookAt(vec3(0, 0, 400), vec3(0), vec3(0, 1, 0)),
         currentBlockView = lookAt(vec3(0, 0, 1400), vec3(0), vec3(0, 1, 0));

    mat4 itemModel = scale(mat4(1.0f), vec3(0.08f, 0.1f, 0.08f)) *
        rotate(mat4(1.0f), radians(-90.0f), vec3(0, 0, 1)) *
        rotate(mat4(1.0f), radians(30.0f), vec3(1, 0, 0)) *
        rotate(mat4(1.0f), radians(45.0f), vec3(0, 1, 0))
        ;
                                           //rotate(mat4(1.0f), radians(inventory.mainInventorySlots[3][i].angle), vec3(0, 1, 0))));
    mat4 breakModel = mat4(1.0f);

    inventory.initInventorySlots();

    int renderX = 0, renderY = 0;
    
    float lastXChange = 0.0f, lastYChange = 0.0f;
    float lastYaw = 0.0f, lastPitch = 0.0f;

    float time = 300.0f, lowTime = 10.0f, maxTime = 1000.0;
    bool night = false;
    sky.buildSky();

    std::vector<std::thread> workers;
    for (int i = 0; i < 5; ++i) {
        if(i < 2)
            workers.push_back(thread(chunkWorker)); // worker thread is somewhere in threading.h
        workers.push_back(thread(updateChunkJob));
    }

    mainLight = DirectionalLight(mainWindow.getBufferWidth(), mainWindow.getBufferHeight(),
        1.0f, 1.0f, 1.0f,
        0.5f, time / maxTime,
        0.0f, -CHUNK_SIZE * CHUNK_SIZE, 1.0f);

    auxLight = DirectionalLight(100, 100,
         1.0f, 1.0f, 1.0f,
         0.7f, 0.5f,
        -1.0f, 1.5f, 0.0f);

    float dt = 0.01;
    int person_view = 0;
    bool tp = 0;
    float breakAngle = 10.0f;

    Text position("position "), craftedItemName("air"), cursorPos;
    position.model = translate(mat4(1.0f), vec3(50, 1700, 0));

    //thread inventoryWorker(handleInvSlotClicks);
    int count = 0, count2 = 01;
    mainLight.setShadowPos(firstCamera.getPosition());
    while (!mainWindow.getShouldClose()) {
        handleInvSlotClicks();
        cursor.x = mainWindow.getXPos();
        cursor.y = mainWindow.getYPos();
        //cout << cursor.x << ":x " << cursor.y << ":y " << endl;
        //itemModel = scale(mat4(1.0f), vec3(0.08f, 0.1f, 0.08f)) *
        //    rotate(mat4(1.0f), radians(-90.0f), vec3(0, 0, 1)) *
        //    rotate(mat4(1.0f), radians(30.0f), vec3(1, 0, 0)) *
        //    rotate(mat4(1.0f), radians(45.0f), vec3(0, 1, 0)) *
        //    
        //rotate(mat4(1.0f), radians(inventory.mainInventorySlots[3][0].angle-=0.5), vec3(1, 0, 0));
        lastYaw = firstCamera.getYaw(); lastPitch = firstCamera.getPitch();
        thirdCamera_back.setPosition(firstCamera.getPosition() + firstCamera.getFront() * vec3(-5));
        if (abs(firstCamera.getPosition().x) > abs(mainLight.getShadowPos().x) + 200 ||
            abs(firstCamera.getPosition().z) > abs(mainLight.getShadowPos().z) + 200){
            mainLight.setShadowPos(firstCamera.getPosition());
        }
        if(person_view % 3 != 2)
            activeCamera.setFront(firstCamera.getFront());
            for (int i = -renderX; i < renderX; i++) {
                //if (count >= 8) { count = 0; break; } // Just in case to limit the number of chunks generated per frame
                for (int j = -renderY; j < renderY; j ++) {
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
                count++;
            } 
            if (renderX < renderDistance && count >= 0) {
                renderX++;
                renderY++;
            }
        
        Textures[BLOCK_TEX]->useTexture();
        shaders[0]->useShader();
        glUniform3f(glGetUniformLocation(shaders[0]->getShaderId(), "camPos"), firstCamera.getPosition().x, firstCamera.getPosition().y, firstCamera.getPosition().z);
        glUniform1f(glGetUniformLocation(shaders[0]->getShaderId(), "fogStart"), 0.7*CHUNK_SIZE * renderDistance);
        glUniform1f(glGetUniformLocation(shaders[0]->getShaderId(), "fogEnd"), 0.73*CHUNK_SIZE * renderDistance);

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

        Textures[BREAK_STAGE_TEX]->useTexture(GL_TEXTURE3);
        glUniform1i(glGetUniformLocation(shaders[0]->getShaderId(), "breakStageTexture"), 3);

        renderWorld();
        vec3 headPos, headFront;
        headPos = firstCamera.getPosition(), headFront = firstCamera.getFront();
        BlockData lookBlock = world.getBlockAt(lookingAtBlock());

        view = activeCamera.calcViewMatrix();
        if (projAngle > 45.0f) {
            projection = perspective(radians(projAngle), (float(mainWindow.getBufferWidth()) / float(mainWindow.getBufferHeight())), 0.01f, 1000.0f);
        }

        glUniformMatrix4fv(shaders[0]->getModelLocation(), 1, GL_FALSE, value_ptr(model));
        glUniformMatrix4fv(shaders[0]->getViewLocation(), 1, GL_FALSE, value_ptr(view));
        glUniformMatrix4fv(shaders[0]->getProjectionLocation(), 1, GL_FALSE, value_ptr(projection));
        glUniform3f(shaders[0]->getColorMaskLocation(), 1.0f, 1.0f, 1.0f);
        shaders[0]->setDirectionalLight(&mainLight); //replaced : mainLight.useLight(shaders[0]->getAmbientIntensityLocation(), shaders[0]->getAmbientColorLocation(), shaders[0]->getDiffuseIntensityLocation(), shaders[0]->getDirectionLocation());
        shaders[0]->setPointLights(pointLights, pointLightCount);

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

        if (mainWindow.getKeyPressed() >= GLFW_KEY_1 && mainWindow.getKeyPressed() <= GLFW_KEY_9) {
            slot = mainWindow.getKeyPressed() - GLFW_KEY_1; inventory.invDidChange(1);;
        }
        
        float lastPress = 0.0, maxDelay = 0.2 + glfwGetTime();
        if (!inventory.mainInventoryOn && !inventory.craftingTableInventoryOn) {
        if (mainWindow.getKeys()[GLFW_KEY_F]) {
            mainWindow.initializeFullScreen();
        }
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
            //breakModel = rotate(breakModel, radians(1.0f), vec3(0, 0, 1));
            breakAngle = -30.f;
        }
        if (mainWindow.getKeys()[GLFW_KEY_T] || mainWindow.leftClickButtonPressed()) {
            //std::this_thread::sleep_for(std::chrono::milliseconds(200));
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
                            placeReqQueue.push(vec3(1.0f));
                            blockPlacingOut = true;
                        }
                        //double now_ = glfwGetTime();
                        //while (!(now_ - lastPress > maxDelay)) { now_ = glfwGetTime(); }
                    }
                }
                else {
                    inventory.craftingTableInventoryOn = true;
                }
                breakAngle = -30.f;
            }

            breakAngle = (breakAngle < 0.0f) ? breakAngle + 5 : 0.0f;
            breakModel = ((breakAngle < 0.0f) ? rotate(mat4(1.0f), radians(breakAngle), vec3(0, 0, 1)) *
                translate(mat4(1.0f), vec3(-breakAngle, -breakAngle, 2 * -breakAngle)) : mat4(1.0f));
            //cout << breakAngle << " degs ";
            if (mainWindow.getKeys()[GLFW_KEY_CAPS_LOCK]) {
                projAngle = (projAngle < 90.0) ? projAngle + 4 : 90.0;
            }
            else {
                projAngle = (projAngle > 45.0) ? projAngle - 4 : 45.0;
            }
            if (mainWindow.getKeys()[GLFW_KEY_Q]) {
                int xdrop, ydrop;
                if (inventory.mainInventoryOn) {
                    xdrop = (int)slotX, ydrop = 3 - (int)slotY;
                }
                else {
                    xdrop = slot, ydrop = 3;
                }
                if (inventory.mainInventorySlots[ydrop][xdrop].item != AIR) {
                    dropped.push_back(Projectile());
                    dropped.back().shoot(firstCamera.getPosition() + normalize(firstCamera.getFront()), vec3(firstCamera.getFront().x, 0.25, firstCamera.getFront().z), inventory.mainInventorySlots[ydrop][xdrop].item);
                    dropped.back().mesh = world.createProjectileMesh(vec3(0), -4.0f, inventory.mainInventorySlots[ydrop][xdrop].item);
                    inventory.mainInventorySlots[ydrop][xdrop].count--;
                    inventory.invDidChange(1);
                }
            }

            if (mainWindow.getKeys()[GLFW_KEY_U]) {
                ball.shoot(firstCamera.getPosition(), firstCamera.getFront(), CLOUD, vec3(1.0f));
                ball.mesh = world.createMeshCube(vec3(0), -4.0f, ball.item);
            }

            if (mainWindow.getKeys()[GLFW_KEY_F3]) {
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                person_view = (++person_view % 3);
            }

            if (mainWindow.getKeys()[GLFW_KEY_F5]) {
                mat4 compassModel = translate(mat4(1.0f), headPos + headFront);
                glUniformMatrix4fv(shaders[0]->getModelLocation(), 1, GL_FALSE, value_ptr(compassModel));
                compassMesh.renderMesh();
                glUniformMatrix4fv(shaders[0]->getModelLocation(), 1, GL_FALSE, value_ptr(model));
            }

            if (!breakResQueue.empty()) {
                Block droppedItem = breakResQueue.front();
                breakAngle = (breakAngle <= -25.f) ? breakAngle + 1 : -30.f;
                dropped.back().shoot(droppedItem.position, vec3(-firstCamera.getFront().x, 0.5, -firstCamera.getFront().z), droppedItem.type);
                dropped.back().mesh = world.createProjectileMesh(vec3(0), -4.0f, droppedItem.type);
                dropped.push_back(Projectile());
                breakResQueue.pop();
            }
        }
        
        if (person_view == 0) {
            activeCamera = firstCamera;
        }
        else if (person_view == 1) {
            activeCamera = thirdCamera_back;
        }

        Textures[FACE_TEX]->useTexture();
        Textures[TOP_TEX]->unbindNextTexture(); // unbinding grass texture upon other operations
        
        mat4 modelHead = translate(mat4(1.0f), headPos);
        mat4 rotation(1.0f);
        vec3 dir = normalize(firstCamera.getFront());
        vec3 right = cross(vec3(0, 1, 0), dir);
        vec3 up = cross(dir, right);
        rotation[0] = vec4(normalize(right), 0.0f); rotation[1] = vec4(normalize(up), 0.0f); rotation[2] = vec4(normalize(-dir), 0.0f);
        modelHead *= rotation;
        glUniformMatrix4fv(shaders[0]->getModelLocation(), 1, GL_FALSE, value_ptr(modelHead));
        headMesh.renderMesh();
        Textures[SLOT_TEX]->useTexture();
        
        vec3 cameraPosition = firstCamera.getPosition();
        
        if (!blockExistsAt((vec3(ftoint(ball.position.x), ftoint(ball.position.y - 0.5), ftoint(ball.position.z))))) {
            tp = 1;
            if (ball.shot) {
                ball.update();
            }
            ball.model = translate(mat4(1.0f), ball.position) * rotate(mat4(1.0f), radians(ball.angle++), vec3(0.0f, 1.0f, 0.0f));
            glUniformMatrix4fv(shaders[0]->getModelLocation(), 1, GL_FALSE, value_ptr(ball.model));
            ball.draw();
            glUniformMatrix4fv(shaders[0]->getModelLocation(), 1, GL_FALSE, value_ptr(model));
        }
        else {
            ball.shot = 0;
            if (tp == 1) {
                firstCamera.setPosition(ball.position);
                ball.velocity = vec3(0);
                ball.initial_velocity = vec3(0);
                tp = 0;
            }
        }
        
        Textures[BLOCK_TEX]->useTexture();
        Textures[TOP_TEX]->useNextTexture();
        glUniform1i(glGetUniformLocation(shaders[0]->getShaderId(), "topTexture"), 2);
        for (Projectile& drop : dropped) {
            drop.shot = !blockExistsAt((vec3(ftoint(drop.position.x) + 0.5, drop.position.y, ftoint(drop.position.z) + 0.5)));
            if (drop.shot) {
                drop.update();
            }
            else {
                drop.velocity *= vec3(0, 1, 0);
                drop.initial_velocity = vec3(0);
            }
            
            drop.model = translate(mat4(1.0f), drop.position);
            drop.model *= rotate(mat4(1.0f), radians(drop.angle++), vec3(0, 1, 0));
            glUniformMatrix4fv(shaders[0]->getModelLocation(), 1, GL_FALSE, value_ptr(drop.model));
            drop.draw();
            glUniformMatrix4fv(shaders[0]->getModelLocation(), 1, GL_FALSE, value_ptr(model));
        }

        shaders[5]->useShader();
        view = activeCamera.calcViewMatrix();
        glUniformMatrix4fv(shaders[5]->getModelLocation(), 1, GL_FALSE, value_ptr(model));
        glUniformMatrix4fv(shaders[5]->getViewLocation(), 1, GL_FALSE, value_ptr(view));
        glUniformMatrix4fv(shaders[5]->getProjectionLocation(), 1, GL_FALSE, value_ptr(projection));

        //For block highlighting
        ivec3 lookPosition = lookingAtBlock();
        if (lookPosition.y >= 0) {
            mat4 modelLooking = translate(mat4(1.0f), vec3(lookPosition));
            glUniformMatrix4fv(shaders[5]->getModelLocation(), 1, GL_FALSE, value_ptr(modelLooking));
            lookingMesh.renderMesh();
            glUniformMatrix4fv(shaders[5]->getModelLocation(), 1, GL_FALSE, value_ptr(model));
        }

        glDisable(GL_DEPTH_TEST); // so crosshair draws on top
        shaders[1]->useShader();
        glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE);

        if (person_view == 0) {
            glUniformMatrix4fv(shaders[1]->getOrthoLocation(), 1, GL_FALSE, glm::value_ptr(ortho));
            crosshair.drawCrosshair();
        }

        shaders[2]->useShader();
        Textures[SLOT_TEX]->useTexture();
        inventory.defineHotbarSlotSelectorGeometry();
        inventory.drawHotbarSlotSelector();

        glUniformMatrix4fv(shaders[2]->getOrthoLocation(), 1, GL_FALSE, glm::value_ptr(ortho));
        Textures[MAIN_INV_TEX]->useTexture();
        glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE);
        inventory.drawHotbar();
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        Textures[LARGE_INV_TEX]->useTexture();
        if (mainWindow.getKeys()[GLFW_KEY_C]) {
            if (craftedItem.item != AIR) {
                for (int i = (sizeof(inventory.mainInventorySlots) / sizeof(inventory.mainInventorySlots[3])) - 1; i >= 0; i--) {
                    for (int j = 0; j < (sizeof(inventory.mainInventorySlots[3]) / sizeof(InventorySlot)); j++) {
                        if (inventory.mainInventorySlots[i][j].item == AIR
                            || inventory.mainInventorySlots[i][j].item == craftedItem.item
                            ) {
                            inventory.mainInventorySlots[i][j].item = craftedItem.item;
                            inventory.mainInventorySlots[i][j].count += craftedItem.count;
                            if (inventory.mainInventoryOn)
                                for (int k = 0; k < 2; k++) {
                                    for (int l = 0; l < 2; l++) {
                                        inventory.mainCraftingSlots[k][l].count--;
                                    }
                                }
                            else if (inventory.craftingTableInventoryOn) {
                                for (int k = 0; k < 3; k++) {
                                    for (int l = 0; l < 3; l++) {
                                        inventory.craftingTableSlots[k][l].count--;
                                    }
                                }
                            }
                            goto invcleared;
                        }
                    }
                }
            invcleared:
                craftedItem.count--;
                inventory.invDidChange(1);
            }
        }

        if (mainWindow.getKeys()[GLFW_KEY_E]) {
            std::this_thread::sleep_for(chrono::milliseconds(100));
            inventory.mainInventoryOn = !inventory.mainInventoryOn;
            inventory.craftingTableInventoryOn = false;
            inventory.invDidChange(1);
        }

        if (mainWindow.getKeys()[GLFW_KEY_RIGHT_SHIFT] && mainWindow.getKeys()[GLFW_KEY_E]) {
            for (int i = 0; i < (sizeof(inventory.mainInventorySlots) / sizeof(inventory.mainInventorySlots[3])); i++) {
                for (int j = 0; j < (sizeof(inventory.mainInventorySlots[3]) / sizeof(InventorySlot)); j++) {
                    if (inventory.mainInventorySlots[i][j].item == AIR) {
                        for (int k = 0; k < sizeof(inventory.mainCraftingSlots) / sizeof(inventory.mainCraftingSlots[0]); k++) {
                            for (int l = 0; l < sizeof(inventory.mainCraftingSlots[0]) / sizeof(InventorySlot); l++) {
                                if (inventory.mainCraftingSlots[k][l].count > 0) {
                                    inventory.mainInventorySlots[i][j] = inventory.mainCraftingSlots[k][l];
                                    inventory.mainCraftingSlots[k][l].count = 0;
                                    inventory.invDidChange(1);
                                    craftedItem.count = 0;
                                }
                            }
                        }
                        goto done;
                    }
                }
            }
            done:
            inventory.mainInventoryOn = false;
            inventory.craftingTableInventoryOn = false;
            //cursor = Cursor();
            firstCamera.mouseControl(lastXChange, lastYChange);
            inventory.invDidChange(1);
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

            inventory.drawMainInventorySlots(ortho, itemView, itemProj);

            if (mainWindow.getKeys()[GLFW_KEY_RIGHT_SHIFT] && mainWindow.getKeys()[GLFW_KEY_ENTER]) {
                if (mainWindow.getKeyPressed() >= GLFW_KEY_0 && mainWindow.getKeyPressed() <= GLFW_KEY_3) {
                    int x = (mainWindow.getKeyPressed() - GLFW_KEY_0) % 2, y = (mainWindow.getKeyPressed() - GLFW_KEY_0) / 2;
                    if (inventory.mainCraftingSlots[y][x].item == AIR || count == 0) {
                        inventory.mainCraftingSlots[y][x].item = inventory.mainInventorySlots[3 - (int)slotY][(int)slotX].item;
                        inventory.mainCraftingSlots[y][x].count += craftedItem.count;
                        inventory.mainInventorySlots[3 - (int)slotY][(int)slotX].count--;
                        inventory.invDidChange(1);
                    }
                }
            }

            //Crafting inventory slots are being drawn here.
            for (int i = 0; i < (sizeof(inventory.mainCraftingSlots) / sizeof(inventory.mainCraftingSlots[0])); i++) {
                mat4 mod = translate(mat4(1.0f), vec3(centerX, centerY, 0)), craftmod = translate(mat4(1.0f), vec3(centerX + 100, centerY, 0));;
                for (int j = 0; j < (sizeof(inventory.mainCraftingSlots[0]) / sizeof(InventorySlot)); j++) {
                    render3Din2D(itemModel, inventory.mainCraftingSlots[i][j].mesh, inventory.mainCraftingSlots[i][j].model, inventory.mainCraftingSlots[i][j].quadMesh, ortho, itemView, itemProj, inventory.mainCraftingSlots[i][j].item);
                    inventory.mainCraftingSlots[i][j].textCount.drawText(ortho);
                }
            }
            render3Din2D(itemModel, craftedItem.mesh, craftedItem.model, craftedItem.quadMesh, ortho, itemView, itemProj, craftedItem.item);
            craftedItem.textCount.drawText(ortho);

            render3Din2D(itemModel, cursor.mesh, translate(mat4(1.0f), vec3((float(cursor.x) / mainWindow.getBufferWidth())* WIDTH, ((float)cursor.y / mainWindow.getBufferHeight())* HEIGHT, 0)), cursor.quadMesh, ortho, itemView, itemProj, cursor.item);
            cursor.textCount.model = cursor.model;
            cursor.textCount.drawText(ortho);
            //inventory.invDidChange(1);
        }

        // crafting inventory GUI appears here
        if (inventory.craftingTableInventoryOn) {
            if (mainWindow.getKeys()[GLFW_KEY_RIGHT_SHIFT] && mainWindow.getKeys()[GLFW_KEY_ENTER]) {
                if (mainWindow.getKeyPressed() >= GLFW_KEY_0 && mainWindow.getKeyPressed() <= GLFW_KEY_8) {
                    int x = (mainWindow.getKeyPressed() - GLFW_KEY_0) % 3, y = (mainWindow.getKeyPressed() - GLFW_KEY_0) / 3;
                    if (inventory.craftingTableSlots[y][x].item == AIR) { // eg pressing 4 gives [5/3][5%3] = [1][2]
                        inventory.craftingTableSlots[y][x].item = inventory.mainInventorySlots[3 - (int)slotY][(int)slotX].item;
                        inventory.craftingTableSlots[y][x].count += craftedItem.count;
                        inventory.mainInventorySlots[3 - (int)slotY][(int)slotX].count--;
                    }
                }
                inventory.invDidChange(1);
            }

            Textures[CRAFT_GUI_TEX]->useTexture();
            glUniformMatrix4fv(shaders[2]->getOrthoLocation(), 1, GL_FALSE, glm::value_ptr(ortho));
            glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE);
                inventory.drawMainInventory();
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            Textures[SLOT_TEX]->useTexture();
            glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE);
                inventory.defineCrafingInvSlotSelectorGeometry();
                inventory.drawInvSlotSelector();
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            Textures[BLOCK_TEX]->useTexture();

            inventory.drawMainInventorySlots(ortho, itemView, itemProj);

            for (int i = 0; i < (sizeof(inventory.craftingTableSlots) / sizeof(inventory.craftingTableSlots[0])); i++) {
                for (int j = 0; j < (sizeof(inventory.craftingTableSlots[0]) / sizeof(InventorySlot)); j++) {
                    render3Din2D(itemModel, inventory.craftingTableSlots[i][j].mesh, inventory.craftingTableSlots[i][j].model, inventory.craftingTableSlots[i][j].quadMesh, ortho, itemView, itemProj, inventory.craftingTableSlots[i][j].item);
                    inventory.craftingTableSlots[i][j].textCount.drawText(ortho);
                }
            }
            render3Din2D(itemModel, craftedItem.mesh, craftedItem.model, craftedItem.quadMesh, ortho, itemView, itemProj, craftedItem.item);
            craftedItem.textCount.drawText(ortho);

            render3Din2D(itemModel, cursor.mesh, translate(mat4(1.0f), vec3((float(cursor.x) / mainWindow.getBufferWidth()) * WIDTH, ((float)cursor.y / mainWindow.getBufferHeight()) * HEIGHT, 0)), cursor.quadMesh, ortho, itemView, itemProj, cursor.item);
            cursor.textCount.model = cursor.model;
            cursor.textCount.drawText(ortho);
            //inventory.invDidChange(1);
        }

        position.replaceWord("position  x: " 
            + to_string((int)headPos.x) + " y: " 
            + to_string((int)headPos.y) + " z: " 
            + to_string((int)headPos.z) + " cursor count -> " 
            + to_string(cursor.count) + " block : " + itemTypeString[cursor.item.id] 
            + "\n"
            + "looking at "
            + itemTypeString[lookBlock.blockType.id]
            , vec3(0.4, 1, 0.7));
        craftedItemName.replaceWord("main craft slot 1 contains: " + itemTypeString[craftedItem.item.id]
            + ", " + to_string(inventory.mainCraftingSlots[0][1].count) + (inventory.mainCraftingSlots[0][1].count <= 1?" item":" items")
            + (inventory.invChange() ? " inventory updating...." : " inventory up to date!"), normalize(vec3(1.3, 1, 0)), vec2(50, 1600));

        cursorPos.replaceWord("cursor position: x = " + to_string(cursor.x) + ", y = " + to_string(cursor.y), vec3(0, 1, 0), vec2(50, 1550));
        position.drawText(ortho), craftedItemName.drawText(ortho), cursorPos.drawText(ortho);
        
        for (int i = 0; i < 9; i++) {
            mat4 itemModel = scale(mat4(1.0f), vec3(0.1f, 0.12f, 0.1f)) * rotate(mat4(1.0f), radians(-90.0f), vec3(0, 0, 1)) * 
                             ((!inventory.hotbarSlots[i].item.isFlat()) ? rotate(mat4(1.0f), radians( 30.0f), vec3(1, 0, 0)) * rotate(mat4(1.0f), radians(45.0f), vec3(0, 1, 0)) : mat4(1.0f)) *
                             rotate(mat4(1.0f), radians(inventory.hotbarSlots[i].angle), vec3(0, 1, 0));
            inventory.hotbarSlots[i].angle += 0.5f;
            render3Din2D(itemModel, inventory.hotbarSlots[i].mesh, inventory.hotbarSlots[i].model, inventory.hotbarSlots[i].quadMesh, ortho, itemView, itemProj, inventory.hotbarSlots[i].item);
            inventory.hotbarSlots[i].textCount.drawText(ortho);
        }
        Textures[BLOCK_TEX]->useTexture();
        inventory.updateCurrentBlock();
        render3Din2D(itemModel * breakModel
                               * rotate(mat4(1.0f), radians((!currentBlock.item.isTool() ?-35.f :  0.f)), vec3(1, 1, 1))
                               * rotate(mat4(1.0f), radians((!currentBlock.item.isTool() ? 45.f :  0.f)), vec3(0, 1, 0))
                               //* rotate(mat4(1.0f), radians(( currentBlock.item.isTool() ?180.f :  0.f)), vec3(0, 1, 0))
                               //* rotate(mat4(1.0f), radians(( currentBlock.item.isTool() ? 75.f :  0.f)), vec3(0, 0, 1))
                                 , currentBlock.mesh, translate(mat4(1.0f), vec3(centerX + 600 - 20 * (firstCamera.getYaw() - lastYaw), centerY - 650 - 20 * (firstCamera.getPitch() - lastPitch) - 2 * (firstCamera.initial_velocity.y + firstCamera.velocity.y), 0)), currentBlock.quadMesh, ortho, currentBlockView, itemProj, currentBlock.item);

        if (inventory.invChange()) { inventory.updateInventory(); }

        if (mainWindow.getShouldClose()) {
            chunkGenRunning = false;
            queueCV.notify_all(); // wake up sleeping threads

            for (auto& t : workers)
                t.join();

            chunkGenRunning = false;
            chunkGenRunning2 = false;
            chunkUpdateGenRunning = false;
            stopChunkUpdaters = true;
            blockPlacing = false;
            blockBreaking = false;//chunkGenThread.join(); //chunkGenThread2.join(); //chunkGenThread3.join();
            blockBreakThread.join();
            blockPlaceThread.join();
            //chunkUpdateThread.join();
        }

        if (spawn <= 511) {
            spawn++;
        }

        bool onGround = blockExistsAt(vec3(ftoint(cameraPosition.x + 0.5), cameraPosition.y - 1.5, ftoint(cameraPosition.z + 0.5)));
        if (!onGround && spawn > 511) {
            firstCamera.initial_velocity.y -= 0.5;
            //firstCamera.velocity_factor = vec3(1, 1, 1);
            //firstCamera.calculateCamPos(0.01); // 0.01 instead of dt
            //cout << "\x1b[2J\x1b[H"; // clears console
        }
        else if (onGround) {
            firstCamera.velocity = vec3(0);
            firstCamera.initial_velocity *= vec3(0);
            firstCamera.velocity_factor = vec3(1);
            //firstCamera.initial_velocity.y += 0.5;
        }
        mainWindow.swapBuffers();
    }
}////

void addTextures() {
    Textures.push_back(make_unique<GL_Texture>("textures/block_atlas_32.png"    ));//#define BLOCK_TEX           0
    Textures.push_back(make_unique<GL_Texture>("textures/clear_toolbar_2.png"   ));//#define MAIN_INV_TEX        1
    Textures.push_back(make_unique<GL_Texture>("textures/clear_toolbar_3.png"   ));//#define SLOT_TEX            2
    Textures.push_back(make_unique<GL_Texture>("textures/main_inventory.png"    ));//#define LARGE_INV_TEX       3
    Textures.push_back(make_unique<GL_Texture>("textures/tools_atlas_4.png"     ));//#define TOOLS_TEX           4
    Textures.push_back(make_unique<GL_Texture>("textures/crafting_table_gui.png"));//#define CRAFT_GUI_TEX       5
    Textures.push_back(make_unique<GL_Texture>("textures/block_overlay_2.png"   ));//#define TOP_TEX             6
    Textures.push_back(make_unique<GL_Texture>("textures/steve_face.jpg"        ));//#define FACE_TEX            7
    Textures.push_back(make_unique<GL_Texture>("textures/break_stage_3.png"     ));//#define BLOCK_STAGES_TEX    8
    Textures.push_back(make_unique<GL_Texture>("textures/text.png"              ));//#define TEXT_TEX            9
    //Textures.push_back(new Texturegl("textures\\inventory_base.png"));

    for (int i = BLOCK_TEX; i < Textures.size(); i++) { Textures[i]->loadTexture(); }
}

void renderWorld() {
    for (auto& chunks : world.chunkData) {
        ivec2 coords = chunks.second->coords();
        Chunk& chunk = *chunks.second;

        if (!(chunk.neighboursPresent & 1)) {
            if ((chunk.neighboursPresent & 0x1E) != 0x1E) {
                int dirsX[] = { -1, 0, 0, 1 }, dirsY[] = { 0, -1, 1, 0 };
                for (int i = 0; i < 4; i++) {
                    if (world.chunkData.count(pack(ivec2(coords.x + dirsX[i], coords.y + dirsY[i]))) > 0) {
                        chunk.neighboursPresent |= (1 << (i + 1));
                    }
                }
            }
            if (chunk.neighboursPresent == 0x1E) { // 1 1110
                {
                    std::lock_guard<std::mutex> lock(chunkUpdateRequestMutex);
                    chunkUpdateRequestQueue.push(chunk.coords());
                }
                chunkUpdateCV.notify_one();
                chunk.neighboursPresent |= 1;
                
            }
            continue;
        }
        
        if (chunk.mesh) {
            if ((coords.x >= firstCamera.getPosition().x / CHUNK_SIZE - renderDistance * 3 && coords.x <= firstCamera.getPosition().x / CHUNK_SIZE + renderDistance * 3) &&
                (coords.y >= firstCamera.getPosition().z / CHUNK_SIZE - renderDistance * 3 && coords.y <= firstCamera.getPosition().z / CHUNK_SIZE + renderDistance * 3)) {

                if (chunks.second->unloaded) {
                    { chunkUpdateRequestQueue.push(chunk.coords()); /*std::lock_guard<std::mutex> lock(chunkUpdateRequestMutex); */ }
                    chunk.needUpdate = true;
                    chunk.unloaded = false;
                }
                if (chunk.needUpdate && chunk.mesh) {
                    chunk.mesh->createMesh(chunk.mesh->vertices, chunk.mesh->indices, chunk.mesh->vertices.size(), chunk.mesh->indices.size());
                    chunk.needUpdate = false;
                }
                if (mainWindow.getKeys()[GLFW_KEY_M])
                    chunk.mesh->renderMeshAsLines();
                else
                    chunk.mesh->renderMesh();
            }
            else {
                if (chunk.mesh && chunk.mesh->vertices.size()) {
                    chunk.mesh->vertices.clear();
                    chunk.mesh->indices.clear();
                    chunk.mesh.reset();
                    chunk.unloaded = true;
                }
            }
        }
    }
}

void renderShadowWorld(){
    for (auto& chunks : world.chunkData) {
        ivec2 coords = chunks.second->coords();
        Chunk& chunk = *chunks.second;
        if (chunk.mesh) {
            if ((coords.x >= firstCamera.getPosition().x / CHUNK_SIZE - renderDistance * 1.5 && coords.x <= firstCamera.getPosition().x / CHUNK_SIZE + renderDistance * 1.5) &&
                (coords.y >= firstCamera.getPosition().z / CHUNK_SIZE - renderDistance * 1.5 && coords.y <= firstCamera.getPosition().z / CHUNK_SIZE + renderDistance * 1.5)) {

                if (chunks.second->unloaded) {
                    { chunkUpdateRequestQueue.push(chunk.coords()); /*std::lock_guard<std::mutex> lock(chunkUpdateRequestMutex); */ }
                    chunk.needUpdate = true;
                    chunk.unloaded = false;
                }
                if (chunk.needUpdate) {
                    chunk.mesh->createMesh(chunk.mesh->vertices, chunk.mesh->indices, chunk.mesh->vertices.size(), chunk.mesh->indices.size());
                    chunk.needUpdate = false;
                }
                if (mainWindow.getKeys()[GLFW_KEY_M])
                    chunk.mesh->renderMeshAsLines();
                else
                    chunk.mesh->renderMesh();
            }
        }
    }
}

void directionalShadowPass(DirectionalLight* light, mat4 model) {
    directionalShadowShader->useShader();
    directionalShadowShader->setDirectionalLightTransform(light->calcLightTransform());
    glUniformMatrix4fv(directionalShadowShader->getModelLocation(), 1, GL_FALSE, value_ptr(model));
    light->shadow_map->write();
    glClear(GL_DEPTH_BUFFER_BIT);
    renderShadowWorld();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    shaders[0]->useShader();
    shaders[0]->setDirectionalLightTransform(light->directionalLightTransform);
}////

int ftoint(float num) {
    return num >= 0 ? num : num - 1;
}