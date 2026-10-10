#include <math.h>

#include "../include/pbm.h"

#define WIDTH 256
#define HEIGHT 256
#define SCALE_X (M_PI * 2.0 / WIDTH)
#define SCALE_Y (HEIGHT / 8.0)
#define OFFSET_Y (HEIGHT / 2.0)
#define FUNC sin

int main(void)
{
	PBMMeta meta = (PBMMeta){
	    .type = PBMTypeBitmap,
	    .width = WIDTH,
	    .height = HEIGHT,
	};
	PBM pbm = pbm_new(meta);

	for (int x = 0; x < WIDTH; x++) {
		int y = round(FUNC(x * SCALE_X) * SCALE_Y + OFFSET_Y);

		// Color all pixels below the curve.
		for (; y < HEIGHT; y++) {
			pbm_set(pbm, x, y, PBM_PIXEL_BLACK);
		}
	}

	pbm_write_ascii(pbm, stdout);
	pbm_free(pbm);
	pbm = NULL;

	return 0;
}
