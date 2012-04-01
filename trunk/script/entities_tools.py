# -*-coding:Utf-8 -*
"""Entities-related class and functions"""

from math import pi, degrees
from OiwEngine import Entity, CollisionTable, EntityCollisions
from decimal import Decimal
from . import globals
import sf, thor


def loadTexture(path, engine):
	"""Load a texture by path, raise an exception if failed"""
	texture_key = thor.Resources.TextureKey.fromFile(path)
	texture		= engine.loadTexture(texture_key)
	if not texture.valid():
		raise RuntimeError("cannot load texture '" + path + "'")
	return texture.get()

def loadFont(path, engine):
	"""Load a font by path, raise an exception if failed"""
	font_key = thor.Resources.FontKey.fromFile(path)
	font	 = engine.loadFont(font_key)
	if not font.valid():
		raise RuntimeError("cannot load font '" + path + "'")
	return font.get()

def entitySize(entity):
	"""Compute an entity's size, taking in account its scale"""
	bounds, scale = entity.getLocalBounds(), entity.getScale()
	return sf.Vector2f(bounds.width * scale.x, bounds.height * scale.y)

def originAtCenter(entity):
	"""Define entity's origin at his center"""
	bounds = entity.getLocalBounds()
	entity.setOrigin(bounds.width/2, bounds.height/2)


class SpriteBasedEntity (Entity):
	"""Base class for sprite-based entities"""

	show_collision_boxes  = False	# Draw collision boxes (used by pixel perfect test)
	collision_boxes_color = sf.Color(255, 175, 200, 150)

	def __init__(self, engine, id, pos = sf.Vector2f(), angle = 90):
		Entity.__init__(self, id, pos, angle)
		subclass = self.__class__ # provides access to subclass static variables
		if not isinstance(subclass.texture, sf.Texture):
			subclass.texture = loadTexture(subclass.texture_path, engine)
		self.sprite = sf.Sprite(subclass.texture)
		self.collision_table = CollisionTable(self.sprite.getTexture().copyToImage())
		if SpriteBasedEntity.show_collision_boxes:
			self.collision_rect = sf.RectangleShape()
			self.collision_rect.setFillColor(SpriteBasedEntity.collision_boxes_color)
		else: # avoid unnecessary memory consumption
			self.collision_rect = None

	def draw(self, target, states):
		if self.collision_rect != None:
			rect = self.getGlobalBounds()
			self.collision_rect.setSize(sf.Vector2f(rect.width, rect.height))
			self.collision_rect.setPosition(rect.left, rect.top)
			target.draw(self.collision_rect)
		states.transform *= self.getTransform()
		target.draw(self.sprite, states)

	def getLocalBounds(self):
		return self.sprite.getLocalBounds()


class FpsCounter (Entity):
	"""Prints FPS in top-left corner"""

	refresh_time = 1.0 # refresh every second
	font_path	 = "data/Crimson-Roman.otf"
	font		 = None

	def __init__(self, engine):
		Entity.__init__(self, "FpsCounter")
		self.font = loadFont(self.font_path, engine)
		self.text = sf.Text("FPS :  0", self.font)
		self.text.setStyle(sf.Text.Bold)
		self.clock = thor.StopWatch(True)

	def update(self, elapsed_time):
		fps = FpsCounter.compute_fps(elapsed_time)
		if self.clock.getElapsedTime().asSeconds() < self.refresh_time:
			return True
		self.text.setString("FPS : {0:4.0f}".format(fps))
		self.clock.reset(True)

		return True

	def draw(self, target, states):
		"""Rendered in the default view"""
		target.setView(target.getDefaultView())
		states.transform *= self.getTransform()
		target.draw(self.text, states)
		target.setView(globals.camera)

	frame_counter, frame_time, fps = 0, 0, 0
	@staticmethod
	def compute_fps(elapsed_time):
		"""Compute FPS from frametime. Works above 1000 fps."""
		fps = 1 / (elapsed_time.asSeconds()+0.000001)
		return fps
		FpsCounter.frame_counter += 1
		FpsCounter.frame_time	 += elapsed_time.asMilliseconds()
		print(FpsCounter.frame_time)
		if FpsCounter.frame_time >= 100:
			FpsCounter.fps, FpsCounter.frame_counter = FpsCounter.frame_counter, 0
			FpsCounter.frame_time -= 100
		return FpsCounter.fps

	def getSubRect(self):
		return self.text.getTextureRect()

	def getLocalBounds(self):
		return self.text.getLocalBounds()

	def getGlobalBounds(self):
		return self.text.getGlobalBounds()
