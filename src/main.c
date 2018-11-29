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

#define LEN(x) sizeof(x)/sizeof(x[0])

// Window and OpenGL context parameters given to GLFW
#define WIN_W 0 // Window width (0 if fullscreen)
#define WIN_H 0 // Window height (0 if fullscreen)
#define WIN_T "Sassy Square Sally" // Window title
#define OGL_API GLFW_OPENGL_ES_API // OpenGL API
#define OGL_V 20 // OpenGL API version (Mm - Major, minor)
#define VSYNC true // Vertical synchronization - on/off
#define AA 4 // Sample count if multisample anti-aliased, 0 otherwise

// Level parameters, used to (re)load levels
#define LVL_FIRST 1
#define LVL_LAST 32
#define LVL_PATH_BUFFER_SIZE 11
#define LVL_PATH_FMTS "rsc/%d.ppm"

// Paths to audio files
#define OGG_MUSIC    "rsc/music.ogg"
#define OGG_COIN     "rsc/coin.ogg"
#define OGG_GRAVITON "rsc/graviton.ogg"
#define OGG_KEY      "rsc/key.ogg"
#define OGG_JUMP     "rsc/jump.ogg"
#define OGG_EJECT    "rsc/eject.ogg"
#define OGG_SWITCHH  "rsc/switchh.ogg"
#define OGG_DIE      "rsc/die.ogg"
#define OGG_WIN      "rsc/win.ogg"

// Key bindings
#define K_UP (const int[]) \
{GLFW_KEY_UP,GLFW_KEY_W,GLFW_KEY_H,GLFW_KEY_KP_2,GLFW_KEY_KP_5,GLFW_KEY_SPACE}
#define K_LEFT (const int[]) \
{GLFW_KEY_LEFT,GLFW_KEY_A,GLFW_KEY_J,GLFW_KEY_KP_4}
#define K_RIGHT (const int[]) \
{GLFW_KEY_RIGHT,GLFW_KEY_D,GLFW_KEY_K,GLFW_KEY_KP_6}
#define K_R (const int[]) {GLFW_KEY_R}
#define K_SHFT (const int[]) {GLFW_KEY_LEFT_SHIFT, GLFW_KEY_RIGHT_SHIFT}
#define K_TAB (const int[]) {GLFW_KEY_TAB}
#define K_Q (const int[]) {GLFW_KEY_Q, GLFW_KEY_ESCAPE}

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
 * getKey - gets key state.
 * Runs glfwGetKey(win, k[i]) for each key and returns true if any is pressed.
 * win - the window to query key state in.
 * n - the number of keys in the array k.
 * k - the array of keys.
 */
static bool getKey(GLFWwindow *win, size_t n, const int *k);

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
    audio.switchh = audioSoundLoad(OGG_SWITCHH);
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
            kUpLeftRightRShftTab[0] = getKey(win, LEN(K_UP), K_UP);
            kUpLeftRightRShftTab[1] = getKey(win, LEN(K_LEFT), K_LEFT);
            kUpLeftRightRShftTab[2] = getKey(win, LEN(K_RIGHT), K_RIGHT);
            kUpLeftRightRShftTab[3] = getKey(win, LEN(K_R), K_R);
            kUpLeftRightRShftTab[4] = getKey(win, LEN(K_SHFT), K_SHFT);
            kUpLeftRightRShftTab[5] = getKey(win, LEN(K_TAB), K_TAB);
            if (getKey(win, LEN(K_Q), K_Q)) {
                glfwSetWindowShouldClose(win, true);
            }
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
    audioSoundFree(audio.switchh);
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

static bool getKey(GLFWwindow *win, size_t n, const int *k) {
    for (size_t i = 0; i < n; ++i) {
        if (glfwGetKey(win, k[i])) {
            return true;
        }
    }
    return false;
}

// v1.1:
// 1. 35 levels.
// 2. Clean-up.
// 3. README.
// 4. Packages.

// IDEA: TUI
// IDEA: hero kinds.
// IDEA: zones.
// IDEA: lights.
// IDEA: allow jumping after falling off the edge for a few frames
// IDEA: make kDown do something - either squash hero or make it dive
// IDEA: death particles
// IDEA: jumping animation
// IDEA: tail/trail gfx?
// IDEA: make key, lock, antilock colors more distinguishable
// IDEA: draw switch poly (if s.hero.n >= 3)
