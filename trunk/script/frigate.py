# -*-coding:Utf-8 -*
"""Fighter"""

from sf import IntRect
from . import globals
from .spaceship import SpaceShip
from .module_slot import ModuleSlot
from .modules.power_generator_mk3 import *
from .entities_tools import originAtCenter

class Frigate (SpaceShip):
	"""Frigate class"""
	class_name = "Frigate"
	textures_paths = {
		"main":"data/frigate.png" }
	textures = { }

	def __init__(self, engine, id=None):
		SpaceShip.__init__(self, engine, id)
		self.angle = 0
		self.pos = (400, 300)

	def initialize_slots(self, engine):
		"""Define all the Frigate slots"""
		self.modules_slots["multipurpose_rear"] = ModuleSlot(
			self.generate_equipment_id("slot_multipurpose_rear"),
			engine, IntRect(8, 159, 24, 24), ModuleSlot.types.MULTIPURPOSE, 3)
		originAtCenter(self.modules_slots["multipurpose_rear"])

	def mount_default_modules(self, engine):
		"""Mount all the default modules"""
		self.modules_slots["multipurpose_rear"].mount_module(
			ModulePowerGeneratorMK3(engine,
				self.generate_equipment_id("power_generator")))

	def update(self, elapsed_time):
		if not SpaceShip.update(self, elapsed_time):
			return False
		if globals.action_map.IsActive("player_rotate_left"):
			self.angle -= elapsed_time.AsSeconds() * 250
		elif globals.action_map.IsActive("player_rotate_right"):
			self.angle += elapsed_time.AsSeconds() * 250
		return True
