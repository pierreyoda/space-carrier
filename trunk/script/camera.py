# -*-coding:Utf-8 -*
"""Game camera, in fact an extended sf.View."""

import sf
from . import globals

class Camera (sf.View):
	def __init__(self):
		sf.View.__init__(self, sf.FloatRect(0, 0, globals.screen_size[0],
			globals.screen_size[1]))

	def focus_on_entity(self, entity):
		"""Camera will center on the specified entity"""
		self.SetCenter(entity.pos)

	def reset(self):
		self.Reset(sf.FloatRect(0, 0, globals.screen_size[0],
			globals.screen_size[1]))
