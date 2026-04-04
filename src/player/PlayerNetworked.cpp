#include "player/PlayerNetworked.hpp"
#include "Exceptions.hpp"
#include "Room.hpp"
#include "player/CommandParser.hpp"
#include "player/PlayerPhysical.hpp"
#include "script/ScriptedThing.hpp"

void PlayerNetworked::handleRequest(std::shared_ptr<Thing> owner, World& world)
{
    const std::string request = streamRequest.str();
    if (request.empty())
        return;

    Event event{};
    try
    {
        event = CommandParser::Parse(request);
    }
    catch (InvalidCommand& e)
    {
        addResponse(ColorString(e.what(), Color::Red));
        return;
    }

    if (event.verb.empty())
        return;

    if (event.verb == "login")
    {
        if (isLoggedIn())
        {
            addResponse(ColorString("You are already logged in!\n", Color::Red));
            return;
        }

        if (!inDatabase(event.target))
        {
            addResponse("You need to register first! Please use the command 'register <name> "
                        "<password>'.\n");
            return;
        }

        if (!event.object.size())
        {
            addResponse(ColorString("Please do 'login {name} {password}'.\n", Color::Red));
            return;
        }

        owner->name = event.target;
        doDatabaseLoad(owner);

        if (password != event.object)
        {
            Log("Wrong password for player " << owner->name);
            addResponse("Wrong password! Please try again.\n");
            doDisconnect(owner);
            return;
        }

        setLoggedIn(true);

        addResponse(ColorString("You are logged in as " + owner->name + ".\n", Color::Green));
    }

    else if (event.verb == "register")
    {
        if (inDatabase(event.target))
        {
            addResponse(ColorString("A player with that name already exists!\n", Color::Red));
        }

        owner->name = event.target;
        password = event.object;

        if (password.empty())
        {
            addResponse(ColorString("You need to set a password!\n", Color::Red));
            return;
        }

        addResponse("You are registered!\n");
        addResponse(ColorString("You are logged in as " + owner->name + ".\n", Color::Green));

        doDatabaseStore(owner);

        setLoggedIn(true);
    }
    else
    {
        if (!isLoggedIn())
        {
            addResponse(ColorString("You need to log in!\n", Color::Red));
            return;
        }

        owner->notifier()->event = event;
    }
}

PlayerNetworked::PlayerNetworked() {}

void PlayerNetworked::getRequest(std::shared_ptr<Thing> owner, World& world)
{
    try
    {
        streamRequest.str(Server::getMessage(*socket));
    }

    catch (PlayerDisconnect& error)
    {
        doDatabaseStore(owner);
        doDisconnect(owner);
    }

    if (!streamRequest.str().size())
        return;

    Log(*owner << ": " << streamRequest.str() << "\r\r\r"
               << "(" << streamRequest.str().size() << " bytes"
               << ")");
}

void PlayerNetworked::sendResponse(std::shared_ptr<Thing> owner)
{
    if (!(streamResponse.str().size() || streamRequest.str().size()))
    {
        clearStreams();
        return;
    }

    if (!isOnline())
    {
        Server::sendMessage(*socket, streamResponse.str());
        clearStatusLine();
        clearStreams();
        return;
    }

    if (owner->_physical && owner->physical()->current_room)
    {
        setStatusLine(owner->name + "@" + owner->physical()->current_room->name);
    }
    else
    {
        clearStatusLine();
    }

    std::string response = streamResponse.str();
    if (!response.empty() && response.back() != '\n')
    {
        response += '\n';
    }

    if (!getStatusLine().empty())
    {
        response += "[" + getStatusLine() + "] >> ";
    }
    else
    {
        response += ">> ";
    }

    Server::sendMessage(*socket, response);
    clearStreams();
}

void PlayerNetworked::doDatabaseLoad(std::shared_ptr<Thing> owner)
{
    if (!inDatabase(owner->name))
        return;

    Log("Loading player " << owner->name << " from database...");

    std::string filename{"./db/players/" + owner->name};
    db.open(filename);

    if (!db.is_open())
    {
        Log("Player " << owner->name << " not found in database!");
        return;
    }

    std::string line;

    db >> line >> line >> line;

    db >> password;

    db >> line;

    // Load INVENTORY
    {
        db >> line;

        while (line != "END" && !db.eof())
        {

            auto t = ScriptedThing(line);

            t->networked()->doDatabaseLoad(t);

            owner->physical()->gainItem(t);

            db >> line;
        }
    }

    // Load STATS

    {
        float val;

        db >> line;

        db >> val; // Load XP

        owner->achiever()->gainXP(val);

        db >> line;

        while (line != "END" && !db.eof())
        {
            db >> val;

            owner->achiever()->setStat(line, val);

            db >> line;
        }

        db >> line;
    }

    db.close();
}

const std::string PlayerNetworked::doDatabaseSave(std::shared_ptr<Thing> owner)
{
    std::stringstream info;

    info << "PLAYER: " << owner->name << '\n';

    info << "PASSWORD " << password << '\n';

    info << "INVENTORY\n";

    for (auto& t : owner->physical()->inventory)
    {
        info << t->name << ' ' << t->networked()->doDatabaseSave(t) << '\n';
    }

    info << "END\n";

    info << "STATS\n";

    // XP is not loaded as a string
    info << owner->achiever()->getXP() << ' ';

    for (auto [stat_name, stat] : owner->achiever()->extra_stats)
    {
        info << stat_name << ' ' << stat->value << '\n';
    }

    info << "END\n";

    info << '\n';

    info << "END\n";

    return info.str();
}

void PlayerNetworked::doDatabaseStore(std::shared_ptr<Thing> owner)
{

    if (!std::experimental::filesystem::exists("./db/players"))
    {
        std::experimental::filesystem::create_directories("./db/players");
    }

    std::string filename{"./db/players/" + owner->name};

    db.open(filename, std::ios::trunc);

    if (!db.is_open())
    {
        std::ofstream file{filename};
        db.open(filename);
    }

    db << doDatabaseSave(owner);

    db.close();

    Log("Player " << owner->name << " saved!");
}

bool PlayerNetworked::inDatabase(const std::string& name)
{
    std::string filename{"./db/players/" + name};
    return std::experimental::filesystem::exists(filename);
}
