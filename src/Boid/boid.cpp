#include "Boid/boid.hpp"
#include <glm/gtc/random.hpp>
#include <vector>
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include "p6/p6.h"

// les comportements sont bons
// TO DO Vérifier la fonction flock, enlever l'acceleration
// Voir pour mettre une séparation minimale de radius, pour faire un slider pour le nombre de boids

using vec = glm::vec3;

Boid::Boid()
    : m_position(vec(p6::random::point(), -10.f)), m_velocity(vec(p6::random::direction(), -10.f)), m_direction(vec(p6::random::direction(), -10.f)){};

Boid::Boid(const Boid& b)
    : m_position(b.m_position), m_velocity(b.m_velocity), m_direction(b.m_direction){};

Boid::Boid(vec pos, vec vel, vec direction)
    : m_position(pos), m_velocity(vel), m_direction(direction){};

void Boid::draw(p6::Context& ctx, float radius)
{
    ctx.fill = {1.f};
    ctx.circle(p6::Center{m_position}, p6::Radius{radius});
};

void Boid::setIntensities(Intensity intensities)
{
    m_intensities = intensities;
}

std::vector<Boid> Boid::searchNeighbors(const std::vector<Boid>& boids)
{
    std::vector<Boid> neighbors{};
    for (const auto& other : boids)
    {
        float distance = glm::distance(m_position, other.m_position);
        if (&other != this && distance <= m_intensities.m_perceptionIntensity)
        {
            neighbors.emplace_back(other);
        }
    }
    return neighbors;
}

vec Boid::align(const std::vector<Boid>& neighbors)
{
    vec alignment = vec{0.f};
    for (const auto& boid : neighbors)
    {
        alignment += boid.m_direction;
    }
    if (!neighbors.empty())
    {
        alignment = glm::normalize(alignment);
    }
    return alignment;
};

vec Boid::cohesion(const std::vector<Boid>& neighbors)
{
    vec cohesion = vec{0.f};
    for (const auto& boid : neighbors)
    {
        cohesion += boid.m_position;
    }
    if (!neighbors.empty())
    {
        cohesion /= static_cast<float>(neighbors.size());
        cohesion -= m_position;
        cohesion = glm::normalize(cohesion);
    }
    return cohesion;
};

vec Boid::separation(const std::vector<Boid>& neighbors)
{
    vec separation = vec{0.f};
    for (const auto& boid : neighbors)
    {
        vec difference = m_position - boid.m_position;
        if (vec::length() > 0)
            separation += glm::normalize(difference) / static_cast<float>(vec::length());
    }
    if (!neighbors.empty())
    {
        separation = glm::normalize(separation);
    }
    return separation;
}

void Boid::flock(const std::vector<Boid>& boids, p6::Context& ctx)
{
    vec cohesion = this->cohesion(searchNeighbors(boids)) * vec{m_intensities.m_cohesionIntensity};

    vec alignment = this->align(searchNeighbors(boids)) * vec{m_intensities.m_alignmentIntensity};

    vec separation = this->separation(searchNeighbors(boids)) * vec{m_intensities.m_separationIntensity};

    m_direction += cohesion + alignment + separation;
    m_direction = glm::normalize(m_direction);

    m_velocity = vec{0.1f} * m_direction; // voir si normalize ou pas
    m_position += m_velocity * vec{2.0f} * vec{ctx.delta_time()};
}

void Boid::avoidEdges(p6::Context& ctx, float radius)
{
    if (m_position.x <= -ctx.aspect_ratio() + radius)
    {
        m_position.x = ctx.aspect_ratio() - radius;
    }
    else if (m_position.x >= ctx.aspect_ratio() - radius)
    {
        m_position.x = -ctx.aspect_ratio() + radius;
    }
    else if (m_position.y <= -1 + radius)
    {
        m_position.y = 1 - radius;
    }
    else if (m_position.y >= 1 - radius)
    {
        m_position.y = -1 + radius;
    }
}

bool Boid::closeToEdges(p6::Context& ctx, float /*radius*/) const
{
    return m_position.x <= -ctx.aspect_ratio() || m_position.x >= ctx.aspect_ratio() || m_position.y <= -1 || m_position.y >= 1;
};