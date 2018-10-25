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
     * \param t a multiplicator
     * \param color the color to multiply
     * \return the color once multiplied
     */
    HSVColor operator*(float t, const HSVColor& color);


    /** \brief  XYZ colorspace representation */
    class XYZColor
    {
        public:
            /**
             * \brief  Constructor
             * \param _x x component
             * \param _y y component
             * \param _z z component
             * \param _a a component
             */
            XYZColor(float _x, float _y, float _z, float _a=1.0f);

            /**
             * \brief  Convert a RGB color to a XYZ color object
             * \param color the color to convert
             */
            XYZColor(const Color& color);

            /**
             * \brief  Copy constructor
             * \param copy the object to copy
             */
            XYZColor(const XYZColor& copy);

            /**
             * \brief  Movement constructor
             * \param mvt The object to move
             */
            XYZColor(XYZColor&& mvt);

            /**
             * \brief  Affectation operator
             * \param copy the object to copy
             * \return *this
             */
            XYZColor& operator=(const XYZColor& copy);

            XYZColor operator+(const XYZColor& color) const;
            XYZColor operator-(const XYZColor& color) const;
            XYZColor operator*(float t) const;

            void operator+=(const XYZColor& color);
            void operator-=(const XYZColor& color);
            void operator*=(float t);

            /**
             * \brief  Convert a RGB color
             * \param color the object to convert
             */
            void setFromRGB(const Color& color);

            /**
             * \brief  Convert this object in a RGB colorspace object
             * \return  An RGB color
             */
            Color toRGB() const;

            float x; /*!< The X component*/
            float y; /*!< The Y component*/
            float z; /*!< The Z component*/
            float a; /*!< The A component*/

            static const XYZColor reference;
    };

    /** \brief  The LAB colorspace representation */
    class LABColor
    {
        public:
            /**
             * \brief  Constructor
             * \param _l L component
             * \param _a A component
             * \param _b B component
             * \param _transparency Transparency component
             */
            LABColor(float _l, float _a, float _b, float _transparency=1.0f);

            /**
             * \brief  Convert a RGB color to LABColor
             * \param color the color to convert
             */
            LABColor(const Color& color);

            /**
             * \brief  Convert a XYZ color to LABColor
             * \param color the color to convert
             */
            LABColor(const XYZColor& color);

            /**
             * \brief  Copy Constructor
             * \param copy the color to copy
             */
            LABColor(const LABColor& copy);

            /**
             * \brief  Movement Constructor
             * \param mvt the color to move
             */
            LABColor(LABColor&& mvt);
            /**
             * \brief  Assignment operator
             * \param copy the object to assign
             * \return   
             */
            LABColor& operator=(const LABColor& copy);

            LABColor operator+(const LABColor& color) const;
            LABColor operator-(const LABColor& color) const;
            LABColor operator*(float t) const;

            void operator+=(const LABColor& color);
            void operator-=(const LABColor& color);
            void operator*=(float t);

            /**
             * \brief  Convert a RGB Color and store the result in this object
             * \param color the RGB Color object
             */
            void setFromRGB(const Color& color);

            /**
             * \brief  Convert a XYZ Color and store the result in this object
             * \param color the XYZ Color object
             */
            void setFromXYZ(const XYZColor& color);

            /**
             * \brief  Convert this object to an XYZ Color object
             * \return   The XYZ Color object
             */
            XYZColor toXYZ()   const;

            /**
             * \brief  Convert this object to an RGB Color object
             * \return   The RGB Color object
             */
            Color    toRGB() const;

            float l;
            float a;
            float b;
            float transparency;
        private:
            /**
             * \brief  a private function which helps determining the three component value. 7.787*v+16.0/116.0 otherwise. theta = 6.0/29.0 -> theta^3 = 0.008856
             * \param v the value to convert
             * \return  v^(1.0/3.0) if v > 0.008856 
             */
            float f(float v) const {return (v > 0.008856 ? pow(v, 1.0/3.0) : 4.83333f*v + 4.0/29.0);}
            /** \brief the inverse function which helps determining the three component value. 0.128418*(v-4.0/29.0) otherwise, thata = 6.0/29.0 -> 3*theta^2 = 0.128418
             * \param vthe value to determine
             * \return v^/3.0 if v > 6.0/29.0*/
            float invF(float v) const {return (v > 6.0/29.0 ? v*v*v : 0.128418*(v - 4.0/29.0));}
    };

    /** \brief  LUV colorspace */
    class LUVColor
    {
        public:
            /**
             * \brief  Constructor
             * \param _l L component 
             * \param _u U component
             * \param _v V component
             */
            LUVColor(float _l, float _u, float _v, float _a=1.0f);

            /**
             * \brief  Constructor. Convert a RGB Color to a LUV Color
             * \param color the RGB Color to convert
             */
            LUVColor(const Color& color);

            /**
             * \brief  Constructor. Convert a XYZ Color to a XYZ Color
             * \param xyz the XYZ Color to convert
             */
            LUVColor(const XYZColor& xyz);

            /**
             * \brief Copy Constructor. Copy an existing LUVColor
             * \param copy the color to copy
             */
            LUVColor(const LUVColor& copy);

            /**
             * \brief  Movement Constructor. Move an existing LUVColor
             * \param mvt the color to move
             */
            LUVColor(LUVColor&& mvt);

            LUVColor& operator=(const LUVColor& copy);

            LUVColor operator+(const LUVColor& color) const;
            LUVColor operator-(const LUVColor& color) const;
            LUVColor operator*(float t) const;

            void operator+=(const LUVColor& color);
            void operator-=(const LUVColor& color);
            void operator*=(float t);

            /**
             * \brief  Convert a RGB Value
             * \param color the value to convert
             */
            void setFromRGB(const Color&    color);
            /**
             * \brief  Convert a XYZ Value
             * \param color the xyz value to convert
             */
            void setFromXYZ(const XYZColor& color);

            /**
             * \brief  Convert this object to a RGB colorspace
             * \return   the RGB color value
             */
            Color toRGB() const;

            /**
             * \brief  Convert this object to a XYZ colorspace
             * \return   the XYZ color value
             */
            XYZColor toXYZ() const;

            float l; /*!< The L component*/
            float u; /*!< The U component*/
            float v; /*!< The V component*/
            float a; /*!< The alpha component*/
    };

    /* \brief The MSH Colorspace (see Diverging Color Maps for Scientific Visualization)*/
    class MSHColor
    {
        public:
            /**
             * \brief  Constructor
             * \param _m The M component
             * \param _s The S component
             * \param _h The H component
             * \param _a The alpha component
             */
            MSHColor(float _m, float _s, float _h, float _a=1.0);

            /**
             * \brief  Constructor. Convert a RGB Color value to MSH color value
             * \param color the value to convert
             */
            MSHColor(const Color& color);

            /**
             * \brief  Constructor. Convert a XYZ Color value to MSH color value
             * \param color the value to convert
             */
            MSHColor(const XYZColor& color);

            /**
             * \brief  Constructor. Convert a LAB Color value to MSH color value
             * \param color the value to convert
             */
            MSHColor(const LABColor& color);

            /**
             * \brief  Copy Constructor
             * \param copy the value to copy
             */
            MSHColor(const MSHColor& copy);

            /**
             * \brief  Movement constructor
             * \param mvt the value to move
             */
            MSHColor(MSHColor&& mvt);

            /**
             * \brief  Assignment operator
             * \param copy the value to copy
             * \return  *this
             */
            MSHColor& operator=(const MSHColor& copy);

            MSHColor operator+(const MSHColor& color) const;
            MSHColor operator-(const MSHColor& color) const;
            MSHColor operator*(float t) const;

            void operator+=(const MSHColor& color);
            void operator-=(const MSHColor& color);
            void operator*=(float t);

            /**
             * \brief  Convert a LAB color value to MSH color value
             * \param lab the color to convert
             */
            void setFromLAB(const LABColor& lab);

            /**
             * \brief  Convert a XYZ color value to MSH color value
             * \param xyz the color to convert
             */
            void setFromXYZ(const XYZColor& xyz);

            /**
             * \brief  Convert a RGB color value to MSH color value
             * \param rgb the color to convert
             */
            void setFromRGB(const Color&    rgb);

            /**
             * \brief  Convert this object to a LAB colorspace value
             * \return   the LABColor corresponding to this object
             */
            LABColor toLAB() const;

            /**
             * \brief  Convert this object to a XYZ colorspace value
             * \return   the XYZColor corresponding to this object
             */
            XYZColor toXYZ() const;

            /**
             * \brief  Convert this object to a RGB colorspace value
             * \return   the RGBColor corresponding to this object
             */
            Color    toRGB() const;


            /**
             * \brief  Interpolate from c1 to c2 at step interp (between 0 and 1)
             *
             * \param c1 the left color (interp = 0)
             * \param c2 the right color (interp = 1)
             * \param interp the step
             *
             * \return  the color once interpolated. The medium color is usually white
             */
            MSHColor fromColorInterpolation(const Color& c1, const Color& c2, float interp) const;

            /**
             * \brief  Adjust the hue
             * \param sat The saterated color
             * \param m The unsaturated M component
             * \return  the hue adjusted 
             */
            float adjustHue(const MSHColor& sat, float m) const;
            
            float m; /*!< The M component*/
            float s; /*!< The S component*/
            float h; /*!< The H component*/
            float a; /*!< The alpha component*/
    };
}
#endif
