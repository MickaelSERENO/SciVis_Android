#ifndef  CYLINDER_INC
#define  CYLINDER_INC

#include "Graphics/Shapes/Geometry.h"
#include <cmath>

namespace sereno
{
    /* \brief Represents a Cylinder WITHOUT THE TOP AND BOTTOM CIRCLES ! */
    class Cylinder : public Geometry
    {
        public:
            /* \brief Constructor
             * \param nbLattitude the number of lattitude. Minimum : 3 */
            Cylinder(uint32_t nbLattitude);
    };
}

#endif
