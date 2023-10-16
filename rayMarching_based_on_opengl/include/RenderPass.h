#ifndef RENDER_PASS
#define RENDER_PASS

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
#include <functional>

using RenderBehaviour = vector<std::function<void( Shader&)>>;

class RenderPass {

public:

    //��Ⱦ������ί��ʱ��
    RenderBehaviour renderBehaviours;

    GLuint FBO = 0;

    GLuint depthBuffer; 

    //����
    vector<GLuint> attachments;
    //��������
    std::vector<GLuint> attachmentTypes;

    vector<Shader> shaders;

    bool finalPass = false;
    void bindData(bool finalPass = false) {

        

        //����������һ��pass,����һ��֡����
        if (!finalPass) glGenFramebuffers(1, &FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);

        // ���� finalPass ��󶨸���
        if (!finalPass) {

            //std::vector<GLuint> attachments;
            for (int i = 0; i < attachments.size(); i++) {

                glBindTexture(GL_TEXTURE_2D, attachments[i]);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, attachments[i], 0);// ����ɫ����󶨵� i ����ɫ����
                //attachments.push_back(GL_COLOR_ATTACHMENT0 + i);
            }
            glDrawBuffers(attachmentTypes.size(), &attachmentTypes[0]);
        }

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void draw() {
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        //glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        for (int i = 0; i < renderBehaviours.size(); i++)
        {
            //std::cout << "ִ��  " << i << endl;;
            const auto& func = renderBehaviours[i];
            func(shaders[i]);
        }
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
};

#endif
