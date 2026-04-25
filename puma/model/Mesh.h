#pragma once
#include <glad/glad.h>
#include <vector>

class Mesh {
public:
    Mesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
    ~Mesh();

    // Zablokowanie kopiowania
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    // Przenoszenie
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    void Draw() const;

private:
    unsigned int VAO, VBO, EBO;
    unsigned int indicesCount;

    void setupMesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
};
