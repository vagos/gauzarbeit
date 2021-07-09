#ifndef ROOM_HPP
#define ROOM_HPP

#include <cstdint>
#include <memory>
#include <sstream>
#include <unordered_map>
#include <list>
#include <random>
#include <filesystem>
#include <iterator>
#include <algorithm>
#include <memory>

#include "Thing.hpp"

class Player;
class RoomInspectable;

class Room : public Thing
{
public:
    Room(int x, int y): x(x), y(y) 
    {
        //inspectable = std::make_unique<RoomInspectable>();
    }

    static std::shared_ptr<Room> get(std::int32_t x, std::int32_t y);
    static std::shared_ptr<Room> get(const std::string &r_t, std::int32_t x, std::int32_t y);

    int x, y;

    void removePlayer(std::shared_ptr<Thing> player);
    void removeThing(std::shared_ptr<Thing> thing);
        
    void addPlayer(std::shared_ptr<Thing> player);
    void addThing(std::shared_ptr<Thing> thing);

    const std::shared_ptr<Thing> getThing(const std::string& name);
    const std::shared_ptr<Thing> getPlayer(const std::string& name);

    const std::shared_ptr<Thing> getAnything(const std::string& name);

    const std::string onInspect(std::shared_ptr<Thing> owner, std::shared_ptr<Thing> inspector);
    


    virtual void doUpdate(World& world);

    virtual void doGeneration()
    {

    }

public:

    static std::unordered_map< std::int64_t, std::shared_ptr< Room > > mapRooms;
    
    std::vector< std::shared_ptr<Thing> > players;
    std::vector< std::shared_ptr<Thing> > things;
};

//class BasicRoom : public Room
//{
//public:
//
//    BasicRoom(int x, int y): Room(x, y)
//    {
//     
//    }
//
//    void doGeneration() override
//    {
//
//       // Testing Generation
//        
//        std::vector<std::string> script_names;
// 
//        
//        for (const auto& dir_entry : std::filesystem::directory_iterator("./Scripts/"))
//        {
//            std::string filename{ dir_entry.path().filename() };
//
//            if (filename.substr(filename.size() - 3) == "lua")
//            {
//                //std::clog << filename << '\n';
//
//                auto name = filename.substr(0, filename.size() - 4);
//
//                script_names.push_back(name);
//            }
//        }
//
//        std::vector<std::string> loaded_scripts;
//
//        std::sample(script_names.begin(), script_names.end(), std::back_inserter(loaded_scripts), 2, std::mt19937{std::random_device{}()});
//
//        for (const auto& s : loaded_scripts)
//        {
//            //std::clog << s << '\n';
//
//            auto t = std::make_shared<ScriptedThing>(s);
//
//            if (t -> physical) t -> physical -> doMove(t, x, y);
//            else addThing( t );
//
//        }
//    }
//
//    void doUpdate(World &world) override
//    {
//        Room::doUpdate(world);
//    }
//
//private:
//    int max_enemies = 10;
//
//};


// Work on this later.

class ScriptedRoom : public Room
{
public:
    ScriptedRoom(const std::string& room_type, int x, int y);

    void doUpdate(World &world) override; 

    void doGeneration() override;

    private:
        std::string room_type;

};

#endif//ROOM_HPP
