#include "PlayerThinker.hpp"

#include "../Thing.hpp"
#include "../Room.hpp"

void PlayerThinker::doThink(const std::shared_ptr<Thing> &owner)
{
    owner->notifier()->setEvent(owner);

    const auto& event = owner->notifier()->event;

    switch (event.type)
    {
        case Event::Type::Attack:
        {
            auto enemy = owner -> physical() -> getRoom() -> getAnything( event.target );

            if (!enemy) throw TargetNotFound();

            DoFight( owner, enemy );

            break;
        }

        case Event::Type::Leave:
        {
            owner->networked()->addResponse("Goodbye!\n");
            owner->networked()->doDatabaseStore(owner);
            owner->networked()->doDisconnect(owner);
            break;
        }
    }
}
