#ifndef  EVENT_INC
#define  EVENT_INC

namespace sereno
{
    /* \brief The EventType sendable to Android*/
    enum EventType
    {
        RESIZE,     /*!< Resize event*/
        KEYDOWN,    /*!< Keydown event*/
        KEYUP,      /*!< Keyup event*/
        TOUCH_MOVE, /*!< Touch move event*/
        TOUCH_DOWN, /*!< Touch down event */
        TOUCH_UP    /*!< Touch up event */
    };

    /* \brief Describes a touch event */
    struct TouchEvent
    {
        uint32_t id; /*!< The touch ID */
        float    x;  /*!< x position */
        float    y;  /*!< y position */
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

    /* \brief Describes an event */
    struct Event
    {
        union
        {
            KeyEvent   keyEvent;   /*!< A key event */
            TouchEvent touchEvent; /*!< A touch event*/
            SizeEvent  sizeEvent;  /*!< A resize event*/
        };

        EventType type; /*!< The type of the event*/
    };
}

#endif
