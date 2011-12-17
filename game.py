# -*-coding:Utf-8 -*
"""Space Carrier - Game Script main package."""

import sf, thor
import glob
import os.path
import re
from OiwEngine import State, EntityManager
from script.entities_tools import FpsCounter
from script.fighter import Fighter
from script import globals

def take_screenshot(render_window, folder = "screens/"):
	"""Take a screenshot and saves it to the specified folder ('/screens/' by default)"""
	# filename generation : comes from http://www.akeric.com/blog/?p=632
	current_screens = glob.glob(folder + "spacecarrier_*.png")
	num_list = [0]
	for screen in current_screens:
		i = os.path.splitext(screen)[0]
		try:
			num = re.findall('[0-9]+$', i)[0]
			num_list.append(int(num))
		except IndexError:
			pass
	num_list = sorted(num_list)
	filename = "spacecarrier_{}.png".format(num_list[-1]+1)
    # saving current screen
	print("Screenshot saved as {}".format(filename))
	if not os.path.isdir(folder):
		os.mkdir(folder) # create /screens not existing yet
	render_window.Capture().SaveToFile(folder + filename)


class TestState (State):
	"""Test state"""

	def __init__(self, engine):
		State.__init__(self, engine)
		self.engine = engine
		self.entities_mgr = EntityManager()
		globals.action_map = thor.ActionStrMap(engine.getRenderWindow())

	def init(self):
		if not State.init(self):
			raise RuntimeError("parent function State.init returned false")
		# Events
		globals.action_map["quit"] = thor.Action(sf.Keyboard.Escape, thor.Action.PressOnce)
		globals.action_map["debug"] = thor.Action(sf.Keyboard.F1, thor.Action.PressOnce)
		globals.action_map["print_mouse_pos"] = thor.Action(sf.Keyboard.F2, thor.Action.PressOnce)
		globals.action_map["screenshot"] = thor.Action(sf.Keyboard.F12, thor.Action.PressOnce)
		globals.action_map["player_rotate_left"] = \
			thor.Action(sf.Keyboard.Q, thor.Action.Realtime) #or \
			# thor.Action(sf.Keyboard.Left, thor.Action.Realtime)
		globals.action_map["player_rotate_right"] = \
			thor.Action(sf.Keyboard.D, thor.Action.Realtime) #or \
			# thor.Action(sf.Keyboard.Right, thor.Action.Realtime)
		# Entities
		self.entities_mgr.addEntity(Fighter(self.engine, "fighter"))
		self.entities_mgr.addEntity(FpsCounter(self.engine))
		self.engine.getRenderWindow().SetFramerateLimit(0)
		self.engine.getRenderWindow().EnableVerticalSync(False)

		return True

	def update(self, elapsed_time):
		# Game main events
		if globals.action_map.IsActive("quit"):
			self.engine.quit()
			return
		elif globals.action_map.IsActive("debug"):
			globals.debug = not globals.debug
			print("debug={}".format(globals.debug))
		elif globals.action_map.IsActive("print_mouse_pos"):
			print("Mouse relative position is : {}".format(
				sf.Mouse.GetPosition(self.engine.getRenderWindow())))
		elif globals.action_map.IsActive("screenshot"):
			take_screenshot(self.engine.getRenderWindow())

		# Game entities
		self.entities_mgr.updateAll(elapsed_time)

		globals.action_map.ClearEvents()

	def render(self, target):
		target.Clear(sf.Color(50, 200, 100))

		self.entities_mgr.renderAll(target)

	def handleEvent(self, event):
		globals.action_map.PushEvent(event)


def SpaceCarrier_initScript(engine):
	"""
	Script main entry
	"""
	print("Initializing game script...")
	game_mode = TestState(engine)
	try:
		if not game_mode.init():
			raise RuntimeError("") # unknown error
	except RuntimeError as error:
		message = str(error)
		print("Cannot initialize Test State", end = "")
		if message and not message.isspace():
			print(" :", message, end = "")
		print(".")
		return False
	print("Test State was succesfully initialized!")
	engine.addState(game_mode, "Test", True)
	engine.getRenderWindow().EnableKeyRepeat(False)
	return True
