#pragma once
#include <vector>

class Cylinder {
public:
    int sectors = 36;
    float cylRadius = 0.5f;
    float cylHeight = 5.0f;
    unsigned int cylVAO, cylVBO;
    std::vector<float> cylVerts;

    Cylinder()
    {
        for (int i = 0; i < sectors; ++i) {
            float a1 = (float)i / sectors * 2.0f * (float)M_PI;
            float a2 = (float)(i + 1) / sectors * 2.0f * (float)M_PI;

            float nx1 = cos(a1), nz1 = sin(a1);
            float nx2 = cos(a2), nz2 = sin(a2);

            float x1 = cylRadius * nx1, z1 = cylRadius * nz1;
            float x2 = cylRadius * nx2, z2 = cylRadius * nz2;

            // Bok walca
            cylVerts.insert(cylVerts.end(), { x1, -cylHeight / 2, z1, nx1, 0, nz1, 0, 0 });
            cylVerts.insert(cylVerts.end(), { x2, -cylHeight / 2, z2, nx2, 0, nz2, 1, 0 });
            cylVerts.insert(cylVerts.end(), { x1,  cylHeight / 2, z1, nx1, 0, nz1, 0, 1 });

            cylVerts.insert(cylVerts.end(), { x1,  cylHeight / 2, z1, nx1, 0, nz1, 0, 1 });
            cylVerts.insert(cylVerts.end(), { x2, -cylHeight / 2, z2, nx2, 0, nz2, 1, 0 });
            cylVerts.insert(cylVerts.end(), { x2,  cylHeight / 2, z2, nx2, 0, nz2, 1, 1 });

            // Górna podstawa
            cylVerts.insert(cylVerts.end(), { 0,  cylHeight / 2, 0,  0, 1, 0, 0.5f, 0.5f });
            cylVerts.insert(cylVerts.end(), { x1, cylHeight / 2, z1, 0, 1, 0, 0.0f, 0.0f });
            cylVerts.insert(cylVerts.end(), { x2, cylHeight / 2, z2, 0, 1, 0, 1.0f, 0.0f });

            // Dolna podstawa
            cylVerts.insert(cylVerts.end(), { 0, -cylHeight / 2, 0,  0, -1, 0, 0.5f, 0.5f });
            cylVerts.insert(cylVerts.end(), { x2, -cylHeight / 2, z2, 0, -1, 0, 1.0f, 0.0f });
            cylVerts.insert(cylVerts.end(), { x1, -cylHeight / 2, z1, 0, -1, 0, 0.0f, 0.0f });
        }
    }
};