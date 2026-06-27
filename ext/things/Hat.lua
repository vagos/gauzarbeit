function Hat:doInit()

end

function Hat:onUse(event)
    if (not event.actor:hasItem(self)) then
        return
    end

    event.actor:sendMessage("You wore a pretty hat!\n")
end
