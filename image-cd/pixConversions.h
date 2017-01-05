/*
 * pixConversions.h
 * Header file of the pixConversions interface
 * October 21st, 2016
 * Georgios Pesmazoglou, Alexandru Ungureanu
 *
 * This interface is responsible for conversions from and to RGB color space, 
 * video component color space, and cosine elements of 4 video component pixels.
 *
 * All the public methods of this interface have an inverse public method. That 
 * is, you can convert components from a certain color space to another, as well
 * as from the other color space to the initial one.
 *
 * The public data structures of the pixConversions interface are structures of 
 * components of the color spaces supported by this interface.
 */

#include <stdio.h>
#include <stdint.h>
#include "pnm.h"


/*
 * structure containing the intensities (luma values) of 4 pixels
 * each luma value is a floating-point number in the [0, 1] interval
 */
struct intensities {
        float y1;
        float y2;
        float y3;
        float y4;
};

/*
 * structure containing the cosine coefficients that correspond to 4 pixels' 
 * luma values
 * the a-coefficient is a floating-point number in the [0, 1], and b, c, d
 * are floating-point numbers in the [-0.3, 0.3] interval (other values
 * are pretty rare so we shrinked the [-0.5, 0.5] interval)
 */
struct cosines {
        float a;
        float b;
        float c;
        float d;
};

/*
 * structure containing the video component color space values of a pixel
 * the Y-coefficient (luma) is a floating-point number in the interval [0, 1]
 * the chroma values (Pb, Pr) are floating-point numbers in [-0.5, 0.5] interval
 */
struct video {
        float y;
        float pb;
        float pr;
};

/*
 * functions responsible for converting between cosine coefficients and luma
 * values of 4 pixels; functions take in an intensities/cosines structure and 
 * return the appropriate cosine/intensities structure obtained by conversion
 * it is a checked-runtime error for both functions if the coefficients of the 
 * structure-parameters are not within the bounds described previously 
 */
extern struct cosines Y_to_cos(struct intensities ys);
extern struct intensities cos_to_Y(struct cosines cos);

/*
 * functions responsible for converting a pixel's components between RGB and 
 * video color space; functions take in a RGB/comp structure and return the 
 * appropriate comp/RGB structure obtained by conversion
 * it is a checked-runtime error for both functions if the coefficients of the 
 * structure-parameters are not within the bounds described previously   
 */  
extern struct video RGB_to_video(struct Pnm_rgb rgb, unsigned rgb_scale);
extern struct Pnm_rgb video_to_RGB(struct video comps, unsigned rgb_scale);

