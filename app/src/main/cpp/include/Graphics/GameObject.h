#ifndef  GAMEOBJECT_INC
#define  GAMEOBJECT_INC

#include "Transformable.h"
#include "Graphics/Materials/Material.h"
#include "Graphics/Drawable.h"
#include <list>

namespace sereno
{
    /* \brief GameObject class. Represent an object that can be drawned on screen with mobile parts and children hierarchy*/
    class GameObject : public Drawable, public Transformable
    {
        public:
            /* \brief Constructor. Initialize this GameObject with a default material
             * \param parent the parent of this GameObject (can be NULL)
             * \param renderer the OpenGL context object
             * \param mtl the material to use. */
            GameObject(GameObject* parent, GLRenderer* renderer, Material* mtl);

            /* \brief Virtual destructor. Does nothing yet. */
            virtual ~GameObject();

            /* \brief Set the GameObject parent of this object
             * \param parent the new parent. Can be NULL (no parent)
             * \param pos the new object position in the parent hierarchy list */
            virtual void setParent(GameObject* parent, int pos=-1);

            /* \brief Add a child to this GameObject
             * \param child the new child to add. Cannot be NULL
             * \param pos the child position */
            virtual void addChild(GameObject* child, int pos=-1);

            /* \brief Remove child from the children list of this GameObject
             * \param child the child to remove
             * \return true if found, false otherwise */
            virtual bool removeChild(GameObject *child);

            /* \brief Delete all children of this GameObject */
            virtual void clearChild();

            /* \brief Tells wheter or not child is a child of this GameObject object
             * \param child the object to test the affiliation
             * \param true if child is a child of this object, false otherwise */
            virtual bool isChild(GameObject* child);

            /* \brief Update the gameObject BEFORE drawing it. Call onUpdate before calling update method for each children
             * Every gameObject are updated before any draw attempts
             * \param render the render to use to render this GameObject if needed. See Render::addToDraw*/
            virtual void update(Render* render);

            /* \brief Function called when the object is being updated
             * \parma render the render to use */
            virtual void onUpdate(Render* render);

            /* \brief Get the parent associated to this GameObject
             * \return the parent of this gameObject. NULL is no parent */
            GameObject* getParent() {return m_parent;}
        protected:
            GameObject*            m_parent    = NULL; /*!< The parent GameObject*/
            std::list<GameObject*> m_children;         /*!< The children of this GameObject*/
            bool                   m_isVisible = true; /*!< Is this gameObject visible ?*/
    };
}

#endif
