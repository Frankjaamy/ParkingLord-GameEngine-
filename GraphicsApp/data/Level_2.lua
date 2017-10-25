Level = {
	level_name = "Level_2",
	player = "data/Level2/MyCar.lua",
	player_initial_position = { 300.0, -200.0},
	level_row_numbers = 3,
	level_row_data = {"row1", "row2", "row3"},
	row1 = {
		initial_position = { -360,0 },
		number_slots = 8,
		distance_between = 80,
		empty_slot_size = 2,
		empty_slot = {3,4},
		car = "data/Level2/BlueCar.lua"
	},
	row2 = {
		initial_position = { -360,-120 },
		number_slots = 8,
		distance_between = 80,
		empty_slot_size = 2,
		empty_slot = {4,5},
		car = "data/Level2/BlueCar.lua"
	},
	row3 = {
		initial_position = { -360,-240 },
		number_slots = 8,
		distance_between = 80,
		empty_slot_size = 2,
		empty_slot = {3,4},
		car = "data/Level2/BlueCar.lua"
	},
	level_finish_position = {-120,-240}
}