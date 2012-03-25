# -*-coding:Utf-8 -*
"""Spaceship's module slot class"""

from sf import Vector2f, FloatRect
from OiwEngine import Entity
from .entities_tools import originAtCenter
from .python_tools import enum
from .module import Module, ModuleEmpty


class ModuleSlot (Entity):
	""" Spaceship's attachment able to mount modules.
		Defined by :
		- a sf.IntRect (-> relative position and size)
		- a type
			> multipurpose : anything but engine modules
			> turret	   : only turret-based (weapons) modules
			> generator	   : only generator (shield or power) modules
			> engine	   : only engine modules
		- a mark (advancement level, determines which module you can mount)
		Invisible by default, but the Draw functions can be redefined in
			the derived classes (joints...).
	"""

	types = enum("MULTIPURPOSE", "TURRET", "GENERATOR", "ENGINE")

	def __init__(self, id, engine, rect, type, mark, origin_at_center=True):
		Entity.__init__(self, id)
		self.type, self.mark = type, mark
		self.size = Vector2f(rect.width, rect.height)
		self.pos = (rect.left + self.size.x, rect.top + self.size.y) # don't know why but needed...
		#self.pos = (rect.left, rect.top)
		if origin_at_center:
			originAtCenter(self)
		self.mounted_module = ModuleEmpty(engine, self.id+"-MODULE_EMPTY")

	def mount_module(self, module):
		if not self.is_module_compatible(module):
			print("Cannot mount module {0} in slot of ID : {1}".format(
				module.module_name, self.id))
			return False
		self.mounted_module = module
		return True

	def is_empty(self):
		return isinstance(self.mounted_module, ModuleEmpty)

	def is_module_compatible(self, module):
		# invalid module class
		if not isinstance(module, Module):
			return False
		# multipurpose
		if (self.type == ModuleSlot.types.MULTIPURPOSE and
			module.properties["type"] != Module.types.ENGINE):
			return True
		# turret
		if (self.type == ModuleSlot.types.TURRET and
			module.properties["turret_based"]):
			return True
		# power or shield generator
		elif (self.type == ModuleSlot.types.GENERATOR and
			(module.affects["power"] > 0 or module.affects["power"] > 0)):
			return True
		# engine
		elif (self.type == ModuleSlot.types.ENGINE and
			module.properties["type"] == Module.types.ENGINE):
			return True
		return False

	def draw(self, target, states):
		states.transform *= self.getTransform()
		target.draw(self.mounted_module, states)

	def update(self, elapsed_time):
		self.mounted_module.update(elapsed_time)
		return True

	def getLocalBounds(self):
		return FloatRect(0, 0, self.size.x, self.size.y)
