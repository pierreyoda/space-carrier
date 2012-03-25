# -*-coding:Utf-8 -*
"""Spaceship's module turret slot class"""

from sf import Vector2f
from math import atan2, degrees
from .module_slot import ModuleSlot


class ModuleTurretSlot (ModuleSlot):
	""" Spaceship's specific attachment able to mount turrets modules (weapons).
		Adds angle constraints (in degrees).
		Important note : angle of 0 ==> aligned with the spaceship
		 (conversion from normal angle : +90).
	"""

	def __init__(self, id, engine, rect, mark, angles, origin_at_center=True):
		"""angles argument is a tuple of type : (left_max, right_max)"""
		ModuleSlot.__init__(self, id, engine, rect, ModuleSlot.types.TURRET,
			mark, origin_at_center)
		self.angles = angles
		self.target_angle = 0
		self.rotated_angle = 0
		self.turret_center = Vector2f(rect.left+rect.width/2,
			rect.top+rect.height/2)

	# TODO : better left/right rotation decision
	def update(self, elapsed_time):
		if not ModuleSlot.update(self, elapsed_time):
			return False
		# turret rotation
		current_angle = self.mounted_module.angle
		diff = (current_angle - self.target_angle) % 360
		if diff < 1:   # useless
			return True
		# Rotate left
		if diff < 180:
			delta = -self.rotation_speed * elapsed_time.asSeconds()
			if self.rotated_angle+delta > self.angles[0]:	# limit check
				self.mounted_module.rotate(delta)
				self.rotated_angle += delta
		# Rotate right
		else:
			delta = self.rotation_speed * elapsed_time.asSeconds()
			if self.rotated_angle+delta < self.angles[1]:	# limit check
				self.mounted_module.rotate(delta)
				self.rotated_angle += delta
		return True

	def mount_module(self, module):
		if not ModuleSlot.mount_module(self, module):
			return False
		# "center" turret
		module.angle = (self.angles[0] + self.angles[1]) / 2
		self.target_angle = module.angle
		self.rotation_speed = module.properties["rotation_speed"]

	def align_on(self, direction, spaceship_angle):
		"""Point mounted turret module on the given position,
			if allowed by angle constraints."""
		# +90 cause angle is with the spaceship
		if abs(direction.x) < 15.0 and abs(direction.y) < 15.0:
			return # very near directions : ignore, otherwise provokes weird angles
		self.target_angle = (degrees(atan2(direction.y, direction.x))+90) - spaceship_angle
