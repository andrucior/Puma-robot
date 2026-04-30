#include "RobotPart.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

RobotPart::RobotPart(std::shared_ptr<Mesh> mesh)
	: m_mesh(mesh), m_localTransform(1.0f), m_rotationAxis(0.0f, 1.0f, 0.0f), m_pivotPoint(0.0f), m_jointAngle(0.0f)
{
}

void RobotPart::AddChild(std::shared_ptr<RobotPart> child)
{
    m_children.push_back(child);
}

void RobotPart::SetJointAngle(float angle)
{
    m_jointAngle = angle;
}

float RobotPart::GetJointAngle() const
{
    return m_jointAngle;
}

void RobotPart::SetLocalTransform(const glm::mat4& transform)
{
    m_localTransform = transform;
}

void RobotPart::SetRotationAxis(const glm::vec3& axis)
{
    m_rotationAxis = axis;
}

void RobotPart::SetPivotPoint(const glm::vec3& pivot)
{
    m_pivotPoint = pivot;
}

void RobotPart::Draw(SceneShader& shader, const glm::mat4& parentTransform) const
{
    glm::mat4 currentTransform = parentTransform * m_localTransform;

    currentTransform = glm::translate(currentTransform, m_pivotPoint);
    currentTransform = glm::rotate(currentTransform, glm::radians(m_jointAngle), m_rotationAxis);
    currentTransform = glm::translate(currentTransform, -m_pivotPoint);

    shader.SetModelMatrix(currentTransform);
    if (m_mesh)
    {
        m_mesh->Draw();
    }
    for (const auto& child : m_children)
    {
        child->Draw(shader, currentTransform);
    }
}

void RobotPart::DrawShadow(DepthShader& shader, const glm::mat4& parentTransform) const
{
    glm::mat4 currentTransform = parentTransform * m_localTransform;

    currentTransform = glm::translate(currentTransform, m_pivotPoint);
    currentTransform = glm::rotate(currentTransform, glm::radians(m_jointAngle), m_rotationAxis);
    currentTransform = glm::translate(currentTransform, -m_pivotPoint);

    shader.SetModelMatrix(currentTransform);
    if (m_mesh)
    {
        m_mesh->Draw();
    }
    for (const auto& child : m_children)
    {
        child->DrawShadow(shader, currentTransform);
    }
}
