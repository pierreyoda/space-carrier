# -*-coding:Utf-8 -*
"""Module : base - weapon"""

from ..module import Module

class ModuleBaseWeapon (Module):
	""" Abstract class for (turret-based) weapon modules.
		Extra properties :
			"rotation_speed" : in degree per second
	"""

	default_rotation_speed = 90 # °/s

	def __init__(self, engine, id):
		Module.__init__(self, engine, id)
		self.properties["type"] = Module.types.WEAPON
		self.properties["turret_based"] = True
		self.properties["rotation_speed"] = ModuleBaseWeapon.default_rotation_speed
