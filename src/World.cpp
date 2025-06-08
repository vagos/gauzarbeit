#include "World.hpp"

World::World()
{
    Player::setPlayerCommands();
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
            player->thinker()->doThink(player);
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
