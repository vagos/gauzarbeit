#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <exception>
#include <string>

#include <cassert>

struct TargetNotFound : public std::exception
{

    TargetNotFound()
    {
    //    assert(false);
    }

    virtual const char* what() const throw()
    {
        return "Nothing with that name was found!\n";
    }

};


struct InvalidCommand : public std::exception
{
    virtual const char* what() const throw()
    {
        return "There is no such command!\n";
    }

};

struct MissingComponent : public std::exception
{
    const char * what() const throw()
    {
        return "You can't do that!\n";
    }

};

struct PlayerDisconnect : public std::exception
{
    const char * what() const throw()
    {
        return "Player disconnected!\n";
    }

};

#endif//EXCEPTIONS_HPP
