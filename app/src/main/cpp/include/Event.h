#ifndef  EVENT_INC
#define  EVENT_INC

namespace sereno
{

    /* \brief Type of touch state a finger can be */
    enum TouchType
    {
        TOUCH_TYPE_DOWN = 0, /*!< Down state (touching screen) */
        TOUCH_TYPE_UP   = 1, /*!< Up state   (not touching screen)*/
        TOUCH_TYPE_MOVE = 2  /*!< Move state (moving on the screen)*/
    };

    /** \brief structure which contain the touch information*/
    struct TouchCoord
    {
        TouchType type = TOUCH_TYPE_UP; /*!< The type of the touching*/
        float startX   = 0.0f;          /*!< Where the touch event has started on X axis*/
        float startY   = 0.0f;          /*!< Where the touch event has started on Y axis*/
        float oldX     = 0.0f;          /*!< Old x position*/
        float oldY     = 0.0;           /*!< Old y position*/
        float x        = 0.0f;          /*!< The last x coords*/
        float y        = 0.0f;          /*!< The last y coords*/
    };

    /* \brief The EventType sendable to Android*/
    enum EventType
    {
        UNKNOWN,    /*!< Unknown event*/
        RESIZE,     /*!< Resize event*/
        KEYDOWN,    /*!< Keydown event*/
        KEYUP,      /*!< Keyup event*/
        TOUCH_DOWN, /*!< Touch down event */
        TOUCH_UP,   /*!< Touch up event */
        TOUCH_MOVE, /*!< Touch move event*/
        VISIBILITY, /*!< Visibility event*/
    };

    /* \brief Describes a touch event */
    struct TouchEvent
    {
        uint32_t id; /*!< The touch ID */
        float    x;  /*!< x position */
        float    y;  /*!< y position */
        //If movement : 
        float    startX; /*!< The old X position*/
        float    startY; /*!< The old Y position*/
        float    oldX;   /*!< When does the movement initiated ? (X axis)*/
        float    oldY;   /*!< When does the movement initiated ? (Y axis)*/
    };

    /* \brief Describes a key event */
    struct KeyEvent
    {
        uint32_t keyCode; /*!< The key code */
    };

    /* \brief Describes an event relative to the size (like a resize) */
    struct SizeEvent
    {
        uint32_t width;  /*!< The width of the surface */
        uint32_t height; /*!< The height of the surface*/
    };

    /* \brief Describes a change regarding the surface visibility*/
    struct VisibilityEvent
    {
        bool visibility; /*!< Is the surface visible?*/
    };

    /* \brief Describes an event */
    struct Event
    {
        union
        {
            KeyEvent        keyEvent;   /*!< A key event */
            TouchEvent      touchEvent; /*!< A touch event*/
            SizeEvent       sizeEvent;  /*!< A resize event*/
            VisibilityEvent visibility; /*!< A visibility event*/
        };

        EventType type; /*!< The type of the event*/
    };
}

#endif
