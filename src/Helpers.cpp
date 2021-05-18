#include "Helpers.hpp"

bool IsNumber(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();s.end();
}

bool PartlyMatch(const std::string& s1, const std::string& s2, int n)
{
    auto it1 = s1.begin(), it2 = s2.begin();

    int m = 0;

    while (it1 != s1.end() && it2 != s2.end() && m < n)
    {
        if (*it1 == *it2) m++;
        else break;
        
        ++it1;
        ++it2;
    }

    return m == n;
}
