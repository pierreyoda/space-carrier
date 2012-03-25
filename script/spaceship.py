# -*-coding:Utf-8 -*
"""Spaceship base class"""

import sf
from OiwEngine import Entity
from .entities_tools import loadTexture, originAtCenter
from .module import *
from . import globals
from math import degrees, cos, sin

def id_from_spaceship_class(spaceship):
	return spaceship.class_name + str(spaceship.count)


class SpaceShip (Entity):
	""" Base class for all spaceships.
		Handle damage localization (critical modules can be defined), spaceship depending on the caracteristics...
	"""

	count = 0 # used by ID assignment

	def __init__(self, engine, id=None):
		"""If id is None : a ID will be assigned (ex : 'Fighter3')"""
		Entity.__init__(self, id if id is not None else id_from_spaceship_class(self.__class__))
		subclass = self.__class__
		subclass.count += 1
		# Hull
		if "main" not in self.textures.keys() or self.textures["main"] == None:
			subclass.textures["main"] = loadTexture(subclass.textures_paths["main"], engine)
		self.sprite_hull = sf.Sprite(subclass.textures["main"])
		originAtCenter(self)
		#self.collision_table = CollisionTable(self.sprite.getTexture().copyToImage())

		# Modules slots and modules
		self.modules_slots = { }
		self.initialize_slots(engine)
		self.mount_default_modules(engine)

	def generate_equipment_id(self, name):
		"""Generate an equipment ID (module or module slot)"""
		return "{}-{}".format(self.id, name)

	def draw(self, target, states):
		states.transform *= self.getTransform()
		target.draw(self.sprite_hull, states)
		for slot in self.modules_slots.values():
			target.draw(slot, states)

	def update(self, elapsed_time):
		for slot in self.modules_slots.values():
			slot.update(elapsed_time)
		return True

	def getLocalBounds(self): #TODO : take in acoount modules
		return self.sprite_hull.getLocalBounds()
