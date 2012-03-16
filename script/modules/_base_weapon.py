# -*-coding:Utf-8 -*
"""Module : base - weapon"""

from ..module import Module

class ModuleBaseWeapon (Module):
	"""Abstract class for (turret-based) weapon modules"""

	def __init__(self, engine, id):
		Module.__init__(self, engine, id)
		self.properties["type"] = Module.types.WEAPON
