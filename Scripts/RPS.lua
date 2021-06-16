function RPS:doInit()
    self.enemyPlayer = {ptr = nil, choice = nil}
    self.originalPlayer = {ptr = nil, choice = nil}

    print("Created!")

    function self:runGame()
        print("RUNNING THE GAME")

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


function RPS:onUse(user)

    print(self.enemyPlayer.ptr)

    if self.enemyPlayer.ptr ~= nil then 
        self.originalPlayer.choice = user:getEventInfo().extra
        self.originalPlayer.ptr = user
        return
    end
        
    self.enemyPlayer.ptr = user:getPlayer( user:getEventInfo().extra )

    if self.enemyPlayer.ptr then 
        local msg = user:getName().." has challenged you to a game of Rock, Paper, Scissors!\n"
        self.enemyPlayer.ptr:sendMessage(msg) 

        user:sendMessage("You have challenged "..self.enemyPlayer.ptr:getName().."!\n"..
        "Type 'use RPS {choice}' to make a move!\n")
    else 
        user:sendMessage("Player not found!\n")
    end
    
    if self.enemyPlayer.choice and self.originalPlayer.choice then self:runGame() end

end


function RPS:onNotify(actor, notification_type, target)

    if (self.enemyPlayer.ptr == nil or actor:getName() ~= self.enemyPlayer.ptr:getName()) then 
        return
    end

    if notification_type == Gauzarbeit.Event.Info then
        self.enemyPlayer.choice = actor:getEventInfo().extra 

        if self.enemyPlayer.choice and self.originalPlayer.choice then self:runGame() end
    end  

end
