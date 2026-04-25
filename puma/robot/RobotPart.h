#pragma once
#include "../model/Mesh.h"
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include "../RobotShader.h" 

class RobotPart {
public:
    RobotPart(std::shared_ptr<Mesh> mesh);

    void AddChild(std::shared_ptr<RobotPart> child);

    void SetJointAngle(float angle);
    float GetJointAngle() const;
    void SetLocalTransform(const glm::mat4& transform);
    void SetRotationAxis(const glm::vec3& axis);
    void SetPivotPoint(const glm::vec3& pivot);

    void Draw(RobotShader& shader, const glm::mat4& parentTransform) const;

private:
    std::shared_ptr<Mesh> m_mesh;
    std::vector<std::shared_ptr<RobotPart>> m_children;

    glm::mat4 m_localTransform; // Przesuniêcie i pocz¹tkowy obrót stawu wzglêdem rodzica
    glm::vec3 m_rotationAxis;   // Oœ obrotu
    glm::vec3 m_pivotPoint;     // Punkt obrotu
    float m_jointAngle;         // Aktualny k¹t obrotu w stopniach 
};
