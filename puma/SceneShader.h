#pragma once
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "view/Camera.h"

class SceneShader {
	GLuint shader;
	Camera* camera;
	glm::mat4& P;
	glm::vec3 lightPosTop;
	glm::vec3 lightPosLeft;

	const char* vertexSrc = R"(
		#version 330 core

		layout(location = 0) in vec3 aPos;
		layout(location = 1) in vec3 aNormal;

		uniform mat4 MVP;
		uniform mat4 model;
		uniform mat4 lightSpaceMatrix;

		out vec3 FragPos;
		out vec3 Normal;
		out vec4 fragPosLightSpace; 

		void main()
		{
			FragPos = vec3(model * vec4(aPos, 1.0));
			Normal = mat3(transpose(inverse(model))) * aNormal;
			fragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);

			gl_Position = MVP * vec4(aPos, 1.0);
		}
	)";


	const char* fragmentSrc = R"(
		#version 330 core

		out vec4 FragColor;

		in vec3 FragPos;
		in vec3 Normal;
		in vec4 fragPosLightSpace; 

		uniform vec3 lightPosTop;
		uniform vec3 lightPosLeft;
		uniform vec3 viewPos;
		uniform vec3 objectColor;
		uniform float alphaValue;
		uniform float specStrength;
		uniform int shininess;
		uniform bool receiveShadows;
		uniform bool ambientOnly;
		uniform sampler2D shadowMap;

		float ShadowCalc(vec4 fragPosLightSpace) {
			vec3 proj = fragPosLightSpace.xyz / fragPosLightSpace.w;
			proj = proj * 0.5 + 0.5;
    
			if (proj.z > 1.0) return 0.0;

			float currentDepth = proj.z;
			float bias = max(0.05 * (1.0 - dot(normalize(Normal), normalize(lightPosTop - FragPos))), 0.005);
    
			float shadow = 0.0;
			vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    
			// Pętla wokół aktualnego piksela
			for(int x = -2; x <= 2; ++x) {
				for(int y = -2; y <= 2; ++y) {
					float pcfDepth = texture(shadowMap, proj.xy + vec2(x, y) * texelSize).r; 
					shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
				}    
			}
    
			return shadow / 25; 
		}

		void main()
		{
			// ambient
			float ambientStrength = 0.3;
			vec3 ambient = ambientStrength * vec3(1.0);

			if (ambientOnly) {
				FragColor = vec4(ambient * objectColor, 1.0);
				return;
			}

			vec3 norm = normalize(Normal);
			vec3 viewDir = normalize(viewPos - FragPos);

			// light top
			vec3 lightDirTop = normalize(lightPosTop - FragPos);
			float diffTop = max(dot(norm, lightDirTop), 0.0);
			vec3 diffuseTop = diffTop * vec3(1.0);
			vec3 reflectDirTop = reflect(-lightDirTop, norm);
			float specTop = pow(max(dot(viewDir, reflectDirTop), 0.0), float(shininess));
			vec3 specularTop = specStrength * specTop * vec3(1.0);

			// light left
			vec3 lightDirLeft = normalize(lightPosLeft - FragPos);
			float diffLeft = max(dot(norm, lightDirLeft), 0.0);
			vec3 diffuseLeft = diffLeft * vec3(1.0);
			vec3 reflectDirLeft = reflect(-lightDirLeft, norm);
			float specLeft = pow(max(dot(viewDir, reflectDirLeft), 0.0), float(shininess));
			vec3 specularLeft = specStrength * specLeft * vec3(1.0);

			vec3 diffuse = (diffuseTop + diffuseLeft) / 2;
			vec3 specular = (specularTop + specularLeft) / 2;
			
			float shadow = 0.0;
			if (receiveShadows) {
				shadow = ShadowCalc(fragPosLightSpace);
			}

			vec3 color = (ambient + (1.0 - shadow) * (diffuse + specular)) * objectColor;
			FragColor = vec4(color, alphaValue);
		}
	)";

	GLuint compileShader(GLenum type, const char* src);

	GLuint createProgram();

	bool ambientOnly = false;

public:
	SceneShader(glm::mat4& P, Camera* camera);

	void Use();
	void SetModelMatrix(const glm::mat4& model);
	void SetMaterial(const glm::vec3& color, float specStrength, int shininess, float alpha = 1.0f) const;
	void SetAmbientOnly(bool isAmbient);
	void SetLightSpaceMatrix(const glm::mat4& lightSpaceMtx) const;
	void SetShadowMap() const;
	void SetReceiveShadows(bool receive) const;
};

