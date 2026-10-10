#ifndef PBM_H
#define PBM_H

#include <stdio.h>
#include <stdbool.h>

#define MAX_PBM_LEVEL 1
#define MAX_PGM_LEVEL ((1 << 16) - 1)
#define MAX_PPM_LEVEL ((1 << 16) - 1)

#define PBM_PIXEL_BLACK ((PBMPixel){ .mono = 1 })
#define PBM_PIXEL_WHITE ((PBMPixel){ .mono = 0 })

/* The supported PBM types. */
typedef enum {
    /* Bit map, each pixel is either 0 or 1. */
    PBMTypeBitmap,
    /* Gray map, each pixel is between 0 and MAX_PGM_LEVEL. */
    PBMTypeGraymap,
    /* Pixel map, each pixel channel is between 0 and MAX_PPM_LEVEL. */
    PBMTypePixmap,
} PBMType;

/* A pixel in a PBM image. */
typedef union {
    /* The monochrome value (PBM and PGM). */
    unsigned short mono;
    /* The RGB value (PPM). */
    struct {
        unsigned short red;
        unsigned short green;
        unsigned short blue;
    } color;
} PBMPixel;

/* Describes the metadata of a PBM image. */
typedef struct {
    /* The image type. */
    PBMType type;
    /* The width of the image. */
    size_t width;
    /* The height of the image. */
    size_t height;
    /* The maximum color level. This is always set to MAX_PBM_LEVEL for PortableBitMap. */
    unsigned short max_color;
} PBMMeta;

/* Returns the magic number of the PBM type. */
const char *pbm_magic(PBMType type, bool ascii);

/* A handle for a PBM image. */
typedef struct pbm *PBM;

/* Allocates a new PBM image using the metadata. NULL is returned if width or height is 0. */
PBM pbm_new(PBMMeta meta);

/* Returns the image's metadata. */
PBMMeta pbm_meta(PBM pbm);

/* Gets the pixel at (x, y). */
/* If the PBM is not wide, the pixel's MSB is always 0. */
/* -1 is returned if (x, y) is out of range. */
PBMPixel pbm_get(PBM pbm, size_t x, size_t y);

/* Sets the pixel at (x, y). */
/* The value is clamped between 0 and the max level defined in the metadata. */
/* If the PBM is not wide, the pixel's MSB is ignored. */
void pbm_set(PBM pbm, size_t x, size_t y, PBMPixel pixel);

/* Writes the PBM image in ASCII format to the file stream. */
void pbm_write_ascii(PBM pbm, FILE *stream);

/* Writes the PBM image in binary format to the file stream. */
void pbm_write_binary(PBM pbm, FILE *stream);

/* Frees the PBM image from memory. */
void pbm_free(PBM pbm);

#endif
