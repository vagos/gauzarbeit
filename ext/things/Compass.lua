Thing = Compass

local function room_name_at(x, y)
    local room = Gauzarbeit.GetRoom(x, y)
    if room and room.name and string.len(room.name) > 0 then
        return room.name
    end

    return "unknown"
end

local function build_compass_reading(user)
    local current_room = user:getRoom()
    local x = current_room and current_room.x
    local y = current_room and current_room.y
    if not x or not y then
        return "The compass cannot find where you are.\n"
    end

    return "Compass\n" ..
        "Coordinates: (" .. x .. ", " .. y .. ")\n" ..
        "North: " .. room_name_at(x, y + 1) .. "\n" ..
        "West: " .. room_name_at(x - 1, y) .. "\n" ..
        "South: " .. room_name_at(x, y - 1) .. "\n" ..
        "East: " .. room_name_at(x + 1, y) .. "\n"
end

function Compass:onInspect(inspector)
    return "A brass compass marked with room names.\n"
end

function Compass:onUse(user)
    user:sendMessage(build_compass_reading(user))
end
