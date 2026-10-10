#include <stdio.h>
#include <stdlib.h>

#include "../include/pbm.h"

#define PBM_WIDTH 6
#define PBM_HEIGHT 10

int main(void)
{
	PBMMeta meta = (PBMMeta){
	    .type = PBMTypeBitmap,
	    .width = PBM_WIDTH,
	    .height = PBM_HEIGHT,
	};
	PBM pbm = pbm_new(meta);
	if (pbm == NULL) {
		fprintf(stderr, "error: PBM file size (%d, %d) is invalid.\n",
			PBM_WIDTH, PBM_HEIGHT);
		return 1;
	}

	// The cross should be one horizontal line and one vertical line, both
	// centered. Draw the horizontal line:
	for (int y = 4; y < 6; y++) {
		for (int x = 0; x < 6; x++) {
			pbm_set(pbm, x, y, PBM_PIXEL_BLACK);
		}
	}
	// Then the vertical line:
	for (int y = 0; y < PBM_HEIGHT; y++) {
		for (int x = 2; x < 4; x++) {
			pbm_set(pbm, x, y, PBM_PIXEL_BLACK);
		}
	}

	pbm_write_ascii(pbm, stdout);
	pbm_free(pbm);
	pbm = NULL;

	return 0;
}
