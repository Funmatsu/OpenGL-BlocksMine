#define GLEW_STATIC

#include "libraries.h"
#include "items.h"
#include "Block.h"
#include "shaderlist.h"
//#include "Chunk.h"
#include "recipes.h"
#include "World.h"
#include "threading.h"
//#include "Frustum.h"
#include "normals.h"
#include "inventory.h"
#include "Frustum.h"

using namespace std;
using namespace glm;

#define WIDTH         1800
#define HEIGHT        1800

// TO DO : Implement spectator mode kind of face culling;;;; Block right next to block of Air can be rendered.

int renderDistance = CHUNK_SIZE * 5;

vector<Mesh> meshes;

Window mainWindow;

vector<Texturegl*> Textures;
Light mainLight;

GLfloat deltaTime = 2.0f;
GLfloat lastTime = 0.0f;

int currentBlockType = 1;

//
void renderWorld(mat4 view, mat4 projection) {
    //mat4 projection(1.0f), view(1.0f);
    //projection = perspective(radians(45.0f), (float(mainWindow.getBufferWidth()) / float(mainWindow.getBufferHeight())), 0.1f, 500.0f);
    //view = camera.calcViewMatrix();
    // compute VP once per frame
    //glm::mat4 VP = projection * view;
    //Plane frustum[6];
    //extractFrustumPlanes(VP, frustum);

    //for (int i = 0; i < world.chunks.size(); ++i) {
    //    Chunk& ch = world.chunks[i];

    //    // compute chunk AABB in world coords (store it in Chunk for reuse)
    //    vec3 chunkOrigin = vec3(ch.coords.x * CHUNK_SIZE, 
    //                            0.0f, 
    //                            ch.coords.y * CHUNK_SIZE);
    //    glm::vec3 chunkMin = chunkOrigin - vec3(50.0f);
    //    glm::vec3 chunkMax = chunkOrigin + vec3(CHUNK_SIZE, CHUNK_SIZE * CHUNK_SIZE + 25, CHUNK_SIZE) + vec3(50.0f);

    //    if (!aabbIntersectsFrustum(chunkMin, chunkMax, frustum)) {  
    //        continue; // skip mesh creation and rendering for this chunk
    //    }

    //    // chunk is visible or intersects; ensure mesh exists and render
    //    if (ch.needUpdate) {
    //        ch.mesh.createMesh(ch.vertices, ch.indices, ch.vertices.size(), ch.indices.size());
    //        ch.needUpdate = false;
    //    }
    //    ch.mesh.renderMesh();
    //    //cout << ch.coords.x << " " << ch.coords.y << endl;
    //}

    //mat4 view = camera.calcViewMatrix();
    //mat4 projection = perspective(radians(45.0f), (float(mainWindow.getBufferWidth()) / float(mainWindow.getBufferHeight())), 0.1f, 1000.0f);
    //Frustum frustum = extractFrustumPlanes(projection * view);
    //for (int i = 0; i >= 0 && i < world.chunks.size(); i++) {
    //    //if (!isChunkVisible(frustum, world.chunks[i].coords)) {
    //    //    continue;
    //    //}
    //    //cout << i << " " << world.chunks.size() << endl;
    //    if (world.chunks[i].needUpdate) {
    //        //if (!clearDistantChunks(world.chunks[i])) {
    //        world.chunks[i].mesh.createMesh(world.chunks[i].vertices, world.chunks[i].indices, world.chunks[i].vertices.size(), world.chunks[i].indices.size());
    //        world.chunks[i].needUpdate = false;
    //        //}
    //    }
    //    //if (!clearDistantChunks(world.chunks[i])) {
    //        world.chunks[i].mesh.renderMesh();
    //    //}
    //}
  

    //for (int i = 0; i < world.chunks.size(); i++) {
    //    world.chunks[i].vertices = {};
    //    world.chunks[i].vertices = {};
    //    for (int j = 0; j < world.chunks[i].blocks.size(); j++) {
    //        if (world.chunks[i].needUpdate) {
    //            world.chunks[i].vertices.insert(world.chunks[i].vertices.end(), world.chunks[i].blocks[j].vertices.begin(), world.chunks[i].blocks[j].vertices.end());
    //            world.chunks[i].indices.insert(world.chunks[i].indices.end(), world.chunks[i].blocks[j].indices.begin(), world.chunks[i].blocks[j].indices.end());
    //            world.chunks[i].needUpdate = false;
    //        }
    //        //world.chunks[i].mesh.createMesh(world.chunks[i].vertices, world.chunks[i].indices, world.chunks[i].vertices.size(), world.chunks[i].indices.size());
    //        //world.chunks[i].mesh.renderMesh();
    //        world.chunks[i].blocks[j].blockMesh.createMesh(world.chunks[i].blocks[j].vertices, world.chunks[i].blocks[j].indices, world.chunks[i].blocks[j].vertices.size(), world.chunks[i].blocks[j].indices.size());
    //        world.chunks[i].blocks[j].blockMesh.renderMesh();
    //    }
    //}
    for (int i = 0; i < world.chunks.size(); i++) {
        if (world.chunks[i].needUpdate) {
            world.chunks[i].mesh.createMesh(world.chunks[i].vertices, world.chunks[i].indices, world.chunks[i].vertices.size(), world.chunks[i].indices.size());
            world.chunks[i].needUpdate = false;
        }
        world.chunks[i].mesh.renderMesh();
    }
    // int c = 0, numblocks = 0;
    //for (int i = 0; i < world.chunks.size(); i++) {
    //    //cout << " C " << world.chunks[i].blocks.size() << endl;
    //    for (int j = 0; j < world.chunks[i].blocks.size(); j++) {
    //        //cout << " B " << world.chunks[i].blocks[j].indices.size() << endl;
    //        //for (int k = 1; k < 6; k++) {
    //        if(
    //            !blockExistsAt(world.chunks[i].blocks[j].position + vec3(-1.0f, 0.0, 0.0f)) || 
    //            !blockExistsAt(world.chunks[i].blocks[j].position + vec3( 1.0f, 0.0, 0.0f)) || 
    //            !blockExistsAt(world.chunks[i].blocks[j].position + vec3(0.0f, -1.0, 0.0f)) || 
    //            !blockExistsAt(world.chunks[i].blocks[j].position + vec3(0.0f,  1.0, 0.0f)) ||
    //            !blockExistsAt(world.chunks[i].blocks[j].position + vec3(0.0f, 0.0, -1.0f)) ||
    //            !blockExistsAt(world.chunks[i].blocks[j].position + vec3(0.0f, 0.0,  1.0f))
    //        ) {
    //            if (world.chunks[i].needUpdate) {
    //                world.chunks[i].blocks[j].blockMesh.createMesh(world.chunks[i].blocks[j].vertices, world.chunks[i].blocks[j].indices, world.chunks[i].blocks[j].vertices.size(), world.chunks[i].blocks[j].indices.size());
    //                //cout << " Done creating Mesh! " << c++ << endl;
    //                //cout << numblocks << endl;
    //                
    //            }
    //        }
    //        world.chunks[i].blocks[j].blockMesh.renderMesh();
    //    }
    //    numblocks += world.chunks[i].blocks.size();
    //    world.chunks[i].needUpdate = false;
    //}
}

int main()
{
    mainWindow = Window(WIDTH, HEIGHT);
    mainWindow.initialize(); 

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glfwSwapInterval(0);
    /*Textures.push_back(new Texturegl("textures\\block_atlas_4.png"));*/
    Textures.push_back(new Texturegl("textures\\block_atlas_24.png"));
    Textures.push_back(new Texturegl("textures\\clear_toolbar_2.png"));
    Textures.push_back(new Texturegl("textures\\clear_toolbar_3.png"));
    Textures.push_back(new Texturegl("textures\\main_inventory.jpg"));
    Textures.push_back(new Texturegl("textures\\tools_atlas_3.png"));
    Textures.push_back(new Texturegl("textures\\crafting_table_gui.png"));
    //Textures.push_back(new Texturegl("textures\\inventory_base.png"));

    Textures[0]->loadTexture();
    Textures[1]->loadTexture();
    Textures[2]->loadTexture();
    Textures[3]->loadTexture();
    Textures[4]->loadTexture();
    Textures[5]->loadTexture();

    createShaders();
    worldBlocks.clear();
    

    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    //glFrontFace(GL_CW);
    
    chunkCoords.push_back({ 0, 0 });
    //generateChunkWith(1);
    {
        chunkGenRunning = true;
        std::lock_guard<std::mutex> lock(chunkRequestMutex);
        chunkRequestQueue.push(chunkCoords.back());
    }
    {
        std::lock_guard<std::mutex> lock(chunkResultMutex);
        //cout << chunkResultQueue.empty() << endl;
        while (!chunkResultQueue.empty()) {
            Chunk chunk = std::move(chunkResultQueue.front());
            chunkResultQueue.pop();
            world.chunks.push_back(std::move(chunk));
        }
    }

    camera.setCameraPos(vec3(CHUNK_SIZE / 2, CHUNK_SIZE * CHUNK_SIZE + 10, CHUNK_SIZE / 2));

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
    //mat4 modelCur(1.0f);
    projection = perspective(radians(45.0f), (float(mainWindow.getBufferWidth()) / float(mainWindow.getBufferHeight())), 0.5f, 200.0f);    

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
    //world.chunks.push_back(Chunk());
    //createCube((int)camera.getCameraPos().x, (int)camera.getCameraPos().y - 3, (int)camera.getCameraPos().z, 6);
    //deleteBlockFromWorld(0.0f, 0.0f, 0.0f, 8);
    int offs = 1, ctrl = 0;
    inventoryMesh.createMesh(versInv, indsInv, 24, 6);
    mat4 modelCurSlots[4][9];
    mat4 modelCraftInvSlots[2][2], modelbigCraftInvSlots[3][3];;
    mat4 modelCraftedInvSlot(1.0f), modelbigCraftedInvSlot(1.0f);
    mat4 modelCurSlotsMain[9];
    mat4 modelCurSlot(1.0f);
    mat4 modelCur(1.0f);
    //modelCur = translate(modelCur, vec3((2 * (-200.0) / (float)WIDTH), 2 * (-200.0) / (float)HEIGHT, 0.5f));
    //modelCur = translate(modelCur, vec3(((centerX + offsetX)) / WIDTH, -(centerY - 200) / HEIGHT, 0.0f));
    for (int j = 0; j < 9; j++) {
        modelCurSlotsMain[j] = mat4(1.0f);
        modelCurSlotsMain[j] = translate(modelCurSlotsMain[j], vec3((centerX - 40.0f + j * 137) / WIDTH, 2 * (centerY + offsetY + 70.0f) / HEIGHT, 0.0f));
    }
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 9; j++) {
            modelCurSlots[i][j] = mat4(1.0f);
            modelCurSlots[i][j] = translate(modelCurSlots[i][j], vec3((centerX + 18.0f + (float)j * 123) / WIDTH, 2 * (centerY + offsetY + 323.0f + (3 - i) * 10 + 80.0f) / HEIGHT, 0.0f));
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

    float time = 10.0f, lowTime = 10.0f, maxTime = 1000.0;
    bool night = false;



    while (!mainWindow.getShouldClose()) {
        //mainLight = Light(1.0f, 1.0f, 1.0f, camera.getCameraPos().y/20);
        mainLight = Light(1.0f, 1.0f, 1.0f, 0.2 + 1.2 * time / maxTime, 0.0000f, -0.7071f, 0.7071f, 0.85 * time / maxTime);
        for (int i = -renderX; i < renderX; i++) {
            for (int j = -renderY; j < renderY; j += 1) {

                if (find(chunkCoords.begin(), chunkCoords.end(), vec2((int)(camera.getCameraPos().x / CHUNK_SIZE) + i, (int)(camera.getCameraPos().z / CHUNK_SIZE) + j)) == chunkCoords.end() || (chunkCoords[chunkCoords.size() - 1].x == 0 && chunkCoords[chunkCoords.size() - 1].y == 0) &&
                    find(chunkCoords.begin(), chunkCoords.end(), vec2((int)(camera.getCameraPos().x / CHUNK_SIZE) + i, (int)(camera.getCameraPos().z / CHUNK_SIZE) + j + 1)) == chunkCoords.end() || (chunkCoords[chunkCoords.size() - 1].x == 0 && chunkCoords[chunkCoords.size() - 1].y == 0)
                    ) {
                    chunkCoords.push_back({ int(camera.getCameraPos().x / CHUNK_SIZE) + i, int(camera.getCameraPos().z / CHUNK_SIZE) + j });
                    //generateChunkAt(chunkCoords.back());
                    {
                        chunkGenRunning = true;
                        std::lock_guard<std::mutex> lock(chunkRequestMutex);
                        chunkRequestQueue.push(chunkCoords.back());
                    }

                    //chunkCoords.push_back({ int(camera.getCameraPos().x / CHUNK_SIZE) + i, int(camera.getCameraPos().z / CHUNK_SIZE) + j + 1 });
                    ////generateChunkAt(chunkCoords[chunkCoords.size() - 1]);
                    //{
                    //    chunkGenRunning2 = true;
                    //    std::lock_guard<std::mutex> lock(chunkRequestMutex2);
                    //    chunkRequestQueue2.push(chunkCoords.back());
                    //}

                    //chunkCoords.push_back({ int(camera.getCameraPos().x / CHUNK_SIZE) + i, int(camera.getCameraPos().z / CHUNK_SIZE) + j + 2 });
                    ////generateChunkAt(chunkCoords[chunkCoords.size() - 1]);
                    //{
                    //    chunkGenRunning3 = true;
                    //    std::lock_guard<std::mutex> lock(chunkRequestMutex3);
                    //    chunkRequestQueue3.push(chunkCoords.back());
                    //}

                }

                {
                    std::lock_guard<std::mutex> lock(chunkResultMutex);
                    //cout << chunkResultQueue.empty() << endl;
                    while (!chunkResultQueue.empty()) {
                        Chunk chunk = std::move(chunkResultQueue.front());
                        chunkResultQueue.pop();
                        world.addChunk(std::move(chunk));
                    }
                }

                //{
                //    std::lock_guard<std::mutex> lock(chunkResultMutex2);
                //    //cout << chunkResultQueue.empty() << endl;
                //    while (!chunkResultQueue2.empty()) {
                //        Chunk chunk = std::move(chunkResultQueue2.front());
                //        chunkResultQueue2.pop();
                //        world.addChunk(std::move(chunk));
                //    }
                //}

                //{
                //    std::lock_guard<std::mutex> lock(chunkResultMutex3);
                //    //cout << chunkResultQueue.empty() << endl;
                //    while (!chunkResultQueue3.empty()) {
                //        Chunk chunk = std::move(chunkResultQueue3.front());
                //        chunkResultQueue3.pop();
                //        world.addChunk(std::move(chunk));
                //    }
                //}

            }

        }
 
        if (renderX < renderDistance) {
            renderX++;
            renderY++;
        }
        //else {
        //    renderX = renderDistance / 2;
        //    renderY = renderDistance / 2;
        //}
        
        Textures[0]->useTexture();
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

        glClearColor(0.2f + 0.1 * time / maxTime,time / maxTime + 0.1, 0.4 + time / maxTime, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwPollEvents();
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

        view = camera.calcViewMatrix();
        glUniformMatrix4fv(shaders[0]->getModelLocation(), 1, GL_FALSE, value_ptr(model));
        glUniformMatrix4fv(shaders[0]->getViewLocation(), 1, GL_FALSE, value_ptr(view));
        glUniformMatrix4fv(shaders[0]->getProjectionLocation(), 1, GL_FALSE, value_ptr(projection));

        mainLight.useLight(shaders[0]->getAmbientIntensityLocation(), shaders[0]->getAmbientColorLocation(), shaders[0]->getDiffuseIntensityLocation(), shaders[0]->getDirectionLocation());

        if (mainWindow.getKeys()[GLFW_KEY_SPACE]) {
            //cout << jumping << " " << ctrlJump << endl;

            if (flying) {
                jumpCount += 0.1f;
            }
            if(jumping)
            camera.setCameraPos(vec3(camera.getCameraPos().x, camera.getCameraPos().y + 0.1f, camera.getCameraPos().z));
            if (!ctrlJump) {
                jumping = true;
            }
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

        if (mainWindow.getKeys()[GLFW_KEY_I]) {
            //currentBlockType = 6;
            inventory.inf_blocks = true;
        }
        else if (mainWindow.getKeys()[GLFW_KEY_N]) {
            inventory.inf_blocks = false;
        }
        //slot = currentBlockType - 1;
        if (mainWindow.getKeys()[GLFW_KEY_P]) {
            if (mainWindow.getKeys()[GLFW_KEY_1])
                world.addBlocklook_at(items[1]);
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
                std::lock_guard<std::mutex> lock(breakResMutex);
                if (!breakResQueue.empty() && blockBreakingOut) {
                    breakResQueue.pop();
                }
            }
        }

        if (mainWindow.rightClickButtonPressed()) {
            if (!recipe.itemUsable(worldBlocks[{lookingAtBlock()}].type)) {
                if (inventory.inv_slots[3][slot] != AIR && recipe.itemPlaceable(inventory.inv_slots[3][slot])) {
                    {
                        blockPlacingOut = true;
                        std::lock_guard<std::mutex> lock(placeReqMutex);
                        placeReqQueue.push(vec3(1.0f));
                    }
                    {
                        std::lock_guard<std::mutex> lock(placeResMutex);
                        if (!placeResQueue.empty()) {
                            placeResQueue.pop();
                        }
                    }
                }
            }
            else {
                inventory.craftingInventoryOn = true;
            }
        }

        if (mainWindow.getShouldClose()) {
            chunkGenRunning = false;
            chunkGenRunning2 = false;
            chunkGenRunning3 = false;
            blockPlacing = false;
            blockBreaking = false;
            chunkGenThread.join();
            chunkGenThread2.join();
            chunkGenThread3.join();
            blockBreakThread.join();
            blockPlaceThread.join();
        }
        if (mainWindow.getKeys()[GLFW_KEY_Q]) {
            inventory.inv_slots[3][slot] = AIR;
            inventory.currInvSlot[3][slot].clearMesh();
            inv_change = true;
        }

        glEnable(GL_DEPTH_TEST);
        renderWorld(view, projection); // your blocks, terrain, etc.
        shaders[13]->useShader();
        view = camera.calcViewMatrix();
        glUniformMatrix4fv(shaders[13]->getModelLocation(), 1, GL_FALSE, value_ptr(model));
        glUniformMatrix4fv(shaders[13]->getViewLocation(), 1, GL_FALSE, value_ptr(view));
        glUniformMatrix4fv(shaders[13]->getProjectionLocation(), 1, GL_FALSE, value_ptr(projection));

        if (world.chunks.size() > 20) {
            Block cloud = world.getBlockAt(vec3(lookingAtBlock().x, lookingAtBlock().y, lookingAtBlock().z));
            //if (cloud.type != AIR) {
                cloud = world.createMeshCube(cloud.position, 0.05f, cloud.type);
                cloud.blockMesh.createMesh(cloud.vertices, cloud.indices, cloud.vertices.size(), cloud.indices.size());
                cloud.blockMesh.renderMesh();
            //}
        }

        glDisable(GL_DEPTH_TEST); // so crosshair draws on top
        shaders[1]->useShader();
        glUniformMatrix4fv(glGetUniformLocation(shaders[1]->getShaderId(), "ortho"), 1, GL_FALSE, glm::value_ptr(ortho));
        glBindVertexArray(vao);
        glLineWidth(3.0f);
        glDrawArrays(GL_LINES, 0, 4);
        Textures[1]->useTexture();

        if (inv_change)
        {
            if (inventory.inv_slots[3][slot] != AIR) {
                currentBlock.blockMesh = world.createMeshCube(centerX + offsetX, centerY + offsetY, 0.0f, 400.0f, inventory.inv_slots[3][slot]);
                currentBlock.type = inventory.inv_slots[3][slot];
            }
            else {
                currentBlock.blockMesh.clearMesh();
            }
            float itemHeight = 0.0;
            
            for (int i = 0; i < (sizeof(inventory.inv_slots) / sizeof(inventory.inv_slots[0])) - 2; i++) {
                for (int j = 0; j < (sizeof(inventory.inv_slots[0]) / sizeof(Item)); j++) {
                    if (inventory.inv_slots[i][j] != GRASS) {
                        itemHeight = 10.0f;
                    }
                    if (inventory.currInvSlot[i][j].verts.size() == 0 && inventory.inv_slots[i][j] != AIR) {
                        inventory.currInvSlot[i][j] = world.createMeshCube(centerX / 5, (3 - i) * 90.0f, 0.0f, 35.0f, inventory.inv_slots[i][j]);
                    }
                }
            }

            for (int j = 0; j < (sizeof(inventory.inv_slots[0]) / sizeof(Item)); j++) {
                if (inventory.currInvSlot[3][j].verts.size() == 0 && inventory.inv_slots[3][j] != AIR) {
                    inventory.currInvSlot[3][j] = world.createMeshCube(centerX / 5, 0.0f, 0.0f, 35.0f, inventory.inv_slots[3][j]);
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

            if (inventory.craftingInventoryOn) {
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
        Textures[1]->useTexture();
        inventoryMesh.renderMesh();
        Textures[3]->useTexture();
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
        //if (mainWindow.getKeys()[GLFW_KEY_D]) {
        //    craftInv[0][0] = 0;
        //    craftInv[0][1] = 0;
        //    craftInv[1][0] = 0;
        //    craftInv[1][1] = 0;
        //}
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

            Textures[3]->useTexture();
            shaders[2]->useShader();
            glUniformMatrix4fv(glGetUniformLocation(shaders[2]->getShaderId(), "ortho"), 1, GL_FALSE, glm::value_ptr(ortho));
            mainInventory.renderMesh();
            vector<GLfloat> versCraftInvSlotSelector = {
                inventoryVertices[0].x + round(slotX) * 62 + 28, inventoryVertices[0].y + 323.0f + 10.0f + 5.0f + round(slotY) * 100 + (((int)slotY == 0) ? 0 : 20), 0.0, 0.0f, 0.0f, 1.0f,     0.0f, 0.0f, 0.0f,
                inventoryVertices[4].x + round(slotX) * 62 + 28, inventoryVertices[4].y + 323.0f + 10.0f + 5.0f + round(slotY) * 100 + (((int)slotY == 0) ? 0 : 20), 0.0, 0.0f, 1.0f, 1.0f,     0.0f, 0.0f, 0.0f,
                inventoryVertices[2].x + round(slotX) * 62 + 28, inventoryVertices[2].y + 323.0f + +5.0f + round(slotY) * 100 + (((int)slotY == 0) ? 0 : 20), 0.0, 1.0f, 0.0f, 1.0f,            0.0f, 0.0f, 0.0f,
                inventoryVertices[5].x + round(slotX) * 62 + 28, inventoryVertices[5].y + 323.0f + +5.0f + round(slotY) * 100 + (((int)slotY == 0) ? 0 : 20), 0.0, 1.0f, 1.0f, 1.0f,            0.0f, 0.0f, 0.0f
            };

            vector<unsigned int> indsCraftInvSlotSelector = {
                0, 2, 1,
                1, 2, 3
            };

            Textures[2]->useTexture();
            shaders[2]->useShader();
            craftInvSlotSelector.createMesh(versCraftInvSlotSelector, indsCraftInvSlotSelector, 24, 6);
            craftInvSlotSelector.renderMesh();
            Textures[0]->useTexture();

            for (int i = 0; i < (sizeof(inventory.inv_slots) / sizeof(inventory.inv_slots[3])); i++) {
                for (int j = 0; j < (sizeof(inventory.inv_slots[3]) / sizeof(Item)); j++) {
                    if (inventory.currInvSlot[i][j].verts.size() == 0 && inventory.inv_slots[i][j] != AIR) {
                        inventory.currInvSlot[i][j] = world.createMeshCube(centerX / 5, (3 - i) * 90 + 10, 0.0f, 35.0f, inventory.inv_slots[i][j]);
                    }
                }
            }
            for (int i = 0; i < (sizeof(inventory.inv_slots) / sizeof(inventory.inv_slots[3])); i++) {
                for (int j = 0; j < (sizeof(inventory.inv_slots[3]) / sizeof(Item)); j++) {
                    Textures[0]->useTexture();
                    InventoryShaders[9 * i + j]->useShader();
                    glUniformMatrix4fv(glGetUniformLocation(InventoryShaders[9 * i + j]->getShaderId(), "ortho"), 1, GL_FALSE, glm::value_ptr(ortho));
                    glUniformMatrix4fv(InventoryShaders[9 * i + j]->getModelLocation(), 1, GL_FALSE, value_ptr(modelCurSlots[i][j]));
                    if (recipe.isTool(inventory.inv_slots[i][j])){
                        Textures[4]->useTexture();
                    }
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
            //if (mainWindow.getKeys()[GLFW_KEY_RIGHT_SHIFT]) {
            //    if (mainWindow.getKeys()[GLFW_KEY_ENTER]) {
            //        for (int i = 0; i < (sizeof(inventory.inv_slots) / sizeof(inventory.inv_slots[3])); i++) {
            //            for (int j = 0; j < (sizeof(inventory.inv_slots[3]) / sizeof(Item)); j++) {
            //                if (inventory.inv_slots[i][j] != NULL) {
            //                     //inventory.craftInv[0][0] = inventory.inv_slots[i][j];
            //                    for (int k = 0; k < sizeof(inventory.craftInv) / sizeof(inventory.craftInv[0]); k++) {
            //                        for (int l = 0; l < sizeof(inventory.craftInv[0]) / sizeof(Item); l++) {
            //                            if (inventory.craftInv[k][l] == NULL) {
            //                                inventory.craftInv[k][l] = inventory.inv_slots[i][j];
            //                                blockCrafting = true;
            //                                blockAdded = true;
            //                                currInvSlot[i][j] = Mesh();
            //                                inventory.inv_slots[i][j] = NULL;
            //                                break;
            //                            }
            //                        }
            //                        if (blockCrafting) {
            //                            break;
            //                        }
            //                    }
            //                    if (blockAdded) {
            //                        break;
            //                    }
            //                }
            //            }
            //            if (blockAdded) {
            //                break;
            //            }
            //        }
            //    }
            //}
            for (int i = 0; i < (sizeof(inventory.craftInv) / sizeof(inventory.craftInv[0])); i++) {
                for (int j = 0; j < (sizeof(inventory.craftInv[0]) / sizeof(Item)); j++) {
                    Textures[0]->useTexture();
                    craftInvShaders[2 * i + j]->useShader();
                    glUniformMatrix4fv(glGetUniformLocation(craftInvShaders[2 * i + j]->getShaderId(), "ortho"), 1, GL_FALSE, glm::value_ptr(ortho));
                    glUniformMatrix4fv(craftInvShaders[2 * i + j]->getModelLocation(), 1, GL_FALSE, value_ptr(modelCraftInvSlots[i][j]));
                    if (recipe.isTool(inventory.craftInv[i][j])) {
                        Textures[4]->useTexture();
                    }
                    inventory.craftInvSlot[i][j].renderMesh();
                    inv_change = true;
                }
                craftInvShaders[4]->useShader();
                glUniformMatrix4fv(glGetUniformLocation(craftInvShaders[4]->getShaderId(), "ortho"), 1, GL_FALSE, glm::value_ptr(ortho));
                glUniformMatrix4fv(craftInvShaders[4]->getModelLocation(), 1, GL_FALSE, value_ptr(modelCraftedInvSlot));
                if (recipe.isTool(craftedItem.type)) {
                    Textures[4]->useTexture();
                }
                else {
                    Textures[0]->useTexture();
                }
                craftedItem.blockMesh.renderMesh();
            }
        }

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

            Textures[5]->useTexture();
            shaders[2]->useShader();
            glUniformMatrix4fv(glGetUniformLocation(shaders[2]->getShaderId(), "ortho"), 1, GL_FALSE, glm::value_ptr(ortho));
            mainInventory.renderMesh();

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

            Textures[2]->useTexture();
            shaders[2]->useShader();
            craftInvSlotSelector.createMesh(versCraftInvSlotSelector, indsCraftInvSlotSelector, 24, 6);
            craftInvSlotSelector.renderMesh();
            Textures[0]->useTexture();

            for (int i = 0; i < (sizeof(inventory.inv_slots) / sizeof(inventory.inv_slots[3])); i++) {
                for (int j = 0; j < (sizeof(inventory.inv_slots[3]) / sizeof(Item)); j++) {
                    if (inventory.currInvSlot[i][j].verts.size() == 0 && inventory.inv_slots[i][j] != AIR) {
                        inventory.currInvSlot[i][j] = world.createMeshCube(centerX / 5, (3 - i) * 90 + 10, 0.0f, 35.0f, inventory.inv_slots[i][j]);
                    }
                }
            }

            for (int i = 0; i < (sizeof(inventory.inv_slots) / sizeof(inventory.inv_slots[3])); i++) {
                for (int j = 0; j < (sizeof(inventory.inv_slots[3]) / sizeof(Item)); j++) {
                    Textures[0]->useTexture();
                    InventoryShaders[9 * i + j]->useShader();
                    glUniformMatrix4fv(glGetUniformLocation(InventoryShaders[9 * i + j]->getShaderId(), "ortho"), 1, GL_FALSE, glm::value_ptr(ortho));
                    glUniformMatrix4fv(InventoryShaders[9 * i + j]->getModelLocation(), 1, GL_FALSE, value_ptr(modelCurSlots[i][j]));
                    if (recipe.isTool(inventory.inv_slots[i][j])) {
                        Textures[4]->useTexture();
                    }
                    inventory.currInvSlot[i][j].renderMesh();
                    inv_change = true;
                }
            }

            for (int i = 0; i < (sizeof(inventory.bigCraftInv) / sizeof(inventory.bigCraftInv[0])); i++) {
                for (int j = 0; j < (sizeof(inventory.bigCraftInv[0]) / sizeof(Item)); j++) {
                    Textures[0]->useTexture();
                    bigCraftInvShaders[3 * i + j]->useShader();
                    glUniformMatrix4fv(glGetUniformLocation(bigCraftInvShaders[3 * i + j]->getShaderId(), "ortho"), 1, GL_FALSE, glm::value_ptr(ortho));
                    glUniformMatrix4fv(bigCraftInvShaders[3 * i + j]->getModelLocation(), 1, GL_FALSE, value_ptr(modelbigCraftInvSlots[i][j]));
                    if (recipe.isTool(inventory.bigCraftInv[i][j])) {
                        Textures[4]->useTexture();
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
                Textures[4]->useTexture();
            }
            else {
                Textures[0]->useTexture();
            }
            craftedItem.blockMesh.renderMesh();
        }

        Textures[2]->useTexture();
        shaders[2]->useShader();
        currInvSlotSelector.renderMesh();
        Textures[0]->useTexture();
        
        for (int i = 0; i < 9; i++) {
            shaders[4 + i]->useShader();
            glUniformMatrix4fv(glGetUniformLocation(shaders[4 + i]->getShaderId(), "ortho"), 1, GL_FALSE, glm::value_ptr(ortho));
            glUniformMatrix4fv(shaders[4 + i]->getModelLocation(), 1, GL_FALSE, value_ptr(modelCurSlotsMain[i]));
            if (recipe.isTool(inventory.inv_slots[3][i])) {
                Textures[4]->useTexture();
            }
            else {
                Textures[0]->useTexture();
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
            Textures[4]->useTexture();
        }
        else {
            Textures[0]->useTexture();
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
        //        camera.setCameraPos(vec3(camera.getCameraPos().x, camera.getCameraPos().y - 0.01 * ctrl, camera.getCameraPos().z));
        //        ctrl = true;
        //    }
        //    else {
        //        ctrlJump = false;
        //    }
        //}
    }
}