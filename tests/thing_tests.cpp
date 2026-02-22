#include "Room.hpp"
#include "TestSupport.hpp"
#include "player/Player.hpp"
#include <doctest/doctest.h>

TEST_CASE("Physical pickup and drop move items between room and inventory")
{
    auto room = std::make_shared<Room>(0, 0);
    auto owner = MakeBasicThing("Owner");
    owner->physical()->current_room = room;
    room->addThing(owner);

    auto item = MakeBasicThing("Item");
    room->addThing(item);

    owner->physical()->pickupItem(item);
    CHECK(owner->physical()->hasItem(item));
    CHECK(room->getThing("Item") == nullptr);

    owner->physical()->dropItem(item);
    CHECK(!owner->physical()->hasItem(item));
    CHECK(room->getThing("Item") == item);
}

TEST_CASE("Physical pickup respects immovable items")
{
    auto room = std::make_shared<Room>(1, 1);
    auto owner = MakeBasicThing("Owner");
    owner->physical()->current_room = room;
    room->addThing(owner);

    auto item = MakeBasicThing("Anvil");
    item->physical()->movable = false;
    room->addThing(item);

    owner->physical()->pickupItem(item);
    CHECK(!owner->physical()->hasItem(item));
    CHECK(room->getThing("Anvil") == item);
}

TEST_CASE("Physical give and equip move items between inventories")
{
    auto giver = MakeBasicThing("Giver");
    auto receiver = MakeBasicThing("Receiver");
    auto item = MakeBasicThing("Ring");

    giver->physical()->gainItem(item);
    giver->physical()->giveItem(receiver, item);

    CHECK(!giver->physical()->hasItem(item));
    CHECK(receiver->physical()->hasItem(item));

    auto gear = MakeBasicThing("Helmet");
    giver->physical()->gainItem(gear);
    giver->physical()->equipItem(gear);

    CHECK(!giver->physical()->hasItem(gear));
    CHECK(giver->physical()->equipment.size() == 1);
    CHECK(giver->physical()->equipment.front() == gear);
}

TEST_CASE("Physical doMove updates the owning room")
{
    Room::mapRooms.clear();

    auto room = std::make_shared<Room>(0, 0);
    auto mover = MakeBasicThing("Mover");
    mover->physical()->current_room = room;
    room->addThing(mover);

    mover->physical()->doMove(mover, 5, 6);

    CHECK(mover->physical()->current_room->x == 5);
    CHECK(mover->physical()->current_room->y == 6);
    CHECK(room->getThing("Mover") == nullptr);
    CHECK(mover->physical()->current_room->getThing("Mover") == mover);
}

TEST_CASE("Attackable doAttack kills and removes target")
{
    auto room = std::make_shared<Room>(0, 0);
    auto attacker = MakeBasicThing("Attacker");
    auto target = MakeBasicThing("Target");

    attacker->physical()->current_room = room;
    target->physical()->current_room = room;
    room->addThing(attacker);
    room->addThing(target);

    target->attackable()->setMaxHealth(1);
    attacker->attackable()->dmg = 1;

    attacker->attackable()->doAttack(attacker, target);

    CHECK(!target->attackable()->is_alive());
    CHECK(target->physical()->current_room == nullptr);
    CHECK(room->getThing("Target") == nullptr);
}

TEST_CASE("Achiever completes quests and grants rewards")
{
    auto owner = MakeBasicThing("Player");
    auto quest = MakeBasicThing("Quest");

    auto tasker = std::make_shared<Tasker>();
    tasker->addTask("Find cheese");
    tasker->addTask("Return to chef");
    tasker->tickTask(0);
    tasker->tickTask(1);
    quest->_tasker = tasker;

    owner->achiever()->gainQuest(quest);
    owner->achiever()->doUpdate(owner);

    CHECK(owner->achiever()->getXP() == 2);
    CHECK(owner->achiever()->quests.empty());
    CHECK(owner->achiever()->completed_quests.size() == 1);
    CHECK(owner->achiever()->completed_quests.front() == quest);
}

TEST_CASE("Tasker completion and difficulty tracking")
{
    Tasker tasker;
    CHECK(tasker.getDifficulty() == 0);
    CHECK(tasker.isCompleted());

    auto task_index = tasker.addTask("Collect herbs");
    CHECK(tasker.getDifficulty() == 1);
    CHECK(!tasker.isCompleted());

    tasker.tickTask(task_index);
    CHECK(tasker.isCompleted());
}

TEST_CASE("Room routes players into players list when added as a thing")
{
    auto room = std::make_shared<Room>(0, 0);
    auto player = std::make_shared<Player>();

    room->addThing(player);

    CHECK(std::find(room->players.begin(), room->players.end(), player) != room->players.end());
    CHECK(std::find(room->things.begin(), room->things.end(), player) == room->things.end());
}
