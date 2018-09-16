#include "dragon.h"

#include <string.h>
#include <stdlib.h>

#include <GLES2/gl2.h>

static struct {
    GLuint prog;
    struct {
        GLint aPos, aClr;
        GLint uMatModel, uMatSpace, uMatWorld;
        GLint uClrModel, uClrSpace, uClrWorld;
    } loc;
    struct {
        bool depthTest, transparency;
    } capability;
    struct {
        struct {
            float matrix[4][4], color[4];
        } model, space, world;
    } pipeline;
} r;

void rInit(void) {
    const char *VERT =
    "#version 100\n"
    "attribute vec3 aPos;\n"
    "attribute vec4 aClr;\n"
    "varying vec4 vClr;\n"
    "uniform mat4 uMatModel, uMatSpace, uMatWorld;\n"
    "void main(void) {\n"
    "    gl_Position = uMatWorld * uMatSpace * uMatModel * vec4(aPos.xyz,1);\n"
    "    vClr = aClr / 255.0;\n"
    "}\n";

    const char *FRAG =
    "#version 100\n"
    "precision mediump float;\n"
    "varying vec4 vClr;\n"
    "uniform vec4 uClrModel, uClrSpace, uClrWorld;\n"
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

    r.loc.aPos = glGetAttribLocation(r.prog, "aPos");
    r.loc.aClr = glGetAttribLocation(r.prog, "aClr");
    r.loc.uMatModel = glGetUniformLocation(r.prog, "uMatModel");
    r.loc.uMatSpace = glGetUniformLocation(r.prog, "uMatSpace");
    r.loc.uMatWorld = glGetUniformLocation(r.prog, "uMatWorld");
    r.loc.uClrModel = glGetUniformLocation(r.prog, "uClrModel");
    r.loc.uClrSpace = glGetUniformLocation(r.prog, "uClrSpace");
    r.loc.uClrWorld = glGetUniformLocation(r.prog, "uClrWorld");

    float m[4][4] = {{1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1}};
    float c[4] = {1,1,1,1};
    rPipeModel(m, c, NULL, NULL);
    rPipeSpace(m, c, NULL, NULL);
    rPipeWorld(m, c, NULL, NULL);

    rCapability(R_CAPABILITY_DEPTH_TEST, false);
    rCapability(R_CAPABILITY_TRANSPARENCY, false);
}

void rExit(void) {
    glUseProgram(0);
    glDeleteProgram(r.prog);
}

bool rCapability(RCapability capability, bool enabled) {
    bool state = false;
    switch (capability) {
        case R_CAPABILITY_DEPTH_TEST:
            if (enabled) {
                glEnable(GL_DEPTH_TEST);
            } else {
                glDisable(GL_DEPTH_TEST);
            }
            state = r.capability.depthTest;
            r.capability.depthTest = enabled;
            break;
        case R_CAPABILITY_TRANSPARENCY:
            if (enabled) {
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            } else {
                glDisable(GL_BLEND);
            }
            state = r.capability.transparency;
            r.capability.transparency = enabled;
            break;
        default:
            break;
    }
    return state;
}

void rPipeModel(const float m[4][4],const float c[4],float M[4][4],float C[4]){
    float pm[4][4], pc[4];

    if (m) {
        glUniformMatrix4fv(r.loc.uMatModel, 1, GL_FALSE, (const void*)m);
        memcpy(pm, r.pipeline.model.matrix, sizeof(pm));
        memcpy(r.pipeline.model.matrix, m, sizeof(pm));
    }

    if (c) {
        glUniform4fv(r.loc.uClrModel, 1, c);
        memcpy(pc, r.pipeline.model.color, sizeof(pc));
        memcpy(r.pipeline.model.color, c, sizeof(pc));
    }

    if (M) memcpy(M, pm, sizeof(pm));
    if (C) memcpy(C, pc, sizeof(pc));
}

void rPipeSpace(const float m[4][4],const float c[4],float M[4][4],float C[4]){
    float pm[4][4], pc[4];

    if (m) {
        glUniformMatrix4fv(r.loc.uMatSpace, 1, GL_FALSE, (const void*)m);
        memcpy(pm, r.pipeline.space.matrix, sizeof(pm));
        memcpy(r.pipeline.space.matrix, m, sizeof(pm));
    }

    if (c) {
        glUniform4fv(r.loc.uClrSpace, 1, c);
        memcpy(pc, r.pipeline.space.color, sizeof(pc));
        memcpy(r.pipeline.space.color, c, sizeof(pc));
    }

    if (M) memcpy(M, pm, sizeof(pm));
    if (C) memcpy(C, pc, sizeof(pc));
}

void rPipeWorld(const float m[4][4],const float c[4],float M[4][4],float C[4]){
    float pm[4][4], pc[4];

    if (m) {
        glUniformMatrix4fv(r.loc.uMatWorld, 1, GL_FALSE, (const void*)m);
        memcpy(pm, r.pipeline.world.matrix, sizeof(pm));
        memcpy(r.pipeline.world.matrix, m, sizeof(pm));
    }

    if (c) {
        glUniform4fv(r.loc.uClrWorld, 1, c);
        memcpy(pc, r.pipeline.world.color, sizeof(pc));
        memcpy(r.pipeline.world.color, c, sizeof(pc));
    }

    if (M) memcpy(M, pm, sizeof(pm));
    if (C) memcpy(C, pc, sizeof(pc));
}


void rClear(const float c[4]) {
    if (c) {
        glClearColor(c[0], c[1], c[2], c[3]);
    } else {
        glClearColor(0,0,0,0);
    }

    glClear(GL_COLOR_BUFFER_BIT);

    if (r.capability.depthTest) {
        glClearColor(0, 0, 0, 0);
        glClear(GL_DEPTH_BUFFER_BIT);
    }
}

void rDraw(RDrawMode mode, size_t n, const RVertex *v) {
    glEnableVertexAttribArray((GLuint)r.loc.aPos);
    glEnableVertexAttribArray((GLuint)r.loc.aClr);

    glVertexAttribPointer((GLuint)r.loc.aPos, 3, GL_FLOAT, GL_FALSE, sizeof(*v), &v->x);
    glVertexAttribPointer((GLuint)r.loc.aClr, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(*v), &v->r);

    switch (mode) {
    case R_DRAW_MODE_POINTS:
        glDrawArrays(GL_POINTS, 0, (GLsizei)n);
        break;
    case R_DRAW_MODE_LINES:
        glDrawArrays(GL_LINES, 0, (GLsizei)n);
        break;
    case R_DRAW_MODE_LINE_LOOP:
        glDrawArrays(GL_LINE_LOOP, 0, (GLsizei)n);
        break;
    case R_DRAW_MODE_TRIANGLES:
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)n);
        break;
    case R_DRAW_MODE_TRIANGLE_STRIP:
        glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)n);
        break;
    case R_DRAW_MODE_TRIANGLE_FAN:
        glDrawArrays(GL_TRIANGLE_FAN, 0, (GLsizei)n);
        break;
    default:
        break;
    }

    glDisableVertexAttribArray((GLuint)r.loc.aClr);
    glDisableVertexAttribArray((GLuint)r.loc.aPos);
}

void rDrawIndexed(RDrawMode mode,size_t ni,const uint16_t *i,const RVertex *v){
    glEnableVertexAttribArray((GLuint)r.loc.aPos);
    glEnableVertexAttribArray((GLuint)r.loc.aClr);

    glVertexAttribPointer((GLuint)r.loc.aPos, 3, GL_FLOAT, GL_FALSE, sizeof(*v), &v->x);
    glVertexAttribPointer((GLuint)r.loc.aClr, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(*v), &v->r);

    switch (mode) {
    case R_DRAW_MODE_POINTS:
        glDrawElements(GL_POINTS, (GLsizei)ni, GL_UNSIGNED_SHORT, i);
        break;
    case R_DRAW_MODE_LINES:
        glDrawElements(GL_LINES, (GLsizei)ni, GL_UNSIGNED_SHORT, i);
        break;
    case R_DRAW_MODE_LINE_LOOP:
        glDrawElements(GL_LINE_LOOP, (GLsizei)ni, GL_UNSIGNED_SHORT, i);
        break;
    case R_DRAW_MODE_TRIANGLES:
        glDrawElements(GL_TRIANGLES, (GLsizei)ni, GL_UNSIGNED_SHORT, i);
        break;
    case R_DRAW_MODE_TRIANGLE_STRIP:
        glDrawElements(GL_TRIANGLE_STRIP, (GLsizei)ni, GL_UNSIGNED_SHORT, i);
        break;
    case R_DRAW_MODE_TRIANGLE_FAN:
        glDrawElements(GL_TRIANGLE_FAN, (GLsizei)ni, GL_UNSIGNED_SHORT, i);
        break;
    default:
        break;
    }

    glDisableVertexAttribArray((GLuint)r.loc.aClr);
    glDisableVertexAttribArray((GLuint)r.loc.aPos);
}

void rViewport(int x, int y, int w, int h) {
    glViewport(x, y, w, h);
}
