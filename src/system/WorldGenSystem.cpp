#include "World.hpp"
#include "system/WorldGenSystem.hpp"
#include "script/ScriptPaths.hpp"
#include "script/ScriptedThing.hpp"

std::uint64_t WorldGenSystem::roomKey(int x, int y) const
{
    std::uint64_t key = seed;
    key ^= static_cast<std::uint64_t>(x) + 0x9e3779b97f4a7c15ULL + (key << 6) + (key >> 2);
    key ^= static_cast<std::uint64_t>(y) + 0x9e3779b97f4a7c15ULL + (key << 6) + (key >> 2);
    return key;
}

std::string WorldGenSystem::generateRoomName(std::mt19937_64& rng) const
{
    static const std::vector<std::string> adjectives =
        LoadLines(ScriptPaths::Resolve("data/adjectives.txt"));
    static const std::vector<std::string> nouns = LoadLines(ScriptPaths::Resolve("data/nouns.txt"));

    if (adjectives.empty() || nouns.empty())
        return "Unnamed Room";

    std::uniform_int_distribution<std::size_t> adjective_dist(0, adjectives.size() - 1);
    std::uniform_int_distribution<std::size_t> noun_dist(0, nouns.size() - 1);

    return CapitalizeWord(adjectives[adjective_dist(rng)]) + " " +
           CapitalizeWord(nouns[noun_dist(rng)]);
}

std::shared_ptr<Thing> WorldGenSystem::generateThing(std::mt19937_64& rng) const
{
    static const std::string things_dir = ScriptPaths::ResolveDir("things");

    if (World::spawn_table.empty())
        return nullptr;

    std::uniform_int_distribution<std::size_t> spawn_dist(0, World::spawn_table.size() - 1);
    return ScriptedThing(World::spawn_table[spawn_dist(rng)], things_dir);
}

void WorldGenSystem::populateRoom(const std::shared_ptr<Room>& room, std::mt19937_64& rng)
{
    std::uniform_int_distribution<int> count_dist(5, 20);

    const int thing_count = count_dist(rng);
    for (int i = 0; i < thing_count; ++i)
    {
        try
        {
            auto thing = generateThing(rng);
            if (!thing)
                return;

            if (thing->_physical)
                thing->physical()->doMove(thing, room);
            else
                room->addThing(thing);
        }
        catch (const std::exception& e)
        {
            Log("World generation skipped thing: " << e.what());
        }
    }
}

std::shared_ptr<Room> WorldGenSystem::generateRoom(int x, int y)
{
    std::mt19937_64 rng(roomKey(x, y));

    auto room = std::make_shared<ScriptedRoom>(x, y);
    room->name = generateRoomName(rng);

    const std::int64_t key = (x & 0xFFFF) << 16 | (y & 0xFFFF);
    Room::mapRooms[key] = room;

    populateRoom(room, rng);

    return room;
}
