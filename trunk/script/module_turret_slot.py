# -*-coding:Utf-8 -*
"""Spaceship's module turret slot class"""

from .module_slot import ModuleSlot


class ModuleTurretSlot (ModuleSlot):
	""" Spaceship's specific attachment able to mount turrets modules (weapons).
		Adds angle constraints (in degrees).
		Important note : angle of 0 ==> right
	"""

	def __init__(self, id, engine, rect, mark, angles, origin_at_center=True):
		"""angles argument is a tuple of type : (min_angle, max_angle)"""
		ModuleSlot.__init__(self, id, engine, rect, ModuleSlot.types.TURRET,
			mark, origin_at_center)


