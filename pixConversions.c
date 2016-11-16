/*
 * pixConversions.c
 * Implementation (source file) of the pixConversions interface
 * October 21st, 2016
 * Georgios Pesmazoglou, Alexandru Ungureanu
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include "assert.h"
#include "pnm.h"
#include "pixConversions.h"

/*
 * constant global variables that represent the lower and upper bounds of the 
 * pixel coefficients used in the implementation
 */
const float LUMA_MIN = 0;                  /* lower bound for intensity value */
const float LUMA_MAX = 1;                  /* upper bound for intensity value */
const float BCD_MAX = 0.3;           /* upper bound for b, c, d cosine coeffs */
const float BCD_MIN = -0.3;          /* lower bound for b, c, d cosine coeffs */
const float A_MAX = 1;                  /* upper bound for the a cosine coeff */
const float A_MIN = 0;                  /* lower bound for the a cosine coeff */ 
const unsigned RGB_SCALE = 255;        /* scale of the RGB color space coeffs */
const float P_MIN = -0.5;                    /* lower bound of a chroma value */
const float P_MAX = 0.5;                     /* upper bound of a chroma value */  
const float RGB_MIN = 0;      /* lower bound for a scaled to [0, 1] RGB coeff */
const float RGB_MAX = 1;      /* lower bound for a scaled to [0, 1] RGB coeff */

/*
 * private function used to check if the luma values of 4 pixels are within the
 * corresponding bounds; parameter: the structure of intensities
 * checked-runtime error if one of the values is not within the bounds
 */
static void check_luma_bounds(struct intensities);

/* 
 * private function used to check if 4 cosine coefficients are within the 
 * corresponding bounds; parameter: the structure of cosine coefficients
 * checked runtime error if one of the coefficients is not within the bounds 
 */
static void check_cos_bounds(struct cosines);

/*
 * private function used to convert a floating-point value to the closest 
 * integer; parameter: the floating-point number; returns: the rounded value
 */
static int round_value(float);

/*
 * private function used to shrink/adjust a floating-point value if it is not
 * within the designated bounds; the new value is the lower or upper bound of
 * the designated range (whichever is closer to the value)
 * parameters: floating-point value, lower bound, upper bound (floats)
 * returns the new (shrinked) value or the initial value if shrinking not needed
 */
static float adjust_float(float, float, float);

/*
 * converts the intensities of 4 pixels to scaled cosine coefficients
 * parameter: structure containing the 4 intensities (luma values)
 * returns a structure containing the scaled cosine coeffs as following:
 * a - floating-point number in the [0, 1] interval
 * b, c, d - floating-point numbers in the [-0.3, 0.3] interval
 * checked runtime error if intensitites are not withing the designated bounds
 */
struct cosines Y_to_cos(struct intensities ys)
{
        check_luma_bounds(ys);
        struct cosines cos;        /* 
         * get the cosine coefficients using the conversion formulas
         * because floating-point arithmetic is not precise, if the coefficients
         * are out of bounds adjust them to the closest bound 
         */
        cos.a = adjust_float((ys.y4 + ys.y3 + ys.y2 + ys.y1) / 4, A_MIN, A_MAX);
        cos.b = adjust_float((ys.y4 + ys.y3 - ys.y2 - ys.y1) / 4, BCD_MIN, 
                                                                       BCD_MAX);
        cos.c = adjust_float((ys.y4 - ys.y3 + ys.y2 - ys.y1) / 4, BCD_MIN, 
                                                                       BCD_MAX);
        cos.d = adjust_float((ys.y4 - ys.y3 - ys.y2 + ys.y1) / 4, BCD_MIN, 
                                                                       BCD_MAX);

        return cos;
}

/*
 * converts the cosine coefficients of 4 pixels to luma (intensity) values
 * parameter: cosines structure containing the 4 cosine coefficients
 * returns an intensities structure containing the 4 luma values (floating-point
 * integers in the [0, 1] interval)
 * checked runtime error if one of the coefficients is not withing its bounds 
 */
struct intensities cos_to_Y(struct cosines cos)
{
        check_cos_bounds(cos);
        struct intensities ys;
        /*
         * use the conversion formulas to compute the intensities
         * since floating-point arithmetic is not precise, the intensities may
         * go out of bounds, so adjust them to the closest bound if necessary
         */
        ys.y1 = adjust_float(cos.a - cos.b - cos.c + cos.d, LUMA_MIN, LUMA_MAX);
        ys.y2 = adjust_float(cos.a - cos.b + cos.c - cos.d, LUMA_MIN, LUMA_MAX);
        ys.y3 = adjust_float(cos.a + cos.b - cos.c - cos.d, LUMA_MIN, LUMA_MAX);
        ys.y4 = adjust_float(cos.a + cos.b + cos.c + cos.d, LUMA_MIN, LUMA_MAX);

        return ys;
}


/*
 * public method used to convert the RGB values of a pixel to component video
 * color space - Y (intensity), Pb, Pr (chroma values)
 * parameters: structure of RGB values of the pixel, the scale of the RGB values
 * returns a structure of video component color space values
 * checked runtime error if red, green, blue values are out of scale
 */
struct video RGB_to_video(struct Pnm_rgb rgb, unsigned rgb_scale)
{
        assert(rgb.red <= rgb_scale && rgb.green <= rgb_scale && rgb.blue <= 
                                                                   rgb_scale);
        float r, g, b, y, pb, pr;
        /* "unscale" the red, green, blue values to interval [0, 1] */
        r = ((float) rgb.red) / rgb_scale; 
        g = ((float) rgb.green) / rgb_scale;
        b = ((float) rgb.blue) / rgb_scale;
        /* 
         * compute the intensity using conversion formula; if it goes out of 
         * bounds because floating-point arithmetic is not precise, adjust it
         */
        y = adjust_float(0.299 * r + 0.587 * g + 0.114 * b, LUMA_MIN, LUMA_MAX);
        pb = adjust_float(-0.168736 * r - 0.331264 * g + 0.5 * b, P_MIN, P_MAX);
        pr = adjust_float(0.5 * r - 0.418688 * g - 0.081312 * b, P_MIN, P_MAX);
        
        struct video comp = {y, pb, pr};
        return comp;
}

/*
 * public method used to convert video component color space values to RGB 
 * values for a pixel; parameters: the structure containing the Y, Pb, Pr 
 * video component-values, and the RGB scale to be used
 * returns a structure contating the red, green, blue values of the pixel
 * checked runtime error if the component video values are not within the
 * appropriate bounds: [0, 1] for luma, [-0.5, 0.5] for chromas
 */
struct Pnm_rgb video_to_RGB(struct video comp, unsigned rgb_scale)
{
        assert(LUMA_MIN <= comp.y && comp.y <= LUMA_MAX);
        assert(P_MIN <= comp.pb && P_MIN <= comp.pr && comp.pb <= P_MAX &&
                                                          comp.pr <= P_MAX);
        float r, g, b;
        unsigned red, green, blue;
        /*
         * compute the "unscaled" red, green, blue values using the formulas
         * if they go out of bounds because of lost precision, adjust them
         */
        r = 1.0 * comp.y + 0.0 * comp.pb + 1.402 * comp.pr;
        r = adjust_float(r, RGB_MIN, RGB_MAX);
        g = 1.0 * comp.y - 0.344136 * comp.pb - 0.714136 * comp.pr;
        g = adjust_float(g, RGB_MIN, RGB_MAX);
        b = 1.0 * comp.y + 1.772 * comp.pb + 0.0 * comp.pr;
        b = adjust_float(b, RGB_MIN, RGB_MAX);
        /* scale the RGB floating-point values to unsigned integer values */
        red = (unsigned) round_value(r * rgb_scale);
        green = (unsigned) round_value(g * rgb_scale);
        blue = (unsigned) round_value(b * rgb_scale);
        
        struct Pnm_rgb rgb = {red, green, blue};
        return rgb;
}

float adjust_float(float x, float lower, float upper) 
{
        if(x > upper) {
                x = upper;
        }
        else if(x < lower) {
                x = lower;
        }
        return x;
}

void check_cos_bounds(struct cosines cos) 
{

        assert(cos.a >= A_MIN && cos.a <= A_MAX);
        assert(cos.b >= BCD_MIN && cos.b <= BCD_MAX);
        assert(cos.c >= BCD_MIN && cos.c <= BCD_MAX);
        assert(cos.d >= BCD_MIN && cos.d <= BCD_MAX);
}

void check_luma_bounds(struct intensities ys)
{
        assert(ys.y1 >= LUMA_MIN && ys.y1 <= LUMA_MAX);
        assert(ys.y2 >= LUMA_MIN && ys.y2 <= LUMA_MAX);
        assert(ys.y3 >= LUMA_MIN && ys.y3 <= LUMA_MAX);
        assert(ys.y4 >= LUMA_MIN && ys.y4 <= LUMA_MAX);
}

int round_value(float x)
{
        if (x - floor(x) < 0.5) {
                return floor(x);
        }
        else {
                return ceil(x);
        }
}


