# -*-coding:Utf-8 -*
"""Module : power generator, mark III"""

from ..module import Module

class ModulePowerGeneratorMK3 (Module):
	"""Power Generator, mark III."""

	module_name  = "PowerGeneratorMK3"
	texture 	 = None
	texture_path = "data/shield_generator_mk3.png"

	def __init__(self, engine, id):
		Module.__init__(self, engine, id)
		self.properties["hp"]	= 1000
		self.properties["mark"] = 3
		self.properties["cost"] = 250000
		self.affects["power"]	= 25 # +25 POWER
		self.angle = 0
