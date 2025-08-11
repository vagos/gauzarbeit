#include "player/PlayerNetworked.hpp"
#include "player/PlayerPhysical.hpp"

void PlayerNetworked::handleRequest(std::shared_ptr<Thing> owner, World& world)
{
    std::string verb, target, object;
    std::stringstream req{streamRequest.str()};

    req >> verb >> target >> object;

    if (verb == "login")
    {
        if (isLoggedIn())
        {
            addResponse(ColorString("You are already logged in!\n", Color::Red));
            return;
        }

        if (!inDatabase(target))
        {
            addResponse("You need to register first! Please use the command 'register <name> "
                        "<password>'.\n");
            return;
        }

        if (!object.size())
        {
            addResponse(ColorString("Please do 'login {name} {password}'.\n", Color::Red));
            return;
        }

        owner->name = target;
        doDatabaseLoad(owner);

        if (password != object)
        {
            Log("Wrong password for player " << owner->name);
            addResponse("Wrong password! Please try again.\n");
            doDisconnect(owner);
            return;
        }

        setLoggedIn(true);

        addResponse(ColorString("You are logged in as " + owner->name + ".\n", Color::Green));
    }

    else if (verb == "register")
    {
        if (inDatabase(target))
        {
            addResponse(ColorString("A player with that name already exists!\n", Color::Red));
        }

        owner->name = target;
        password = object;

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
        if (!isLoggedIn() && verb.size())
            addResponse(ColorString("You need to log in!\n", Color::Red));
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
        goto CLEAR;

    addResponse(">> ");

    Server::sendMessage(*socket, streamResponse.str());

CLEAR:

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

            auto t = std::make_shared<ScriptedThing>(line);

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
