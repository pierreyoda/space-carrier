# -*-coding:Utf-8 -*
"""Spaceship base class"""

import sf
from OiwEngine import Entity
from .entities_tools import loadTexture, spriteSize, originAtCenter
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
		if "hull" not in self.textures.keys() or self.textures["hull"] == None:
			subclass.textures["hull"] = loadTexture(subclass.textures_paths["hull"], engine)
		self.sprite_hull = sf.Sprite(subclass.textures["hull"])
		originAtCenter(self)
		self.modules = { }
		#self.collision_table = CollisionTable(self.sprite.GetTexture().CopyToImage())
		self.init_modules(engine)

	def init_modules(self, engine):
		""" Init spaceship modules according to these static class variables :
		In SpaceShip child : textures_paths = {'hull':..., 'modules':..., [ + optionnal : other images] }"""
		modules_infos = self.__class__.modules_infos
		if len(modules_infos) == 0:
			return # no defined modules
		# Instanciate all modules
		self.modules = { }
		for name, info in modules_infos.items():
			if info.texture_path == None:
				info.texture_path = self.textures_paths["modules"]
			self.modules[name] = Module("{}-{}".format(self.id, name), engine,
				modules_infos[name])

	def Draw(self, target, states):
		states.Transform *= self.GetTransform()
		if globals.debug:
			target.Draw(self.sprite_hull, states)
			for module in self.modules.values():
					target.Draw(module, states)
		else:
			for module in self.modules.values():
				if module.infos.draw:
					target.Draw(module, states)
			target.Draw(self.sprite_hull, states)

	def update(self, elapsed_time):
		return True

	def getLocalBounds(self): #TODO : take in acoount modules
		return self.sprite_hull.GetLocalBounds()
