#ifndef  CIRCLE_INC
#define  CIRCLE_INC

#include "Graphics/Shapes/Geometry.h"
#include <cmath>

namespace sereno
{
    class Circle : public Geometry
    {
        public:
            /* \brief Create a circle
             * \param nbEdges the number of edges for this circle */
            Circle(uint32_t nbEdges);
    };
}

#endif
