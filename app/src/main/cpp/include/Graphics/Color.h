#ifndef DEF_COLOR_INCLUDE
#define DEF_COLOR_INCLUDE

#include <cmath>

namespace sereno
{
    /** \brief A Color basic class */
    class Color
    {
        public:
            /** \brief Create a color
             * red, green, blue and alpha must to be between 0.0f and 1.0f
             * \param red red component
             * \param green green component
             * \param blue blue component
             * \param alpha alpha component*/
            Color(float red=0.0f, float green=0.0f, float blue=0.0f, float alpha=1.0f);

            /** \brief Create a Color from another
             * \param c the color which will be copied */
            Color(const Color& c);

            /** \brief Create a color from a color array
             * \param color color array. Need the following components on this order : red, green, blue, alpha, between 0.0f and 1.0f. */
            Color(float* color);

            /** \brief copied operator
             * \param c Color which will be copied */
            Color operator=(const Color& c);

            /** \brief Get the component array
             * \param array array which components will be saved. */
            void getFloatArray(float* array) const;

            float r; /** <red component */
            float g; /** <green component */
            float b; /** <blue component */
            float a; /** <alpha component */

            static const Color WHITE;/** <White color */
            static const Color BLACK;/** <Black color */
            static const Color RED;/** <Red color */
            static const Color GREEN;/** <Green color */
            static const Color BLUE;/** <Blue color */
            static const Color MAGENTA;/** <Magenta color */
            static const Color YELLOW;/** <Yellow color*/
            static const Color CYAN;/** <Cyan color */
            static const Color TRANSPARENT;/** <Transparent color */
    };

    /** \brief  The HSV Colorspace description */
    class HSVColor
    {
        public:
            /* \brief Constructor
             * \param _h the hue
             * \param _s the saturation
             * \param _v the value
             * \param _a the alpha */
            HSVColor(float _h, float _s, float _v, float _a=1.0f);

            /* \brief Constructor
             * \param col the color to convert */
            HSVColor(const Color& col);

            /* \brief Constructor
             * \param copy the parameter to copy */
            HSVColor(const HSVColor& copy);

            /* \brief Constructor
             * \param mvt the value to move */
            HSVColor(HSVColor&& mvt);

            /* \brief operator=
             * \param color the value to copy */
            HSVColor& operator=(const HSVColor& color);

            HSVColor operator+(const HSVColor& color) const;
            HSVColor operator-(const HSVColor& color) const;
            HSVColor operator*(float t) const;

            void operator+=(const HSVColor& color);
            void operator-=(const HSVColor& color);
            void operator*=(float t);

            /* \brief Set the HSV colorspace value from RGB colorspace value 
             * \param color the color to convert*/
            void setFromRGB(const Color& color);

            /* \brief Convert from the HSV colorspace to the RGB colorspace
             * \return the color in RGB space */
            Color toRGB() const;

            float h; /*!< The Hue between 0 and 360°*/
            float s; /*!< The Saturation*/
            float v; /*!< The value*/
            float a; /*!< The alpha*/
    };

    /**
     * \brief  Returns t*color
     *
     * \param t a multiplicator
     * \param color the color to multiply
     *
     * \return the color once multiplied
     */
    HSVColor operator*(float t, const HSVColor& color);


    class XYZColor
    {
        public:
            XYZColor(float _x, float _y, float _z, float _a);
            XYZColor(const Color& color);
            XYZColor(const XYZColor& copy);
            XYZColor(XYZColor&& mvt);
            XYZColor& operator=(const XYZColor& copy);
    };
}
#endif
