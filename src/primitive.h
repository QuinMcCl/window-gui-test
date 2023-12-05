#ifndef PRIMITIVE_H
#define PRIMITIVE_H
#include <vector>
#include "shader.h"
#include "types.h"

enum primitive_type
{
    TRIANGLE,
    SQUARE,
    CUBE
};



class Primitive
{
public:
    Primitive(primitive_type type);
    ~Primitive();
    void draw();
private:
    std::vector<Vertex> mVertices;
    std::vector<unsigned int> mIndeces;
    unsigned int VAO, VBO, EBO;

};

#endif