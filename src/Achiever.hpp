#ifndef ACHIEVER_HPP
#define ACHIEVER_HPP

#include <memory>
#include <vector>
#include <unordered_map>
#include <string>

class Thing;

class Achiever 
{
public:

    struct Stat
    {

        Stat()
        {

        }

        virtual const std::string getName()
        {
            return "";
        }

        float value = 0;
    };

    std::vector< std::shared_ptr<Thing> > quests;
    std::vector< std::shared_ptr<Thing> > completed_quests;
    std::unordered_map< std::string, std::shared_ptr<Stat> > extra_stats;

    void setStat(const std::string &s_n, float val);
    float getStat(const std::string &s_n);

    virtual void gainQuest(std::shared_ptr<Thing> quest)
    {
        quests.push_back(quest);
    }
    const std::shared_ptr<Thing> getQuest(const std::string& q_name);

    void doUpdate(const std::shared_ptr<Thing> &owner);
    
    virtual void onQuestComplete(const std::shared_ptr<Thing> &owner, const std::shared_ptr<Thing> &quest);

    void gainXP(int extra_xp)
    {
       xp += extra_xp; 
    }

    virtual void getRewards(const std::shared_ptr<Thing> owner, int size)
    {
        gainXP(size);
    }

    int getLevel()
    {
        return xp/10;
    } 

    int getXP() {return xp;}

private:
    int xp = 0;

};

#endif
