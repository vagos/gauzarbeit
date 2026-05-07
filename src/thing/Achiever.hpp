#ifndef ACHIEVER_HPP
#define ACHIEVER_HPP

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class Thing;

class Achiever
{
  public:
    struct Stat
    {
        Stat() {}
        virtual const std::string getName() { return std::to_string(value); }
        float value = 0;
    };

    std::unordered_map<std::string, std::shared_ptr<Stat>> extra_stats;

    virtual void setStat(const std::string& s_n, float val);
    float getStat(const std::string& s_n);

    void gainXP(int extra_xp) { xp += extra_xp; }
    virtual void getRewards(const std::shared_ptr<Thing> owner, int size) { gainXP(size); }
    int getLevel() { return xp / 10; }
    int getXP() { return xp; }

  private:
    int xp = 0;
};

#endif
