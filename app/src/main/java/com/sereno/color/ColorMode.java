package com.sereno.color;

/** \brief Defines ColorMode usable*/
public class ColorMode
{
    public static final int RAINBOW          = 0; /*!< Rainbow            colormode*/
    public static final int GRAYSCALE        = 1; /*!< Grayscale          colormode*/
    public static final int WARM_COLD_CIELAB = 2; /*!< CIELAB blue to red colormode*/
    public static final int WARM_COLD_CIELUV = 3; /*!< CIELUV blue to red colormode*/
    public static final int WARM_COLD_MSH    = 4; /*!< MSH blue to red    colormode*/

    public static final Color coldRGB = new Color(59.0f/255.0f, 76.0f/255.0f, 192.0f/255.0f, 1.0f);
    public static final Color warmRGB = new Color(180.0f/255.0f, 4.0f/255.0f, 38.0f/255.0f, 1.0f);

    public static final LABColor coldLAB  = new LABColor(coldRGB);
    public static final LABColor whiteLAB = new LABColor(Color.WHITE);
    public static final LABColor warmLAB  = new LABColor(warmRGB);

    public static final LUVColor coldLUV  = new LUVColor(coldRGB);
    public static final LUVColor whiteLUV = new LUVColor(Color.WHITE);
    public static final LUVColor warmLUV  = new LUVColor(warmRGB);
}
