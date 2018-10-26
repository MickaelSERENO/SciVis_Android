#ifndef  COLORMODE_INC
#define  COLORMODE_INC

namespace sereno
{
    /* \brief Corresponds to the color mode available on this program */
    enum ColorMode
    {
	    RAINBOW,          /*!< Rainbow colormode*/
        GRAYSCALE,        /*!< Greyscale colormode*/
        WARM_COLD_CIELAB, /*!< Red to blue (white in the middle) colormode. Based on CIELAB*/
        WARM_COLD_CIELUV, /*!< Red to blue (white in the middle) colormode. Based on CIELUV*/
        WARM_COLD_MSH     /*!< Red to blue (white in the middle) colormode. Based on MSH*/
    };
}

#endif