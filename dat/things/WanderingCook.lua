Thing = WanderingCook -- This is in case we want to change the name of our "Thing" later.

function Thing:onTalk(talker)

    self:doSay(talker, "Looking to cook, eh?\n")

end
