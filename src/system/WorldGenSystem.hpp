#pragma once

#include "Room.hpp"
#include "System.hpp"
#include <cstdint>
#include <memory>
#include <random>
#include <string>
#include <vector>

struct WorldGenSystem : System
{
    explicit WorldGenSystem(World& world, std::uint64_t seed = 1) : System(world), seed(seed) {}

    void doUpdate(World& world) override {}

    std::shared_ptr<Room> generateRoom(int x, int y);
    std::shared_ptr<Thing> generateThing(std::mt19937_64& rng) const;

    std::uint64_t seed;

  private:
    std::uint64_t roomKey(int x, int y) const;
    std::string generateRoomName(std::mt19937_64& rng) const;
    void populateRoom(const std::shared_ptr<Room>& room, std::mt19937_64& rng);

};
