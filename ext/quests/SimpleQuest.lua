--[[
function SimpleQuest:doInit()
 
     self.moveTask = self:addTask()
     
 end
 
 function SimpleQuest:onNotify(actor, notification_type)
    
     if (notification_type == Gauzarbeit.Event.Move) then
         self:tickTask(self.moveTask)
     end
 
 end
 
 function SimpleQuest:onInspect()
     return "You can move between rooms with the command MOVE!\n"
end
--]]

Quest = SimpleQuest

function Quest:doInit()

    --self.ratTask = { n = 2, i = self:addTask("Kill 2 Rats.\r\r") }
    -- We set ratTask as a table of a number and an index. The index will be used when the task is completed.

    self.cheeseTask = self:addTask("Pickup a piece of Cheese.") -- We dont have to keep track of the times cheeseTask is completed.
end

function Quest:onNotify(actor, notification_type, target)
--    if (notification_type == Gauzarbeit.Event.Kill and target:getName() == "Rat") then
--        self.ratTask.n = self.ratTask.n - 1
--
--        if self.ratTask.n <= 0 then self:tickTask( self.ratTask.i ) end -- The ratTask was completed
--    end

    if (notification_type == Gauzarbeit.Event.Gain and target:getName() == "Cheese") then
        self:tickTask( self.cheeseTask ) -- The cheese task was completed.

        print("cheese!")
    end

end


function Quest:onInspect(inspector)
    return "Kill those rats and take their food!\n"
end
