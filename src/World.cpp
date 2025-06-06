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
	updateRooms();

	/*for (auto& [name, player] : playersOnline)
	{
		try { player -> talker() -> doUpdate(player); }
		catch (std::exception& e) { HandleException(player, e); }
	}

	for (auto& [name, player] : playersOnline)
	{
		try { player -> physical() -> doUpdate(player, *this); }
		catch (std::exception& e) { HandleException(player, e); }
	}

	for (auto& [name, player] : playersOnline)
	{
		try { player -> achiever() -> doUpdate(player); }
		catch (std::exception& e) { HandleException(player, e); }
	}

	*/

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

	for (auto& [name, player] : playersOnline) // make this last
		player->notifier()->clearEvent();

	removeOfflinePlayers(); // TODO move this to server
}

void World::addPlayer(std::shared_ptr<Thing> player)
{
	playersOnline[player->networked()->getID()] = player;
}

void World::removePlayer(const std::shared_ptr<Thing>& player)
{
	playersOnline.erase(player->networked()->getID());
}

void World::updateRooms()
{
	for (auto& [room_id, room] : Room::mapRooms)
	{
		room->doUpdate(*this);
	}
}

void World::removeOfflinePlayers()
{
	for (auto it = playersOnline.begin(); it != playersOnline.end();)
	{
		if (!it->second->networked()->isOnline())
		{
			it->second->networked()->doDisconnect(it->second);
			it = playersOnline.erase(it);
		}
		else
		{
			++it;
		}
	}
}
