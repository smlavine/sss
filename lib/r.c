#include "lib.h"

#include <GLES2/gl2.h>

static GLuint prog, aPos, aClr, uMul, uAdd;

void rInit(void) {
    const char *VERT =
    "#version 100\n"
    "attribute vec2 aPos;\n"
    "attribute vec3 aClr;\n"
    "varying vec3 vClr;\n"
    "uniform vec2 uMul, uAdd;\n"
    "void main(void) {\n"
    "    gl_Position = vec4(aPos * uMul + uAdd, 0, 1);\n"
    "    vClr = aClr / 255.0;\n"
    "}\n";

    const char *FRAG =
    "#version 100\n"
    "precision mediump float;\n"
    "varying vec3 vClr;\n"
    "void main(void) {\n"
    "    gl_FragColor = vec4(vClr.rgb, 1);\n"
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
    uMul = glGetUniformLocation(prog, "uMul");
    uAdd = glGetUniformLocation(prog, "uAdd");

    glUniform2f(uMul, 1, 1);
    glUniform2f(uAdd, 0, 0);
}

void rExit(void) {
    glUseProgram(0);
    glDeleteProgram(prog);
}

void rPipe(float mulX, float mulY, float addX, float addY) {
    glUniform2f(uMul, mulX, mulY);
    glUniform2f(uAdd, addX, addY);
}

void rClear(const float *c) {
    glClearColor(c?c[0]:0, c?c[1]:0, c?c[2]:0, c?c[3]:0);
    glClear(GL_COLOR_BUFFER_BIT);
}

void rDrawIndexedTriangles(size_t ni,const uint32_t *i,const RVertex *v){
    glEnableVertexAttribArray(aPos);
    glEnableVertexAttribArray(aClr);

    glVertexAttribPointer(aPos, 2, GL_FLOAT, GL_FALSE, sizeof(*v), &v->x);
    glVertexAttribPointer(aClr, 3, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(*v), &v->r);

    glDrawElements(GL_TRIANGLES, ni, GL_UNSIGNED_INT, i);

    glDisableVertexAttribArray(aClr);
    glDisableVertexAttribArray(aPos);
}

void rViewport(int x, int y, int w, int h) {
    glViewport(x, y, w, h);
}
