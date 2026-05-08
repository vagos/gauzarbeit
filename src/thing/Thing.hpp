#ifndef THING_HPP
#define THING_HPP

#include "Achiever.hpp"
#include "Attackable.hpp"
#include "Exceptions.hpp"
#include "Helpers.hpp"
#include "Inspectable.hpp"
#include "Networked.hpp"
#include "Physical.hpp"
#include "Talker.hpp"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <functional>
#include <iostream>
#include <memory>
#include <queue>
#include <sstream>
#include <string>

class World;
class Thing;
class Room;
class Physical;
class Usable;
class Attackable;
class Talker;
class Notifier;
class Achiever;
class Tasker;
class Inspectable;
class Thinker;

class Thing : public std::enable_shared_from_this<Thing>
{
  public:
    enum class ScriptLanguage
    {
        None,
        Lua,
        JS,
    };

    ~Thing() { Log(name << " got destroyed!"); }
    Thing();
    Thing(const std::string& name);

  public:
    std::string name;

    std::shared_ptr<Networked> _networked = nullptr;
    std::shared_ptr<Physical> _physical = nullptr;
    std::shared_ptr<Usable> _usable = nullptr;
    std::shared_ptr<Attackable> _attackable = nullptr;
    std::shared_ptr<Talker> _talker = nullptr;
    std::shared_ptr<Notifier> _notifier = nullptr;
    std::shared_ptr<Achiever> _achiever = nullptr;
    std::shared_ptr<Tasker> _tasker = nullptr;
    std::shared_ptr<Inspectable> _inspectable = nullptr;
    std::shared_ptr<Thinker> _thinker = nullptr;

    static std::vector<std::shared_ptr<Thing>> things;

    std::size_t id;
    static std::size_t lastID;
    bool is_player = false;
    double next_update_time = 0.0;
    ScriptLanguage script_language = ScriptLanguage::None;

    friend std::ostream& operator<<(std::ostream& os, const Thing& thing)
    {
        os << thing.name;

        return os;
    }

    void doUpdate(World& world);
    std::size_t getID() const { return id; }

    std::shared_ptr<Networked>& networked()
    {
        if (_networked)
            return _networked;
        throw MissingComponent();
    }
    std::shared_ptr<Physical>& physical()
    {
        if (_physical)
            return _physical;
        throw MissingComponent();
    }
    std::shared_ptr<Usable>& usable()
    {
        if (_usable)
            return _usable;
        throw MissingComponent();
    }
    std::shared_ptr<Attackable>& attackable()
    {
        if (_attackable)
            return _attackable;
        throw MissingComponent();
    }
    std::shared_ptr<Talker>& talker()
    {
        if (_talker)
            return _talker;
        throw MissingComponent();
    }
    std::shared_ptr<Notifier>& notifier()
    {
        if (_notifier)
            return _notifier;
        throw MissingComponent();
    }
    std::shared_ptr<Achiever>& achiever()
    {
        if (_achiever)
            return _achiever;
        throw MissingComponent();
    }
    std::shared_ptr<Tasker>& tasker()
    {
        if (_tasker)
            return _tasker;
        throw MissingComponent();
    }
    std::shared_ptr<Inspectable>& inspectable()
    {
        if (_inspectable)
            return _inspectable;
        throw MissingComponent();
    }
    std::shared_ptr<Thinker>& thinker()
    {
        if (_thinker)
            return _thinker;
        throw MissingComponent();
    }
};

class Notifier
{
  public:
    Notifier() {}

    virtual void doNotify(const std::shared_ptr<Thing>& owner, Event::Type notification_type,
                          const std::shared_ptr<Thing>& target = nullptr)
    {
    }

    virtual void onNotify(const std::shared_ptr<Thing>& owner, const std::shared_ptr<Thing>& actor,
                          Event::Type notification_type,
                          const std::shared_ptr<Thing>& target = nullptr)
    {
    }

    virtual void setEvent(const std::shared_ptr<Thing>& owner) {}

    void setEventPayload(std::string p) { event.payload = p; }

    void clearEvent()
    {
        event.type = Event::Type::Invalid;
        event.verb = "";
        event.object = "";
        event.target = "";
        event.extra = "";
        event.payload = "";
    }

    virtual void doUpdate(const std::shared_ptr<Thing>& owner) {}

    Event event;
};

class Tasker
{

  public:
    struct Task
    {
        Task(std::string desc) : description(std::move(desc)) {}

        virtual ~Task() = default;
        Task(const Task&) = default;
        Task& operator=(const Task&) = default;

        virtual bool onNotify(const std::shared_ptr<Thing>& owner,
                              const std::shared_ptr<Thing>& actor, Event::Type notification_type,
                              const std::shared_ptr<Thing>& target)
        {
            return false;
        }

        virtual std::string format() const { return description; }

        bool tick = false;
        std::string description;
    };

    std::vector<std::unique_ptr<Task>> tasks;
    std::vector<std::unique_ptr<Task>> done_tasks;

    int addTask(std::string description)
    {
        tasks.push_back(std::make_unique<Task>(std::move(description)));

        return tasks.size() - 1;
    }

    int addTask(std::unique_ptr<Task> task)
    {
        tasks.push_back(std::move(task));
        return tasks.size() - 1;
    }

    std::string formatTask(const Task& task) const
    {
        return task.format();
    }

    Task* getTask(const std::string& description)
    {
        auto it = std::find_if(tasks.begin(), tasks.end(),
                               [&description](const std::unique_ptr<Task>& task)
                               { return PartlyMatch(task->description, description); });
        return it == tasks.end() ? nullptr : it->get();
    }

    bool hasTask(const std::string& description) const
    {
        return std::find_if(tasks.begin(), tasks.end(),
                            [&description](const std::unique_ptr<Task>& task)
                            { return PartlyMatch(task->description, description); }) !=
               tasks.end();
    }

    bool hasDoneTask(const std::string& description) const
    {
        const auto done = std::find_if(done_tasks.begin(), done_tasks.end(),
                                       [&description](const std::unique_ptr<Task>& task)
                                       { return PartlyMatch(task->description, description); }) !=
                          done_tasks.end();
        return done;
    }

    bool giveTask(const std::shared_ptr<Thing>& receiver, std::unique_ptr<Task> task)
    {
        if (!receiver || !receiver->_tasker)
            return false;

        if (!task)
            return false;

        if (receiver->tasker()->hasTask(task->description) ||
            receiver->tasker()->hasDoneTask(task->description))
            return false;

        receiver->tasker()->addTask(std::move(task));
        return true;
    }

    void tickTask(int i_task)
    {
        if (i_task >= 0 && static_cast<std::size_t>(i_task) < tasks.size())
            tasks[i_task]->tick = true;
    }

    bool tickTask(const std::string& description)
    {
        auto task = getTask(description);
        if (!task)
            return false;

        task->tick = true;
        return true;
    }

    bool rewardTask(const std::shared_ptr<Thing>& owner, const std::string& description)
    {
        auto it = std::find_if(tasks.begin(), tasks.end(), [&description](const std::unique_ptr<Task>& task)
                               { return task->tick && PartlyMatch(task->description, description); });
        if (it == tasks.end())
            return false;

        onTaskComplete(owner, *(*it));
        done_tasks.push_back(std::move(*it));
        tasks.erase(it);
        return true;
    }

    void onNotify(const std::shared_ptr<Thing>& owner, const std::shared_ptr<Thing>& actor,
                  Event::Type notification_type, const std::shared_ptr<Thing>& target)
    {
        for (auto& task : tasks)
        {
            if (task->onNotify(owner, actor, notification_type, target))
                task->tick = true;
        }
    }

    bool isCompleted()
    {
        return std::find_if(tasks.begin(), tasks.end(),
                            [](const std::unique_ptr<Task>& t) { return !t->tick; }) ==
               tasks.end();
    }

    virtual void onTaskComplete(const std::shared_ptr<Thing>& owner, const Task& task)
    {
        if (owner->_achiever)
            owner->achiever()->getRewards(
                owner, task.description
                           .length()); // The longer the task description, the more XP it gives.
    }

    void doUpdate(const std::shared_ptr<Thing>& owner)
    {
        (void)owner;
    }

    int getDifficulty() { return tasks.size(); }
};

class Thinker
{
  public:
    Thinker() {}

    virtual void doThink(const std::shared_ptr<Thing>& owner, World& world);
};

class Usable
{
  public:
    virtual void onUse(const std::shared_ptr<Thing>& owner, const std::shared_ptr<Thing>& user) {}

    virtual void doUpdate(const std::shared_ptr<Thing>& owner) {}
};

#endif // THING_HPP
