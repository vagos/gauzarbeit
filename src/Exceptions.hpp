#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <exception>
#include <string>

struct TargetNotFound : public std::exception
{
    virtual const char* what() const throw()
    {
        return "Nothing with that name was found!\n";
    }

};


struct InvalidCommand : public std::exception
{
    virtual const char* what() const throw()
    {
        return "Invalid command!\n";
    }

};

struct MissingComponent : public std::exception
{
    const char * what() const throw()
    {
        return "You can't do that!\n";
    }

};

#endif//EXCEPTIONS_HPP
