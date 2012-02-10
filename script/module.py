# -*-coding:Utf-8 -*
"""Spaceship's module class"""

import sf
from OiwEngine import Entity
from.entities_tools import loadTexture, spriteSize, originAtCenter
from math import degrees

class ModuleInfo:
	""" Class containing all informations needed to create and handle a space ship module.
		Should be a static variable in the Spaceship's child class.
	"""
	def __init__(self, texture_path=None, rect=None, relative_pos=None, hp=1, draw=False):
		"""If texture_path is None, main modules image will be used;
		if rect is None, the whole image will be used;
		if relative_pos (position on spaceship, starting from top-left) is None, rect will be used to determine it;
		if hp is None, the module will be indestructible (if = 1 : one-shoot);
		setting draw to True : module will be drawn.
		"""
		self.texture_path, self.rect, self.relative_pos, self.hp, self.draw = \
			texture_path, rect, relative_pos, hp, draw


class Module (Entity):
	""" Spaceship's critical internal module, which can be destroyed.
		Used for damages localization.
		Defined by :
		- an ID (must be unique)
		- an image path
		- a drawing and collision rect
		- a relative position on the spaceship
		- optionnal : 'health point'

		==> all that stuff is provided by ModuleInfo
	"""
	def __init__(self, id, engine, infos):
		Entity.__init__(self, id)
		# keep a reference of ModuleInfos
		self.infos = infos
		# load texture
		self.texture = loadTexture(infos.texture_path, engine)
		# drawn subrect - if not specified, use the whole image
		self.rect = infos.rect if infos.rect is not None\
			else sf.IntRect(0, 0, self.texture.GetWidth(), self.texture.GetHeight())
		# module sprite
		self.sprite = sf.Sprite(self.texture, self.rect)
		self.relative_pos = infos.relative_pos if infos.relative_pos is not None else \
			sf.Vector2f(self.rect.Left, self.rect.Top)
		self.SetPosition(self.relative_pos)

	def Draw(self, target, states):
		states.Transform *= self.GetTransform()
		target.Draw(self.sprite, states)

	def update(self, elapsed_time):
		return True

	def getSize(self):
		return spriteSize(self.sprite)

	def update_angle(self):
		self.sprite.SetRotation(degrees(self.angle) - 90.0) # angle is whith the abscissa
