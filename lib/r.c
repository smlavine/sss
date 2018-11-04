#include "dragon.h"

#include <GLES2/gl2.h>

static GLuint prog, aPos, aClr, uMat;

void rInit(void) {
    const char *VERT =
    "#version 100\n"
    "attribute vec2 aPos;\n"
    "attribute vec4 aClr;\n"
    "varying vec4 vClr;\n"
    "uniform mat4 uMat;\n"
    "void main(void) {\n"
    "    gl_Position = uMat * vec4(aPos.xy, 0, 1);\n"
    "    vClr = aClr / 255.0;\n"
    "}\n";

    const char *FRAG =
    "#version 100\n"
    "precision mediump float;\n"
    "varying vec4 vClr;\n"
    "void main(void) {\n"
    "    gl_FragColor = vClr;\n"
    "}\n";

    prog = glCreateProgram();
    GLuint vert = glCreateShader(GL_VERTEX_SHADER);
    GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(vert, 1, &VERT, NULL);
    glShaderSource(frag, 1, &FRAG, NULL);
    glCompileShader(vert);
    glCompileShader(frag);
    glAttachShader(prog, vert);
    glAttachShader(prog, frag);
    glLinkProgram(prog);
    glDetachShader(prog, vert);
    glDetachShader(prog, frag);
    glDeleteShader(frag);
    glDeleteShader(vert);

    glUseProgram(prog);

    aPos = glGetAttribLocation(prog, "aPos");
    aClr = glGetAttribLocation(prog, "aClr");
    uMat = glGetUniformLocation(prog, "uMat");

    float m[] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
    glUniformMatrix4fv(uMat, 1, GL_FALSE, m);
}

void rExit(void) {
    glUseProgram(0);
    glDeleteProgram(prog);
}

void rPipe(float mulX, float mulY, float addX, float addY, float divX) {
    float m[16] = {
        mulX / divX,    0, 0, addX,
                  0, mulY, 0, addY,
                  0,    0, 1, 0,
                  0,    0, 0, 1
    };
    glUniformMatrix4fv(uMat, 1, GL_TRUE, m);
}

void rClear(const float *c) {
    glClearColor(c?c[0]:0, c?c[1]:0, c?c[2]:0, c?c[3]:0);
    glClear(GL_COLOR_BUFFER_BIT);
}

void rDrawIndexed(RDrawMode mode,size_t ni,const uint32_t *i,const RVertex *v){
    glEnableVertexAttribArray(aPos);
    glEnableVertexAttribArray(aClr);

    glVertexAttribPointer(aPos, 2, GL_FLOAT, GL_FALSE, sizeof(*v), &v->x);
    glVertexAttribPointer(aClr, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(*v), &v->r);

    switch (mode) {
    case R_DRAW_MODE_POINTS:
        glDrawElements(GL_POINTS, ni, GL_UNSIGNED_INT, i);
        break;
    case R_DRAW_MODE_LINES:
        glDrawElements(GL_LINES, ni, GL_UNSIGNED_INT, i);
        break;
    case R_DRAW_MODE_LINE_LOOP:
        glDrawElements(GL_LINE_LOOP, ni, GL_UNSIGNED_INT, i);
        break;
    case R_DRAW_MODE_TRIANGLES:
        glDrawElements(GL_TRIANGLES, ni, GL_UNSIGNED_INT, i);
        break;
    case R_DRAW_MODE_TRIANGLE_STRIP:
        glDrawElements(GL_TRIANGLE_STRIP, ni, GL_UNSIGNED_INT, i);
        break;
    case R_DRAW_MODE_TRIANGLE_FAN:
        glDrawElements(GL_TRIANGLE_FAN, ni, GL_UNSIGNED_INT, i);
        break;
    default:
        break;
    }

    glDisableVertexAttribArray(aClr);
    glDisableVertexAttribArray(aPos);
}

void rViewport(int x, int y, int w, int h) {
    glViewport(x, y, w, h);
}
