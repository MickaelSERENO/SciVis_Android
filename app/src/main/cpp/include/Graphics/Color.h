#ifndef DEF_COLOR_INCLUDE
#define DEF_COLOR_INCLUDE

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
}

#endif
