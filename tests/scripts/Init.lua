-- Load base init first so existing world setup remains intact.
dofile("./scripts/Init.lua")

-- Add a test room with scripted test entities.
Gauzarbeit.GetRoom("TestLab", 0, 1)
