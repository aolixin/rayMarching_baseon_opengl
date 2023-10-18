#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <shader.h>
#include <camera.h>
#include <model.h>
#include <iostream>
#include "Utils.h"
#include "RenderPass.h"



void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

const float cloudSize = 50;
float cloudHeight = 5;
float cloudThickness = 4;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//光源
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
glm::vec3 sunPos(0.0f, 10.0f, 0.0f);



int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    //Shader ourShader("Resources/shaders/draw_model.vert", "Resources/shaders/draw_model.frag");
    Shader blinn_phong_shader("Resources/shaders/blinn_phong.vert", "Resources/shaders/blinn_phong.frag");
    Shader rayMarchingShader("Resources/shaders/rayMarching.vert", "Resources/shaders/rayMarching.frag");
    //天空盒shader
    Shader renderSkyBoxShader("Resources/shaders/renderSkyBoxShader.vert", "Resources/shaders/renderSkyBoxShader.frag");
    //渲染纹理到屏幕
    Shader TexToScreenShader("Resources/shaders/TexToScreen.vert", "Resources/shaders/TexToScreen.frag");
    Shader cloudPostProcessingShader("Resources/shaders/cloudPostProcessing.vert", "Resources/shaders/cloudPostProcessing.frag");

    // load models
    // -----------
    Model bunny("Resources/models/spot.obj");
    Model front("Resources/models/front.obj");


    GLint envCubemap = buildEnvcubMap();

    // pass1 --------------------------------------------------------------------------
    // 渲染基础场景
    RenderPass pass1;
    pass1.attachments.push_back(getTextureRGBA32F(SCR_WIDTH, SCR_HEIGHT)); //颜色
    pass1.attachmentTypes.push_back(GL_COLOR_ATTACHMENT0);
    
    pass1.depthBuffer = getDepthBuffer();

    //渲染天空盒
    pass1.renderBehaviours.push_back(renderSkyBox);
    //渲染两个模型
    pass1.renderBehaviours.push_back(std::bind(&Model::Draw, std::ref(front), std::placeholders::_1));
    pass1.renderBehaviours.push_back(std::bind(&Model::Draw, std::ref(bunny), std::placeholders::_1));

    
    pass1.shaders.push_back(renderSkyBoxShader);
    pass1.shaders.push_back(blinn_phong_shader);
    pass1.shaders.push_back(blinn_phong_shader);

    pass1.bindData();//绑定数据

    // pass2 --------------------------------------------------------------------------
    // 渲染体积云
    RenderPass pass2;
    //pass2.attachments.push_back(pass1.attachments[0]); //颜色
    pass2.attachments.push_back(getTextureRGBA32F(SCR_WIDTH, SCR_HEIGHT)); //颜色
    pass2.attachmentTypes.push_back(GL_COLOR_ATTACHMENT0);

    pass2.depthBuffer = pass1.depthBuffer;

    pass2.renderBehaviours.push_back(drawCube);
    pass2.renderBehaviours.push_back(renderToScreen);

    pass2.shaders.push_back(rayMarchingShader);
    pass2.shaders.push_back(cloudPostProcessingShader);

    pass2.bindData();

    // pass3 --------------------------------------------------------------------------
    // 输出到屏幕
    RenderPass pass3;
    pass3.renderBehaviours.push_back(renderToScreen);
    pass3.shaders.push_back(TexToScreenShader);
    pass3.bindData(true);

    //绑定纹理
    rayMarchingShader.use();
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, pass1.attachments[0]);
    rayMarchingShader.setInt("TexBgColor", 2);
    
    cloudPostProcessingShader.use();
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, pass2.attachments[0]);
    cloudPostProcessingShader.setInt("TexInput", 3);


    
    /*glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, noiseMap);
    rayMarchingShader.setInt("NoiseMap", 3);*/



    // render loop
    // -----------

    int frameCount = 0;
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        //std::cout << frameCount++ << endl;
        // input
        // -----
        processInput(window);
        frameCount++;
        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        //renderSkyBox(passToScreenShader, envCubemap, camera);
        // pass1 ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        blinn_phong_shader.use();

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down

        blinn_phong_shader.setMat4("projection", projection);
        blinn_phong_shader.setMat4("view", view);
        blinn_phong_shader.setMat4("model", model);

        blinn_phong_shader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        blinn_phong_shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        blinn_phong_shader.setVec3("lightPos", lightPos);
        blinn_phong_shader.setVec3("viewPos", camera.Position);

        //天空盒shader
        renderSkyBoxShader.use();
        glm::mat4 passToProjection = glm::perspective(glm::radians(camera.Zoom), (float)1920 / (float)1080, 0.1f, 100.0f);
        glm::mat4 passToView = camera.GetViewMatrix();
        
        renderSkyBoxShader.setMat4("projection", passToProjection);
        renderSkyBoxShader.setMat4("view", passToView);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
        renderSkyBoxShader.setInt("environmentMap", 0);


        glViewport(0, 0, 1920, 1080);
        
        pass1.draw();

        // pass2 ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        rayMarchingShader.use();
        rayMarchingShader.setMat4("projection", projection);
        rayMarchingShader.setMat4("view", view);
        model = glm::translate(model, glm::vec3(0.0f, cloudHeight, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(cloudSize, cloudThickness, cloudSize));	// it's a bit too big for our scene, so scale it down
        //model = glm::scale(model, glm::vec3(1.0, 1.0f, 1.0));
        rayMarchingShader.setMat4("model", model);
        rayMarchingShader.setVec3("viewPos", camera.Position);
        rayMarchingShader.setFloat("cloudSize", cloudSize);
        rayMarchingShader.setFloat("cloudHeight", cloudHeight);
        rayMarchingShader.setFloat("cloudThickness", cloudThickness);
        rayMarchingShader.setVec3("lightPos", sunPos);
        rayMarchingShader.setInt("frameCount", frameCount);

        pass2.draw(true);

        // pass3 ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        TexToScreenShader.use();
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, pass1.attachments[0]);
        TexToScreenShader.setInt("TexBackground", 4);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, pass2.attachments[0]);
        TexToScreenShader.setInt("TexCloud", 5);


        TexToScreenShader.setFloat("cloudSize", cloudSize);

        pass3.draw();


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        cloudHeight += 0.5;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        cloudHeight -= 0.5;

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}