#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "image.h"
#include "bio.h"

Image *imageCopy(const Image *src, Image *dst) {
    if (!dst) {
        dst = malloc(sizeof(*dst));
    }
    dst = memcpy(dst, src, sizeof(*dst));
    dst->id = strcpy(malloc((uint32_t)strlen(src->id)), src->id);
    size_t imageSize = dst->w * dst->h * sizeof(*dst->p);
    dst->p = malloc(imageSize);
    dst->p = memcpy(dst->p, src->p, imageSize);
    return dst;
}

Image *imageDel(Image *image, bool freeHandle) {
    free(image->id);
    free(image->p);
    if (freeHandle) free(image);
    return NULL;
}

Image *imageRead(const void *p, Image *image) {
    if (!image) {
        image = malloc(sizeof(*image));
    }

    uint32_t idLength;
    p = bioReadU32LE(p, &idLength);
    image->id = malloc(idLength + 1);
    p = bioReadU8v(p, idLength, (uint8_t*)image->id);
    image->id[idLength] = '\0';

    p = bioReadU32LE(p, &image->w);
    p = bioReadU32LE(p, &image->h);
    image->p = malloc(image->w * image->h * sizeof(*image->p));

    for (uint32_t y = 0; y < image->h; ++y) {
        for (uint32_t x = 0; x < image->w; ++x) {
            p = bioReadU8(p, &image->p[y * image->w + x].r);
            p = bioReadU8(p, &image->p[y * image->w + x].g);
            p = bioReadU8(p, &image->p[y * image->w + x].b);
            p = bioReadU8(p, &image->p[y * image->w + x].a);
        }
    }

    return image;
}

void *imageWrite(const Image *image, void *p) {
    uint32_t idLength = (uint32_t)strlen(image->id);
    p = bioWriteU32LE(p, idLength);
    p = bioWriteU8v(p, idLength, (const uint8_t*)image->id);

    p = bioWriteU32LE(p, image->w);
    p = bioWriteU32LE(p, image->h);

    for (uint32_t y = 0; y < image->h; ++y) {
        for (uint32_t x = 0; x < image->w; ++x) {
            p = bioWriteU8(p, image->p[y * image->w + x].r);
            p = bioWriteU8(p, image->p[y * image->w + x].g);
            p = bioWriteU8(p, image->p[y * image->w + x].b);
            p = bioWriteU8(p, image->p[y * image->w + x].a);
        }
    }

    return p;
}

Image *imageLoad(const char *path, Image *image) {
    return imageLoadFromFile(fopen(path, "rb"), true, image);
}

void imageSave(const Image *image, const char *path) {
    imageSaveToFile(image, fopen(path, "wb"), true);
}

Image *imageLoadFromFile(void *file, bool close, Image *image) {
    if (!image) {
        image = malloc(sizeof(*image));
    }

    uint32_t idLength = bioScanU32LE(file);
    image->id = malloc(idLength + 1);
    bioScanU8v(file, idLength, (uint8_t*)image->id);
    image->id[idLength] = '\0';

    image->w = bioScanU32LE(file);
    image->h = bioScanU32LE(file);
    image->p = malloc(image->w * image->h * sizeof(*image->p));

    for (uint32_t y = 0; y < image->h; ++y) {
        for (uint32_t x = 0; x < image->w; ++x) {
            image->p[y * image->w + x].r = bioScanU8(file);
            image->p[y * image->w + x].g = bioScanU8(file);
            image->p[y * image->w + x].b = bioScanU8(file);
            image->p[y * image->w + x].a = bioScanU8(file);
        }
    }

    if (close) {
        fclose(file);
    }

    return image;
}

void imageSaveToFile(const Image *image, void *file, bool close) {
    uint32_t idLength = (uint32_t)strlen(image->id);
    bioPrintU32LE(file, idLength);
    bioPrintU8v(file, idLength, (const uint8_t*)image->id);

    bioPrintU32LE(file, image->w);
    bioPrintU32LE(file, image->h);

    for (uint32_t y = 0; y < image->h; ++y) {
        for (uint32_t x = 0; x < image->w; ++x) {
            bioPrintU8(file, image->p[y * image->w + x].r);
            bioPrintU8(file, image->p[y * image->w + x].g);
            bioPrintU8(file, image->p[y * image->w + x].b);
            bioPrintU8(file, image->p[y * image->w + x].a);
        }
    }

    if (close) {
        fclose(file);
    }
}

size_t imageSize(const Image *image) {
    return 4 + strlen(image->id) + 4 * 2 + image->w * image->h * 4;
}

void imageSet(Image *image, uint32_t x, uint32_t y, ImagePixel p) {
    image->p[y * image->w + x] = p;
}

ImagePixel imageGet(const Image *image, uint32_t x, uint32_t y) {
    return image->p[y * image->w + x];
}
