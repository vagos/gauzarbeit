Thing = Notepad

function Thing:doInit()
    self.text = ""
end

function Thing:onUse(user)
    
    event = user:getEventInfo()

    text = event.extra

    if event.object == "add" and string.len(text) then
        user:sendMessage("Written to Notepad: "..text.."\n")
        self.text = self.text.."\n"..text
    end
end

function Thing:onInspect(inspector)
    return "Notepad: "..self.text.."\n\n"
end
