#include <math.h>

#include "../include/pbm.h"

#define WIDTH 1024
#define HEIGHT 1024
#define SCALE_X (M_PI * 2.0 / WIDTH)
#define SCALE_Y (HEIGHT / 8.0)
#define OFFSET_Y (HEIGHT / 2.0)
#define FUNC sin

int main(void)
{
	PBMMeta meta = (PBMMeta){
	    .type = PBMTypePixmap,
	    .width = WIDTH,
	    .height = HEIGHT,
	    .max_color = 255,
	};
	PBM pbm = pbm_new(meta);

	for (int x = 0; x < WIDTH; x++) {
		int y = round(FUNC(x * SCALE_X) * SCALE_Y + OFFSET_Y);

		// Color all pixels below the curve.
		for (; y < HEIGHT; y++) {
			unsigned short green = round((float)y / HEIGHT * 255);

			PBMPixel pixel = (PBMPixel){
			    .color = {.red = 255, .blue = 0, .green = green}};
			pbm_set(pbm, x, y, pixel);
		}
	}

	pbm_write_binary(pbm, stdout);
	pbm_free(pbm);
	pbm = NULL;

	return 0;
}
