#include "MeshLoader.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

struct Position {
    float x, y, z;
};

Mesh MeshLoader::LoadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filepath);
    }

    // 1. Wczytywanie liczby i wspó³rzêdnych unikalnych pozycji
    int k;
    if (!(file >> k)) {
        throw std::runtime_error("Cannot read number of positions from file: " + filepath);
    }

    std::vector<Position> positions(k);
    for (int i = 0; i < k; ++i) {
        file >> positions[i].x >> positions[i].y >> positions[i].z;
    }

    // 2. Wczytywanie wierzcho³ków z informacjami o normalnych
    int l;
    file >> l;

    std::vector<float> finalVertices;
    finalVertices.reserve(l * 6);

    for (int i = 0; i < l; ++i) {
        int posIndex;
        float nx, ny, nz;
        file >> posIndex >> nx >> ny >> nz;

        // Pobieranie x,y,z
        Position pos = positions[posIndex];

        finalVertices.push_back(pos.x);
        finalVertices.push_back(pos.y);
        finalVertices.push_back(pos.z);
        finalVertices.push_back(nx);
        finalVertices.push_back(ny);
        finalVertices.push_back(nz);
    }

    // 3. Wczytywanie trójk¹tów
    int m;
    file >> m;

    std::vector<unsigned int> finalIndices;
    finalIndices.reserve(m * 3);

    for (int i = 0; i < m; ++i) {
        unsigned int i1, i2, i3;
        file >> i1 >> i2 >> i3;
        finalIndices.push_back(i1);
        finalIndices.push_back(i2);
        finalIndices.push_back(i3);
    }

    // 4. Wczytywanie krawêdzi
    int n;
    std::vector<Edge> finalEdges;
    if (file >> n) {
        finalEdges.reserve(n);
        for (int i = 0; i < n; ++i) {
            int e1, e2, t1, t2;
            file >> e1 >> e2 >> t1 >> t2;
            finalEdges.push_back({ static_cast<unsigned int>(e1), static_cast<unsigned int>(e2), t1, t2 });
        }
    }

    file.close();

    return Mesh(finalVertices, finalIndices, finalEdges);
}
