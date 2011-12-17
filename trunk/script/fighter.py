# -*-coding:Utf-8 -*
"""Fighter"""

from sf import IntRect, Color
from .spaceship import *
from .module import *
from .entities_tools import half_pi
from . import globals

class Fighter (SpaceShip):
	"""Fighter class"""
	class_name = "Fighter"
	textures_paths = {
		"hull":"data/fighter_hull.png",
		"modules":"data/fighter_modules.png",
		"module_robot":"data/module_robot.png" }
	textures = { }
	modules_infos = {
		"engine1":ModuleInfo(rect = IntRect(45, 450, 85, 129), hp = 100, draw = True),
		"engine2":ModuleInfo(rect = IntRect(260, 450, 85, 129), hp = 100, draw = True),
		"canon1" :ModuleInfo(rect = IntRect(5, 395, 35, 65), hp = 100),
		"canon2" :ModuleInfo(rect = IntRect(350, 390, 10, 50), hp = 100),
		"sensor" :ModuleInfo(rect = IntRect(140, 0, 110, 70), draw = True),
		"pilot"	 :ModuleInfo(rect = IntRect(180, 200, 30, 30)),
		"robot"  :ModuleInfo(textures_paths["module_robot"], relative_pos = sf.Vector2f(90, 402), draw=True),
		"targeting_system":ModuleInfo(rect = IntRect(60, 230, 85, 145))
	}

	def __init__(self, engine, id=None):
		SpaceShip.__init__(self, engine, id)
		self.angle = half_pi # pi/2 = 90°
		self.pos = (400, 300)
		self.sprite_hull.SetPosition(self.pos)
		self.update_angle()

	def update(self, elapsed_time):
		global action_map

		if not SpaceShip.update(self, elapsed_time):
			return False
		if globals.action_map.IsActive("player_rotate_left"):
			self.angle -= elapsed_time / 500
			self.update_angle()
		elif globals.action_map.IsActive("player_rotate_right"):
			self.angle += elapsed_time / 500
			self.update_angle()
		return True
