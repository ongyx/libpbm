#include <math.h>
#include <stdlib.h>

#include "../include/pbm.h"

#define CLAMP(value, min, max) (value > max ? max : (value < min ? min : value))
#define DIGITS(value) (floor(log10(value)) + 1)
#define MAX_ASCII_FORMAT 6

const char *pbm_magic(PBMType type, bool ascii)
{
	switch (type) {
	case PBMTypeBitmap:
		return ascii ? "P1" : "P4";
	case PBMTypeGraymap:
		return ascii ? "P2" : "P5";
	case PBMTypePixmap:
		return ascii ? "P3" : "P6";
	default:
		return "";
	}
}

struct pbm {
	PBMMeta meta;
	PBMPixel *pix;
};

PBM pbm_new(PBMMeta meta)
{
	// Make sure the metadata is valid...
	if (meta.width == 0 || meta.height == 0) {
		return NULL;
	}

	switch (meta.type) {
	case PBMTypeBitmap:
		meta.max_color = MAX_PBM_LEVEL;
		break;
	case PBMTypeGraymap:
		meta.max_color = CLAMP(meta.max_color, 0, MAX_PGM_LEVEL);
		break;
	case PBMTypePixmap:
		meta.max_color = CLAMP(meta.max_color, 0, MAX_PPM_LEVEL);
		break;
	}

	PBM pbm = malloc(sizeof(struct pbm));
	pbm->meta = meta;
	pbm->pix = malloc(meta.width * meta.height * sizeof(PBMPixel));

	return pbm;
}

PBMMeta pbm_options(PBM pbm) { return pbm->meta; }

size_t _pbm_index(PBM pbm, size_t x, size_t y)
{
	if (x >= pbm->meta.width || y >= pbm->meta.height) {
		return -1;
	}

	return (y * pbm->meta.width) + x;
}

PBMPixel *pbm_get(PBM pbm, size_t x, size_t y)
{
	size_t index = _pbm_index(pbm, x, y);
	if (index < 0) {
		return NULL;
	}

	return &pbm->pix[index];
}

void pbm_set(PBM pbm, size_t x, size_t y, PBMPixel pixel)
{
	size_t index = _pbm_index(pbm, x, y);
	if (index < 0) {
		return;
	}

	switch (pbm->meta.type) {
	case PBMTypeBitmap:
	case PBMTypeGraymap:
		pixel.mono = CLAMP(pixel.mono, 0, pbm->meta.max_color);
		break;
	case PBMTypePixmap:
		pixel.color.red =
		    CLAMP(pixel.color.red, 0, pbm->meta.max_color);
		pixel.color.green =
		    CLAMP(pixel.color.green, 0, pbm->meta.max_color);
		pixel.color.blue =
		    CLAMP(pixel.color.blue, 0, pbm->meta.max_color);
		break;
	}

	pbm->pix[index] = pixel;
}

void pbm_write_ascii(PBM pbm, FILE *stream)
{
	// In ASCII format, PBM images always start with their magic number,
	// followed by dimensions and max color level (for PGM and PPM images).

	fprintf(stream, "%s\n", pbm_magic(pbm->meta.type, true));
	fprintf(stream, "%ld %ld\n", pbm->meta.width, pbm->meta.height);

	if (pbm->meta.type == PBMTypeGraymap ||
	    pbm->meta.type == PBMTypePixmap) {
		fprintf(stream, "%d\n", pbm->meta.max_color);
	}

	// For PGM and PPM images, pixel values must be padded up to the number
	// of digits taken up by the max color.
	int width = DIGITS(pbm->meta.max_color);

	const char *sep;
	for (size_t y = 0; y < pbm->meta.height; y++) {
		sep = "";

		for (size_t x = 0; x < pbm->meta.width; x++) {
			PBMPixel *pixel = pbm_get(pbm, x, y);

			switch (pbm->meta.type) {
			case PBMTypeBitmap:
			case PBMTypeGraymap:
				fprintf(stream, "%s%d", sep, pixel->mono);
				break;
			case PBMTypePixmap:
				fprintf(stream, "%s%*d %*d %*d", sep, width,
					pixel->color.red, width,
					pixel->color.green, width,
					pixel->color.blue);
				break;
			}

			// Only add the separator from the 2nd element onward.
			sep = " ";
		}

		fprintf(stream, "\n");
	}
}

void pbm_free(PBM pbm)
{
	free(pbm->pix);
	pbm->pix = NULL;
}
