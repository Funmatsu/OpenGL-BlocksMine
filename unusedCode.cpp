//Textures[2]->loadTexture();
//Textures[3]->loadTexture();
//Textures[4]->loadTexture();
//Textures[5]->loadTexture();
//Textures[6]->loadTexture();

    //Textures.push_back(new Texture("textures\\grass_block_all.png"));
    //Textures.push_back(new Texture("textures\\dirt_face.jpg"));
    //Textures.push_back(new Texturegl("textures\\diamond_ore.jpg"));
    //Textures.push_back(new Texture("textures\\block_atlas.png"));
    //Textures.push_back(new Texture("textures\\grass_stone_atlas.png"));
    //Textures.push_back(new Texture("textures\\all_blocks_atlas.jpg"));

//void createCube(float xoffset, float yoffset, float zoffset, int blockType) {
//    float x = 0, y = 1, xoffsetTop = 0, yoffsetTop = 0, xoffsetBottom = 0, yoffsetBottom = 0, transparency = 1.0f;
//    int xdimens = 3, ydimens = 3;
//
//    int arbitraryoff = 0;
//    if (blockType == 2) {
//        x = 0, y = 2; xoffsetTop = 2; yoffsetTop = 0; xoffsetBottom = 2; yoffsetBottom = 1;
//    }
//
//    if (blockType == 3) {
//        x = 1; y = 1 + arbitraryoff;
//    }
//
//    if (blockType == 4) {
//        x = 1; y = 2;
//    }
//
//    if (blockType == 5) {
//        x = 2; y = 0;
//    }
//
//    if (blockType == 6) {
//        x = 2, y = 1;
//    }
//
//    vector<unsigned int> indices1 = {
//        0 + (world1.world.chunks[world1.world.chunks.size() - 1].blockNum * 6 * 6), 1 + (world1.world.chunks[world1.world.chunks.size() - 1].blockNum * 6 * 6), 2 + (world1.world.chunks[world1.world.chunks.size() - 1].blockNum * 6 * 6),
//        3 + (world1.world.chunks[world1.world.chunks.size() - 1].blockNum * 6 * 6), 4 + (world1.world.chunks[world1.world.chunks.size() - 1].blockNum * 6 * 6), 5 + (world1.world.chunks[world1.world.chunks.size() - 1].blockNum * 6 * 6),
//        6 + (world1.world.chunks[world1.world.chunks.size() - 1].blockNum * 6 * 6), 7 + (world1.world.chunks[world1.world.chunks.size() - 1].blockNum * 6 * 6), 8 + (world1.world.chunks[world1.world.chunks.size() - 1].blockNum * 6 * 6),
//        9 + (world1.world.chunks[world1.world.chunks.size() - 1].blockNum * 6 * 6), 10 + (world1.world.chunks[world1.world.chunks.size() - 1].blockNum * 6 * 6), 11 + (world1.world.chunks[world1.world.chunks.size() - 1].blockNum * 6 * 6),
//        12 + (world1.world.chunks[world1.world.chunks.size() - 1].blockNum * 6 * 6), 13 + (world1.world.chunks[world1.world.chunks.size() - 1].blockNum * 6 * 6), 14 + (world1.world.chunks[world1.world.chunks.size() - 1].blockNum * 6 * 6),
//        15 + (world1.world.chunks[world1.world.chunks.size() - 1].blockNum * 6 * 6), 16 + (world1.world.chunks[world1.world.chunks.size() - 1].blockNum * 6 * 6), 17 + (world1.world.chunks[world1.world.chunks.size() - 1].blockNum * 6 * 6),
//        18 + (world1.world.chunks[world1.world.chunks.size() - 1].blockNum * 6 * 6), 19 + (world1.world.chunks[world1.world.chunks.size() - 1].blockNum * 6 * 6), 20 + (world1.world.chunks[world1.world.chunks.size() - 1].blockNum * 6 * 6),
//        21 + (world1.world.chunks[world1.world.chunks.size() - 1].blockNum * 6 * 6), 22 + (world1.world.chunks[world1.world.chunks.size() - 1].blockNum * 6 * 6), 23 + (world1.world.chunks[world1.world.chunks.size() - 1].blockNum * 6 * 6),
//        24 + (world1.world.chunks[world1.world.chunks.size() - 1].blockNum * 6 * 6), 25 + (world1.world.chunks[world1.world.chunks.size() - 1].blockNum * 6 * 6), 26 + (world1.world.chunks[world1.world.chunks.size() - 1].blockNum * 6 * 6),
//        27 + (world1.world.chunks[world1.world.chunks.size() - 1].blockNum * 6 * 6), 28 + (world1.world.chunks[world1.world.chunks.size() - 1].blockNum * 6 * 6), 29 + (world1.world.chunks[world1.world.chunks.size() - 1].blockNum * 6 * 6),
//        30 + (world1.world.chunks[world1.world.chunks.size() - 1].blockNum * 6 * 6), 31 + (world1.world.chunks[world1.world.chunks.size() - 1].blockNum * 6 * 6), 32 + (world1.world.chunks[world1.world.chunks.size() - 1].blockNum * 6 * 6),
//        33 + (world1.world.chunks[world1.world.chunks.size() - 1].blockNum * 6 * 6), 34 + (world1.world.chunks[world1.world.chunks.size() - 1].blockNum * 6 * 6), 35 + (world1.world.chunks[world1.world.chunks.size() - 1].blockNum * 6 * 6)
//    };
//
//    vector<GLfloat> globalUVs =
//    {
//        (0.05f + x) / xdimens,   (0.05f + y) / ydimens, transparency,
//        (0.05f + x) / xdimens,   (0.95f + y) / ydimens, transparency,
//        (0.95f + x) / xdimens,   (0.05f + y) / ydimens, transparency,
//        (0.05f + x) / xdimens,   (0.95f + y) / ydimens, transparency,
//        (0.95f + x) / xdimens,   (0.95f + y) / ydimens, transparency,
//        (0.95f + x) / xdimens,   (0.05f + y) / ydimens, transparency,
//
//        (0.05f + x) / xdimens,   (0.05f + y) / ydimens, transparency,
//        (0.95f + x) / xdimens,   (0.05f + y) / ydimens, transparency,
//        (0.05f + x) / xdimens,   (0.95f + y) / ydimens, transparency,
//        (0.05f + x) / xdimens,   (0.95f + y) / ydimens, transparency,
//        (0.95f + x) / xdimens,   (0.05f + y) / ydimens, transparency,
//        (0.95f + x) / xdimens,   (0.95f + y) / ydimens, transparency,
//
//        (0.05f + x + xoffsetBottom) / xdimens,   (0.05f + y + yoffsetBottom) / ydimens, transparency,
//        (0.95f + x + xoffsetBottom) / xdimens,   (0.05f + y + yoffsetBottom) / ydimens, transparency,
//        (0.05f + x + xoffsetBottom) / xdimens,   (0.95f + y + yoffsetBottom) / ydimens, transparency,
//
//        (0.05f + x + xoffsetBottom) / xdimens,   (0.95f + y + yoffsetBottom) / ydimens, transparency,
//        (0.95f + x + xoffsetBottom) / xdimens,   (0.05f + y + yoffsetBottom) / ydimens, transparency,
//        (0.95f + x + xoffsetBottom) / xdimens,   (0.95f + y + yoffsetBottom) / ydimens, transparency,
//
//        (0.05f + x + xoffsetTop) / xdimens,   (0.05f + y + yoffsetTop) / ydimens, transparency,
//        (0.05f + x + xoffsetTop) / xdimens,   (0.95f + y + yoffsetTop) / ydimens, transparency,
//        (0.95f + x + xoffsetTop) / xdimens,   (0.05f + y + yoffsetTop) / ydimens, transparency,
//        (0.05f + x + xoffsetTop) / xdimens,   (0.95f + y + yoffsetTop) / ydimens, transparency,
//        (0.95f + x + xoffsetTop) / xdimens,   (0.95f + y + yoffsetTop) / ydimens, transparency,
//        (0.95f + x + xoffsetTop) / xdimens,   (0.05f + y + yoffsetTop) / ydimens, transparency,
//
//        (0.05f + x) / xdimens,   (0.05f + y) / ydimens, transparency,
//        (0.95f + x) / xdimens,   (0.05f + y) / ydimens, transparency,
//        (0.05f + x) / xdimens,   (0.95f + y) / ydimens, transparency,
//        (0.05f + x) / xdimens,   (0.95f + y) / ydimens, transparency,
//        (0.95f + x) / xdimens,   (0.05f + y) / ydimens, transparency,
//        (0.95f + x) / xdimens,   (0.95f + y) / ydimens, transparency,
//
//        (0.05f + x) / xdimens,   (0.05f + y) / ydimens, transparency,
//        (0.05f + x) / xdimens,   (0.95f + y) / ydimens, transparency,
//        (0.95f + x) / xdimens,   (0.05f + y) / ydimens, transparency,
//        (0.05f + x) / xdimens,   (0.95f + y) / ydimens, transparency,
//        (0.95f + x) / xdimens,   (0.95f + y) / ydimens, transparency,
//        (0.95f + x) / xdimens,   (0.05f + y) / ydimens, transparency
//    };
//
//    vector<GLfloat> triangle = {
//        0.0f + xoffset, 0.0f + yoffset, 0.0f + zoffset,
//        0.0f + xoffset, 1.0f + yoffset, 0.0f + zoffset,
//        0.0f + xoffset, 0.0f + yoffset, 1.0f + zoffset,
//        0.0f + xoffset, 1.0f + yoffset, 0.0f + zoffset,
//        0.0f + xoffset, 1.0f + yoffset, 1.0f + zoffset,
//        0.0f + xoffset, 0.0f + yoffset, 1.0f + zoffset,
//
//        1.0f + xoffset, 0.0f + yoffset, 0.0f + zoffset,
//        1.0f + xoffset, 0.0f + yoffset, 1.0f + zoffset,
//        1.0f + xoffset, 1.0f + yoffset, 0.0f + zoffset,
//        1.0f + xoffset, 1.0f + yoffset, 0.0f + zoffset,
//        1.0f + xoffset, 0.0f + yoffset, 1.0f + zoffset,
//        1.0f + xoffset, 1.0f + yoffset, 1.0f + zoffset,
//
//        0.0f + xoffset, 0.0f + yoffset, 0.0f + zoffset,
//        0.0f + xoffset, 0.0f + yoffset, 1.0f + zoffset,
//        1.0f + xoffset, 0.0f + yoffset, 0.0f + zoffset,
//
//        1.0f + xoffset, 0.0f + yoffset, 0.0f + zoffset,
//        0.0f + xoffset, 0.0f + yoffset, 1.0f + zoffset,
//        1.0f + xoffset, 0.0f + yoffset, 1.0f + zoffset,
//
//        0.0f + xoffset, 1.0f + yoffset, 0.0f + zoffset,
//        1.0f + xoffset, 1.0f + yoffset, 0.0f + zoffset,
//        0.0f + xoffset, 1.0f + yoffset, 1.0f + zoffset,
//        1.0f + xoffset, 1.0f + yoffset, 0.0f + zoffset,
//        1.0f + xoffset, 1.0f + yoffset, 1.0f + zoffset,
//        0.0f + xoffset, 1.0f + yoffset, 1.0f + zoffset,
//
//        0.0f + xoffset, 0.0f + yoffset, 0.0f + zoffset,
//        1.0f + xoffset, 0.0f + yoffset, 0.0f + zoffset,
//        0.0f + xoffset, 1.0f + yoffset, 0.0f + zoffset,
//        0.0f + xoffset, 1.0f + yoffset, 0.0f + zoffset,
//        1.0f + xoffset, 0.0f + yoffset, 0.0f + zoffset,
//        1.0f + xoffset, 1.0f + yoffset, 0.0f + zoffset,
//
//        0.0f + xoffset, 0.0f + yoffset, 1.0f + zoffset,
//        0.0f + xoffset, 1.0f + yoffset, 1.0f + zoffset,
//        1.0f + xoffset, 0.0f + yoffset, 1.0f + zoffset,
//        0.0f + xoffset, 1.0f + yoffset, 1.0f + zoffset,
//        1.0f + xoffset, 1.0f + yoffset, 1.0f + zoffset,
//        1.0f + xoffset, 0.0f + yoffset, 1.0f + zoffset,
//    };
//
//    vector<GLfloat> finalVerts;
//    for (int i = 0; i < 36; i++) {
//        finalVerts.push_back(triangle[3 * i + 0]);
//        finalVerts.push_back(triangle[3 * i + 1]);
//        finalVerts.push_back(triangle[3 * i + 2]);
//
//        finalVerts.push_back(globalUVs[3 * i + 0]);
//        finalVerts.push_back(globalUVs[3 * i + 1]);
//        finalVerts.push_back(globalUVs[3 * i + 2]);
//    }
//
//    addMeshToWorld(vec3(xoffset, yoffset, zoffset), finalVerts, indices1, finalVerts.size(), indices1.size(), blockType);
//    world1.world.chunks[world1.world.chunks.size() - 1].blockNum++;
//}


//for (int i = 0; i >= -5; i--) {
//    for (int j = 0; j <= 5; j++) {
//        chunkCoords.push_back({ int(camera.getCameraPos().x / CHUNK_SIZE) + i, int(camera.getCameraPos().z / CHUNK_SIZE) + j });
//        //generateChunkAt(chunkCoords[chunkCoords.size() - 1]);
//        {
//            chunkGenRunning = true;
//            std::lock_guard<std::mutex> lock(chunkRequestMutex);
//            chunkRequestQueue.push(chunkCoords.back());
//        }
//        {
//            std::lock_guard<std::mutex> lock(chunkResultMutex);
//            //cout << chunkResultQueue.empty() << endl;
//            while (!chunkResultQueue.empty()) {
//                Chunk chunk = std::move(chunkResultQueue.front());
//                chunkResultQueue.pop();
//                world1.world.chunks.push_back(std::move(chunk));
//            }
//        }
//    }
//}
//
//for (int i = 0; i >= -5; i--) {
//    for (int j = 0; j >= -5; j--) {
//        chunkCoords.push_back({ int(camera.getCameraPos().x / CHUNK_SIZE) + i, int(camera.getCameraPos().z / CHUNK_SIZE) + j });
//        //generateChunkAt(chunkCoords[chunkCoords.size() - 1]);
//        {
//            chunkGenRunning = true;
//            std::lock_guard<std::mutex> lock(chunkRequestMutex);
//            chunkRequestQueue.push(chunkCoords.back());
//        }
//        {
//            std::lock_guard<std::mutex> lock(chunkResultMutex);
//            //cout << chunkResultQueue.empty() << endl;
//            while (!chunkResultQueue.empty()) {
//                Chunk chunk = std::move(chunkResultQueue.front());
//                chunkResultQueue.pop();
//                world1.world.chunks.push_back(std::move(chunk));
//            }
//        }
//    }
//}
//
//for (int i = 0; i >= -5; i--) {
//    for (int j = 0; j <= 5; j++) {
//        chunkCoords.push_back({ int(camera.getCameraPos().x / CHUNK_SIZE) + i, int(camera.getCameraPos().z / CHUNK_SIZE) + j });
//        //generateChunkAt(chunkCoords[chunkCoords.size() - 1]);
//        {
//            chunkGenRunning = true;
//            std::lock_guard<std::mutex> lock(chunkRequestMutex);
//            chunkRequestQueue.push(chunkCoords.back());
//        }
//        {
//            std::lock_guard<std::mutex> lock(chunkResultMutex);
//            //cout << chunkResultQueue.empty() << endl;
//            while (!chunkResultQueue.empty()) {
//                Chunk chunk = std::move(chunkResultQueue.front());
//                chunkResultQueue.pop();
//                world1.world.chunks.push_back(std::move(chunk));
//            }
//        }
//    }
//}



        //if ((camera.getCameraPos().x / CHUNK_SIZE) > (int)(chunkCoords.back().x - 20) ||
        //    (camera.getCameraPos().z / CHUNK_SIZE) > (int)(chunkCoords.back().y - 20)
        //    )
        //{
            //int sizeChunk = chunkCoords.size();
            //for (int i = 0; i <= 20; i++) {
            //    for (int j = 0; j <= 20; j++) {
            //        if (!contains(chunkCoords, { (int)(camera.getCameraPos().x / CHUNK_SIZE) + i, (int)(camera.getCameraPos().z / CHUNK_SIZE) + j })) {
            //            chunkCoords.push_back({ int(camera.getCameraPos().x / CHUNK_SIZE) + i, int(camera.getCameraPos().z / CHUNK_SIZE) + j });
            //            //generateChunkAt(chunkCoords[chunkCoords.size() - 1]);
            //            {
            //                chunkGenRunning = true;
            //                std::lock_guard<std::mutex> lock(chunkRequestMutex);
            //                chunkRequestQueue.push(chunkCoords.back());
            //            }
            //            {
            //                std::lock_guard<std::mutex> lock(chunkResultMutex);
            //                //cout << chunkResultQueue.empty() << endl;
            //                while (!chunkResultQueue.empty()) {
            //                    Chunk chunk = std::move(chunkResultQueue.front());
            //                    chunkResultQueue.pop();
            //                    world1.world.chunks.push_back(std::move(chunk));
            //                }
            //            }
            //        }
            //    }
            //}
        //}
