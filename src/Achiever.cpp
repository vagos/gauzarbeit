#include "Achiever.hpp"
#include "Script/ScriptedAchiever.hpp"
#include "Helpers.hpp"
#include "Thing.hpp"

const std::shared_ptr<Thing> Achiever::getQuest(const std::string& q_name)
{
    return FindByName(quests, q_name);
}


void Achiever::doUpdate(const std::shared_ptr<Thing> &owner)
{
    for (auto& q : quests)
    {
        if (q -> tasker() -> isCompleted())
        {
            q -> tasker() -> doReward( q, owner );
            onQuestComplete(owner, q);
        }
    }

    quests.erase( std::remove_if( quests.begin(), quests.end(), 
                [](auto& t){return t -> tasker() -> isCompleted();}), 
            quests.end()); // Remove completed quests
}


void Achiever::onQuestComplete(const std::shared_ptr<Thing> &owner, const std::shared_ptr<Thing> &quest)
{
    getRewards(owner, quest -> tasker() -> getDifficulty());
    completed_quests.push_back(quest);
}

void Achiever::setStat(const std::string &s_n, float val)
{
    if (!extra_stats[s_n])
    {
        extra_stats[s_n] = std::make_shared<ScriptedAchiever::ScriptedStat>(s_n);
    }

    extra_stats[s_n] -> value = val; 
}

float Achiever::getStat(const std::string &s_n)
{
    if (!extra_stats[s_n]) return 0;

    return extra_stats[s_n] -> value;
}
