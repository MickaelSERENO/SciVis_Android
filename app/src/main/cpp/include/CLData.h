#ifndef  CLDATA_INC
#define  CLDATA_INC

#include "CL/cl.h"

/** \brief  The OpenCL data class */
class CLData
{
    public:
        /** \brief  Constructor, initialize the opencl context */
        CLData();

        /** \brief  Destructor, destroy the opencl context */
        ~CLData();
    private:
        cl_context      m_ctx;         /*!< OpenCL context */
        cl_device_id*   m_deviceIDs;   /*!< The OpenCL device IDs available*/
        cl_platform_id* m_platformIDs; /*!< The OpenCL platform IDs available*/
        int             m_platformID;  /*!< The platform chosen*/
};

#endif
