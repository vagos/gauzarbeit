function Winston:onUse(user)
    user:sendMessage("You just smoked a winston cigarrete!\nYou feel healthier...\n")
    user:loseItem(self)
end
