#include "primitive.h"

#include "types.h"

std::vector<Vertex> triangle_vertices = {
    Vertex{{-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f}},
    Vertex{{1.0f, -1.0f, -1.0f}, {1.0f, 0.0f}},
    Vertex{{1.0f, 1.0f, -1.0f}, {0.0f, 1.0f}},
};
std::vector<unsigned int> triangle_indeces = {
    0,
    1,
    2,
};

std::vector<Vertex> square_vertices = {
    Vertex{{-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f}},
    Vertex{{1.0f, -1.0f, -1.0f}, {1.0f, 0.0f}},
    Vertex{{1.0f, 1.0f, -1.0f}, {1.0f, 1.0f}},
    Vertex{{-1.0f, 1.0f, -1.0f}, {0.0f, 1.0f}},
};
std::vector<unsigned int> square_indeces = {
    0,
    1,
    2,
    2,
    3,
    1,
};

std::vector<Vertex> cube_vertices = {
    Vertex{{-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f}},
    Vertex{{1.0f, -1.0f, -1.0f}, {1.0f, 0.0f}},
    Vertex{{1.0f, 1.0f, -1.0f}, {1.0f, 1.0f}},
    Vertex{{-1.0f, 1.0f, -1.0f}, {0.0f, 1.0f}},

    Vertex{{1.0f, -1.0f, 1.0f}, {0.0f, 0.0f}},
    Vertex{{-1.0f, -1.0f, 1.0f}, {1.0f, 0.0f}},
    Vertex{{-1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
    Vertex{{1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

    Vertex{{1.0f, -1.0f, 1.0f}, {1.0f, 1.0f}},
    Vertex{{-1.0f, -1.0f, 1.0f}, {0.0f, 1.0f}},
    Vertex{{1.0f, 1.0f, -1.0f}, {0.0f, 0.0f}},
    Vertex{{1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},

};
std::vector<unsigned int> cube_indeces = {
    0, 3, 2,
    2, 1, 0,
    4, 7, 6,
    6, 5, 4,
    0, 5, 6,
    6, 3, 0,
    4, 1, 2,
    2, 7, 4,
    0, 1, 8,
    8, 9, 0,
    10, 3, 6,
    6, 11, 10,
};

Primitive::Primitive(primitive_type type)
{
    
    switch (type)
    {
    case TRIANGLE:
        mVertices = triangle_vertices;
        mIndeces = triangle_indeces;
        break;
    case SQUARE:
        mVertices = square_vertices;
        mIndeces = square_indeces;
        break;
    case CUBE:
        mVertices = cube_vertices;
        mIndeces = cube_indeces;
        break;

    default:
        break;
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), &mVertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndeces.size() * sizeof(unsigned int), &mIndeces[0], GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Position));
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, TexCoords));
    glEnableVertexAttribArray(1);
}

Primitive::~Primitive(){

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

}

void Primitive::draw(Shader shader){
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, mIndeces.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}