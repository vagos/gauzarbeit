#include "Talker.hpp"
#include "Thing.hpp"
#include "Quest.hpp"

#include "Helpers.hpp"

#include <iomanip>

void Talker::onTalk(const std::shared_ptr<Thing> &owner, const std::shared_ptr<Thing> talker)
{
    const auto& event = talker -> notifier() -> event;

    auto q = owner -> achiever() -> getQuest(event.object);

    std::clog << event.object << '\n';

    if (!q) return;

    talker -> achiever() -> gainQuest( std::make_shared<ScriptedQuest>(q -> name) );
    
    if (!talker -> _networked) return;

    std::stringstream res;

    res << q -> inspectable() -> onInspect(q, talker) << '\n';

    res << "Quest " << std::quoted( q -> name ) << " added to Quest Log!";

    talker -> networked() -> addResponse( res.str() );
}
