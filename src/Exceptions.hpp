#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <cassert>
#include <exception>
#include <string>

struct TargetNotFound : public std::exception
{
    explicit TargetNotFound(std::string target)
        : message("Target not found: " + (target.empty() ? std::string("<unknown>") : target) +
                  "\n")
    {
    }

    const char* what() const noexcept override { return message.c_str(); }

  private:
    std::string message;
};

struct InvalidCommand : public std::exception
{
    virtual const char* what() const throw() { return "There is no such command!\n"; }
};

struct MissingComponent : public std::exception
{
    const char* what() const throw() { return "You can't do that!\n"; }
};

struct PlayerDisconnect : public std::exception
{
    const char* what() const throw() { return "Player disconnected!\n"; }
};

#endif // EXCEPTIONS_HPP
