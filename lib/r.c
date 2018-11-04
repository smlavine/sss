#include "dragon.h"

#include <string.h>
#include <stdlib.h>

#include <GLES2/gl2.h>

static struct {
    GLuint prog;
    GLint aPos, aClr;
    GLint uMat;
} r;

void rInit(void) {
    const char *VERT =
    "#version 100\n"
    "attribute vec2 aPos;\n"
    "attribute vec4 aClr;\n"
    "varying vec4 vClr;\n"
    "uniform mat4 uMat;\n"
    "void main(void) {\n"
    "    gl_Position = uMat * vec4(aPos.xy,0,1);\n"
    "    vClr = aClr / 255.0;\n"
    "}\n";

    const char *FRAG =
    "#version 100\n"
    "precision mediump float;\n"
    "varying vec4 vClr;\n"
    "void main(void) {\n"
    "    gl_FragColor = vClr;\n"
    "}\n";

    r.prog = glCreateProgram();
    GLuint vert = glCreateShader(GL_VERTEX_SHADER);
    GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(vert, 1, &VERT, NULL);
    glShaderSource(frag, 1, &FRAG, NULL);
    glCompileShader(vert);
    glCompileShader(frag);
    glAttachShader(r.prog, vert);
    glAttachShader(r.prog, frag);
    glLinkProgram(r.prog);
    glDetachShader(r.prog, vert);
    glDetachShader(r.prog, frag);
    glDeleteShader(frag);
    glDeleteShader(vert);

    glUseProgram(r.prog);

    r.aPos = glGetAttribLocation(r.prog, "aPos");
    r.aClr = glGetAttribLocation(r.prog, "aClr");
    r.uMat = glGetUniformLocation(r.prog, "uMat");

    float m[4][4] = {{1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1}};
    rMatrix(m);
}

void rExit(void) {
    glUseProgram(0);
    glDeleteProgram(r.prog);
}

void rMatrix(const float m[4][4]) {
    if (m) {
        glUniformMatrix4fv(r.uMat, 1, GL_FALSE, (const void*)m);
    }
}


void rClear(const float *c) {
    if (c) {
        glClearColor(c[0], c[1], c[2], c[3]);
    } else {
        glClearColor(0,0,0,0);
    }
    glClear(GL_COLOR_BUFFER_BIT);
}

void rDrawIndexed(RDrawMode mode,size_t ni,const uint32_t *i,const RVertex *v){
    glEnableVertexAttribArray((GLuint)r.aPos);
    glEnableVertexAttribArray((GLuint)r.aClr);

    glVertexAttribPointer((GLuint)r.aPos, 2, GL_FLOAT, GL_FALSE, sizeof(*v), &v->x);
    glVertexAttribPointer((GLuint)r.aClr, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(*v), &v->r);

    switch (mode) {
    case R_DRAW_MODE_POINTS:
        glDrawElements(GL_POINTS, (GLsizei)ni, GL_UNSIGNED_INT, i);
        break;
    case R_DRAW_MODE_LINES:
        glDrawElements(GL_LINES, (GLsizei)ni, GL_UNSIGNED_INT, i);
        break;
    case R_DRAW_MODE_LINE_LOOP:
        glDrawElements(GL_LINE_LOOP, (GLsizei)ni, GL_UNSIGNED_INT, i);
        break;
    case R_DRAW_MODE_TRIANGLES:
        glDrawElements(GL_TRIANGLES, (GLsizei)ni, GL_UNSIGNED_INT, i);
        break;
    case R_DRAW_MODE_TRIANGLE_STRIP:
        glDrawElements(GL_TRIANGLE_STRIP, (GLsizei)ni, GL_UNSIGNED_INT, i);
        break;
    case R_DRAW_MODE_TRIANGLE_FAN:
        glDrawElements(GL_TRIANGLE_FAN, (GLsizei)ni, GL_UNSIGNED_INT, i);
        break;
    default:
        break;
    }

    glDisableVertexAttribArray((GLuint)r.aClr);
    glDisableVertexAttribArray((GLuint)r.aPos);
}

void rViewport(int x, int y, int w, int h) {
    glViewport(x, y, w, h);
}

