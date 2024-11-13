#include "auxgrid.h"

vis::helper::AuxGrid::AuxGrid()
{
    slices = 100;
    shaderProgram =
        vis::common::CreateShaderProgram(vert, frag);

    glUseProgram(shaderProgram);

    mLoc = glGetUniformLocation(shaderProgram, "model_matrix");
    vLoc = glGetUniformLocation(shaderProgram, "view_matrix");
    projLoc = glGetUniformLocation(shaderProgram, "proj_matrix");
    SetupVertices();

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size() * sizeof(glm::vec3),
        glm::value_ptr(vertices[0]),
        GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(glm::uvec4),
        glm::value_ptr(indices[0]),
        GL_STATIC_DRAW);

    glBindVertexArray(0);

    lenght = (GLuint)indices.size() * 4;
}

void vis::helper::AuxGrid::RenderUi()
{
}

void vis::helper::AuxGrid::Display(const vis::common::TeagineData *teagine_data)
{
    glEnable(GL_DEPTH_TEST);

    glBindVertexArray(vao);

    glUseProgram(shaderProgram);

    // 计算view proj model
    vMat = teagine_data->view;
    pMat = teagine_data->proj;
    mMat = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

    SetupParameters();

    glDrawElements(GL_LINES, lenght, GL_UNSIGNED_INT, NULL);

    glBindVertexArray(0);

    glUseProgram(0);

    glDisable(GL_DEPTH_TEST);
}

void vis::helper::AuxGrid::SetupVertices(void)
{
    for (int j = -slices / 2; j <= slices / 2; ++j)
    {
        for (int i = -slices / 2; i <= slices / 2; ++i)
        {
            float x = (float)i;
            float y = 0;
            float z = (float)j;
            vertices.push_back(glm::vec3(x, y, z));
        }
    }

    for (int j = 0; j < slices; ++j)
    {
        for (int i = 0; i < slices; ++i)
        {
            int row1 = j * (slices + 1);
            int row2 = (j + 1) * (slices + 1);

            indices.push_back(glm::uvec4(row1 + i, row1 + i + 1, row1 + i + 1, row2 + i + 1));
            indices.push_back(glm::uvec4(row2 + i + 1, row2 + i, row2 + i, row1 + i));
        }
    }
}

void vis::helper::AuxGrid::SetupParameters(void)
{

    glUniformMatrix4fv(mLoc, 1, GL_FALSE, glm::value_ptr(mMat));
    glUniformMatrix4fv(vLoc, 1, GL_FALSE, glm::value_ptr(vMat));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
}
