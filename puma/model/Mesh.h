#pragma once
#include <glad/glad.h>
#include <vector>

struct Edge {
    unsigned int e1;
    unsigned int e2;
    int t1;
    int t2;
};

class Mesh {
public:
    Mesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices, const std::vector<Edge>& edges = {});
    ~Mesh();

    // Zablokowanie kopiowania
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    // Przenoszenie
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    void Draw() const;

    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    std::vector<Edge> edges;
    std::vector<unsigned int> vertToPos;
private:
    unsigned int VAO, VBO, EBO;
    unsigned int indicesCount;

    void setupMesh();
};
