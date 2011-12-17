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
		if "hull" not in self.textures.keys() or self.textures["hull"] == None:
			subclass.textures["hull"] = loadTexture(subclass.textures_paths["hull"], engine)
		self.sprite_hull = sf.Sprite(subclass.textures["hull"])
		originAtCenter(self.sprite_hull)
		self.modules = { }
		self.update_angle()
		self.sprite_hull.SetPosition(self.pos)
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

	def render(self, target):
		if globals.debug:
			target.Draw(self.sprite_hull)
			for module in self.modules.values():
					module.render(target)
		else:
			for module in self.modules.values():
				if module.infos.draw:
					module.render(target)
			target.Draw(self.sprite_hull)

	def update(self, elapsed_time):
		return True

	def update_angle(self):
		self.sprite_hull.SetRotation(degrees(self.angle) - 90.0) # angle is whith the abscissa
		for module in self.modules.values():
			module.angle = self.angle
			module.update_angle()
		self.update_modules_pos()

	def update_modules_pos(self):
		size = self.getSize()
		for module in self.modules.values():
			module.pos = self.sprite_hull.TransformToGlobal(module.relative_pos+module.sprite.GetSize()/2)
			module.sprite.SetPosition(module.pos)

	def getSize(self): #TODO : take in acoount modules
		return sf.Vector2f(self.sprite_hull.GetSubRect().Width,
			self.sprite_hull.GetSubRect().Height)

	def transformToLocal(self, point):
		return self.sprites[hull].TransformToLocal(point)

	def transformToGlobal(self, point):
		return self.sprites[hull].TransformToGlobal(point)
