#include "PlayerNotifier.hpp"
#include "Player.hpp"

void PlayerNotifier::setEvent(const std::shared_ptr<Thing> &owner)
{
        std::stringstream req{ owner -> networked() -> getRequestStream().str() };

        auto words = TokenizeString(req.str());

        //if (words.size() >= 4 && req.str().find("from") != std::string::npos) 
        //{
        //    event.verb = words[0];
        //    event.object = words[1];
        //    event.target = words[3];
        //
        //    return;
        //}

        //else if (words.size() >= 4 && req.str().find("with") != std::string::npos)
        //{
        //    event.verb = words[0];
        //    event.target = words[1];
        //    event.object = words[3];
        //    
        //    return;

        //}
        //
        //else if (words.size() >= 4 && req.str().find("on") != std::string::npos)
        //{
        //    event.verb = words[0];
        //    event.target = words[1];
        //    event.object = words[3];
        //    
        //    return;
        //}
    
        req >> event.verb >> event.target >> event.object;
        
        std::getline( req >> std::ws, event.extra );

        event.type = Player::playerCommands[event.verb];

        if (event.verb.size() && event.type == Event::Type::Invalid) throw InvalidCommand();
}
