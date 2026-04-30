#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "robot/PumaRobot.h"

struct Particle {
    glm::vec3 Position, PrevPosition, Velocity;
    glm::vec4 Color;
    float Life;
    float MaxLife;

    Particle() : Position(0.0f), PrevPosition(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f), MaxLife(1.0f) {}
};

class ParticleSystem {
    unsigned int nr_particles = 500;
    unsigned int lastUsedParticle = 0;
    PumaRobot& robot;
    glm::vec3 offset = glm::vec3(1.5f);
    float gravity = -9.81f;

public:
    std::vector<Particle> particles;

    ParticleSystem(PumaRobot& robot) : robot(robot) {
        for (unsigned int i = 0; i < nr_particles; ++i)
            particles.push_back(Particle());
    }

    void Update(float dt) {
        unsigned int nr_new_particles = 2;

        for (unsigned int i = 0; i < nr_new_particles; ++i)
        {
            int unusedParticle = FirstUnusedParticle();
            RespawnParticle(particles[unusedParticle]);
        }

        for (unsigned int i = 0; i < nr_particles; ++i)
        {
            Particle& p = particles[i];
            p.Life -= dt; 
            if (p.Life > 0.0f)
            {
                p.PrevPosition = p.Position;
                p.Velocity.y += gravity * dt;
                p.Position += p.Velocity * dt;
                p.Color.a = (p.Life / p.MaxLife);
            }
        }
    }

    unsigned int FirstUnusedParticle()
    {
        // search from last used particle, this will usually return almost instantly
        for (unsigned int i = lastUsedParticle; i < nr_particles; ++i) {
            if (particles[i].Life <= 0.0f) {
                lastUsedParticle = i;
                return i;
            }
        }
        // otherwise, do a linear search
        for (unsigned int i = 0; i < lastUsedParticle; ++i) {
            if (particles[i].Life <= 0.0f) {
                lastUsedParticle = i;
                return i;
            }
        }
        // override first particle if all others are alive
        lastUsedParticle = 0;
        return 0;
    }

    void RespawnParticle(Particle& particle)
    {
        glm::vec3 effectorPos = robot.GetTipPosition();

        particle.MaxLife = 0.5f + static_cast<float>(rand() % 100) / 200.0f; 
        particle.Life = particle.MaxLife;

        particle.Position = effectorPos;
        particle.PrevPosition = effectorPos;
        particle.Color = glm::vec4(0.5f, 0.1f, 0.05f, 0.5f);

        // Rozrzut w poziomie (X, Z) i silny impuls w górę (Y)
        float rx = ((rand() % 100) - 50) / 50.0f; // -1 do 1
        float rz = ((rand() % 100) - 50) / 50.0f; // -1 do 1
        float ry = ((rand() % 100) / 100.0f); // 0.0 do 1.0 

        float speed = 2.0f + (rand() % 100 / 50.0f);
        particle.Velocity = glm::vec3(rx * 0.5f, ry, rz * 0.5f) * speed;
    }
};