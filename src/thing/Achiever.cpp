#include "Helpers.hpp"
#include "thing/Thing.hpp"

void Achiever::setStat(const std::string& s_n, float val)
{
    if (!extra_stats[s_n])
    {
        extra_stats[s_n] = std::make_shared<Achiever::Stat>();
    }

    extra_stats[s_n]->value = val;
}

float Achiever::getStat(const std::string& s_n)
{
    if (!extra_stats[s_n])
        return 0;

    return extra_stats[s_n]->value;
}
