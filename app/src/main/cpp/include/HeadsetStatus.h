#ifndef  HEADSETSTATUS_INC
#define  HEADSETSTATUS_INC

#include <cstdint>
#include <glm/glm.hpp>
#include "Quaternion.h"

namespace sereno
{
    enum HeadsetCurrentAction
    {
        HEADSET_CURRENT_ACTION_NOTHING   = 0,
        HEADSET_CURRENT_ACTION_MOVING    = 1,
        HEADSET_CURRENT_ACTION_SCALING   = 2,
        HEADSET_CURRENT_ACTION_ROTATING  = 3,
        HEADSET_CURRENT_ACTION_SKETCHING = 4
    };

    /** \brief  Headset status parameters */
    struct HeadsetStatus
    {
        int32_t              id;            /*!< Headset ID*/
        int32_t              color;         /*!< Headset Color*/
        HeadsetCurrentAction currentAction; /*!< Headset current action status (moving, scaling, etc.)*/
        glm::vec3            position;      /*!< Headset 3D position*/
        Quaternionf          rotation;      /*!< Headset 3D rotation*/
    };
}

#endif
