#include "common.h"

void vis::common::PrintShaderLog(GLuint shader)
{
    int len = 0;
    int ch_written = 0;
    char *log;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
    if (len > 0)
    {
        log = (char *)malloc(len);
        glGetShaderInfoLog(shader, len, &ch_written, log);
        // std::cout << "Shader Info Log: " << log << std::endl;
        std::cout << "Shader Info Log: " << log << std::endl;
        free(log);
    }
}

void vis::common::PrintProgramLog(int prog)
{
    int len = 0;
    int ch_written = 0;
    char *log;
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
    if (len > 0)
    {
        log = (char *)malloc(len);
        glGetProgramInfoLog(prog, len, &ch_written, log);
        std::cout << "Program Info Log: " << log << std::endl;
        free(log);
    }
}

bool vis::common::CheckOpenGLError()
{
    bool found_error = false;
    int glErr = glGetError();
    while (glErr != GL_NO_ERROR)
    {
        std::cerr << "glError: " << glErr << std::endl;
        found_error = true;
        glErr = glGetError();
    }
    return found_error;
}

GLuint vis::common::CreateShaderProgram(const char *vert, const char *frag)
{
    GLint vertCompiled;
    GLint fragCompiled;
    GLint linked;

    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vShader, 1, &vert, NULL);
    glShaderSource(fShader, 1, &frag, NULL);

    std::cout << "编译顶点着色器" << std::endl;
    glCompileShader(vShader); 
    CheckOpenGLError();
    glGetShaderiv(vShader, GL_COMPILE_STATUS, &vertCompiled);
    if (vertCompiled != 1)
    {
        std::cerr << "vertex compilation failed" << std::endl;
        PrintShaderLog(vShader);
    }

    std::cout << "编译片段着色器" << std::endl;
    glCompileShader(fShader);
    CheckOpenGLError();
    glGetShaderiv(fShader, GL_COMPILE_STATUS, &fragCompiled);
    if (fragCompiled != 1)
    {
        std::cerr << "fragment compilation failed" << std::endl;
        PrintShaderLog(fShader);
    }

    GLuint vfProgram = glCreateProgram();
    glAttachShader(vfProgram, vShader);
    glAttachShader(vfProgram, fShader);

    glLinkProgram(vfProgram);
    CheckOpenGLError();
    glGetProgramiv(vfProgram, GL_LINK_STATUS, &linked);
    if (linked != 1)
    {
        std::cerr << "linking failed" << std::endl;
        PrintProgramLog(vfProgram);
    }

    glDeleteShader(vShader);
    glDeleteShader(fShader);
    return vfProgram;
}

GLuint vis::common::CompileComputeShader(const char *shaderSource)
{
    GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(shader, 1, &shaderSource, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Compute Shader Compilation Failed:\n"
                  << infoLog << std::endl;
    }
    return shader;
}
