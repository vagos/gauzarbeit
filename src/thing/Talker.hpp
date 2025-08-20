#ifndef TALKER_HPP
#define TALKER_HPP

#include "Helpers.hpp"
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <vector>

class Thing;

struct Guild
{
    Guild(const std::string name) : name(name) {}

    std::vector<std::shared_ptr<Thing>> online_members;
    std::set<std::string> member_names;

    std::vector<std::shared_ptr<Thing>> quests;

    void addMember(const std::shared_ptr<Thing>& adder, const std::shared_ptr<Thing> member);
    void removeMember(const std::string& m_name);

    const std::string onInspect();

    void onNotify(const std::shared_ptr<Thing>& actor, Event::Type notification_type,
                  const std::shared_ptr<Thing>& target);

    std::string name;
};

class Talker // This component can give quests/dialog
{
  public:
    Talker() {}

    virtual void doUpdate(const std::shared_ptr<Thing>& owner) {}

    virtual void onTalk(const std::shared_ptr<Thing>& owner, const std::shared_ptr<Thing> talker);

    void createGuild(const std::shared_ptr<Thing> owner, const std::string& g_name)
    {
        guild = std::make_shared<Guild>(g_name);

        guild->addMember(owner, owner);
    }

    std::shared_ptr<Guild> guild;
};

#endif // TALKER_HPP
