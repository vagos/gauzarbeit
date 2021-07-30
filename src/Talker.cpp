#include "Talker.hpp"
#include "Thing.hpp"
#include "Quest.hpp"

#include "Helpers.hpp"

#include <iomanip>

void Talker::onTalk(const std::shared_ptr<Thing> &owner, const std::shared_ptr<Thing> talker)
{
    const auto& event = talker -> notifier() -> event;

    auto q = owner -> achiever() -> getQuest(event.object);

    std::clog << event.object << '\n';

    if (!q) return;

    talker -> achiever() -> gainQuest( std::make_shared<ScriptedQuest>(q -> name) );

    //talker -> achiever() -> gainQuest( q );

    talker -> notifier() -> onNotify( talker, talker, Event::Type::Gain_Quest, q );
}

// GUILD 

const std::string Guild::onInspect()
{
    std::stringstream inspect; 

    inspect << "Members: \n" << VerticalListString( online_members, '-', 
            [](const std::weak_ptr<Thing> &t) 
            {
                return t.lock() -> inspectable() -> getName(t.lock());
            });

    return HeaderString( inspect.str(), "Guild: " + name );
}

void Guild::removeMember(const std::string &m_name)
{
    auto member = FindByName(online_members, m_name);
    
    member -> talker() -> guild = nullptr;

    online_members.erase( std::remove(online_members.begin(), online_members.end(),
                member), online_members.end() );

    member_names.erase( m_name );
}

void Guild::addMember(const std::shared_ptr<Thing> &adder, const std::shared_ptr<Thing> member)
{
    online_members.push_back( member );
    member_names.insert( member -> name );

    member -> talker() -> guild = adder -> talker() -> guild;
}

void Guild::onNotify(const std::shared_ptr<Thing> &actor, Event::Type notification_type, const std::shared_ptr<Thing> &target)
{
    for (auto& q : quests)
    {
        q -> notifier() -> onNotify(q, actor, notification_type, target);
    }
}
