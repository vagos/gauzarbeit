function Hat:doInit()

end

function Hat:onUse(user)
    if (not user:hasItem(self)) then
        return
    end

    user:sendMessage("You wore a pretty hat!\n")
end
