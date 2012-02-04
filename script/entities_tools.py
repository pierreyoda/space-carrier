# -*-coding:Utf-8 -*
"""Entities-related class and functions"""

from math import pi, degrees
from OiwEngine import Entity, CollisionTable
from decimal import Decimal
import sf, thor

half_pi = pi /2

def loadTexture(path, engine):
	"""Load a texture by path, raise an exception if failed"""
	texture_key = thor.Resources.TextureKey.FromFile(path)
	texture		= engine.loadTexture(texture_key)
	if not texture.valid():
		raise RuntimeError("cannot load texture '" + path + "'")
	return texture.get()

def originAtCenter(sprite):
	"""Define sprite's origin at his center"""
	subrect = sprite.GetSubRect()
	sprite.SetOrigin(subrect.Width/2, subrect.Height/2)


class SpriteBasedEntity (Entity):
	"""Base class for sprite-based entities"""

	AABB_collision_boxes  = False	# Draw AABB collision boxes (used by pixel perfect test)
	AABB_boxes_color = sf.Color(255, 175, 200)

	def __init__(self, id, engine, pos = sf.Vector2f(), angle = half_pi):
		Entity.__init__(self, id, pos, angle)
		subclass = self.__class__ # provides access to subclass static variables
		if subclass.texture == None:
			subclass.texture = loadTexture(subclass.texture_path, engine)
		self.sprite = sf.Sprite(subclass.texture)
		originAtCenter(self.sprite)
		self.updateAngle()
		self.sprite.SetPosition(self.pos)
		self.collision_table = CollisionTable(self.sprite.GetTexture().CopyToImage())

	def updateAngle(self):
		self.sprite.SetRotation(degrees(self.angle) - 90.0) # angle is whith the abscissa

	def render(self, target):
		if SpriteBasedEntity.AABB_collision_boxes:
			rect = EntityCollisions.getAABB(self)
			intrect = sf.FloatRect(rect.Left, rect.Top, rect.Width, rect.Height)
			target.Draw(sf.Shape.Rectangle(intrect, SpriteBasedEntity.AABB_boxes_color))
		target.Draw(self.sprite)

	def getSize(self):
		return self.sprite.GetSize()

	def transformToLocal(self, point):
		return self.sprite.TransformToLocal(point)

	def transformToGlobal(self, point):
		return self.sprite.TransformToGlobal(point)


class FpsCounter (Entity):
	"""Prints FPS in top-left corner"""

	refresh_time = 1000 # refresh every second
	font_path	 = "data/Crimson-Roman.otf"
	font		 = None

	def __init__(self, engine):
		Entity.__init__(self, "FpsCounter")
		font_key = thor.Resources.FontKey.FromFile(self.font_path)
		font = engine.loadFont(font_key)
		if not font.valid():
			raise RuntimeError("cannot load font '" + self.font_path + "'")
		self.font = font.get()
		self.fps = sf.Text("FPS : 0", self.font)
		self.fps.SetStyle(sf.Text.Bold)
		self.clock = thor.StopWatch(True)

	def update(self, elapsed_time):
		fps = FpsCounter.compute_fps(elapsed_time)
		if self.clock.GetElapsedTime() < self.refresh_time:
			return True
		self.fps.SetString("FPS : {}".format(fps))
		self.clock.Reset(True)

		return True

	def render(self, target):
		"""Rendered in the default view"""
		current_view = target.GetView()
		target.SetView(target.GetDefaultView())
		target.Draw(self.fps)
		target.SetView(current_view)

	frame_counter, frame_time, fps = 0, 0, 0
	@staticmethod
	def compute_fps(elapsed_time):
		"""Compute FPS from frametime. Works above 1000 fps."""
		FpsCounter.frame_counter += 1
		FpsCounter.frame_time	 += elapsed_time
		if FpsCounter.frame_time >= 1000:
			FpsCounter.fps, FpsCounter.frame_counter = FpsCounter.frame_counter, 0
			FpsCounter.frame_time -= 1000
		return FpsCounter.fps
