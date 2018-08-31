#include <stdbool.h>
#include <stdio.h>
#include "math.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define CFG_PATH "rsc/cfg"
#define WIN_TITLE "Sassy Square Sally"
#define OGL_API GLFW_OPENGL_ES_API
#define OGL_PROF 0
#define OGL_VMAJ 2
#define OGL_VMIN 0

#include "matrix.h"
#include "camera.h"
#include "image.h"
#include "renderer.h"

static GLFWwindow *mkWin(int w, int h, const char *t, bool f, int api, int prof, int V, int v, bool vsync, int aa);

int main(void)
{
    int windowed, winW, winH, vsync, aa;
    FILE *f = fopen(CFG_PATH, "r");
    fscanf(f, "%d%d%d", &windowed, &winW, &winH);
    fscanf(f, "%d%d", &vsync, &aa);
    fclose(f);

    glfwInit();
    GLFWwindow *win = mkWin(winW, winH, WIN_TITLE, !windowed, OGL_API, OGL_PROF, OGL_VMAJ, OGL_VMIN, vsync, aa);

    rendererInit();

    while (!glfwWindowShouldClose(win)) {
        glfwWaitEvents();

        int w, h;
        float matrix[4][4];
        glfwGetFramebufferSize(win, &w, &h);
        rendererViewport(0, 0, w, h);
        Camera2D camera = camera2D((float)w / (float)h);
        camera2Dmatrix(&camera, matrix);
        rendererPipelineWorld(matrix, NULL, NULL, NULL);

        const RendererVertex triangle[] = {
            {-1, -1, 0, 0, 0, 255,   0,   0, 255},
            { 0,  1, 0, 0, 0,   0, 255,   0, 255},
            { 1, -1, 0, 0, 0,   0,   0, 255, 255}
        };
        rendererDraw(RENDERER_DRAW_MODE_TRIANGLES, sizeof(triangle)/sizeof(triangle[0]), triangle);

        glfwSwapBuffers(win);
    }

    rendererExit();
    glfwTerminate();
}

static GLFWwindow *mkWin(int w, int h, const char *t, bool f, int api, int prof, int V, int v, bool vsync, int aa)
{
    GLFWwindow *win;
    int width, height;
    GLFWmonitor *monitor;

    glfwWindowHint(GLFW_CLIENT_API, api);
    if (api == GLFW_OPENGL_API) {
        glfwWindowHint(GLFW_OPENGL_PROFILE, prof);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, V);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, v);
    glfwWindowHint(GLFW_SAMPLES, aa);

    if (f) {
        monitor = glfwGetPrimaryMonitor();
        if (!monitor) {
            return NULL;
        }
        const GLFWvidmode *vidmode = glfwGetVideoMode(monitor);
        if (!vidmode) {
            return NULL;
        }
        width = vidmode->width;
        height = vidmode->height;
        glfwWindowHint(GLFW_RED_BITS, vidmode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, vidmode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, vidmode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, vidmode->refreshRate);
    } else {
        width = w;
        height = h;
        monitor = NULL;
    }

    win = glfwCreateWindow(width, height, t, monitor, NULL);
    glfwMakeContextCurrent(win);
    glfwSwapInterval(vsync ? 1 : 0);

    return win;
}

