#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>

namespace Game
{
    GLFWwindow* window;
    int height, width;

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }

    void InitializeWindow(int width, int height, std::string name)
    {
        if (!glfwInit())
            exit(-1);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);

        if (window == nullptr)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            exit(-1);
        }

        glfwMakeContextCurrent(window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            exit(-1);
        }

        glViewport(0, 0, width, height);

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    }

    void HandleInput()
    {
        
    }

    void Run()
    {
       

        //main application loop
        while (!glfwWindowShouldClose(window))
        {
            //Input
            HandleInput();
            //Rendering
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            //Check events and swap buffers
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }
}


int main()
{
    Game::InitializeWindow(800, 600, "exemple");
    Game::Run();
    return 0;
}
