#include <math.h>
#include <stdlib.h>

#include "../include/pbm.h"

#define MAX_ASCII_FORMAT 6
#define CLAMP(value, min, max) (value > max ? max : (value < min ? min : value))
#define DIGITS(value) (floor(log10(value)) + 1)
#define STRIDE(channels, is_wide) (is_wide ? channels * 2 : channels)
#define READ_WORD_BE(pbm, index)                                               \
	((unsigned short)pbm->pixels[index] << 8 | pbm->pixels[index + 1])
#define WRITE_WORD_BE(pbm, index, value)                                       \
	{                                                                      \
		pbm->pixels[index] = value >> 8;                               \
		pbm->pixels[index + 1] = value & 0xFF;                         \
	}

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
	unsigned char channels;
	bool is_wide;
	size_t length;
	// NOTE: Pixels are laid out in big-endian.
	unsigned char *pixels;
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
	// The equivalent of three pixels are required to store each channel for
	// PPMs.
	pbm->channels = meta.type == PBMTypePixmap ? 3 : 1;
	pbm->is_wide = meta.max_color >= 256;

	pbm->length =
	    meta.width * meta.height * STRIDE(pbm->channels, pbm->is_wide);
	// Twice as much storage is required for the wide representation.
	pbm->pixels = malloc(pbm->length * sizeof(unsigned char));

	return pbm;
}

PBMMeta pbm_meta(PBM pbm) { return pbm->meta; }

size_t _pbm_index(PBM pbm, size_t x, size_t y)
{
	if (x >= pbm->meta.width || y >= pbm->meta.height) {
		return -1;
	}

	return ((y * pbm->meta.width) + x) *
	       STRIDE(pbm->channels, pbm->is_wide);
}

PBMPixel pbm_get(PBM pbm, size_t x, size_t y)
{
	size_t index = _pbm_index(pbm, x, y);
	if (index < 0) {
		return (PBMPixel){.mono = 0};
	}

	PBMPixel result;

	switch (pbm->meta.type) {
	case PBMTypeBitmap:
	case PBMTypeGraymap:
		if (pbm->is_wide) {
			result.mono = READ_WORD_BE(pbm, index);
		} else {
			result.mono = pbm->pixels[index];
		}
		break;
	case PBMTypePixmap:
		if (pbm->is_wide) {
			result.color.red = READ_WORD_BE(pbm, index);
			result.color.green = READ_WORD_BE(pbm, index + 2);
			result.color.blue = READ_WORD_BE(pbm, index + 4);
		} else {
			result.color.red = pbm->pixels[index];
			result.color.green = pbm->pixels[index + 1];
			result.color.blue = pbm->pixels[index + 2];
		}
		break;
	}

	return result;
}

void pbm_set(PBM pbm, size_t x, size_t y, PBMPixel pixel)
{
	size_t index = _pbm_index(pbm, x, y);
	if (index < 0) {
		return;
	}

	switch (pbm->meta.type) {
	case PBMTypeBitmap:
	case PBMTypeGraymap:;
		unsigned short value =
		    CLAMP(pixel.mono, 0, pbm->meta.max_color);

		if (pbm->is_wide) {
			WRITE_WORD_BE(pbm, index, value);
		} else {
			pbm->pixels[index] = (unsigned char)value;
		}
		break;
	case PBMTypePixmap:;
		unsigned short red =
		    CLAMP(pixel.color.red, 0, pbm->meta.max_color);
		unsigned short green =
		    CLAMP(pixel.color.green, 0, pbm->meta.max_color);
		unsigned short blue =
		    CLAMP(pixel.color.blue, 0, pbm->meta.max_color);

		if (pbm->is_wide) {
			WRITE_WORD_BE(pbm, index, red);
			WRITE_WORD_BE(pbm, index + 2, green);
			WRITE_WORD_BE(pbm, index + 4, blue);
		} else {
			pbm->pixels[index] = (unsigned char)red;
			pbm->pixels[index + 1] = (unsigned char)green;
			pbm->pixels[index + 2] = (unsigned char)blue;
		}
		break;
	}
}

void _pbm_write_header(PBM pbm, FILE *stream, bool ascii)
{
	// PBM/PGM/PPM images always start with their magic number,
	// followed by dimensions and max color level (for PGM and PPM images).

	fprintf(stream, "%s\n", pbm_magic(pbm->meta.type, ascii));
	fprintf(stream, "%ld %ld\n", pbm->meta.width, pbm->meta.height);

	if (pbm->meta.type == PBMTypeGraymap ||
	    pbm->meta.type == PBMTypePixmap) {
		fprintf(stream, "%d\n", pbm->meta.max_color);
	}
}

void pbm_write_ascii(PBM pbm, FILE *stream)
{
	_pbm_write_header(pbm, stream, true);

	// For PGM and PPM images, pixel values must be padded up to the number
	// of digits taken up by the max color.
	int width = DIGITS(pbm->meta.max_color);

	const char *sep;
	for (size_t y = 0; y < pbm->meta.height; y++) {
		sep = "";

		for (size_t x = 0; x < pbm->meta.width; x++) {
			PBMPixel pixel = pbm_get(pbm, x, y);

			switch (pbm->meta.type) {
			case PBMTypeBitmap:
			case PBMTypeGraymap:
				fprintf(stream, "%s%d", sep, pixel.mono);
				break;
			case PBMTypePixmap:
				fprintf(stream, "%s%*d %*d %*d", sep, width,
					pixel.color.red, width,
					pixel.color.green, width,
					pixel.color.blue);
				break;
			}

			// Only add the separator from the 2nd element onward.
			sep = " ";
		}

		fprintf(stream, "\n");
	}
}

void pbm_write_binary(PBM pbm, FILE *stream)
{
	_pbm_write_header(pbm, stream, false);

	// Since the internal pixels are stored in big-endian, it can be copied
	// directly into the stream.
	fwrite(pbm->pixels, sizeof(unsigned char), pbm->length, stream);
	fflush(stream);
}

void pbm_free(PBM pbm)
{
	free(pbm->pixels);
	pbm->pixels = NULL;
	free(pbm);
}
