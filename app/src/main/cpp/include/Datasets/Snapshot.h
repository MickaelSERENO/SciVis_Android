#ifndef  SNAPSHOT_INC
#define  SNAPSHOT_INC

#include <cstdint>

namespace sereno
{
    /** \brief  The Snapshot structure containing snapshot data */
    struct Snapshot
    {
        uint32_t  width;  /*!< The snapshot width*/
        uint32_t  height; /*!< The snapshot height*/
        uint32_t* pixels; /*!< The snapshot RGBA8888 pixels.*/

        /* \brief  Constructor
         * \param w the snapshot width
         * \param h the snapshot height
         * \param p the snapshot pixels. This object will own the pixels array (and will free it)*/
        Snapshot(uint32_t w, uint32_t h, uint32_t* p) : width(w), height(h), pixels(p){}

        ~Snapshot()
        {
            if(pixels)
                free(pixels);
        }
    };
}

#endif
