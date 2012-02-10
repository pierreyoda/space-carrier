# -*-coding:Utf-8 -*
"""Entities-related class and functions"""

from math import pi, degrees
from OiwEngine import Entity, CollisionTable
from decimal import Decimal
from . import globals
import sf, thor


def loadTexture(path, engine):
	"""Load a texture by path, raise an exception if failed"""
	texture_key = thor.Resources.TextureKey.FromFile(path)
	texture		= engine.loadTexture(texture_key)
	if not texture.valid():
		raise RuntimeError("cannot load texture '" + path + "'")
	return texture.get()

def spriteSize(sprite):
	"""Compute a sprite's size, taking in account its scale"""
	bounds, scale = sprite.GetLocalBounds(), sprite.GetScale()
	return sf.Vector2f(bounds.Width * scale.x, bounds.Height * scale.y)

def originAtCenter(entity):
	"""Define entity's origin at his center"""
	size = entity.getSize()
	entity.SetOrigin(size.x/2, size.y/2)


class SpriteBasedEntity (Entity):
	"""Base class for sprite-based entities"""

	AABB_collision_boxes  = False	# Draw AABB collision boxes (used by pixel perfect test)
	AABB_boxes_color = sf.Color(255, 175, 200)

	def __init__(self, engine, id, pos = sf.Vector2f(), angle = 90):
		Entity.__init__(self, id, pos, angle)
		subclass = self.__class__ # provides access to subclass static variables
		if not isinstance(subclass.texture, sf.Texture):
			subclass.texture = loadTexture(subclass.texture_path, engine)
		self.sprite = sf.Sprite(subclass.texture)
		originAtCenter(self)
		self.collision_table = CollisionTable(self.sprite.GetTexture().CopyToImage())

	def Draw(self, target, states):
		if SpriteBasedEntity.AABB_collision_boxes:
			rect = EntityCollisions.getAABB(self)
			intrect = sf.FloatRect(rect.Left, rect.Top, rect.Width, rect.Height)
			rect_shape = sf.RectangleShape(rect.Width, rect.Height)
			rect_shape.SetPosition(rect.Left, rect.Top)
			target.Draw(rect_shape)
		states.Transform *= self.GetTransform()
		target.Draw(self.sprite, states)

	def getSize(self):
		return spriteSize(self.sprite)


class FpsCounter (Entity):
	"""Prints FPS in top-left corner"""

	refresh_time = 1.0 # refresh every second
	font_path	 = "data/Crimson-Roman.otf"
	font		 = None

	def __init__(self, engine):
		Entity.__init__(self, "FpsCounter")
		font_key = thor.Resources.FontKey.FromFile(self.font_path)
		font = engine.loadFont(font_key)
		if not font.valid():
			raise RuntimeError("cannot load font '" + self.font_path + "'")
		self.font = font.get()
		self.fps = sf.Text("FPS :  0", self.font)
		self.fps.SetStyle(sf.Text.Bold)
		self.clock = thor.StopWatch(True)

	def update(self, elapsed_time):
		fps = FpsCounter.compute_fps(elapsed_time)
		if self.clock.GetElapsedTime().AsSeconds() < self.refresh_time:
			return True
		self.fps.SetString("FPS : {0:4.0f}".format(fps))
		self.clock.Reset(True)

		return True

	def Draw(self, target, states):
		"""Rendered in the default view"""
		target.SetView(target.GetDefaultView())
		states.Transform *= self.GetTransform()
		target.Draw(self.fps, states)
		target.SetView(globals.camera)

	frame_counter, frame_time, fps = 0, 0, 0
	@staticmethod
	def compute_fps(elapsed_time):
		"""Compute FPS from frametime. Works above 1000 fps."""
		fps = 1 / (elapsed_time.AsSeconds()+0.000001)
		return fps
		FpsCounter.frame_counter += 1
		FpsCounter.frame_time	 += elapsed_time.AsMilliseconds()
		print(FpsCounter.frame_time)
		if FpsCounter.frame_time >= 100:
			FpsCounter.fps, FpsCounter.frame_counter = FpsCounter.frame_counter, 0
			FpsCounter.frame_time -= 100
		return FpsCounter.fps
