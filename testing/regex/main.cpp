#include <regex>
#include <iostream>
#include <string>

void showMatches(const std::smatch& match)
{
    for (int i = 0; i < match.size(); ++i)
    {
        std::cout << i << ": " << match.str(i) << "\n";
    }
}


int main()
{
    std::regex regex(R"((abc?))");
    std::smatch match;

    std::string s("ab vab");

    std::regex_search(s, match, regex);

    showMatches(match);


    return 0;
}
