#pragma once

class World;
struct Thing;

/**
 * A System is any piece of game‐logic that runs once per tick.
 */
struct System
{
	virtual ~System() = default;

	virtual void doUpdate(World& world) = 0;
};
