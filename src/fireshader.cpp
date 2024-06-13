#pragma once
#include"fireshader.h"

fire_particle::fire_particle(){}

void fire_particle::Update(float ts){
    for(auto& particle : m_ParticlePool){
        if(!particle.Active)
            continue;
        particle.LifeRemaining -= ts;
        if(particle.LifeRemaining <= 0.0f){
            particle.Active = false;
            continue;
        }
        particle.Position += particle.Velocity * ts;
        particle.Rotation += 0.01f * ts;
    }
}

void fire_particle::Render(){
    for (auto )
}