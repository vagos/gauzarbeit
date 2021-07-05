// #ifndef CHARACTER_HPP
// #define CHARACTER_HPP
// 
// #include "../Thing.hpp"
// #include "../Script/ScriptedThing.hpp"
// 
// #include "../Quest.hpp"
// #include <memory>
// 
// class ShopNotifier : public Notifier
// {
//    void onNotify(const std::shared_ptr<Thing> &owner, const std::shared_ptr<Thing> &actor, Event::Type notification_type, const std::shared_ptr<Thing> &target) override
//    {
//        if ( notification_type == Event::Type::Buy ) 
//        {
//             
//            //assert(actor -> notifier);
//           
//            auto event = actor -> notifier -> event;
// 
//            auto t = owner -> physical -> getItem( actor -> notifier -> event.object );
// 
//            if (!t)    
//            {
//                assert(actor -> networked);
//             
//                actor -> networked -> addResponse("I don't have that item!\n");
// 
//                return;
//            }
// 
//            owner -> physical -> giveItem( actor, t );
// 
//            assert(actor -> networked);
// 
//            actor -> networked -> addResponse("Here is your " + t -> name + "!\n");
//       }
// 
//        else if ( notification_type == Event::Type::Ask )
//        {
//             owner -> talker -> onTalk(owner, actor);
//        }
//    }
// 
// };
// 
// 
// class TestTalker : public Talker
// {
// 
//     void doUpdate(const std::shared_ptr<Thing> &owner) override
//     {
// 
//     }
// 
//     void onTalk(const std::shared_ptr<Thing> &owner, const std::shared_ptr<Thing> talker) override
//     {
//         assert(talker -> notifier && talker -> networked);
// 
//         const auto& event = talker->notifier->event; 
// 
//         if (event.object == "quests")
//         {
//             showQuests(owner, talker);
//         }
//     }
// 
// private:
// 
//     void showQuests(const std::shared_ptr<Thing>& owner, const std::shared_ptr<Thing>& talker)
//     {
//         if (!owner -> achiever -> quests.size())
//         {
//             talker -> networked -> addResponse("I don't have any quests sadly!\n");
// 
//             // make this no_quest_response
//         }
// 
//         talker -> networked -> addResponse("Here are my quests: \n");
// 
//         talker -> networked -> addResponse( VerticalListString(owner -> achiever -> quests, '*',
//                     [](const Thing& t) {return t.name;}) );
//     }
// 
// 
// };
// 
// class Character : public Thing
// {
// 
// public:
//     Character(const std::string& name): Thing(name)
//     {
//         physical = std::make_shared<Physical>();
//         notifier = std::make_shared<ShopNotifier>();
//         talker = std::make_shared<TestTalker>();
//         achiever = std::make_shared<Achiever>();
// 
//         physical -> gainItem( std::make_shared<ScriptedThing>("Winston") );
// //        achiever -> addQuest( std::make_shared<ScriptedQuest>("Ratz")  );
//     }
// 
// 
// };
// 
// #endif
