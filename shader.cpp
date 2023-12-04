#include "shader.h"

char *read_buff(const char *FilePath, long *length)
{
    FILE *fp;
    long len = 0;
    char *buffer = NULL;

    fp = fopen(FilePath, "r");
    if (fp)
    {
        fseek(fp, 0, SEEK_END);
        len = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        if (len > 0)
        {
            buffer = new char[len + 1l];
        }
        if (buffer != NULL)
        {
            const size_t ret_code = fread(buffer, sizeof(char), len, fp);
            if (ret_code != len)
            {
                if (feof(fp))
                    fprintf(stderr, "Error reading test.bin: unexpected end of file\n");
                else if (ferror(fp))
                    fprintf(stderr, "Error reading test.bin");
            }
            buffer[len] = 0;
        }
        fclose(fp);
    }
    *length = len;
    return buffer;
}

Shader::Shader(const char *vP, const char *fP)
{
    vertexPath = vP;
    fragmentPath = fP;
}

Shader::~Shader()
{
}

int Shader::read_and_compile()
{
    unsigned int vertexShader, fragmentShader;
    int success;
    char infoLog[512];
    char *buffer = NULL;
    long length = 0;

    // build and compile our shader program
    // ------------------------------------

    buffer = read_buff(vertexPath, &length);
    if (length <= 0)
    {
        fprintf(stderr, "ERROR Could not read Vertex Shader file\n");
        return GL_FALSE;
    }

    // vertex shader
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &buffer, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        fprintf(stderr, "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s", infoLog);
        return success;
    }
    delete[] buffer;
    buffer = NULL;

    buffer = read_buff(fragmentPath, &length);
    if (length <= 0)
    {
        fprintf(stderr, "ERROR Could not read Fragment Shader file\n");
        return GL_FALSE;
    }

    // fragment shader
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &buffer, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        fprintf(stderr, "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s", infoLog);
        return success;
    }
    delete[] buffer;
    buffer = NULL;

    // link shaders
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (success != GL_TRUE)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        fprintf(stderr, "ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s", infoLog);
        return success;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return GL_TRUE;
}

void Shader::use()
{
    glUseProgram(shaderProgram);
}

unsigned int Shader::get_ID()
{
    return shaderProgram;
}

// utility uniform functions
// ------------------------------------------------------------------------

// utility uniform functions
// ------------------------------------------------------------------------
void Shader::setInt(const char *name, int value) const
{
    glUniform1i(glGetUniformLocation(shaderProgram, name), value);
}
// ------------------------------------------------------------------------
void Shader::setFloat(const char *name, float value) const
{
    glUniform1f(glGetUniformLocation(shaderProgram, name), value);
}
void Shader::setVec2(const char *name, float x, float y) const
{
    glUniform2f(glGetUniformLocation(shaderProgram, name), x, y);
}
void Shader::setVec3(const char *name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(shaderProgram, name), x, y, z);
}
void Shader::setVec4(const char *name, float x, float y, float z, float w)
{
    glUniform4f(glGetUniformLocation(shaderProgram, name), x, y, z, w);
}
// ------------------------------------------------------------------------
void Shader::setVec2(const char *name, const glm::vec2 &value) const
{
    glUniform2fv(glGetUniformLocation(shaderProgram, name), 1, &value[0]);
}
void Shader::setVec3(const char *name, const glm::vec3 &value) const
{
    glUniform3fv(glGetUniformLocation(shaderProgram, name), 1, &value[0]);
}
void Shader::setVec4(const char *name, const glm::vec4 &value) const
{
    glUniform4fv(glGetUniformLocation(shaderProgram, name), 1, &value[0]);
}
// ------------------------------------------------------------------------
void Shader::setMat2(const char *name, const glm::mat2 &mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(shaderProgram, name), 1, GL_FALSE, &mat[0][0]);
}
void Shader::setMat3(const char *name, const glm::mat3 &mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(shaderProgram, name), 1, GL_FALSE, &mat[0][0]);
}
void Shader::setMat4(const char *name, const glm::mat4 &mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name), 1, GL_FALSE, &mat[0][0]);
}