function Glove:onUse(user)

    print(user:getEventInfo().object)
    
    targetPlayer = user:getPlayer( user:getEventInfo().object )

    print(targetPlayer)

    if (targetPlayer) then
        targetPlayer:sendMessage(user:getName().." slapped you!\n")
    end

end
