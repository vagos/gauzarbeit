function Table:doInit()

end

function Table:doDatabaseSave()
    local items = self:getThings()
    local saved = { tostring(#items) }

    for _, thing in ipairs(items) do
        table.insert(saved, thing:getName())
    end

    return table.concat(saved, "\n") .. "\n"
end

function Table:doDatabaseLoad()
    local count = tonumber(Gauzarbeit.GetDBLine())
    if not count then
        return
    end

    for _, thing in ipairs(self:getThings()) do
        self:loseItem(thing)
    end

    for _ = 1, count do
        local item_name = Gauzarbeit.GetDBLine()
        if not item_name or item_name == "" then
            break
        end

        self:gainItem(item_name)
    end
end

function Table:onInspect(inspector)
    local contentList = "Currently on it:"
    for _, thing in ipairs(self:getThings()) do
        contentList = contentList .. thing:getName() .. ", "
    end
    return "A sturdy table that can hold various items.\n" .. contentList .. "\n"
end
