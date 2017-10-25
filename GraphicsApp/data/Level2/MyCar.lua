Actor = 
{
	name = "Joe",
	initial_position = { 300.0, -200.0},
	player_controller = "ViehcleController",
	controller_info = {60,0.10},
	bounding_box = 
	{
		center  = {0.0 ,50.0},
		extends = {20.0,45.0}
	},

	physics_settings = 
	{
		mass = 3000.0,
		drag = 3.0,
		rotation_ratio = 1.5
	},

	render_settings = 
	{
		sprite = "data\\RedCar.dds"
	}
}
