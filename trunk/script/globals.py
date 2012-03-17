# -*-coding:Utf-8 -*
"""SpaceCarrier's global variables"""

# Debug
debug = False

# Events
action_map = None

# Game
player_refresh_turrets_angle = 0.1 # (in seconds)

# Scren size and camera
camera = None

screen_size = (800, 600)
def in_screen(pos):
	return (pos.x >= 0 and pos.x <= screen_size[0]
		and pos.y >= 0 and pos.y <= screen_size[1])

