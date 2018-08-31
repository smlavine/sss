#include <stdbool.h>
#include <stdio.h>
#include "math.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define CFG_PATH "rsc/cfg"
#define LVL_PATH "lvl/1"
#define WIN_TITLE "Sassy Square Sally"
#define OGL_API GLFW_OPENGL_ES_API
#define OGL_PROF 0
#define OGL_VMAJ 2
#define OGL_VMIN 0

#include "matrix.h"
#include "camera.h"
#include "image.h"
#include "renderer.h"
#include "batch.h"
#include "state.h"

static GLFWwindow *mkWin(int w, int h, const char *t, bool f, int api, int prof, int V, int v, bool vsync, int aa);
static StateInput mkStateInput(GLFWwindow *win);

int main(void) {
    int windowed, winW, winH, vsync, aa;
    FILE *f = fopen(CFG_PATH, "r");
    fscanf(f, "%d%d%d", &windowed, &winW, &winH);
    fscanf(f, "%d%d", &vsync, &aa);
    fclose(f);

    glfwInit();
    GLFWwindow *win = mkWin(winW, winH, WIN_TITLE, !windowed, OGL_API, OGL_PROF, OGL_VMAJ, OGL_VMIN, vsync, aa);
    rendererInit();
    StateInput stateInput = mkStateInput(win);
    State *state = stateNew(LVL_PATH, &stateInput);

    while (!glfwWindowShouldClose(win)) {
        glfwWaitEvents();
        stateInput = mkStateInput(win);
        stateUpdate(state, &stateInput);
        stateDraw(state);
        glfwSwapBuffers(win);
    }

    stateDel(state);
    rendererExit();
    glfwTerminate();
}

static GLFWwindow *mkWin(int w, int h, const char *t, bool f, int api, int prof, int V, int v, bool vsync, int aa) {
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

static StateInput mkStateInput(GLFWwindow *win) {
    StateInput in;
    glfwGetFramebufferSize(win, &in.winW, &in.winH);
    return in;
}

