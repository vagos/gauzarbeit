function Winston:onUse(event)
    event.actor:sendMessage("You just smoked a winston cigarrete!\nYou feel healthier...\n")
    event.actor:loseItem(self)
end
