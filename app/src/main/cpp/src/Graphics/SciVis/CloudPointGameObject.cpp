#include "Graphics/SciVis/CloudPointGameObject.h"

namespace sereno
{

    CloudPointGameObject::CloudPointGameObject(GameObject* parent, GLRenderer* renderer, Material* mtl, std::shared_ptr<CloudPointDataset> cloudPoint, SubDataset* sd) :
        SciVis(parent, renderer, mtl, sd),
        m_dataset(cloudPoint), m_model(sd)
    {
        glGenVertexArrays(1, &m_vaoID);
        glBindVertexArray(m_vaoID);
            glGenBuffers(1, &m_vboID);
            glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
                //Generate the buffer
                //3 position channel (x, y, z), 4 color channel (RGBA)
                glBufferData(GL_ARRAY_BUFFER, sizeof(float)*(3+4)*cloudPoint->getNbPoints(), NULL, GL_DYNAMIC_DRAW);

                //Configurate the VAO
                //Position
                glVertexAttribPointer(MATERIAL_VPOSITION, 3, GL_FLOAT, 0, 0, (void*)(0));
                glEnableVertexAttribArray(MATERIAL_VPOSITION);
                //Color
                glVertexAttribPointer(MATERIAL_VCOLOR, 4, GL_FLOAT, 0, 0, (void*)(3*sizeof(float)*cloudPoint->getNbPoints()));
                glEnableVertexAttribArray(MATERIAL_VCOLOR);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    CloudPointGameObject::~CloudPointGameObject()
    {
        glDeleteVertexArrays(1, &m_vaoID);
        glDeleteBuffers(1, &m_vboID);
    }

    void CloudPointGameObject::draw(const Render& render)
    {
        //TODO
    }

    void CloudPointGameObject::load()
    {
        onTFChanged();
    }

    void CloudPointGameObject::onTFChanged()
    {
        //TODO
    }
}
