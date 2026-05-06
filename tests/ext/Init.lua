-- Load base init first so existing world setup remains intact.
dofile("./ext/Init.lua")

-- Add a test room with scripted test entities.
Gauzarbeit.GetRoom("TestLab", 0, 1)
