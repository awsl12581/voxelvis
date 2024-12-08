#ifndef _TEA_INCLUDED_AUXGRID_H_
#define _TEA_INCLUDED_AUXGRID_H_

#include "common.h"
#include <vector>

namespace vis
{

    namespace helper
    {

        class AuxGrid
        {
        public:
            AuxGrid();
            void RenderUi();

            void Display(const vis::common::TeagineData *teagine_data);

        private:
            void SetupVertices(void);
            void SetupParameters(void);

        private:
            std::vector<glm::vec3> vertices;
            std::vector<glm::uvec3> indices;
            GLuint vao;
            GLuint vbo;
            GLuint ibo;
            GLsizei lenght;
            int slices;

            GLuint shaderProgram;
            GLuint mLoc, vLoc, projLoc;
            glm::mat4 pMat, vMat, mMat;
            const char *vert = "#version 450 \n"
                               "layout (location=0) in vec3 position; \n"
                               "uniform mat4 model_matrix;\n"
                               "uniform mat4 view_matrix; \n"
                               "uniform mat4 proj_matrix; \n"
                               "void main(void) \n"
                               "{gl_Position = proj_matrix * view_matrix * model_matrix * vec4(position,1.0);}\n";
            const char *frag = "#version 450 \n"
                               "out vec4 color; \n"
                               "void main(void) \n"
                               "{color = vec4(255.0, 255.0, 255.0, 1.0);}\n";
        };

    }
}

#endif