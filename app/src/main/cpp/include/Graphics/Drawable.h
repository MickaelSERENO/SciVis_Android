#ifndef  DRAWABLE_INC
#define  DRAWABLE_INC

#include "Transformable.h"
#include "Graphics/Materials/Material.h"
#include <list>

namespace sereno
{
    class Render;

    /* \brief Drawable class. Represent an object that can be drawned on screen (directly through it or via a combinaison of other Drawables) */
    class Drawable : public Transformable
    {
        public:
            /* \brief Constructor. Initialize this Drawable with a default material
             * \param mtl the material to use. */
            Drawable(Drawable* parent, Material* mtl);

            /* \brief Virtual destructor. Does nothing yet. */
            virtual ~Drawable();

            /* \brief Set the Drawable parent of this object
             * \param parent the new parent. Can be NULL (no parent)
             * \param pos the new object position in the parent hierarchy list */
            virtual void setParent(Drawable* parent, int pos=-1);

            /* \brief Add a child to this Drawable
             * \param child the new child to add. Cannot be NULL
             * \param pos the child position */
            virtual void addChild(Drawable* child, int pos=-1);

            /* \brief Remove child from the children list of this Drawable
             * \param child the child to remove
             * \return true if found, false otherwise */
            virtual bool removeChild(Drawable *child);

            /* \brief Delete all children of this Drawable */
            virtual void clearChild();

            /* \brief Tells wheter or not child is a child of this Drawable object
             * \param child the object to test the affiliation
             * \param true if child is a child of this object, false otherwise */
            virtual bool isChild(Drawable* child);

            /* \brief Update the drawable BEFORE drawing it. Call onUpdate before calling update method for each children
             * Every drawable are updated before any draw attempts
             * \param render the render to use to render this Drawable if needed. See Render::addToDraw*/
            virtual void update(Render* render);

            /* \brief Function called when the object is being updated
             * \parma render the render to use */
            virtual void onUpdate(Render* render);

            /* \brief Draw this drawable on the current framebuffer. 
             * \param cameraMat the current camera matrix*/
            virtual void draw(const glm::mat4& cameraMat);

            /* Function called for post processing drawing. 
             * This is called after EVERY children has been drawned
             * \param cameraMat the camera matrix  */
            virtual void postDraw(const glm::mat4& cameraMat);

            /* \brief Get the parent associated to this Drawable
             * \return the parent of this drawable. NULL is no parent */
            Drawable* getParent() {return m_parent;}
        protected:
            Material*            m_mtl       = NULL; /*!< The material to use*/
            Drawable*            m_parent    = NULL; /*!< The parent Drawable*/
            std::list<Drawable*> m_children;         /*!< The children of this Drawable*/
            bool                 m_isVisible = true; /*!< Is this drawable visible ?*/
    };
}

#endif
