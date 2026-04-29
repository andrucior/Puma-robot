#include "Mesh.h"
#include "Mesh.h"
#include <utility>
#include <map>
#include <algorithm>

Mesh::Mesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices, const std::vector<Edge>& edges)
    : VAO(0), VBO(0), EBO(0), indicesCount(0), vertices(vertices), indices(indices), edges(edges) {
    setupMesh();
}

Mesh::~Mesh() {
    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }
}

Mesh::Mesh(Mesh&& other) noexcept
    : VAO(other.VAO), VBO(other.VBO), EBO(other.EBO), indicesCount(other.indicesCount),
      vertices(std::move(other.vertices)), indices(std::move(other.indices)), edges(std::move(other.edges)) {
    other.VAO = 0;
    other.VBO = 0;
    other.EBO = 0;
    other.indicesCount = 0;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept {
    if (this != &other) {
        if (VAO != 0) {
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);
        }
        VAO = other.VAO;
        VBO = other.VBO;
        EBO = other.EBO;
        indicesCount = other.indicesCount;
        vertices = std::move(other.vertices);
        indices = std::move(other.indices);
        edges = std::move(other.edges);

        other.VAO = 0;
        other.VBO = 0;
        other.EBO = 0;
        other.indicesCount = 0;
    }
    return *this;
}

void Mesh::setupMesh() {
    std::vector<unsigned int> adjIndices;
    adjIndices.reserve((indices.size() / 3) * 6);

    std::map<std::pair<unsigned int, unsigned int>, const Edge*> edgeMap;
    for (const auto& e : edges) {
        unsigned int mn = std::min(e.e1, e.e2);
        unsigned int mx = std::max(e.e1, e.e2);
        edgeMap[{mn, mx}] = &e;
    }

    for (size_t t = 0; t < indices.size() / 3; ++t) {
        unsigned int v0 = indices[t * 3 + 0];
        unsigned int v1 = indices[t * 3 + 1];
        unsigned int v2 = indices[t * 3 + 2];

        auto getOppositeVertex = [&](unsigned int vA, unsigned int vB) -> unsigned int {
            unsigned int mn = std::min(vA, vB);
            unsigned int mx = std::max(vA, vB);
            auto it = edgeMap.find({mn, mx});
            if (it != edgeMap.end()) {
                int neighborT = (it->second->t1 == t) ? it->second->t2 : it->second->t1;
                if (neighborT >= 0 && neighborT < static_cast<int>(indices.size() / 3)) {
                    for (int i = 0; i < 3; ++i) {
                        unsigned int v = indices[neighborT * 3 + i];
                        if (v != vA && v != vB) return v;
                    }
                }
            }
            return vA;
        };

        adjIndices.push_back(v0);
        adjIndices.push_back(getOppositeVertex(v0, v1));
        adjIndices.push_back(v1);
        adjIndices.push_back(getOppositeVertex(v1, v2));
        adjIndices.push_back(v2);
        adjIndices.push_back(getOppositeVertex(v2, v0));
    }

    indicesCount = static_cast<unsigned int>(adjIndices.size());

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, adjIndices.size() * sizeof(unsigned int), adjIndices.data(), GL_STATIC_DRAW);

    // Pozycje
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    // Wektory normalne
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindVertexArray(0);
}

void Mesh::Draw() const {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES_ADJACENCY, indicesCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
