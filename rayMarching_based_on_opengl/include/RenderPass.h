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

    //渲染函数的委托时间
    RenderBehaviour renderBehaviours;

    GLuint FBO = 0;

    GLuint depthBuffer; 

    //附件
    vector<GLuint> attachments;
    //附件类型
    std::vector<GLuint> attachmentTypes;

    vector<Shader> shaders;

    bool finalPass = false;
    void bindData(bool finalPass = false) {

        

        //如果不是最后一个pass,创建一个帧缓冲
        if (!finalPass) glGenFramebuffers(1, &FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);

        // 不是 finalPass 则绑定附件
        if (!finalPass) {

            //std::vector<GLuint> attachments;
            for (int i = 0; i < attachments.size(); i++) {

                glBindTexture(GL_TEXTURE_2D, attachments[i]);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, attachments[i], 0);// 将颜色纹理绑定到 i 号颜色附件
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
            //std::cout << "执行  " << i << endl;;
            const auto& func = renderBehaviours[i];
            func(shaders[i]);
        }
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
};

#endif
