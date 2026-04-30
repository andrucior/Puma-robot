#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "robot/PumaRobot.h"

struct Particle {
    glm::vec3 Position, Velocity;
    glm::vec4 Color;
    float     Life;

    Particle()
        : Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) {}
};

class ParticleSystem {
    unsigned int nr_particles = 20;
    unsigned int lastUsedParticle = 0;
    PumaRobot& robot;
    glm::vec3 offset = glm::vec3(1.5f);

public:
    std::vector<Particle> particles;

    ParticleSystem(PumaRobot& robot) : robot(robot) {
        for (unsigned int i = 0; i < nr_particles; ++i)
            particles.push_back(Particle());
    }

    void Update(float dt) {
        unsigned int nr_new_particles = 2;
        // add new particles
        for (unsigned int i = 0; i < nr_new_particles; ++i)
        {
            int unusedParticle = FirstUnusedParticle();
            RespawnParticle(particles[unusedParticle], robot, offset);
        }
        // update all particles
        for (unsigned int i = 0; i < nr_particles; ++i)
        {
            Particle& p = particles[i];
            p.Life -= dt; // reduce life
            if (p.Life > 0.0f)
            {	// particle is alive, thus update
                p.Position += p.Velocity * dt;
                p.Color.a -= dt * 2.5f;
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

    void RespawnParticle(Particle& particle, PumaRobot& object, glm::vec3 offset)
    {
        float randomX = ((rand() % 100) - 50) / 100.0f;
        float randomY = ((rand() % 100) - 50) / 100.0f;
        float rColor = 0.5f + ((rand() % 100) / 100.0f);


        glm::vec3 effectorPos = object.GetTipPosition();

        particle.Position = effectorPos; 
        particle.Color = glm::vec4(1.0f, 0.6f, 0.2f, 1.0f); // Iskry są pomarańczowe
        particle.Life = 0.3f;
        particle.Velocity = glm::vec3(randomX, randomY, 0.1) * 2.0f; // Rozprysk we wszystkich kierunkach
    }
};