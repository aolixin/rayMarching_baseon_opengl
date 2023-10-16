//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>
//#include "shader.h""
//#include "camera.h"
//#include "model.h"
//#include "RenderPass.h"
//#include "Utils.h"
//#include <iostream>
//#include "Scene.h"
//
//
//Scene scene;
//
//
//void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void mouse_callback(GLFWwindow* window, double xpos, double ypos);
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
//void processInput(GLFWwindow* window);
////void renderSkyBox(Shader passToScreenShader, GLint envCubemap);
//
//
//// settings
//const unsigned int SCR_WIDTH = 1920;
//const unsigned int SCR_HEIGHT = 1080;
//
//// camera
//Camera camera(glm::vec3(0.0f, 0.0f, 3.5f));
////Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));
//float lastX = SCR_WIDTH / 2.0f;
//float lastY = SCR_HEIGHT / 2.0f;
//bool firstMouse = true;
//
//// timing
//float deltaTime = 0.0f;
//float lastFrame = 0.0f;
//
//int main() {
//
//    // glfw: initialize and configure
//    // ------------------------------
//    glfwInit();
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//
//#ifdef __APPLE__
//    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//#endif
//
//    // glfw window creation
//    // --------------------
//    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
//    if (window == NULL)
//    {
//        std::cout << "Failed to create GLFW window" << std::endl;
//        glfwTerminate();
//        return -1;
//    }
//    glfwMakeContextCurrent(window);
//    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
//    glfwSetCursorPosCallback(window, mouse_callback);
//    glfwSetScrollCallback(window, scroll_callback);
//
//    // tell GLFW to capture our mouse
//    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//
//    // glad: load all OpenGL function pointers
//    // ---------------------------------------
//    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
//    {
//        std::cout << "Failed to initialize GLAD" << std::endl;
//        return -1;
//    }
//
//    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
//    stbi_set_flip_vertically_on_load(true);
//
//    // configure global opengl state
//    // -----------------------------
//    glEnable(GL_DEPTH_TEST);
//
//    //将立方体贴图渲染出来
//    Shader passToScreenShader("Resources/shaders/passToScreen.vert", "Resources/shaders/passToScreen.frag");
//    Shader pass2Shader("Resources/shaders/pass2.vert", "Resources/shaders/pass2.frag");
//
//    GLint envCubemap = buildEnvcubMap();
//    GLint irradianceMap = buildIrradianceMap(envCubemap);
//
//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
//    // build and compile shaders
//    // -------------------------
//    Shader ourShader("Resources/shaders/RT.vert", "Resources/shaders/RT.frag");
//    ourShader.use();
//    ourShader.setInt("irradianceMap", 0);
//
//    // load models
//    Model front("Resources/models/front.obj");
//    Model left("Resources/models/left.obj");
//    Model right("Resources/models/right.obj");
//    Model top("Resources/models/top.obj");
//    //Model bunny("models/bunny.obj");
//    Model bunny("Resources/models/spot.obj");
//
//    //设置材质的属性
//    front.meshes[0].material.baseColor = vec3(0.5, 0.2, 0.2);
//    left.meshes[0].material.baseColor = vec3(0.2, 0.5, 0.2);
//    right.meshes[0].material.baseColor = vec3(0.2, 0.2, 0.5);
//    top.meshes[0].material.baseColor = vec3(1, 1, 1);
//    bunny.meshes[0].material.baseColor = vec3(0.5, 0.5, 0.5);
//
//    top.meshes[0].material.emissive = vec3(1, 1, 1);
//    left.meshes[0].material.emissive = vec3(0, 1, 0);
//    right.meshes[0].material.emissive = vec3(0, 0, 1);
//    bunny.meshes[0].material.emissive = vec3(0, 0, 0);
//
//    //设置shader
//    //front.setShader(ourShader);
//    front.meshes[0].setShader(ourShader);
//    left.meshes[0].setShader(ourShader);
//    right.meshes[0].setShader(ourShader);
//    top.meshes[0].setShader(ourShader);
//    bunny.meshes[0].setShader(ourShader);
//
//    Scene myScene;
//    myScene.Add(front);
//    myScene.Add(left);
//    myScene.Add(right);
//    myScene.Add(top);
//    myScene.Add(bunny);
//
//
//    myScene.setupScene();
//
//    RenderPass pass1;
//    RenderPass pass2(true);
//
//    pass1.renderBehaviour = std::bind(&Scene::Draw, &myScene);
//    pass2.renderBehaviour = []() { renderToScreen(); };
//
//    int frameCount = 0;
//
//    // render loop
//    // -----------
//    while (!glfwWindowShouldClose(window))
//    {
//        std::cout << "第" << frameCount << "帧\n";
//        // per-frame time logic
//        // --------------------
//        float currentFrame = static_cast<float>(glfwGetTime());
//        deltaTime = currentFrame - lastFrame;
//        lastFrame = currentFrame;
//
//        // input
//        // -----
//        processInput(window);
//
//        // render
//        // ------
//        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//        //绘制天空盒
//        //glBindFramebuffer(GL_FRAMEBUFFER, pass1.FBO);
//        renderSkyBox(passToScreenShader, envCubemap, camera);
//        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
//        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//        // don't forget to enable shader before setting uniforms
//        ourShader.use();
//
//        ourShader.setVec3("eye_pos", camera.Position);
//        ourShader.setVec3("lightPos", vec3(0.0, 0.5, 1));
//        // view/projection transformations
//        //glm::mat4 projection = glm::mat4(1.0f);
//        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
//        glm::mat4 view = camera.GetViewMatrix();
//        ourShader.setMat4("projection", projection);
//        ourShader.setMat4("view", view);
//
//        mat4 view_inverse = inverse(view);
//        mat4 projection_inverse = inverse(projection);
//        //std::cout << projection_inverse[0][0];
//        ourShader.setMat4("view_inverse", view_inverse);
//        ourShader.setMat4("projection_inverse", projection_inverse);
//
//        // render the loaded model
//        glm::mat4 model = glm::mat4(1.0f);
//        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
//        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
//        //model = glm::scale(model, glm::vec3(0.001f, 0.001f, 0.001f));	
//        ourShader.setMat4("model", model);
//        glActiveTexture(GL_TEXTURE1);
//        glBindTexture(GL_TEXTURE_2D, pass1.textureColorBuffer);
//        ourShader.setInt("lastFrame", 1);
//        ourShader.setInt("frameCounter", frameCount);
//        pass1.draw();
//
//        pass2Shader.use();
//        pass2Shader.setMat4("projection", projection);
//        pass2Shader.setMat4("view", view);
//        pass2Shader.setMat4("model", model);
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, pass1.textureColorBuffer);
//        pass2Shader.setInt("lastFrame", 0);
//        pass2.draw();
//        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
//        // -------------------------------------------------------------------------------
//        glfwSwapBuffers(window);
//        glfwPollEvents();
//        frameCount++;
//    }
//
//    // glfw: terminate, clearing all previously allocated GLFW resources.
//    // ------------------------------------------------------------------
//    glfwTerminate();
//    std::cout << "运行结束\n";
//    //system("pause");
//    return 0;
//}
//
//// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
//// ---------------------------------------------------------------------------------------------------------
//void processInput(GLFWwindow* window)
//{
//    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//        glfwSetWindowShouldClose(window, true);
//
//    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
//        camera.ProcessKeyboard(FORWARD, deltaTime);
//    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
//        camera.ProcessKeyboard(BACKWARD, deltaTime);
//    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
//        camera.ProcessKeyboard(LEFT, deltaTime);
//    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
//        camera.ProcessKeyboard(RIGHT, deltaTime);
//}
//
//// glfw: whenever the window size changed (by OS or user resize) this callback function executes
//// ---------------------------------------------------------------------------------------------
//void framebuffer_size_callback(GLFWwindow* window, int width, int height)
//{
//    // make sure the viewport matches the new window dimensions; note that width and 
//    // height will be significantly larger than specified on retina displays.
//    glViewport(0, 0, width, height);
//}
//
//// glfw: whenever the mouse moves, this callback is called
//// -------------------------------------------------------
//void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
//{
//    float xpos = static_cast<float>(xposIn);
//    float ypos = static_cast<float>(yposIn);
//
//    if (firstMouse)
//    {
//        lastX = xpos;
//        lastY = ypos;
//        firstMouse = false;
//    }
//
//    float xoffset = xpos - lastX;
//    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
//
//    lastX = xpos;
//    lastY = ypos;
//
//    camera.ProcessMouseMovement(xoffset, yoffset);
//}
//
//// glfw: whenever the mouse scroll wheel scrolls, this callback is called
//// ----------------------------------------------------------------------
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
//{
//    camera.ProcessMouseScroll(static_cast<float>(yoffset));
//}
//
////void renderSkyBox(Shader passToScreenShader, GLint envCubemap)
////{
////    passToScreenShader.use();
////    glm::mat4 passToProjection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
////    glm::mat4 passToView = camera.GetViewMatrix();
////
////    passToScreenShader.setMat4("projection", passToProjection);
////    passToScreenShader.setMat4("view", passToView);
////
////    glActiveTexture(GL_TEXTURE0);
////    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
////    passToScreenShader.setInt("environmentMap", 0);
////    glDisable(GL_DEPTH_TEST);
////    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
////    //drawSkyBox(screenShader);
////    drawCube(passToScreenShader);
////    glEnable(GL_DEPTH_TEST);
////}
//
//
