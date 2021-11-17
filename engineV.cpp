#define GLEW_STATIC

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "window/Window.h"
#include "window/Events.h"
#include "window/Camera.h"
#include "graphics/Shader.h"
#include "graphics/Texture.h"
#include "graphics/Mesh.h"
#include "graphics/VoxelRenderer.h"
#include "graphics/LineBatch.h"
#include "loaders/image_load.h"
#include "voxels/Chunk.h"
#include "voxels/Chunks.h"
#include "voxels/voxel.h"
#include "light//LightSolver.h"
#include "light//Lightmap.h"
#include "files/file.h"
#include "format/ProgressBar.hpp"



const int WIDTH = 1280;
const int HEIGHT = 720;

float vertices[] = {
    // x    y
   -0.01f,-0.01f,
    0.01f, 0.01f,

   -0.01f, 0.01f,
    0.01f,-0.01f,
};

int attrs[] = {
        2,  0 //null terminator
};

int main()
{
    std::cout << "Initiating...\n";
    Window::init(WIDTH, HEIGHT, "Minecraft 2.0");
    Events::init();
    std::cout << "Resolution: " << WIDTH << "x" << HEIGHT << '\n';

    std::cout << "Shader loading...\n";
    std::cout << "\tmain...";
    Shader* shader = load_shader("res/main.glslv", "res/main.glslf");
    if (shader == nullptr) {
        std::cerr << "[MAIN] Can not load shader\n";
        Window::terminate();
        return 1;
    }
    std::cout << "OK\n";

    std::cout << "\tcrosshair...";
    Shader* crosshairShader = load_shader("res/crosshair.glslv", "res/crosshair.glslf");
    if (crosshairShader == nullptr) {
        std::cerr << "[MAIN] Can not load crosshair shader\n";
        delete shader;
        Window::terminate();
        return 1;
    }
    std::cout << "OK\n";

    std::cout << "\tlines...";
    Shader* linesShader = load_shader("res/lines.glslv", "res/lines.glslf");
    if (linesShader == nullptr) {
        std::cerr << "[MAIN] Can not load lines shader\n";
        delete shader;
        delete crosshairShader;
        Window::terminate();
        return 1;
    }
    std::cout << "OK\n";


    Texture* texture = load_texture("res/block.png");
    if (texture == nullptr) {
        std::cerr << "[MAIN] texture load failed\n";
        delete shader;
        Window::terminate();
        return 1;
    }
    std::cout << "Texture load SUCCESS\n";

    std::cout << "Rendering...";

    Chunks* chunks = new Chunks(4, 4, 4);
    std::cout << "OK - [" << chunks->vol << " chunks loaded]\n";
    Mesh** mesh_aaray = new Mesh * [chunks->vol];

    for (size_t i = 0; i < chunks->vol; i++) {
        mesh_aaray[i] = nullptr;
    }

    size_t capacity = 1024 * 8;

    VoxelRenderer renderer(capacity);
    std::cout << "Render capacity: " << capacity << std::endl;

    LineBatch* lBatch = new LineBatch(4096);
   

    

    glClearColor(0.6f, 0.62f, 0.65f, 1);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    Mesh* crosshair = new Mesh(vertices, 4, attrs);
    Camera* camera = new Camera(vec3(0, 0, 20), glm::radians(90.0f));

    

    float curTime;
    float lastTime = glfwGetTime();
    int frameCount = 0;
    float deltaTime = 0.0f;
    float speed = 5;

    float camX = 0.0f;
    float camY = 0.0f;
    int choosenBlock = 1;

    LightSolver* solverR = new LightSolver(chunks, 0);
    LightSolver* solverG = new LightSolver(chunks, 1);
    LightSolver* solverB = new LightSolver(chunks, 2);
    LightSolver* solverS = new LightSolver(chunks, 3);

    std::cout << "Lightmapping... ";

    for (int y = 0; y < chunks->h * CHUNK_H; y++) {
        for (int z = 0; z < chunks->d * CHUNK_D; z++) {
            for (int x = 0; x < chunks->w * CHUNK_W; x++) {
                voxel* vox = chunks->get(x, y, z);
                if (vox->id == 3) {
                    solverR->add(x, y, z, 15);
                    solverG->add(x, y, z, 15);
                    solverB->add(x, y, z, 15);
                }
            }
        }
    }


    for (int z = 0; z < chunks->d * CHUNK_D; z++) {
        for (int x = 0; x < chunks->w * CHUNK_W; x++) {
            for (int y = chunks->h * CHUNK_H - 1; y >= 0; y--) {
                voxel* vox = chunks->get(x, y, z);
                if (vox->id != 0)
                    break;
                chunks->getChunkByVoxel(x, y, z)->lightmap->setS(x % CHUNK_W, y % CHUNK_H, z % CHUNK_D, 0xF);
            }
        }
    }


    for (int z = 0; z < chunks->d * CHUNK_D; z++) {
        for (int x = 0; x < chunks->w * CHUNK_W; x++) {
            for (int y = chunks->h * CHUNK_H - 1; y >= 0; y--) {
                voxel* vox = chunks->get(x, y, z);
                if (vox->id != 0) {
                    break;
                }
                if (
                    chunks->getLight(x - 1, y, z, 3) == 0 ||
                    chunks->getLight(x + 1, y, z, 3) == 0 ||
                    chunks->getLight(x, y - 1, z, 3) == 0 ||
                    chunks->getLight(x, y + 1, z, 3) == 0 ||
                    chunks->getLight(x, y, z - 1, 3) == 0 ||
                    chunks->getLight(x, y, z + 1, 3) == 0
                    ) {
                    solverS->add(x, y, z);
                }
                chunks->getChunkByVoxel(x, y, z)->lightmap->setS(x % CHUNK_W, y % CHUNK_H, z % CHUNK_D, 0xF);
            }
        }
    }

    solverR->solve();
    solverG->solve();
    solverB->solve();
    solverS->solve();
    std::cout << "OK\n";
    double secondsTimer = glfwGetTime();
    std::cout << "All Done\n";
    while (!Window::isShouldClose()) {
        curTime = glfwGetTime();
        frameCount++;
        deltaTime = curTime - lastTime;
        lastTime = curTime;

        if (curTime - secondsTimer >= 1.0) {

            std::cout << "\rFPS: " << frameCount;
            frameCount = 0;
            secondsTimer = curTime;
        }


        if (Events::isjPress(GLFW_KEY_ESCAPE))
            Window::setShouldClose(true);


        if (Events::isjPress(GLFW_KEY_TAB))
            Events::toogleCursor();


        if (Events::isjPress(GLFW_KEY_E)) {
            glm::vec3 dend, dnorm, diend;
            voxel* vox = chunks->rayCast(camera->pos, vec3(vec4(0, -1, 0, 1)), 25.0f, dend, dnorm, diend);
            if (vox != nullptr) {
                std::cout << "nearest = \n\tx=" << (int)diend.x << "\n\ty=" << (int)diend.x << "\n\tz=" << (int)diend.z << std::endl;
                std::cout << "\Height = " << chunks->t << std::endl;
            }
        }


        if (Events::_cursor_locked) {

            if (Events::isPress(GLFW_KEY_W))
                camera->pos += camera->front * deltaTime * speed;

            if (Events::isPress(GLFW_KEY_A))
                camera->pos -= camera->right * deltaTime * speed;

            if (Events::isPress(GLFW_KEY_S))
                camera->pos -= camera->front * deltaTime * speed;

            if (Events::isPress(GLFW_KEY_D))
                camera->pos += camera->right * deltaTime * speed;

            if (Events::isPress(GLFW_KEY_SPACE))
                camera->pos += camera->up * deltaTime * speed;

            if (Events::isPress(GLFW_KEY_LEFT_SHIFT))
                camera->pos -= camera->up * deltaTime * speed;


            if (Events::isjPress(GLFW_KEY_F5)) {
                std::cout << "Saving...";
                unsigned char* buffer = new unsigned char[chunks->vol * CHUNK_VOL];
                chunks->saveWorld(buffer);
                write_binary_file("world.bin", (const char*)buffer, chunks->vol * CHUNK_VOL);
                delete[] buffer;
                std::cout << "world saved in " << (chunks->vol * CHUNK_VOL) << " bytes" << std::endl;
            }

            if (Events::isjPress(GLFW_KEY_F6)) {
                std::cout << "Load from world.bin...";
                unsigned char* buffer = new unsigned char[chunks->vol * CHUNK_VOL];
                read_binary_file("world.bin", (char*)buffer, chunks->vol * CHUNK_VOL);
                chunks->loadWorld(buffer);
                std::cout << " OK\n";
                delete[] buffer;

            }

            camY += -Events::dy / Window::height;
            camX += -Events::dx / Window::height;

            if (camY < -glm::radians(85.0f)) {
                camY = -radians(85.0f);
            }

            if (camY > glm::radians(85.0f)) {
                camY = radians(85.0f);
            }

            camera->rot = mat4(1.0f);   //CHANGE
            camera->move(camY, camX, 0);
        }



        glm::vec3 end, norm, iend;
        voxel* vox = chunks->rayCast(camera->pos, camera->front, 10.0f, end, norm, iend);

        if (vox != nullptr) {
            lBatch->box(iend.x + 0.5f, iend.y + 0.5f, iend.z + 0.5f, 1.005f, 1.005f, 1.005f, 0, 0, 0, 0.5f);
            if (Events::isjClick(GLFW_MOUSE_BUTTON_1)) {
                int x = (int)iend.x;
                int y = (int)iend.y;
                int z = (int)iend.z;
                chunks->set(x, y, z, 0);

                solverR->remove(x, y, z);
                solverG->remove(x, y, z);
                solverB->remove(x, y, z);

                solverR->solve();
                solverG->solve();
                solverB->solve();

                if (chunks->getLight(x, y + 1, z, 3) == 0xF) {
                    for (int i = y; i >= 0; i--) {
                        if (chunks->get(x, i, z)->id != 0)
                            break;
                        solverS->add(x, i, z, 0xF);
                    }
                }

                solverR->add(x, y + 1, z); solverG->add(x, y + 1, z); solverB->add(x, y + 1, z); solverS->add(x, y + 1, z);
                solverR->add(x, y - 1, z); solverG->add(x, y - 1, z); solverB->add(x, y - 1, z); solverS->add(x, y - 1, z);
                solverR->add(x + 1, y, z); solverG->add(x + 1, y, z); solverB->add(x + 1, y, z); solverS->add(x + 1, y, z);
                solverR->add(x - 1, y, z); solverG->add(x - 1, y, z); solverB->add(x - 1, y, z); solverS->add(x - 1, y, z);
                solverR->add(x, y, z + 1); solverG->add(x, y, z + 1); solverB->add(x, y, z + 1); solverS->add(x, y, z + 1);
                solverR->add(x, y, z - 1); solverG->add(x, y, z - 1); solverB->add(x, y, z - 1); solverS->add(x, y, z - 1);

                solverR->solve();
                solverG->solve();
                solverB->solve();
                solverS->solve();

            }


            if (Events::isjClick(GLFW_MOUSE_BUTTON_2)) {
                int x = (int)(iend.x) + (int)(norm.x);
                int y = (int)(iend.y) + (int)(norm.y);
                int z = (int)(iend.z) + (int)(norm.z);
                chunks->set(x, y, z, choosenBlock);
                solverR->remove(x, y, z);
                solverG->remove(x, y, z);
                solverB->remove(x, y, z);
                solverS->remove(x, y, z);
                for (int i = y - 1; i >= 0; i--) {
                    solverS->remove(x, i, z);
                    if (i == 0 || chunks->get(x, i - 1, z)->id != 0) {
                        break;
                    }
                }
                solverR->solve();
                solverG->solve();
                solverB->solve();
                solverS->solve();
                if (choosenBlock == 3) {
                    solverR->add(x, y, z, 10);
                    solverG->add(x, y, z, 10);
                    solverB->add(x, y, z, 0);
                    solverR->solve();
                    solverG->solve();
                    solverB->solve();
                }
            }

            if (Events::isjPress(GLFW_KEY_F)) {
                std::cout << "look at \n\tx=" << (int)iend.x << "\n\ty=" << (int)iend.x << "\n\tz=" << (int)iend.z << std::endl;
                std::cout << "\tDistance = " << chunks->t << std::endl;
            }
            Chunk* closes[27];
            int ox, oy, oz;
            for (int i = 0; i < chunks->vol; i++) {
                Chunk* chunk = chunks->chunks[i];
                if (!chunk->modified)
                    continue;
                chunk->modified = false;
                if (mesh_aaray[i] != nullptr)
                    delete mesh_aaray[i];

                for (int i = 0; i < 27; i++)
                    closes[i] = nullptr;
                for (size_t j = 0; j < chunks->vol; j++) {
                    Chunk* oth = chunks->chunks[j];
                    ox = oth->x - chunk->x;
                    oy = oth->y - chunk->y;
                    oz = oth->z - chunk->z;

                    if (abs(ox) > 1 || abs(oy) > 1 || abs(oz) > 1)
                        continue;

                    ox++;
                    oy++;
                    oz++;
                    closes[(oy * 3 + oz) * 3 + ox] = oth;
                }
                Mesh* curr_mesh = renderer.render(chunk, (const Chunk**)closes);
                mesh_aaray[i] = curr_mesh;

            }

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            //VAO draw
            shader->use();
            shader->uniform_matrix("projview", camera->getProj() * camera->getView());
            texture->bind();
            glm::mat4 model(1.0f);

            for (size_t i = 0; i < chunks->vol; i++) {
                Chunk* chunk = chunks->chunks[i];
                Mesh* mesh = mesh_aaray[i];
                model = glm::translate(mat4(1.0f), vec3(chunk->x * CHUNK_W + 0.5f,
                    chunk->y * CHUNK_H + 0.5f,
                    chunk->z * CHUNK_D + 0.5f));
                shader->uniform_matrix("model", model);
                mesh->draw(GL_TRIANGLES);
            }

            crosshairShader->use();
            crosshair->draw(GL_LINES);
            linesShader->use();
            linesShader->uniform_matrix("projview", camera->getProj() * camera->getView());


            lBatch->line(
                0.0f, 0.0f, 0.0f,
                0.0f, 10.0f, 0.0f,
                1.0f, 0.0f, 0.0f, 1.0f
            );
            glLineWidth(2.0f);
            lBatch->render();
            Window::swapBuffers();
            Events::refresh();
        }
    }

    delete solverS;
    delete solverB;
    delete solverG;
    delete solverR;

    delete shader;
    delete crosshairShader;
    delete texture;
    delete chunks;
    delete lBatch;
    delete linesShader;

    Window::terminate();
    return 0;
}