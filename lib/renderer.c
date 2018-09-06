#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "image.h"
#include "renderer.h"

#include <GLES2/gl2.h>

struct RendererTexture {
    GLuint id;
};

static struct {
    GLuint prog;
    struct {
        GLint aPos, aTex, aClr;
        GLint uMatModel, uMatSpace, uMatWorld;
        GLint uClrModel, uClrSpace, uClrWorld;
        GLint uSampler;
    } loc;
    struct {
        bool depthTest, transparency;
    } capability;
    struct {
        struct {
            float matrix[4][4], color[4];
        } model, space, world;
    } pipeline;
    struct {
        RendererTexture *null;
        const RendererTexture *active;
    } texture;
} renderer;

void rendererInit(void) {
    const char *VERT =
    "#version 100\n"
    "attribute vec3 aPos;\n"
    "attribute vec2 aTex;\n"
    "attribute vec4 aClr;\n"
    "varying vec2 vTex;\n"
    "varying vec4 vClr;\n"
    "uniform mat4 uMatModel, uMatSpace, uMatWorld;\n"
    "void main(void) {\n"
    "    gl_Position = uMatWorld * uMatSpace * uMatModel * vec4(aPos.xyz, 1);\n"
    "    vTex = aTex;\n"
    "    vClr = aClr / 255.0;\n"
    "}\n";

    const char *FRAG =
    "#version 100\n"
    "precision mediump float;\n"
    "varying vec2 vTex;\n"
    "varying vec4 vClr;\n"
    "uniform vec4 uClrModel, uClrSpace, uClrWorld;\n"
    "uniform sampler2D uSampler;\n"
    "void main(void) {\n"
    "    gl_FragColor = vClr * texture2D(uSampler, vTex);\n"
    "}\n";

    renderer.prog = glCreateProgram();
    GLuint vert = glCreateShader(GL_VERTEX_SHADER);
    GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(vert, 1, &VERT, NULL);
    glShaderSource(frag, 1, &FRAG, NULL);
    glCompileShader(vert);
    glCompileShader(frag);
    glAttachShader(renderer.prog, vert);
    glAttachShader(renderer.prog, frag);
    glLinkProgram(renderer.prog);
    glDetachShader(renderer.prog, vert);
    glDetachShader(renderer.prog, frag);
    glDeleteShader(frag);
    glDeleteShader(vert);

    glUseProgram(renderer.prog);

    renderer.loc.aPos = glGetAttribLocation(renderer.prog, "aPos");
    renderer.loc.aTex = glGetAttribLocation(renderer.prog, "aTex");
    renderer.loc.aClr = glGetAttribLocation(renderer.prog, "aClr");
    renderer.loc.uMatModel = glGetUniformLocation(renderer.prog, "uMatModel");
    renderer.loc.uMatSpace = glGetUniformLocation(renderer.prog, "uMatSpace");
    renderer.loc.uMatWorld = glGetUniformLocation(renderer.prog, "uMatWorld");
    renderer.loc.uClrModel = glGetUniformLocation(renderer.prog, "uClrModel");
    renderer.loc.uClrSpace = glGetUniformLocation(renderer.prog, "uClrSpace");
    renderer.loc.uClrWorld = glGetUniformLocation(renderer.prog, "uClrWorld");
    renderer.loc.uSampler = glGetUniformLocation(renderer.prog, "uSampler");

    rendererPipelineModel((float[][4]){{1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1}}, (float[]){1,1,1,1}, NULL, NULL);
    rendererPipelineSpace((float[][4]){{1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1}}, (float[]){1,1,1,1}, NULL, NULL);
    rendererPipelineWorld((float[][4]){{1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1}}, (float[]){1,1,1,1}, NULL, NULL);
    glUniform1i(renderer.loc.uSampler, 0);

    rendererCapability(RENDERER_CAPABILITY_DEPTH_TEST, false);
    rendererCapability(RENDERER_CAPABILITY_TRANSPARENCY, false);

    renderer.texture.null = rendererTextureNew((Image[]){{NULL,1,1,(ImagePixel[]){{255,255,255,255}}}}, RENDERER_TEXTURE_QUALITY_WORST, RENDERER_TEXTURE_WRAPPING_CLAMP_TO_EDGE);
}

void rendererExit(void) {
    rendererTextureDel(renderer.texture.null);
    glUseProgram(0);
    glDeleteProgram(renderer.prog);
}

bool rendererCapability(RendererCapability capability, bool enabled) {
    bool state = false;
    switch (capability) {
        case RENDERER_CAPABILITY_DEPTH_TEST:
            if (enabled) {
                glEnable(GL_DEPTH_TEST);
            } else {
                glDisable(GL_DEPTH_TEST);
            }
            state = renderer.capability.depthTest;
            renderer.capability.depthTest = enabled;
            break;
        case RENDERER_CAPABILITY_TRANSPARENCY:
            if (enabled) {
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            } else {
                glDisable(GL_BLEND);
            }
            state = renderer.capability.transparency;
            renderer.capability.transparency = enabled;
            break;
        default:
            break;
    }
    return state;
}

void rendererPipelineModel(const float matrix[4][4], const float color[4], float prevMatrix[4][4], float prevColor[4]) {
    float pm[4][4], pc[4];

    if (matrix) {
        glUniformMatrix4fv(renderer.loc.uMatModel, 1, GL_FALSE, (const void*)matrix);
        memcpy(pm, renderer.pipeline.model.matrix, sizeof(pm));
        memcpy(renderer.pipeline.model.matrix, matrix, sizeof(pm));
    }

    if (color) {
        glUniform4fv(renderer.loc.uClrModel, 1, color);
        memcpy(pc, renderer.pipeline.model.color, sizeof(pc));
        memcpy(renderer.pipeline.model.color, color, sizeof(pc));
    }

    if (prevMatrix) memcpy(prevMatrix, pm, sizeof(pm));
    if (prevColor) memcpy(prevColor, pc, sizeof(pc));
}

void rendererPipelineSpace(const float matrix[4][4], const float color[4], float prevMatrix[4][4], float prevColor[4]) {
    float pm[4][4], pc[4];

    if (matrix) {
        glUniformMatrix4fv(renderer.loc.uMatSpace, 1, GL_FALSE, (const void*)matrix);
        memcpy(pm, renderer.pipeline.space.matrix, sizeof(pm));
        memcpy(renderer.pipeline.space.matrix, matrix, sizeof(pm));
    }

    if (color) {
        glUniform4fv(renderer.loc.uClrSpace, 1, color);
        memcpy(pc, renderer.pipeline.space.color, sizeof(pc));
        memcpy(renderer.pipeline.space.color, color, sizeof(pc));
    }

    if (prevMatrix) memcpy(prevMatrix, pm, sizeof(pm));
    if (prevColor) memcpy(prevColor, pc, sizeof(pc));
}

void rendererPipelineWorld(const float matrix[4][4], const float color[4], float prevMatrix[4][4], float prevColor[4]) {
    float pm[4][4], pc[4];

    if (matrix) {
        glUniformMatrix4fv(renderer.loc.uMatWorld, 1, GL_FALSE, (const void*)matrix);
        memcpy(pm, renderer.pipeline.world.matrix, sizeof(pm));
        memcpy(renderer.pipeline.world.matrix, matrix, sizeof(pm));
    }

    if (color) {
        glUniform4fv(renderer.loc.uClrWorld, 1, color);
        memcpy(pc, renderer.pipeline.world.color, sizeof(pc));
        memcpy(renderer.pipeline.world.color, color, sizeof(pc));
    }

    if (prevMatrix) memcpy(prevMatrix, pm, sizeof(pm));
    if (prevColor) memcpy(prevColor, pc, sizeof(pc));
}

void rendererClear(const float color[4]) {
    if (color) {
        glClearColor(color[0], color[1], color[2], color[3]);
    } else {
        glClearColor(0,0,0,0);
    }

    glClear(GL_COLOR_BUFFER_BIT);

    if (renderer.capability.depthTest) {
        glClearColor(0, 0, 0, 0);
        glClear(GL_DEPTH_BUFFER_BIT);
    }
}

void rendererDraw(RendererDrawMode mode, size_t n, const RendererVertex *v) {
    glEnableVertexAttribArray((GLuint)renderer.loc.aPos);
    glEnableVertexAttribArray((GLuint)renderer.loc.aTex);
    glEnableVertexAttribArray((GLuint)renderer.loc.aClr);

    glVertexAttribPointer((GLuint)renderer.loc.aPos, 3, GL_FLOAT, GL_FALSE, sizeof(*v), &v->x);
    glVertexAttribPointer((GLuint)renderer.loc.aTex, 2, GL_FLOAT, GL_FALSE, sizeof(*v), &v->s);
    glVertexAttribPointer((GLuint)renderer.loc.aClr, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(*v), &v->r);

    switch (mode) {
        case RENDERER_DRAW_MODE_POINTS: glDrawArrays(GL_POINTS, 0, (GLsizei)n); break;
        case RENDERER_DRAW_MODE_LINES: glDrawArrays(GL_LINES, 0, (GLsizei)n); break;
        case RENDERER_DRAW_MODE_LINE_LOOP: glDrawArrays(GL_LINE_LOOP, 0, (GLsizei)n); break;
        case RENDERER_DRAW_MODE_TRIANGLES: glDrawArrays(GL_TRIANGLES, 0, (GLsizei)n); break;
        case RENDERER_DRAW_MODE_TRIANGLE_STRIP: glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)n); break;
        case RENDERER_DRAW_MODE_TRIANGLE_FAN: glDrawArrays(GL_TRIANGLE_FAN, 0, (GLsizei)n); break;
        default: break;
    }

    glDisableVertexAttribArray((GLuint)renderer.loc.aClr);
    glDisableVertexAttribArray((GLuint)renderer.loc.aTex);
    glDisableVertexAttribArray((GLuint)renderer.loc.aPos);
}

void rendererDrawIndexed(RendererDrawMode mode, size_t ni, const uint16_t *i, const RendererVertex *v) {
    glEnableVertexAttribArray((GLuint)renderer.loc.aPos);
    glEnableVertexAttribArray((GLuint)renderer.loc.aTex);
    glEnableVertexAttribArray((GLuint)renderer.loc.aClr);

    glVertexAttribPointer((GLuint)renderer.loc.aPos, 3, GL_FLOAT, GL_FALSE, sizeof(*v), &v->x);
    glVertexAttribPointer((GLuint)renderer.loc.aTex, 2, GL_FLOAT, GL_FALSE, sizeof(*v), &v->s);
    glVertexAttribPointer((GLuint)renderer.loc.aClr, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(*v), &v->r);

    switch (mode) {
        case RENDERER_DRAW_MODE_POINTS: glDrawElements(GL_POINTS, (GLsizei)ni, GL_UNSIGNED_SHORT, i); break;
        case RENDERER_DRAW_MODE_LINES: glDrawElements(GL_LINES, (GLsizei)ni, GL_UNSIGNED_SHORT, i); break;
        case RENDERER_DRAW_MODE_LINE_LOOP: glDrawElements(GL_LINE_LOOP, (GLsizei)ni, GL_UNSIGNED_SHORT, i); break;
        case RENDERER_DRAW_MODE_TRIANGLES: glDrawElements(GL_TRIANGLES, (GLsizei)ni, GL_UNSIGNED_SHORT, i); break;
        case RENDERER_DRAW_MODE_TRIANGLE_STRIP: glDrawElements(GL_TRIANGLE_STRIP, (GLsizei)ni, GL_UNSIGNED_SHORT, i); break;
        case RENDERER_DRAW_MODE_TRIANGLE_FAN: glDrawElements(GL_TRIANGLE_FAN, (GLsizei)ni, GL_UNSIGNED_SHORT, i); break;
        default: break;
    }

    glDisableVertexAttribArray((GLuint)renderer.loc.aClr);
    glDisableVertexAttribArray((GLuint)renderer.loc.aTex);
    glDisableVertexAttribArray((GLuint)renderer.loc.aPos);
}

void rendererViewport(int x, int y, int w, int h) {
    glViewport(x, y, w, h);
}

RendererTexture *rendererTextureNew(const Image *image, RendererTextureQuality quality, RendererTextureWrapping wrapping) {
    RendererTexture *texture = malloc(sizeof(*texture));

    glGenTextures(1, &texture->id);
    glBindTexture(GL_TEXTURE_2D, texture->id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)image->w, (GLsizei)image->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->p);

    switch (quality) {
        case RENDERER_TEXTURE_QUALITY_BEST:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glGenerateMipmap(GL_TEXTURE_2D);
            break;
        case RENDERER_TEXTURE_QUALITY_WORST:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            break;
        default:
            break;
    }

    switch (wrapping) {
        case RENDERER_TEXTURE_WRAPPING_CLAMP_TO_EDGE:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            break;
        case RENDERER_TEXTURE_WRAPPING_REPEAT:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            break;
        case RENDERER_TEXTURE_WRAPPING_MIRRORED_REPEAT:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
            break;
        default:
            break;
    }

    renderer.texture.active = texture;

    return texture;
}

RendererTexture *rendererTextureDel(RendererTexture *texture) {
    glDeleteTextures(1, &texture->id);
    free(texture);
    return NULL;
}

void rendererTextureActivate(const RendererTexture *texture) {
    if (renderer.texture.active == texture) return;
    if (renderer.texture.active && texture && renderer.texture.active->id == texture->id) return;
    renderer.texture.active = texture;
    if (texture == NULL) texture = renderer.texture.null;
    glBindTexture(GL_TEXTURE_2D, texture->id);
}

const RendererTexture *rendererTextureActive(void) {
    return renderer.texture.active;
}
