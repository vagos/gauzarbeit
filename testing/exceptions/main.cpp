#include <iostream>
#include <exception>

class TargetNotFoundException : public std::exception
{
    virtual const char* what() const throw()
    {
        return "Nothing with that name was found!\n";
    }


} TargetNotFoundException;


// int main()
// {
//     throw 2;
// 
//     return 0;
// }

int main () {
    
    int i = 0;
  
    try 
    {
        i ++;
        throw TargetNotFoundException;
    }

    catch (std::exception& e)
    {
        std::cout << e.what() << ' ' << i << '\n';
    }

    return 0;
}
