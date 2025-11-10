#define GLEW_STATIC

#include "libraries.h"

PointLight pointLights[MAX_POINT_LIGHTS];
unsigned int pointLightCount = 0;

#include "items.h"
#include "Block.h"
#include "shaderlist.h"
//#include "Chunk.h"
#include "recipes.h"
//#include "Frustum.h"
#include "normals.h"
#include "inventory.h"
#include "Frustum.h"
#include "World.h"
#include "threading.h"
#include "Sky.h"

using namespace std;
using namespace glm;

#define WIDTH         1800
#define HEIGHT        1800

//(DONE!!!) TO DO : Implement spectator mode kind of face culling;;;; Block right next to block of Air can be rendered. 

int renderDistance = 26;

Sky sky;
Window mainWindow;

vector<Texturegl*> Textures;
DirectionalLight mainLight;

GLfloat deltaTime = 2.0f;
GLfloat lastTime = 0.0f;

int currentBlockType = 1;

void renderWorld(mat4 view, mat4 projection) {
    for (auto& chunks : world.chunkData) {
        Chunk& chunk = chunks.second;
        if ((chunk.coords.x >= camera.getCameraPos().x / CHUNK_SIZE - renderDistance * 1.5 && chunk.coords.x <= camera.getCameraPos().x / CHUNK_SIZE + renderDistance * 1.5) &&
            (chunk.coords.y >= camera.getCameraPos().z / CHUNK_SIZE - renderDistance * 1.5 && chunk.coords.y <= camera.getCameraPos().z / CHUNK_SIZE + renderDistance * 1.5)) {
        
            if (chunk.needUpdate) {
                //regenerateChunk(chunk.coords, chunk);
                chunk.mesh.createMesh(chunk.vertices, chunk.indices, chunk.vertices.size(), chunk.indices.size());
                chunk.needUpdate = false;
            }
            chunk.mesh.renderMesh();
        }
    }
}

#define BLOCK_TEX           0
#define MAIN_INV_TEX        1
#define SLOT_TEX            2
#define LARGE_INV_TEX       3
#define TOOLS_TEX           4
#define CRAFT_GUI_TEX       5

int main()
{
    mainWindow = Window(WIDTH, HEIGHT);
    mainWindow.initialize(); 

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glfwSwapInterval(0);
    /*Textures.push_back(new Texturegl("textures\\block_atlas_4.png"));*/
    Textures.push_back(new Texturegl("textures\\block_atlas_27.png"));
    Textures.push_back(new Texturegl("textures\\clear_toolbar_2.png"));
    Textures.push_back(new Texturegl("textures\\clear_toolbar_3.png"));
    Textures.push_back(new Texturegl("textures\\main_inventory.jpg"));
    Textures.push_back(new Texturegl("textures\\tools_atlas_3.png"));
    Textures.push_back(new Texturegl("textures\\crafting_table_gui.png"));
    //Textures.push_back(new Texturegl("textures\\inventory_base.png"));

    Textures[BLOCK_TEX]->loadTexture();
    Textures[MAIN_INV_TEX]->loadTexture();
    Textures[SLOT_TEX]->loadTexture();
    Textures[LARGE_INV_TEX]->loadTexture();
    Textures[TOOLS_TEX]->loadTexture();
    Textures[CRAFT_GUI_TEX]->loadTexture();

    createShaders();
    worldBlocks.clear();
    

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CCW);

    camera.setCameraPos(vec3(CHUNK_SIZE / 2, CHUNK_SIZE * CHUNK_SIZE / 2, CHUNK_SIZE / 2));

    float centerX = WIDTH / 2.0f;
    float centerY = HEIGHT / 2.0;
    float size = 10.0f;

    std::vector<glm::vec2> crosshairVertices = {
        { centerX - size, centerY }, { centerX + size, centerY }, // horizontal line
        { centerX, centerY - 2 * size }, { centerX, centerY + 2 * size }  // vertical line
    };

    vector<GLfloat> vers = {
        crosshairVertices[0].x, crosshairVertices[0].y, 0.0, 0.0f, 0.0f,
        crosshairVertices[1].x, crosshairVertices[1].y, 0.0, 1.0f, 0.0f,
        crosshairVertices[2].x, crosshairVertices[2].y, 0.0, 0.0f, 1.0f,
        crosshairVertices[3].x, crosshairVertices[3].y, 0.0, 1.0f, 1.0f
    };

    vector<unsigned int> inds = {
        0, 1, 2,
        1, 2, 3
    };

    Mesh crosshair;
    crosshair.createMesh(vers, inds, 20, 6);

    int invSizeX = 310;
    int invSizeY = 65;
    int invHeight = 700;
    int slotLength = 69;
    std::vector<glm::vec2> inventoryVertices = {
        { centerX - invSizeX, centerY - invSizeY - invHeight}, { centerX + invSizeX, centerY - invSizeY - invHeight},
        { centerX - invSizeX, centerY + invSizeY - invHeight}, { centerX + invSizeX, centerY + invSizeY - invHeight},

        { centerX - invSizeX + slotLength, centerY - invSizeY - invHeight}, { centerX - invSizeX + slotLength, centerY + invSizeY - invHeight}
    };
    vector<GLfloat> versInv = {
        inventoryVertices[0].x, inventoryVertices[0].y, 0.0, 0.0f, 0.0f, 1.0f,             0.0f, 0.0f, 0.0f,
        inventoryVertices[1].x, inventoryVertices[1].y, 0.0, 1.0f, 0.0f, 1.0f,             0.0f, 0.0f, 0.0f,
        inventoryVertices[2].x, inventoryVertices[2].y, 0.0, 0.0f, 1.0f, 1.0f,             0.0f, 0.0f, 0.0f,
        inventoryVertices[3].x, inventoryVertices[3].y, 0.0, 1.0f, 1.0f, 1.0f,             0.0f, 0.0f, 0.0f
    };

    vector<unsigned int> indsInv = {
        0, 2, 1,
        1, 2, 3
    };

    int offsetX = 600, offsetY = -800;

    vector<GLfloat> versInvBlock = {
        centerX + offsetX + 0.0f,   centerY + offsetY + 0.0f, 0.0, 0.0f, 0.0f, 1.0f,       0.0f, 0.0f, 0.0f,
        centerX + offsetX + 100.0f, centerY + offsetY + 0.0f, 0.0, 1.0f, 0.0f, 1.0f,       0.0f, 0.0f, 0.0f,
        centerX + offsetX + 0.0f,   centerY + offsetY + 100.0f, 0.0, 0.0f, 1.0f, 1.0f,     0.0f, 0.0f, 0.0f,
        centerX + offsetX + 100.0f, centerY + offsetY + 100.0f, 0.0, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f, 0.0f
    };

    vector<unsigned int> indsInvBlock = {
        0, 1, 2,
        1, 3, 2
    };

    Mesh inventoryMesh, currInvSlotSelector, craftInvSlotSelector;
    Block currentBlock;

    mat4 model(1.0f), projection(1.0f), view(1.0f);
    projection = perspective(radians(45.0f), (float(mainWindow.getBufferWidth()) / float(mainWindow.getBufferHeight())), 0.5f, 500.0f);    

    glm::mat4 ortho = glm::ortho(0.0f, float(WIDTH), 0.0f, float(HEIGHT));

    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, crosshairVertices.size() * sizeof(glm::vec2), &crosshairVertices[0], GL_STATIC_DRAW);

    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    int offs = 1, ctrl = 0;
    inventoryMesh.createMesh(versInv, indsInv, 24, 6);
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
    //world.chunks.push_back(Chunk());
    invSizeX = centerX / 3;
    invSizeY = centerY / 2;
    invHeight = 0;
    slotLength = 69;
    std::vector<glm::vec2> inventoryVertices2 = {
        { centerX - invSizeX, centerY - invSizeY - invHeight}, { centerX + invSizeX, centerY - invSizeY - invHeight},
        { centerX - invSizeX, centerY + invSizeY - invHeight}, { centerX + invSizeX, centerY + invSizeY - invHeight},

        { centerX - invSizeX + slotLength, centerY - invSizeY - invHeight}, { centerX - invSizeX + slotLength, centerY + invSizeY - invHeight}
    };
    vector<GLfloat> versInv2 = {
        inventoryVertices2[0].x, inventoryVertices2[0].y, 0.0, 0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 0.0f,
        inventoryVertices2[1].x, inventoryVertices2[1].y, 0.0, 1.0f, 0.0f, 1.0f,   0.0f, 0.0f, 0.0f,
        inventoryVertices2[2].x, inventoryVertices2[2].y, 0.0, 0.0f, 1.0f, 1.0f,   0.0f, 0.0f, 0.0f,
        inventoryVertices2[3].x, inventoryVertices2[3].y, 0.0, 1.0f, 1.0f, 1.0f,   0.0f, 0.0f, 0.0f
    };

    vector<unsigned int> indsInv2 = {
        0, 2, 1,
        1, 2, 3
    };
    Mesh mainInventory, craftingInventory;
    Block craftedItem(vec3(0.0f), AIR, {}, {});
    mainInventory.createMesh(versInv2, indsInv2, versInv2.size(), indsInv2.size());
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

    //Hand arm;
    //arm.createHand();
    while (!mainWindow.getShouldClose()) {
        mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
                                     0.2 + 1.2 * time / maxTime,
                                     0.0000f, -0.7071f, 0.7071f, 
                                     0.85 * time / maxTime);

        for (int i = -renderX; i < renderX; i++) {
            for (int j = -renderY; j < renderY; j += 1) {
                ivec2 chunkPos = ivec2(floorDiv(camera.getCameraPos().x, CHUNK_SIZE) + i, floorDiv(camera.getCameraPos().z, CHUNK_SIZE) + j);
                if (chunkCoords.count(chunkPos) <= 0) {
                //if (find(chunkCoords.begin(), chunkCoords.end(), vec2(floor(camera.getCameraPos().x / CHUNK_SIZE) + i, floor(camera.getCameraPos().z / CHUNK_SIZE) + j)) == chunkCoords.end()) {
                    //chunkCoords.push_back({ floor(camera.getCameraPos().x / CHUNK_SIZE) + i, floor(camera.getCameraPos().z / CHUNK_SIZE) + j });
                    chunkCoords.insert({ chunkPos });
                    //world.chunks.push_back(Chunk());
                    //generateChunkAt(chunkCoords.back(), world.chunks.back());
                    //{
                    //    chunkGenRunning = true;
                    //    std::lock_guard<std::mutex> lock(chunkRequestMutex);
                    //    chunkRequestQueue.push(chunkCoords.back());
                    //}
                    {
                        chunkGenRunning = true;
                        std::lock_guard<std::mutex> lock(queueMutex);
                        chunkRequestQueue.push(chunkPos);
                        queueCV.notify_one();
                    }

                }
            }
        }

        {
            std::lock_guard<std::mutex> lock(resultMutex);
            while (!chunkResultQueue.empty()) {
                Chunk chunk = std::move(chunkResultQueue.front());
                chunkResultQueue.pop();
                world.addChunk(std::move(chunk), ivec2(chunk.coords));
            }
        }

        //{
        //    std::lock_guard<std::mutex> lock(chunkResultMutex);
        //    while (!chunkResultQueue.empty()) {
        //        Chunk chunk = std::move(chunkResultQueue.front());
        //        chunkResultQueue.pop();
        //        world.addChunk(std::move(chunk), ivec2(chunk.coords));
        //    }
        //}
 
        if (renderX < renderDistance) {
            renderX++;
            renderY++;
        }
        
        Textures[BLOCK_TEX]->useTexture();
        shaders[0]->useShader();
        GLfloat now = glfwGetTime();
        deltaTime = now - lastTime;
        lastTime = now;

        if (time >= 3 * maxTime / 5) {
            night = true;
        }
        else if(time <= lowTime){
            night = false;
        }

        if (mainWindow.getKeys()[GLFW_KEY_LEFT]) {
            time-=1.5;
        }
        else if(mainWindow.getKeys()[GLFW_KEY_RIGHT]){
            time+=1.5;
        }

        //glClearColor(0.2f + 0.1 * time / maxTime,time / maxTime + 0.1, 0.4 + time / maxTime, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwPollEvents();
        view = camera.calcViewMatrix();

        int sensitivity = 1.01f;
        camera.keyControl(mainWindow.getKeys(), deltaTime);
        if (inventory.inventoryOn || inventory.craftingInventoryOn) {
            glfwSetInputMode(mainWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            camera.mouseControl(lastXChange, lastYChange);
        }
        else {
            glfwSetInputMode(mainWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
            lastXChange = mainWindow.getXChange(); lastYChange = mainWindow.getYChange();
        }
        
        glUniformMatrix4fv(shaders[0]->getModelLocation(), 1, GL_FALSE, value_ptr(model));
        glUniformMatrix4fv(shaders[0]->getViewLocation(), 1, GL_FALSE, value_ptr(view));
        glUniformMatrix4fv(shaders[0]->getProjectionLocation(), 1, GL_FALSE, value_ptr(projection));

        //mainLight.useLight(shaders[0]->getAmbientIntensityLocation(), shaders[0]->getAmbientColorLocation(), shaders[0]->getDiffuseIntensityLocation(), shaders[0]->getDirectionLocation());
        shaders[0]->setDirectionalLight(&mainLight);
        shaders[0]->setPointLights(pointLights, pointLightCount);

        if (mainWindow.getKeys()[GLFW_KEY_SPACE]) {
            //cout << jumping << " " << ctrlJump << endl;

            //if (flying) {
            //    jumpCount += 0.1f;
            //}
            //if(jumping)
            camera.setCameraPos(vec3(camera.getCameraPos().x, camera.getCameraPos().y + 0.1f, camera.getCameraPos().z));
            //if (!ctrlJump) {
            //    jumping = true;
            //}
            //if (mainWindow.getKeys()[GLFW_KEY_SPACE]) {
            //    flying = !flying;
            //}
        }
        else {
            ctrl = 20;
        }

        if (mainWindow.getKeys()[GLFW_KEY_1]) {
            currentBlockType = 1;
            slot = 0;
            inv_change = true;
        }
        else if (mainWindow.getKeys()[GLFW_KEY_2]) {
            currentBlockType = 2;
            slot = 1;
            inv_change = true;
        }
        else if (mainWindow.getKeys()[GLFW_KEY_3]) {
            currentBlockType = 3;
            slot = 2;
            inv_change = true;
        }
        else if (mainWindow.getKeys()[GLFW_KEY_4]) {
            currentBlockType = 4;
            slot = 3;
            inv_change = true;
        }
        else if (mainWindow.getKeys()[GLFW_KEY_5]) {
            currentBlockType = 5;
            slot = 4;
            inv_change = true;
        }
        else if (mainWindow.getKeys()[GLFW_KEY_6]) {
            currentBlockType = 6;
            slot = 5;
            inv_change = true;
        }
        else if (mainWindow.getKeys()[GLFW_KEY_7]) {
            //currentBlockType = 6;
            slot = 6;
            inv_change = true;
        }
        else if (mainWindow.getKeys()[GLFW_KEY_8]) {
            //currentBlockType = 6;
            slot = 7;
            inv_change = true;
        }
        else if (mainWindow.getKeys()[GLFW_KEY_9]) {
            //currentBlockType = 6;
            slot = 8;
            inv_change = true;
        }
        if (mainWindow.getKeys()[GLFW_KEY_P]) {

            if (mainWindow.getKeys()[GLFW_KEY_I]) {
                //currentBlockType = 6;
                inventory.inf_blocks = true;
            }
            else if (mainWindow.getKeys()[GLFW_KEY_N]) {
                inventory.inf_blocks = false;
            }
            //slot = currentBlockType - 1;
            if (mainWindow.getKeys()[GLFW_KEY_1])
                world.addBlocklook_at(items[TORCH.id]);
            else if (mainWindow.getKeys()[GLFW_KEY_2])
                world.addBlocklook_at(items[2]);
            else if (mainWindow.getKeys()[GLFW_KEY_3])
                world.addBlocklook_at(items[3]);
            else if (mainWindow.getKeys()[GLFW_KEY_4])
                world.addBlocklook_at(items[4]);
            else if (mainWindow.getKeys()[GLFW_KEY_5])
                world.addBlocklook_at(items[5]);
            else if (mainWindow.getKeys()[GLFW_KEY_6])
                world.addBlocklook_at(items[6]);
            else if (mainWindow.getKeys()[GLFW_KEY_7])
                world.addBlocklook_at(items[7]);
            else if (mainWindow.getKeys()[GLFW_KEY_8])
                world.addBlocklook_at(items[8]);
            else if (mainWindow.getKeys()[GLFW_KEY_9])
                world.addBlocklook_at(items[9]);
            else if (mainWindow.getKeys()[GLFW_KEY_0])
                world.addBlocklook_at(CRAFTING_TABLE);
            else {
                world.addBlocklook_at(inventory.inv_slots[3][slot]);
            }

        }

        if (mainWindow.getKeys()[GLFW_KEY_T] || mainWindow.leftClickButtonPressed()) {
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
            Block lookBlock = world.getBlockAt(lookingAtBlock());
            if (!recipe.itemUsable(lookBlock.type)) {
                if (inventory.inv_slots[3][slot] != AIR && recipe.itemPlaceable(inventory.inv_slots[3][slot])) {
                    {
                        std::lock_guard<std::mutex> lock(placeReqMutex);
                        placeReqQueue.push(vec3(1.0f));
                        blockPlacingOut = true;
                    }
                    {
                        //std::lock_guard<std::mutex> lock(placeResMutex);
                        if (!placeResQueue.empty()) {
                            placeResQueue.pop();
                        }
                    }
                }
                if (!inventory.inf_blocks) {
                    inventory.deassignInvSlot(slot, 3);
                }
            }
            else {
                inventory.craftingInventoryOn = true;
            }
        }

        if (mainWindow.getShouldClose()) {
            chunkGenRunning = false;
            queueCV.notify_all(); // wake up sleeping threads

            for (auto& t : workers)
                t.join();

            chunkGenRunning = false;
            chunkGenRunning2 = false;
            chunkGenRunning3 = false;
            blockPlacing = false;
            blockBreaking = false;
            //chunkGenThread.join();
            //chunkGenThread2.join();
            //chunkGenThread3.join();
            blockBreakThread.join();
            blockPlaceThread.join();
        }
        if (mainWindow.getKeys()[GLFW_KEY_Q]) {
            inventory.inv_slots[3][slot] = AIR;
            inventory.currInvSlot[3][slot].clearMesh();
            inv_change = true;
        }
        sky.applySky(view, projection);
        glEnable(GL_DEPTH_TEST);

        shaders[0]->useShader();
        renderWorld(view, projection);

        shaders[13]->useShader();
        view = camera.calcViewMatrix();
        glUniformMatrix4fv(shaders[13]->getModelLocation(),         1, GL_FALSE, value_ptr(model));
        glUniformMatrix4fv(shaders[13]->getViewLocation(),          1, GL_FALSE, value_ptr(view));
        glUniformMatrix4fv(shaders[13]->getProjectionLocation(),    1, GL_FALSE, value_ptr(projection));

        //For block highlighting

        ivec3 lookingPos;
        {
            std::lock_guard<std::mutex> lock(chunkRequestMutex);
            lookingPos = ivec3(lookingAtBlock());
        }
        Block cloud = world.getBlockAt(lookingPos);
        cloud = world.createMeshCube(cloud.position, 0.05f, CLOUD);
        cloud.blockMesh.createMesh(cloud.vertices, cloud.indices, cloud.vertices.size(), cloud.indices.size());
        cloud.blockMesh.renderMesh();

        glDisable(GL_DEPTH_TEST); // so crosshair draws on top
        shaders[1]->useShader();
        //glEnable(GL_BLEND);
        glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE);

        glUniformMatrix4fv(glGetUniformLocation(shaders[1]->getShaderId(), "ortho"), 1, GL_FALSE, glm::value_ptr(ortho));
        glBindVertexArray(vao);
        glLineWidth(3.0f);
        glDrawArrays(GL_LINES, 0, 4);

        //glDisable(GL_BLEND);

        //Textures[BLOCK_TEX]->useTexture();
        //shaders[0]->useShader();
        ////arm.drawHand(ortho);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
                            //cout << bigCraftInv[0][0] << endl;
                        }
                    }
                }
                if (recipe.getRecipe(inventory.bigCraftInv) != AIR) {
                    craftedItem.blockMesh = world.createMeshCube(centerX / 5 + 315, centerY / 4 + 200.0f, 0.0f, 35.0f, recipe.getRecipe(inventory.bigCraftInv));
                }
                craftedItem.type = recipe.getRecipe(inventory.bigCraftInv);

            }

            vector<GLfloat> versInvSlotSelector = {
                inventoryVertices[0].x + (slot) * 69 - 5, inventoryVertices[0].y - 5, 0.0, 0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,
                inventoryVertices[4].x + (slot) * 69 + 5, inventoryVertices[4].y - 5, 0.0, 0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 0.0f,
                inventoryVertices[2].x + (slot) * 69 - 5, inventoryVertices[2].y + 5, 0.0, 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,
                inventoryVertices[5].x + (slot) * 69 + 5, inventoryVertices[5].y + 5, 0.0, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f, 0.0f
            };

            vector<unsigned int> indsInvSlotSelector = {
                0, 2, 1,
                1, 2, 3
            };
            currInvSlotSelector.createMesh(versInvSlotSelector, indsInvSlotSelector, 24, 6);

            inv_change = false;
        }

        shaders[2]->useShader();
        glUniformMatrix4fv(glGetUniformLocation(shaders[2]->getShaderId(), "ortho"), 1, GL_FALSE, glm::value_ptr(ortho));
        Textures[MAIN_INV_TEX]->useTexture();
        glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE);
        inventoryMesh.renderMesh();
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

        if (mainWindow.getKeys()[GLFW_KEY_RIGHT_SHIFT]) {
            if (mainWindow.getKeys()[GLFW_KEY_E]) {
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
        }

        //float deltaTIme = 1.0f;

        if (mainWindow.getKeys()[GLFW_KEY_UP]) {
            if (slotY > -1 && slotY < 3) {
                slotY+=1.0f;
            }
        }

        if (mainWindow.getKeys()[GLFW_KEY_DOWN]) {
            if (slotY > 0 && slotY < 4) {
                slotY-=1.0f;
            }
        }

        if (mainWindow.getKeys()[GLFW_KEY_LEFT]) {
            if (slotX > 0 && slotX < 9) {
                slotX-=1.0f;
            }
        }

        if (mainWindow.getKeys()[GLFW_KEY_RIGHT]) {
            if (slotX > -1 && slotX < 8) {
                slotX+=1.0f;
            }
        }

        if (inventory.inventoryOn) {

            Textures[LARGE_INV_TEX]->useTexture();
            shaders[2]->useShader();
            glUniformMatrix4fv(glGetUniformLocation(shaders[2]->getShaderId(), "ortho"), 1, GL_FALSE, glm::value_ptr(ortho));
            glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE);
            mainInventory.renderMesh();
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            vector<GLfloat> versCraftInvSlotSelector = {
                inventoryVertices[0].x + int(slotX) * 62 + 28, inventoryVertices[0].y + 323.0f + 10.0f + 5.0f + int(slotY) * 100 + (((int)slotY == 0) ? 0 : 20), 0.0, 0.0f, 0.0f, 1.0f,     0.0f, 0.0f, 0.0f,
                inventoryVertices[4].x + int(slotX) * 62 + 28, inventoryVertices[4].y + 323.0f + 10.0f + 5.0f + int(slotY) * 100 + (((int)slotY == 0) ? 0 : 20), 0.0, 0.0f, 1.0f, 1.0f,     0.0f, 0.0f, 0.0f,
                inventoryVertices[2].x + int(slotX) * 62 + 28, inventoryVertices[2].y + 323.0f + +5.0f + int(slotY) * 100 + (((int)slotY == 0) ? 0 : 20), 0.0, 1.0f, 0.0f, 1.0f,            0.0f, 0.0f, 0.0f,
                inventoryVertices[5].x + int(slotX) * 62 + 28, inventoryVertices[5].y + 323.0f + +5.0f + int(slotY) * 100 + (((int)slotY == 0) ? 0 : 20), 0.0, 1.0f, 1.0f, 1.0f,            0.0f, 0.0f, 0.0f
            };

            vector<unsigned int> indsCraftInvSlotSelector = {
                0, 2, 1,
                1, 2, 3
            };

            Textures[SLOT_TEX]->useTexture();
            shaders[2]->useShader();
            craftInvSlotSelector.createMesh(versCraftInvSlotSelector, indsCraftInvSlotSelector, 24, 6);
            glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE);
            craftInvSlotSelector.renderMesh();
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

            bool blockAdded = false, blockCrafting = false;

            if (mainWindow.getKeys()[GLFW_KEY_RIGHT_SHIFT]) {
                if (mainWindow.getKeys()[GLFW_KEY_ENTER]) {
                    if (mainWindow.getKeys()[GLFW_KEY_0]) {
                        if (inventory.craftInv[0][0] == AIR) {
                            inventory.craftInv[0][0] = inventory.inv_slots[3 - (int)slotY][(int)slotX];
                            inventory.currInvSlot[3 - (int)slotY][(int)slotX].clearMesh();
                            inventory.inv_slots[3 - (int)slotY][(int)slotX] = AIR;
                        }
                    }
                    else if (mainWindow.getKeys()[GLFW_KEY_1]) {
                        if (inventory.craftInv[0][1] == AIR) {
                            inventory.craftInv[0][1] = inventory.inv_slots[3 - (int)slotY][(int)slotX];
                            inventory.currInvSlot[3 - (int)slotY][(int)slotX].clearMesh();
                            inventory.inv_slots[3 - (int)slotY][(int)slotX] = AIR;
                        }
                    }
                    else if (mainWindow.getKeys()[GLFW_KEY_2]) {
                        if (inventory.craftInv[1][0] == AIR) {
                            inventory.craftInv[1][0] = inventory.inv_slots[3 - (int)slotY][(int)slotX];
                            inventory.currInvSlot[3 - (int)slotY][(int)slotX].clearMesh();
                            inventory.inv_slots[3 - (int)slotY][(int)slotX] = AIR;
                        }
                    }
                    else if (mainWindow.getKeys()[GLFW_KEY_3]) {
                        if (inventory.craftInv[1][1] == AIR) {
                            inventory.craftInv[1][1] = inventory.inv_slots[3 - (int)slotY][(int)slotX];
                            inventory.currInvSlot[3 - (int)slotY][(int)slotX].clearMesh();
                            inventory.inv_slots[3 - (int)slotY][(int)slotX] = AIR;
                        }
                    }
                    //cout << slotX << " " << 3 - (int)slotY << endl;
                    inv_change = true;
                }
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
            if (mainWindow.getKeys()[GLFW_KEY_RIGHT_SHIFT]) {
                if (mainWindow.getKeys()[GLFW_KEY_ENTER]) {
                    if (mainWindow.getKeys()[GLFW_KEY_0]) {
                        if (inventory.bigCraftInv[0][0] == AIR) {
                            inventory.bigCraftInv[0][0] = inventory.inv_slots[3 - (int)slotY][(int)slotX];
                            inventory.currInvSlot[3 - (int)slotY][(int)slotX].clearMesh();
                            inventory.inv_slots[3 - (int)slotY][(int)slotX] = AIR;
                        }
                    }
                    else if (mainWindow.getKeys()[GLFW_KEY_1]) {
                        if (inventory.bigCraftInv[0][1] == AIR) {
                            inventory.bigCraftInv[0][1] = inventory.inv_slots[3 - (int)slotY][(int)slotX];
                            inventory.currInvSlot[3 - (int)slotY][(int)slotX].clearMesh();
                            inventory.inv_slots[3 - (int)slotY][(int)slotX] = AIR;
                        }
                    }
                    else if (mainWindow.getKeys()[GLFW_KEY_2]) {
                        if (inventory.bigCraftInv[0][2] == AIR) {
                            inventory.bigCraftInv[0][2] = inventory.inv_slots[3 - (int)slotY][(int)slotX];
                            inventory.currInvSlot[3 - (int)slotY][(int)slotX].clearMesh();
                            inventory.inv_slots[3 - (int)slotY][(int)slotX] = AIR;
                        }
                    }
                    else if (mainWindow.getKeys()[GLFW_KEY_3]) {
                        if (inventory.bigCraftInv[1][0] == AIR) {
                            inventory.bigCraftInv[1][0] = inventory.inv_slots[3 - (int)slotY][(int)slotX];
                            inventory.currInvSlot[3 - (int)slotY][(int)slotX].clearMesh();
                            inventory.inv_slots[3 - (int)slotY][(int)slotX] = AIR;
                        }
                    }
                    else if (mainWindow.getKeys()[GLFW_KEY_4]) {
                        if (inventory.bigCraftInv[1][1] == AIR) {
                            inventory.bigCraftInv[1][1] = inventory.inv_slots[3 - (int)slotY][(int)slotX];
                            inventory.currInvSlot[3 - (int)slotY][(int)slotX].clearMesh();
                            inventory.inv_slots[3 - (int)slotY][(int)slotX] = AIR;
                        }
                    }
                    else if (mainWindow.getKeys()[GLFW_KEY_5]) {
                        if (inventory.bigCraftInv[1][2] == AIR) {
                            inventory.bigCraftInv[1][2] = inventory.inv_slots[3 - (int)slotY][(int)slotX];
                            inventory.currInvSlot[3 - (int)slotY][(int)slotX].clearMesh();
                            inventory.inv_slots[3 - (int)slotY][(int)slotX] = AIR;
                        }
                    }
                    else if (mainWindow.getKeys()[GLFW_KEY_6]) {
                        if (inventory.bigCraftInv[2][0] == AIR) {
                            inventory.bigCraftInv[2][0] = inventory.inv_slots[3 - (int)slotY][(int)slotX];
                            inventory.currInvSlot[3 - (int)slotY][(int)slotX].clearMesh();
                            inventory.inv_slots[3 - (int)slotY][(int)slotX] = AIR;
                        }
                    }
                    else if (mainWindow.getKeys()[GLFW_KEY_7]) {
                        if (inventory.bigCraftInv[2][1] == AIR) {
                            inventory.bigCraftInv[2][1] = inventory.inv_slots[3 - (int)slotY][(int)slotX];
                            inventory.currInvSlot[3 - (int)slotY][(int)slotX].clearMesh();
                            inventory.inv_slots[3 - (int)slotY][(int)slotX] = AIR;
                        }
                    }
                    else if (mainWindow.getKeys()[GLFW_KEY_8]) {
                        if (inventory.bigCraftInv[2][2] == AIR) {
                            inventory.bigCraftInv[2][2] = inventory.inv_slots[3 - (int)slotY][(int)slotX];
                            inventory.currInvSlot[3 - (int)slotY][(int)slotX].clearMesh();
                            inventory.inv_slots[3 - (int)slotY][(int)slotX] = AIR;
                        }
                    }
                    //cout << slotX << " " << 3 - (int)slotY << endl;
                    //cout << "Craft inv : " << bigCraftInv[0][0] << endl;
                    inv_change = true;
                }
            }

            Textures[CRAFT_GUI_TEX]->useTexture();
            shaders[2]->useShader();
            glUniformMatrix4fv(glGetUniformLocation(shaders[2]->getShaderId(), "ortho"), 1, GL_FALSE, glm::value_ptr(ortho));
            glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE);
            mainInventory.renderMesh();
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            vector<GLfloat> versCraftInvSlotSelector = {
                inventoryVertices[0].x + (int)(slotX) * 62 + 28, inventoryVertices[0].y + 323.0f + 10.0f + 5.0f + round(slotY) * 100 + (((int)slotY == 0) ? 0 : 20), 0.0, 0.0f, 0.0f, 1.0f,      0.0f, 0.0f, 0.0f,
                inventoryVertices[4].x + (int)(slotX) * 62 + 28, inventoryVertices[4].y + 323.0f + 10.0f + 5.0f + round(slotY) * 100 + (((int)slotY == 0) ? 0 : 20), 0.0, 0.0f, 1.0f, 1.0f,      0.0f, 0.0f, 0.0f,
                inventoryVertices[2].x + (int)(slotX) * 62 + 28, inventoryVertices[2].y + 323.0f + +5.0f + round(slotY) * 100 + (((int)slotY == 0) ? 0 : 20), 0.0, 1.0f, 0.0f, 1.0f,             0.0f, 0.0f, 0.0f,
                inventoryVertices[5].x + (int)(slotX) * 62 + 28, inventoryVertices[5].y + 323.0f + +5.0f + round(slotY) * 100 + (((int)slotY == 0) ? 0 : 20), 0.0, 1.0f, 1.0f, 1.0f,             0.0f, 0.0f, 0.0f
            };

            vector<unsigned int> indsCraftInvSlotSelector = {
                0, 2, 1,
                1, 2, 3
            };

            Textures[SLOT_TEX]->useTexture();
            shaders[2]->useShader();
            craftInvSlotSelector.createMesh(versCraftInvSlotSelector, indsCraftInvSlotSelector, 24, 6);
            glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE);
            craftInvSlotSelector.renderMesh();
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

        Textures[SLOT_TEX]->useTexture();
        shaders[2]->useShader();
        
        glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE);
        currInvSlotSelector.renderMesh();
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
        //modelCur = mat4(1.0f);
        //modelCur = rotate(modelCur, radians(20.0f), vec3(0.5f, 0.5f, 0.0f));
        //if (recipe.isTool(currentBlock.type)) {
            
            
            //modelCur = translate(modelCur, vec3((-(offsetX)) / WIDTH, (200) / HEIGHT, 0.0f));
        //modelCur = translate(modelCur, vec3(((offsetX)) / WIDTH, (-200) / HEIGHT, 0.0f));
            //modelCur = rotate(modelCur, radians(-90.0f), vec3(0.0f, 0.0f, 1.0f));
            
            //modelCur = translate(modelCur, vec3((centerX + offsetX) / WIDTH, 0.0f, 0.0f));
        //}
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
        
        mainWindow.swapBuffers();

        if (jumping && !ctrlJump) {
            jumpCount += 0.1f / 6;
            camera.setCameraPos(vec3(camera.getCameraPos().x, camera.getCameraPos().y + jumpCount, camera.getCameraPos().z));
            if (jumpCount >= 0.5f) {
                jumping = false;
                ctrlJump = true;
                jumpCount = 0;
            }
        }

        //if (!flying) {
        //ivec3 blockPos = ivec3((floor(vec3(camera.getCameraPos().x, camera.getCameraPos().y - 2, camera.getCameraPos().z))));
        //    if (!blockExistsAt(blockPos)) {
        //        camera.setCameraPos(vec3(camera.getCameraPos().x, camera.getCameraPos().y + 1, camera.getCameraPos().z));
        //        ctrl = true;
        //    }
        //    else {
        //        ctrlJump = false;
        //    }
        //}
    }
}