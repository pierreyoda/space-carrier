# -*-coding:Utf-8 -*
"""Spaceship's module turret slot class"""

from sf import Vector2f
from math import atan2, degrees
from .module_slot import ModuleSlot


class ModuleTurretSlot (ModuleSlot):
	""" Spaceship's specific attachment able to mount turrets modules (weapons).
		Adds angle constraints (in degrees).
		Important note : angle of 0 ==> aligned with the spaceship
	"""

	def __init__(self, id, engine, rect, mark, angles, origin_at_center=True):
		"""angles argument is a tuple of type : (left_max, right_max)"""
		ModuleSlot.__init__(self, id, engine, rect, ModuleSlot.types.TURRET,
			mark, origin_at_center)
		self.angles = angles
		self.turret_center = Vector2f(rect.Left+rect.Width/2,
			rect.Top+rect.Height/2)

	def mount_module(self, module):
		if not ModuleSlot.mount_module(self, module):
			return False
		# "center" turret
		module.angle = (self.angles[1] - self.angles[0]) / 2

	def align_on(self, pos):
		"""Point mounted turret module on the given position,
			if allowed by angle constraints."""
		# -90 cause angle is with the spaceship
		# TODO : - rotating turret (not an immediate alignment)
		#		 - angle constraints
		#        - handle very near positions (otherwise weird angles)
		self.mounted_module.angle = degrees(atan2(pos.y, pos.x))+90
