#pragma once
#include "RobotPart.h"
#include "../SceneShader.h"
#include <vector>
#include <memory>

class PumaRobot {
public:
    PumaRobot();

    void Draw(SceneShader& shader, const glm::mat4& baseTransform = glm::mat4(1.0f));

    void SetJointAngle(int jointIndex, float angle);
	float GetJointAngle(int jointIndex) const;
    void ApplyInverseKinematics(glm::vec3 pos, glm::vec3 normal);

    bool isAnimating = false;

private:
    std::shared_ptr<RobotPart> m_root;
    std::vector<std::shared_ptr<RobotPart>> m_parts;
};