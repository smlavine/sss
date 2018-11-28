/*
 * Welcome to the source of Sassy Square Sally! I hope you have a nice stay.
 * If you are new I would advice you to become acquainted with lib/ first.
 * The code there is much cosier - many functions are completely independent.
 * Everything under src/ revolves around a huge singleton defined in src/s.h.
 * The code won't check for errors - success and correctness are assumed.
 * src/main.c contains main() which initializes, runs and terminates the game.
 */

#include "s.h"

#include <stdio.h> // only snprintf()

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

// Window and OpenGL context parameters given to GLFW
#define WIN_W 0 // Window width (0 if fullscreen)
#define WIN_H 0 // Window height (0 if fullscreen)
#define WIN_T "Sassy Square Sally" // Window title
#define OGL_API GLFW_OPENGL_ES_API // OpenGL API
#define OGL_V 20 // OpenGL API version (Mm - Major, minor)
#define VSYNC true // Vertical synchronization - on/off
#define AA 4 // Sample count if multisample anti-aliased, 0 otherwise

// Level parameters, used to (re)load levels
#define LVL_FIRST 1 // The number of the first level to be loaded
#define LVL_LAST 29 // The number of the last level to be loaded
#define LVL_PATH_BUFFER_SIZE 11 // The size of the level path buffer
#define LVL_PATH_FMTS "rsc/%d.ppm" // Format string passed to snprintf()

// Paths to audio files
#define OGG_MUSIC    "rsc/music.ogg"    // The music that loops all the time
#define OGG_COIN     "rsc/coin.ogg"     // The sound of picking up a coin
#define OGG_GRAVITON "rsc/graviton.ogg" // The sound of picking up a graviton
#define OGG_KEY      "rsc/key.ogg"      // The sound of picking up a key
#define OGG_JUMP     "rsc/jump.ogg"     // The sound of jumping
#define OGG_EJECT    "rsc/eject.ogg"    // The sound of being ejected
#define OGG_DIE      "rsc/die.ogg"      // The sound of failure
#define OGG_WIN      "rsc/win.ogg"      // The sound of victory

/*
 * mkW - make window.
 * Prerequisites: GLFW must be successfully initialized.
 * w - the width of the window, in pixels, 0 if fullscreen.
 * h - the height of the window, in pixels, 0 if fullscreen.
 * t - the title of the window.
 * api - the API of the OpenGL context. See GLFW docs.
 * v - the version of the OpenGL API, in Mm - Major, minor format.
 * vs - vertical synchronization: true if on, false if off.
 * aa - sample count if multisample anti-aliased, 0 otherwise.
 * Returns a window handle.
 */
static GLFWwindow*mkW(int w,int h,const char*t,int api,int v,bool vs,int aa);

/*
 * The main function.
 * Initializes, runs and terminates the game.
 * Returns EXIT_SUCCESS.
 */
int main(void) {
    // Create a window and initialize the graphics/event system
    glfwInit();
    GLFWwindow *win = mkW(WIN_W, WIN_H, WIN_T, OGL_API, OGL_V, VSYNC, AA);
    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glfwSetInputMode(win, GLFW_STICKY_KEYS, GLFW_TRUE);
    rInit();

    // Initialize the audio system
    Saudio audio;
    audioInit();
    audio.music = audioMusicLoad(OGG_MUSIC);
    audio.coin = audioSoundLoad(OGG_COIN);
    audio.graviton = audioSoundLoad(OGG_GRAVITON);
    audio.key = audioSoundLoad(OGG_KEY);
    audio.jump = audioSoundLoad(OGG_JUMP);
    audio.eject = audioSoundLoad(OGG_EJECT);
    audio.die = audioSoundLoad(OGG_DIE);
    audio.win = audioSoundLoad(OGG_WIN);
    audioMusicPlay(audio.music, true);

    // Load-run-free the levels until the last one is completed or user exits
    for (int i = LVL_FIRST;!glfwWindowShouldClose(win) && i <= LVL_LAST;++i) {
        // Load
        char lvlPath[LVL_PATH_BUFFER_SIZE];
        snprintf(lvlPath, LVL_PATH_BUFFER_SIZE, LVL_PATH_FMTS, i);
        sLoad(lvlPath);
        glfwSetTime(0);

        // Run
        while (!glfwWindowShouldClose(win)) {
            glfwPollEvents();

            // Perform logic - call sTick()
            float t = glfwGetTime();
            bool kUpLeftRightRShftTab[6];
            kUpLeftRightRShftTab[0] = glfwGetKey(win, GLFW_KEY_UP);
            kUpLeftRightRShftTab[1] = glfwGetKey(win, GLFW_KEY_LEFT);
            kUpLeftRightRShftTab[2] = glfwGetKey(win, GLFW_KEY_RIGHT);
            kUpLeftRightRShftTab[3] = glfwGetKey(win, GLFW_KEY_R);
            kUpLeftRightRShftTab[4] = glfwGetKey(win, GLFW_KEY_LEFT_SHIFT);
            kUpLeftRightRShftTab[5] = glfwGetKey(win, GLFW_KEY_TAB);
            int r = sTick(audio, t, kUpLeftRightRShftTab);

            // Render the current state - call sDraw().
            int winW, winH;
            glfwGetFramebufferSize(win, &winW, &winH);
            sDraw(winW, winH);
            /*
             * This call will block and cap the framerate to screen refresh
             * rate. Thus the physics loop won't be able to go faster than
             * the rendering loop when they should be completely independent
             * from each other and run concurrently. This could be solved by
             * moving the rendering loop into its own thread, and locking the
             * global singleton with a mutex.
             * A problem? Maybe. So far I deem physics to be accurate enough.
             */
            glfwSwapBuffers(win);

            // If the result is negative, restart the level
            // If the result is positive, move on to the next level
            // If the result is zero, continue running this level
            if (r < 0) {
                --i;
                break;
            } else if (r > 0) {
                break;
            }
        }

        // Free
        sFree();
    }

    // Terminate the audio system
    audioMusicStop(audio.music);
    audioMusicFree(audio.music);
    audioSoundFree(audio.coin);
    audioSoundFree(audio.graviton);
    audioSoundFree(audio.key);
    audioSoundFree(audio.jump);
    audioSoundFree(audio.eject);
    audioSoundFree(audio.die);
    audioSoundFree(audio.win);
    audioExit();

    // Terminate the graphics/event system and destroy the window
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

    if (w == 0 || h == 0) {
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

// v1.1:
// 1. 30 levels.
// 2. Alternative keys.
// 3. Clean-up.
// 4. README.
// 5. Packages.

// IDEA: TUI
// IDEA: hero kinds.
// IDEA: zones.
// IDEA: lights.
// IDEA: allow jumping after falling off the edge for a few frames
// IDEA: make kDown do something - either squash hero or make it dive
// IDEA: death particles
// IDEA: jumping animation
// IDEA: tail/trail gfx?
