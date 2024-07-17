
#include "engine/cEngine.h"
cEngine g_Engine;

#include <iostream>
#include <locale.h>

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    g_Engine.KeyCallback(key, scancode, action, mods);
}

void mouseCallback(GLFWwindow* window, int button, int action, int mods)
{
    g_Engine.MouseCallback(button, action, mods);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    g_Engine.ScrollCallback(xoffset, yoffset);
}

void cursorCallback(GLFWwindow* window, double xpos, double ypos) 
{
    g_Engine.CursorCallback(xpos, ypos);
}

void windowSizeCallback(GLFWwindow* window, int width, int height)
{
    g_Engine.WindowResize(width, height);
}

void errorCallback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

int main(void)
{
    setlocale(LC_ALL, "en_US.utf8");
     
    glfwSetErrorCallback(errorCallback);

    if (!glfwInit()) exit(EXIT_FAILURE);

    // for FBO 3.0 or 4.5
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);   
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    int windowWidth = 1200;
    int windowHeight = 800;

    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "HelloWorld", nullptr, nullptr);
    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, keyCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetCursorPosCallback(window, cursorCallback);
    glfwSetMouseButtonCallback(window, mouseCallback);
    glfwSetWindowSizeCallback(window, windowSizeCallback);

    glfwSetInputMode(window, GLFW_LOCK_KEY_MODS, GLFW_TRUE);
 
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(EXIT_FAILURE);
    }

    g_Engine.Initialize(window);

    while (!glfwWindowShouldClose(window))
    {
        g_Engine.Update();
        g_Engine.Render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}