#include "World.hpp"
#include "script/ScriptPaths.hpp"
#include <algorithm>
#include <filesystem>

namespace
{
std::vector<std::string> LoadSpawnTable()
{
    const std::filesystem::path things_dir = ScriptPaths::ResolveDir("things");
    if (!std::filesystem::exists(things_dir))
    {
        Log("World generation could not find thing scripts in " << things_dir.string());
        return {};
    }

    std::vector<std::string> names;
    for (const auto& entry : std::filesystem::directory_iterator(things_dir))
    {
        if (!entry.is_regular_file())
            continue;

        const auto extension = entry.path().extension();
        if (extension != ".lua" && extension != ".js")
            continue;

        names.push_back(entry.path().stem().string());
    }

    std::sort(names.begin(), names.end());
    names.erase(std::unique(names.begin(), names.end()), names.end());
    return names;
}
} // namespace

World* World::current_world = nullptr;
std::vector<std::string> World::spawn_table = LoadSpawnTable();

World::World()
{
    current_world = this;
    Player::setPlayerCommands();
}

World::~World()
{
    if (current_world == this)
        current_world = nullptr;
}

const std::shared_ptr<Thing> World::getPlayer(const std::string& name) const
{
    std::regex r{name, std::regex_constants::icase};

    for (auto& [id, player] : playersOnline)
    {
        if (std::regex_match(player->name, r))
            return player;
    }

    return nullptr;
}

void World::doUpdate()
{
    for (auto& sys : systems)
    {
        sys->doUpdate(*this);
    }

    for (auto& [name, player] : playersOnline)
    {
        try
        {
            player->attackable()->doUpdate(player);
        }
        catch (std::exception& e)
        {
            HandleException(player, e);
        }
    }

    for (auto& [name, player] : playersOnline)
    {
        try
        {
            player->thinker()->doThink(player, *this);
        }
        catch (std::exception& e)
        {
            HandleException(player, e);
        }
    }

    for (auto& [name, player] : playersOnline) // TODO: make this last
        player->notifier()->clearEvent();

    removeOfflinePlayers(); // TODO: move this to server
}

void World::addPlayer(std::shared_ptr<Thing> player)
{
    playersOnline[player->getID()] = player;
}

void World::removePlayer(const std::shared_ptr<Thing>& player)
{
    playersOnline.erase(player->getID());
}

void World::removeOfflinePlayers()
{
    for (auto it = playersOnline.begin(); it != playersOnline.end();)
    {
        if (!it->second->networked()->isOnline())
        {
            it = playersOnline.erase(it);
        }
        else
        {
            ++it;
        }
    }
}
