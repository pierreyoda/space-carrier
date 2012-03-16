# -*-coding:Utf-8 -*
"""Module : laser turret, mark II"""

from ._base_weapon import ModuleBaseWeapon

class ModuleLaserTurretMK2 (ModuleBaseWeapon):
	"""Laser Turret, mark II"""

	module_name  = "LaserTurretMK2"
	texture 	 = None
	texture_path = "data/laser_turret_2.png"

	def __init__(self, engine, id):
		ModuleBaseWeapon.__init__(self, engine, id)
		self.SetOrigin(15, 47)
		self.properties["mark"] = 2
		self.properties["hp"]	= 2500
		self.properties["cost"] = 150000
		self.affects["power"]	= -10
