#pragma once
#include "Helpers.hpp"
#include <memory>

class World;
struct Thing;

/**
 * A System is any piece of game‐logic that runs once per tick.
 */
struct System
{
    virtual ~System() = default;

    explicit System(World& world) : world(world) {}

    virtual void doUpdate(World& world) {}
    virtual void onEvent(World& world, const std::shared_ptr<Thing>& actor, Event::Type event,
                         const std::shared_ptr<Thing>& target = nullptr)
    {
    }

    World& world;
};
