#pragma once
#include "Mesh.h"
#include <string>

class MeshLoader {
public:
    static Mesh LoadFromFile(const std::string& filepath);
};
