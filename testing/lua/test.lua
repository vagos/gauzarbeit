local myThing = Gauzarbeit.NewThing()

--myThing:SetValue(666);

local otherThing = myThing:CreateFriend();

otherThing:SetValue(588)
myThing:SetValue(888)

print(myThing)
print(otherThing)

function doUpdate(owner)
    print("[LUA] Update!")

    owner:PrintSelf()

    owner:SetValue(999);

    owner:PrintSelf()
end
