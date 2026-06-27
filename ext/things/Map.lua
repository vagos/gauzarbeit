Thing = Map

local MAP_WIDTH = 20
local MAP_HEIGHT = 10

local function build_world_map(user)
    local rooms = Gauzarbeit.GetRoom()
    if not rooms or #rooms == 0 then
        return "World map is empty.\n"
    end

    local current_room = user:getRoom()
    local room_by_key = {}
    local current_x = current_room and current_room.x
    local current_y = current_room and current_room.y
    local current_key = current_x and current_y and (current_x .. "," .. current_y) or nil

    for _, room in ipairs(rooms) do
        local key = room.x .. "," .. room.y
        room_by_key[key] = room
    end

    if not current_x or not current_y then
        return "You are nowhere on this map.\n"
    end

    local min_x = current_x - math.floor(MAP_WIDTH / 2)
    local max_x = min_x + MAP_WIDTH - 1
    local min_y = current_y - math.floor(MAP_HEIGHT / 2)
    local max_y = min_y + MAP_HEIGHT - 1

    local lines = {}
    table.insert(lines, "World Map")
    table.insert(lines, "Legend: # = you, @ = room, . = empty")
    table.insert(lines, "")

    for y = max_y, min_y, -1 do
        local row = ""
        for x = min_x, max_x do
            local key = x .. "," .. y
            local room = room_by_key[key]
            if key == current_key then
                row = row .. Gauzarbeit.ColorString("#", Gauzarbeit.Color.Red)
            else
                row = row .. (room and "@" or ".")
            end
        end
        table.insert(lines, row)
    end

    return table.concat(lines, "\n") .. "\n"
end

function Map:onInspect(inspector)
    return "A folded map of the world.\n"
end

function Map:onUse(event)
    event.actor:sendMessage(build_world_map(event.actor))
end
