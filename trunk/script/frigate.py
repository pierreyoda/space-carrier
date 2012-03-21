# -*-coding:Utf-8 -*
"""Fighter"""

from sf import IntRect, Mouse
from . import globals
from .spaceship import SpaceShip
from .module_slot import ModuleSlot
from .module_turret_slot import ModuleTurretSlot
from .modules.laser_turret_mk2 import *
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
		self.engine = engine
		self.angle = 0
		self.pos = (400, 300)
		self.turrets_angle_refresh_time = 0

	def initialize_slots(self, engine):
		"""Define all the Frigate slots"""
		self.modules_slots["multipurpose_rear"] = ModuleSlot(
			self.generate_equipment_id("slot_multipurpose_rear"),
			engine, IntRect(17, 319, 48, 46), ModuleSlot.types.MULTIPURPOSE, 3)
		self.modules_slots["multipurpose_front"] = ModuleTurretSlot(
			self.generate_equipment_id("multipurpose_front"),
			engine, IntRect(15, 69, 50, 48), 3, (-130, 130))

	def mount_default_modules(self, engine):
		"""Mount all the default modules"""
		self.modules_slots["multipurpose_rear"].mount_module(
			ModulePowerGeneratorMK3(engine,
				self.generate_equipment_id("power_generator")))
		self.modules_slots["multipurpose_front"].mount_module(
			ModuleLaserTurretMK2(engine,
				self.generate_equipment_id("laser_turret")))
		# list of slots with weapons mounted
		self.weapon_slots_list = ["multipurpose_front"]

	def update(self, elapsed_time):
		if not SpaceShip.update(self, elapsed_time):
			return False
		elapsed_seconds = elapsed_time.AsSeconds()
		if globals.action_map.IsActive("player_rotate_left"):
			self.angle -= elapsed_seconds * 250
		elif globals.action_map.IsActive("player_rotate_right"):
			self.angle += elapsed_seconds * 250

		# turrets angle refresh (point on mouse)
		self.turrets_angle_refresh_time += elapsed_seconds
		if self.turrets_angle_refresh_time >= globals.player_refresh_turrets_target_angle:
			self.turrets_angle_refresh_time = 0
			mouse_pos = Mouse.GetPosition(self.engine.getRenderWindow())
			if globals.in_screen(mouse_pos):
				self.point_weapons_on(self.engine.getRenderWindow().ConvertCoords(
					mouse_pos.x, mouse_pos.y))

		return True

	def point_weapons_on(self, pos):
		for weapon_slot_name in self.weapon_slots_list:
			weapon_slot = self.modules_slots[weapon_slot_name]
			direction = pos - self.GetTransform().TransformPoint(
				weapon_slot.turret_center)
			weapon_slot.align_on(direction)
