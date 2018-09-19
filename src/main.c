#include "../lib/dragon.h"
#include "s.h"

#include <stdio.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define WIN_W 0
#define WIN_H 0
#define WIN_T "Sassy Square Sally"
#define OGL_API GLFW_OPENGL_ES_API
#define OGL_V 20
#define VSYNC true
#define AA 4
#define LVL_FIRST 10
#define LVL_LAST 17
#define LVL_PATH_BUFFER_SIZE 11
#define LVL_PATH_FMTS "rsc/%d.ppm"
#define MUSIC_PATH "rsc/music.ogg"

static GLFWwindow*mkW(int w,int h,const char*t,int api,int v,bool vs,int aa);

int main(void) {
    glfwInit();
    GLFWwindow *win = mkW(WIN_W, WIN_H, WIN_T, OGL_API, OGL_V, VSYNC, AA);
    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    rInit();
    audioInit();
    AudioMusic *music = audioMusicLoad(MUSIC_PATH);
    audioMusicPlay(music, true);

    for (int i = LVL_FIRST;!glfwWindowShouldClose(win) && i <= LVL_LAST;++i) {
        char lvlPath[LVL_PATH_BUFFER_SIZE];
        snprintf(lvlPath, LVL_PATH_BUFFER_SIZE, LVL_PATH_FMTS, i);
        sLoad(lvlPath);
        glfwSetTime(0);

        while (!glfwWindowShouldClose(win)) {
            glfwPollEvents();

            float t = glfwGetTime();
            bool kUp = glfwGetKey(win, GLFW_KEY_UP);
            bool kLeft = glfwGetKey(win, GLFW_KEY_LEFT);
            bool kRight = glfwGetKey(win, GLFW_KEY_RIGHT);
            bool kR = glfwGetKey(win, GLFW_KEY_R);
            int r = sTick(music, t, kUp, kLeft, kRight, kR);

            int winW, winH;
            glfwGetFramebufferSize(win, &winW, &winH);
            sDraw(winW, winH);
            glfwSwapBuffers(win);

            if (r < 0) {
                --i;
            }
            if (r != 0) {
                break;
            }
        }

        sFree();
    }

    audioMusicStop(music);
    audioMusicFree(music);
    audioExit();
    rExit();
    glfwTerminate();
}

static GLFWwindow*mkW(int w,int h,const char*t,int api,int v,bool vs,int aa) {
    GLFWwindow *win;
    int width, height;
    GLFWmonitor *monitor;

    glfwWindowHint(GLFW_CLIENT_API, api);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, v / 10);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, v % 10);
    glfwWindowHint(GLFW_SAMPLES, aa);

    if (w == 0 && h == 0) {
        monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *vidmode = glfwGetVideoMode(monitor);
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
    glfwSwapInterval(vs ? 1 : 0);

    return win;
}
