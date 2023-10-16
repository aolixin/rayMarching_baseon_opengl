#ifndef UTILS_H
#define UTILS_H
//#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h"
#include "shader.h"
#include "camera.h"
#include "model.h"
#define STB_IMAGE_IMPLEMENTATION
// ������Ⱦ��պе�������
GLfloat cubeVertices[] = {
    // λ������         
    -1.0f, -1.0f, -1.0f, // ���½�
     1.0f, -1.0f, -1.0f,  // ���½�
     1.0f,  1.0f, -1.0f,  // ���Ͻ�
    -1.0f,  1.0f, -1.0f, // ���Ͻ�
    -1.0f, -1.0f,  1.0f, // ���½�
     1.0f, -1.0f,  1.0f,  // ���½�
     1.0f,  1.0f,  1.0f,  // ���Ͻ�
    -1.0f,  1.0f,  1.0f,  // ���Ͻ�
};
// ������Ⱦ��պе�������Ķ�������
GLuint cubeIndices[] = {
    0, 1, 2, 2, 3, 0, // ����
    4, 5, 6, 6, 7, 4, // ����
    3, 2, 6, 6, 7, 3, // ǰ��
    0, 1, 5, 5, 4, 0, // ����
    0, 3, 7, 7, 4, 0, // �����
    1, 2, 6, 6, 5, 1  // �Ҳ���
};



void drawCube(Shader shader)
{
    shader.use();
    //glViewport(0, 0, 1920, 1080);
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // ��������VBO
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    // ��������EBO
    GLuint EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

    // ���ö�������ָ��
    // λ������
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
    //glClear( GL_DEPTH_BUFFER_BIT);
    // ʹ����ɫ������
    shader.use();

    // ��VAO��׼������
    glBindVertexArray(VAO);
    // ����������
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    // ���VAO
    glBindVertexArray(0);
    // ���VAO��VBO��EBO
    glBindVertexArray(0);
}


unsigned int load_hdr_img(string path)
{
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrComponents;
    float* data = stbi_loadf(path.c_str(), &width, &height, &nrComponents, 0);
    unsigned int hdrTexture;
    if (data)
    {
        glGenTextures(1, &hdrTexture);
        glBindTexture(GL_TEXTURE_2D, hdrTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Failed to load HDR image." << std::endl;
    }
    return hdrTexture;
}

GLint LoadTexture(string path)
{
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    return texture;
}


GLint buildEnvcubMap()
{
    //��hdr��Ⱦ����������ͼ
    Shader buildSkyboxShader("Resources/shaders/hdrToSkyobox.vert", "Resources/shaders/hdrToSkyobox.frag");
    //fbo
    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    unsigned int envCubemap;
    glGenTextures(1, &envCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        // note that we store each face with 16 bit floating point values
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
            512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // �������ӵ���ǰ�󶨵�֡�������
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, envCubemap, 0);


    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    //glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 512, 512);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512,512 );
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    unsigned int hdrSkyboxTexture = load_hdr_img("Resources/textrues/hdr/test4.hdr");
    glDepthFunc(GL_LEQUAL);
    // ��һ����׶�(Pass)
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // �������ڲ�ʹ��ģ�建��
    glEnable(GL_DEPTH_TEST);

    buildSkyboxShader.use();
    //hdrSkyboxShader.setMat4("projection", projection);
    //hdrSkyboxShader.setMat4("view", view);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrSkyboxTexture);
    buildSkyboxShader.setInt("equirectangularMap", 0);
    //------------------------------------------------------
    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 100.0f);
    glm::mat4 captureViews[] =
    {
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    buildSkyboxShader.setMat4("projection", captureProjection);

    glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
    for (unsigned int i = 0; i < 6; ++i)
    {
        buildSkyboxShader.setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        drawCube(buildSkyboxShader); // renders a 1x1 cube
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return envCubemap;
}

GLint buildIrradianceMap(GLint envCubeMap = -9999)
{
    //std::cout << "ִ��" << endl;
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    //glClearColor(0.8f, 0.0f, 0.0f, 1.0f);
    
    Shader buildIrradianceMapShader("Resources/shaders/hdrToSkyobox.vert", "Resources/shaders/irradiance_convolution.frag");
    //fbo
    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    unsigned int IrradianceMap;
    glGenTextures(1, &IrradianceMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, IrradianceMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        // note that we store each face with 16 bit floating point values
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
            32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    // �������ӵ���ǰ�󶨵�֡�������
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, IrradianceMap, 0);

    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);
    //glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 32, 32);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (envCubeMap == -9999)
    {
        envCubeMap = buildEnvcubMap();
    }

    //unsigned int hdrSkyboxTexture = load_hdr_img("resources/textrues/hdr/test2.hdr");
    glDepthFunc(GL_LEQUAL);
    // ��һ����׶�(Pass)
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // �������ڲ�ʹ��ģ�建��
    glEnable(GL_DEPTH_TEST);

    buildIrradianceMapShader.use();
    //hdrSkyboxShader.setMat4("projection", projection);
    //hdrSkyboxShader.setMat4("view", view);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubeMap);
    buildIrradianceMapShader.setInt("environmentMap", 0);
    glActiveTexture(GL_TEXTURE0);
    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1280.0f/720.0f,0.1f, 10.0f);
    buildIrradianceMapShader.setMat4("projection", captureProjection);

    glm::mat4 captureViews[] =
    {
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };
    glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
    //glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    for (unsigned int i = 0; i < 6; ++i)
    {
        buildIrradianceMapShader.setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, IrradianceMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        drawCube(buildIrradianceMapShader); // renders a 1x1 cube
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return IrradianceMap;
}

//void renderToScreen()
//{
//    GLfloat vertices[] = {
//        // �������� (x, y) ����ɫ (r, g, b)
//        -1.0f, -1.0f,   // ���½�
//         1.0f, -1.0f,   // ���½�
//         1.0f,  1.0f,  // ���Ͻ�
//        -1.0f,  1.0f,   // ���Ͻ�
//    };
//    GLuint indices[] = {
//    0, 1, 2, // ��һ��������
//    2, 3, 0  // �ڶ���������
//    };
//    // ���������������VAO��
//    GLuint VAO;
//    glGenVertexArrays(1, &VAO);
//    glBindVertexArray(VAO);
//
//    // �������㻺�����VBO��
//    GLuint VBO;
//    glGenBuffers(1, &VBO);
//    glBindBuffer(GL_ARRAY_BUFFER, VBO);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//
//    // ���������������EBO��
//    GLuint EBO;
//    glGenBuffers(1, &EBO);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
//
//    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);
//    glEnableVertexAttribArray(0);
//
//    glBindVertexArray(VAO);
//    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//    glBindVertexArray(0);
//}

void renderToScreen(Shader shader)
{
    shader.use();
    GLfloat vertices[] = {
        // �������� (x, y) 
        -1.0f, -1.0f,   // ���½�
         1.0f, -1.0f,   // ���½�
         1.0f,  1.0f,  // ���Ͻ�
        -1.0f,  1.0f,   // ���Ͻ�
    };
    GLuint indices[] = {
    0, 1, 2, // ��һ��������
    2, 3, 0  // �ڶ���������
    };
    // ���������������VAO��
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // �������㻺�����VBO��
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // ���������������EBO��
    GLuint EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

//void renderSkyBox(Shader passToScreenShader, GLint envCubemap, Camera camera)
//{
//    glViewport(0, 0, 1920, 1080);
//    passToScreenShader.use();
//    glm::mat4 passToProjection = glm::perspective(glm::radians(camera.Zoom), (float)1920 / (float)1080, 0.1f, 100.0f);
//    glm::mat4 passToView = camera.GetViewMatrix();
//
//    passToScreenShader.setMat4("projection", passToProjection);
//    passToScreenShader.setMat4("view", passToView);
//
//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
//    passToScreenShader.setInt("environmentMap", 0);
//    glDisable(GL_DEPTH_TEST);
//    glViewport(0, 0, 1920, 1080);
//    //drawSkyBox(screenShader);
//    drawCube(passToScreenShader);
//    glEnable(GL_DEPTH_TEST);
//}

void renderSkyBox(Shader& passToScreenShader)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, 1920, 1080);
    passToScreenShader.use();

    glDisable(GL_DEPTH_TEST);
    glViewport(0, 0, 1920, 1080);
    //drawSkyBox(screenShader);
    drawCube(passToScreenShader);
    glEnable(GL_DEPTH_TEST);
}


GLuint getTextureRGBA32F(int width, int height) {

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    return tex;
}

GLuint getTextureRGB32F(int width, int height) {

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    return tex;
}

GLuint getDepthBuffer()
{
    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1920, 1080);
    return rbo;
}


#endif // !UTILS
