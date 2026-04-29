#include "PumaRobot.h"
#include "../model/MeshLoader.h"
#include <iostream>
#include <string>

PumaRobot::PumaRobot() {
    try {
        // 1. £adowanie siatek z plików
        for (int i = 1; i <= 6; ++i) {
            std::string path = "resources/mesh" + std::to_string(i) + ".txt";
            auto mesh = std::make_shared<Mesh>(MeshLoader::LoadFromFile(path));
            m_parts.push_back(std::make_shared<RobotPart>(mesh));
        }

        m_root = m_parts[0];

        for (size_t i = 0; i < m_parts.size() - 1; ++i) {
            m_parts[i]->AddChild(m_parts[i + 1]);
        }

        m_parts[1]->SetRotationAxis(glm::vec3(0.0f, 1.0f, 0.0f));
        m_parts[1]->SetPivotPoint(glm::vec3(0.0f, 0.0f, 0.0f));

        m_parts[2]->SetRotationAxis(glm::vec3(0.0f, 0.0f, 1.0f));
        m_parts[2]->SetPivotPoint(glm::vec3(0.0f, 0.27f, 0.0f));

        m_parts[3]->SetRotationAxis(glm::vec3(0.0f, 0.0f, 1.0f));
        m_parts[3]->SetPivotPoint(glm::vec3(-0.91f, 0.27f, 0.0f));

        m_parts[4]->SetRotationAxis(glm::vec3(1.0f, 0.0f, 0.0f));
        m_parts[4]->SetPivotPoint(glm::vec3(0.0f, 0.27f, -0.26f));

        m_parts[5]->SetRotationAxis(glm::vec3(0.0f, 0.0f, 1.0f));
        m_parts[5]->SetPivotPoint(glm::vec3(-1.72f, 0.27f, 0.0f));

    }
    catch (const std::exception& e) {
        std::cerr << "Error loading robot part: " << e.what() << "\n";
    }
}

void PumaRobot::Draw(SceneShader& shader, const glm::mat4& baseTransform) {
    if (m_root) {
        m_root->Draw(shader, baseTransform);
    }
}

void PumaRobot::DrawShadow(DepthShader& shader, const glm::mat4& baseTransform) {
    if (m_root) {
        m_root->DrawShadow(shader, baseTransform);
    }
}

void PumaRobot::SetJointAngle(int jointIndex, float angle) {
    if (jointIndex >= 0 && jointIndex < m_parts.size()) {
        m_parts[jointIndex]->SetJointAngle(angle);
    }
}

float PumaRobot::GetJointAngle(int jointIndex) const {
    if (jointIndex >= 0 && jointIndex < m_parts.size()) {
        return m_parts[jointIndex]->GetJointAngle();
    }
    return 0.0f;
}

void PumaRobot::ApplyInverseKinematics(glm::vec3 pos, glm::vec3 normal)
{
    float a1, a2, a3, a4, a5;
    float l1 = 0.91f, l2 = 0.81f, l3 = 0.33f, dy = 0.27f, dz = 0.26f;

    normal = glm::normalize(normal);
    glm::vec3 pos1 = pos + normal * l3;
    float e = sqrtf(pos1.z * pos1.z + pos1.x * pos1.x - dz * dz);

    a1 = atan2(pos1.z, -pos1.x) + atan2(dz, e);

    glm::vec3 pos2(e, pos1.y - dy, 0.0f);

    float clampVal = std::min(1.0f, std::max(-1.0f, (pos2.x * pos2.x + pos2.y * pos2.y - l1 * l1 - l2 * l2) / (2.0f * l1 * l2)));
    a3 = -acosf(clampVal);

    float k = l1 + l2 * cosf(a3);
    float l = l2 * sinf(a3);
    a2 = -atan2(pos2.y, sqrtf(pos2.x * pos2.x + pos2.z * pos2.z)) - atan2(l, k);

    glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), -a1, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 normal1 = glm::vec3(rotY * glm::vec4(normal, 0.0f));

    glm::mat4 rotZ = glm::rotate(glm::mat4(1.0f), -(a2 + a3), glm::vec3(0.0f, 0.0f, 1.0f));
    normal1 = glm::vec3(rotZ * glm::vec4(normal1, 0.0f));

    a5 = acosf(normal1.x);
    a4 = atan2(normal1.z, normal1.y);

    SetJointAngle(1, glm::degrees(a1));
    SetJointAngle(2, glm::degrees(a2));
    SetJointAngle(3, glm::degrees(a3));
    SetJointAngle(4, glm::degrees(a4));
    SetJointAngle(5, glm::degrees(a5));
}