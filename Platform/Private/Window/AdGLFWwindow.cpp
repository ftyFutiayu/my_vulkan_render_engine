#include "Window/AdGLFWwindow.h"
#include "AdLog.h"
#include "GLFW/glfw3native.h"

namespace ade {
    AdGLFWwindow::AdGLFWwindow(uint32_t width, uint32_t height, const char *title) {
        if (!glfwInit()) {
            LOG_E("Failed to init glfw.");
            return;
        }
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

        mGLFWwindow = glfwCreateWindow(width, height, title, nullptr, nullptr);
        if (!mGLFWwindow) {
            LOG_E("Failed to create glfw window.");
            return;
        }

        GLFWmonitor *primaryMonitor = glfwGetPrimaryMonitor();
        if (primaryMonitor) {
            int xPos, yPos, workWidth, workHeight;
            glfwGetMonitorWorkarea(primaryMonitor, &xPos, &yPos, &workWidth, &workHeight);
            glfwSetWindowPos(mGLFWwindow, workWidth / 2 - width / 2, workHeight / 2 - height / 2);
        }

        // For OpenGL/OpenGLES https://www.glfw.org/docs/latest/group__context.html#ga6d4e0cdf151b5e579bd67f13202994ed
        // This function sets the swap interval for the current OpenGL or OpenGL ES context,
        // i.e. the number of screen updates to wait from the time glfwSwapBuffers was called before swapping the buffers and returning.
        //glfwSwapInterval(0);

        glfwMakeContextCurrent(mGLFWwindow);

        // show window
        glfwShowWindow(mGLFWwindow);
    }

    AdGLFWwindow::~AdGLFWwindow() {
        glfwDestroyWindow(mGLFWwindow);
        glfwTerminate();
        LOG_I("The application running end.");
    }

    bool AdGLFWwindow::ShouldClose() {
        return glfwWindowShouldClose(mGLFWwindow);
    }

    void AdGLFWwindow::PollEvents() {
        glfwPollEvents();
    }

    void AdGLFWwindow::SwapBuffer() {
        glfwSwapBuffers(mGLFWwindow);
    }
}