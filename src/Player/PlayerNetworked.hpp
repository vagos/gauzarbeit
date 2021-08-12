#ifndef PLAYER_NETWORKED_HPP
#define PLAYER_NETWORKED_HPP

#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <iomanip>
#include <experimental/filesystem>

#include "../Thing.hpp"
#include "../Server.hpp"

#include "PlayerPhysical.hpp"
#include "PlayerAttackable.hpp"
#include "PlayerNotifier.hpp"
#include "PlayerTalker.hpp"
#include "PlayerInspectable.hpp"

class PlayerNetworked : public Networked 
{

public:

    PlayerNetworked();

    void getRequest( std::shared_ptr<Thing> owner, World& world );
    void sendResponse(std::shared_ptr<Thing> owner) override;
    void handleRequest(std::shared_ptr<Thing> owner, World& world) override; 

    void doDatabaseLoad(std::shared_ptr<Thing> owner) override;
    const std::string doDatabaseSave(std::shared_ptr<Thing> owner) override;
    void doDatabaseStore(std::shared_ptr<Thing> owner) override ;
    bool inDatabase(const std::string &name);

private: 
    std::string password;
};

#endif
