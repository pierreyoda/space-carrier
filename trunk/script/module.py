# -*-coding:Utf-8 -*
"""Spaceship's module class"""

from .entities_tools import SpriteBasedEntity, originAtCenter
from .python_tools import enum


class Module (SpriteBasedEntity):
	""" Equipment mounted on a SpaceShip's module slot, which must be compatible.
		Inherits from SpriteBasedEntity, but most advanced behaviors can be
			implemented with 'Draw' and 'update' functions.
		Defined by :
		- a module name (static)
		- a texture path (static)
		- an ID (must be unique)
		- a dictionary of internal module properties (if None : undefined)
			"type"	 = see Module.types enum
			"mark"   = determines compatibility with module slots
			"hp"     = hitpoints
			"cost"   = module's price
			"weight"
		- a dictionary of affects on the spaceship (positive : generates, negative : consumes)
			"power" = energy
			"shield_capacity"
			"shield_regeneration_rate"
	"""

	types = enum("UNDEFINED", "WEAPON", "GENERATOR", "ENGINE")

	def __init__(self, engine, id):
		SpriteBasedEntity.__init__(self, engine, id)
		originAtCenter(self)
		self.angle = 0
		self.properties = {"type":Module.types.UNDEFINED, "mark":1, "hp":None,
			"cost":0, "weight":0}
		self.affects = {"power":0}

	def update(self, elapsed_time):
		return True


class ModuleEmpty (Module):
	"""Empty module, mounted by default"""

	module_name  = "EMPTY_MODULE"
	texture		 = None
	texture_path = "data/module_empty.png"

	def __init__(self, engine, id):
		Module.__init__(self, engine, id)
