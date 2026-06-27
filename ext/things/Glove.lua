function Glove:onUse(event)

    print(event.object)
    
    targetPlayer = event.actor:getPlayer(event.object)

    print(targetPlayer)

    if (targetPlayer) then
        targetPlayer:sendMessage(event.actor:getName().." slapped you!\n")
    end

end
