function RPS:doInit()
    self.enemyPlayer = {ptr = nil, choice = nil}
    self.originalPlayer = {ptr = nil, choice = nil}

    print("Created!")

    function self:runGame()
        print(self.originalPlayer.choice)
        print(self.enemyPlayer.choice)

        r_o = self.originalPlayer.choice == "R"
        p_o = self.originalPlayer.choice == "P"
        s_o = self.originalPlayer.choice == "S"

        r_e = self.enemyPlayer.choice == "R"
        p_e = self.enemyPlayer.choice == "P"
        s_e = self.enemyPlayer.choice == "S"

        o_player_won = (r_o and s_e) or (p_o and r_e) or (s_o and p_e)

        winner = o_player_won and self.originalPlayer or self.enemyPlayer

        game_msg = self.originalPlayer.ptr:getName()..": "..self.originalPlayer.choice.."\n"..
        self.enemyPlayer.ptr:getName()..": "..self.enemyPlayer.choice.."\n"..
        "Winner: "..winner.ptr:getName().."\n"

        self.originalPlayer.ptr:sendMessage(game_msg)
        self.enemyPlayer.ptr:sendMessage(game_msg)

        self.originalPlayer.ptr = nil
        self.enemyPlayer.ptr = nil
    end

end


function RPS:onUse(event)

    if self.enemyPlayer.ptr ~= nil then 
        self.originalPlayer.choice = event.object
        self.originalPlayer.ptr = event.actor
        return
    end
    self.enemyPlayer.ptr = event.actor:getPlayer(event.object)

    if self.enemyPlayer.ptr then
        local msg = event.actor:getName().." has challenged you to a game of Rock, Paper, Scissors!\n"
        self.enemyPlayer.ptr:sendMessage(msg)

        event.actor:sendMessage("You have challenged "..self.enemyPlayer.ptr:getName().."!\n"..
        "Type 'use RPS {choice}' to make a move!\n")
    else
        event.actor:sendMessage("Player not found!\n")
    end
    if self.enemyPlayer.choice and self.originalPlayer.choice then self:runGame() end

end


function RPS:onNotify(event)

    if (self.enemyPlayer.ptr == nil or event.actor:getName() ~= self.enemyPlayer.ptr:getName()) then 
        return
    end

    if event.type == Gauzarbeit.Event.Info then
        self.enemyPlayer.choice = event.extra 

        if self.enemyPlayer.choice and self.originalPlayer.choice then self:runGame() end
    end

end

function RPS:onHelp(user)
    return "Do USE RPS {player-name} to challenge another player.\n"
end
