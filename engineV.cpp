//#define GLEW_STATIC

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
#include "files/file.h"



int WIDTH = 1280;
int HEIGHT = 720;

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
    Window::init(WIDTH, HEIGHT, "Minecraft 2.0");
    Events::init();


    Shader* shader = load_shader("res/main.glslv", "res/main.glslf");
    if (shader == nullptr) {
        std::cerr << "[MAIN] Can not load shader\n";
        Window::terminate();
        return 1;
    }

    Shader* crosshairShader = load_shader("res/crosshair.glslv", "res/crosshair.glslf");
    if (crosshairShader == nullptr) {
        std::cerr << "[MAIN] Can not load crosshair shader\n";
        delete shader;
        Window::terminate();
        return 1;
    }

    Shader* linesShader = load_shader("res/lines.glslv", "res/lines.glslf");
    if (linesShader == nullptr) {
        std::cerr << "[MAIN] Can not load lines shader\n";
        delete shader;
        delete crosshairShader;
        Window::terminate();
        return 1;
    }
    std::cout << "Shaders load SUCCESS\n";



    Texture* texture = load_texture("res/block.png");
    if (texture == nullptr) {
        std::cerr << "[MAIN] texture load failed\n";
        delete shader;
        Window::terminate();
        return 1;
    }
    std::cout << "Texture load SUCCESS\n";

    std::cout << "Rendering...";

    Chunks* chunks = new Chunks(4, 2, 4);
    std::cout << "OK - [" << chunks->vol << " chunks loaded]\n";
    Mesh** mesh_aaray = new Mesh * [chunks->vol];

    for (size_t i = 0; i < chunks->vol; i++) {
        mesh_aaray[i] = nullptr;
    }
    VoxelRenderer renderer(1024 * 1024 * 8);

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
    float deltaTime = 0.0f;
    float speed = 5;

    float camX = 0.0f;
    float camY = 0.0f;

    while (!Window::isShouldClose()) {
        curTime = glfwGetTime();
        deltaTime = curTime - lastTime;
        lastTime = curTime;

        
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
                write_binary_file("world.bin", (const char*)buffer, chunks->vol* CHUNK_VOL);
                delete[] buffer;
                std::cout << "world saved in " << (chunks->vol * CHUNK_VOL) << " bytes" << std::endl;
            }

            if (Events::isjPress(GLFW_KEY_F6)) {
                std::cout << "Load from world.bin...";
                unsigned char* buffer = new unsigned char[chunks->vol * CHUNK_VOL];
                read_binary_file("world.bin", (char*)buffer, chunks->vol* CHUNK_VOL);
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
                if (Events::isjClick(GLFW_MOUSE_BUTTON_1)) 
                    chunks->set((int)iend.x, (int)iend.y, (int)iend.z, 0);

                if (Events::isjClick(GLFW_MOUSE_BUTTON_2))
                    chunks->set((int)iend.x + (int)norm.x, (int)iend.y + (int)norm.y, (int)iend.z + (int)norm.z, 2);
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
            Mesh* curr_mesh = renderer.render(chunk, (const Chunk**)closes,true);
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
    delete shader;
    delete crosshairShader;
    delete texture;
    delete chunks;
    delete lBatch;
    delete linesShader;
    Window::terminate();
    return 0;
}