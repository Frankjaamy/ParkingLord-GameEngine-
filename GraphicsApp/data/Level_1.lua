Level = {
	level_name = "Level_1",
	player = "data/Level1/MyCar.lua",
	player_initial_position = { 300.0, -200.0},
	level_row_numbers = 2,
	level_row_data = {"row1", "row2", "row3"},
	row1 = {
		initial_position = { -360,-120 },
		number_slots = 8,
		distance_between = 80,
		empty_slot_size = 1,
		empty_slot = {3},
		car = "data/Level1/BlueCar.lua"
	},
	row2 = {
		initial_position = { -360,-240 },
		number_slots = 8,
		distance_between = 80,
		empty_slot_size = 0,
		empty_slot = {-1},
		car = "data/Level1/BlueCar.lua"
	},
	level_finish_position = {-120,-120}
}