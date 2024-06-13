#pragma once

#include <glm/glm.hpp>
struct fire_shader_properties {
        glm::vec2 Position;
        glm::vec2 Velocity, VelocityVariation;
        glm::vec4 ColorBegin, ColorEnd;
        float SizeBegin, SizeEnd, SizeVariation;
        float LifeTime = 1.0f;
};

class fire_particle {
    public:
    void update(float ts);
    void render();
    void emit(const fire_shader_properties& particleProps);
    private:
    struct particle {
        glm::vec2 Position;
        glm::vec2 Velocity;
        glm::vec4 ColorBegin, ColorEnd;
        float Rotation = 0.0f;
        float SizeBegin, SizeEnd;
        float LifeTime = 1.0f;
        float LifeRemaining = 0.0f;
        bool Active = false;
    };
    int pool_index = 999;
    std::vector<particle> particle_pool;

};