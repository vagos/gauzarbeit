#include "Helpers.hpp"
#include "thing/Thing.hpp"
#include <iomanip>

void Talker::onTalk(const std::shared_ptr<Thing>&, const std::shared_ptr<Thing>,
                    const Event& event)
{
    Log(event.object);

    if (event.object.empty())
        return;
}

// GUILD

const std::string Guild::onInspect()
{
    std::stringstream inspect;

    inspect << "Members: \n"
            << VerticalListString(online_members, '-',
                                  [](const std::shared_ptr<Thing>& t)
                                  { return t->inspectable()->getName(t); });

    return HeaderString(inspect.str(), "Guild: " + name);
}

void Guild::removeMember(const std::string& m_name)
{
    auto member = FindByName(online_members, m_name);

    member->talker()->guild = nullptr;

    online_members.erase(std::remove(online_members.begin(), online_members.end(), member),
                         online_members.end());

    member_names.erase(m_name);
}

void Guild::addMember(const std::shared_ptr<Thing>& adder, const std::shared_ptr<Thing> member)
{
    online_members.push_back(member);
    member_names.insert(member->name);

    member->talker()->guild = adder->talker()->guild;
}

void Guild::onNotify(const std::shared_ptr<Thing>&, const Event&,
                     const std::shared_ptr<Thing>&)
{
}
