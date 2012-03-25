# -*-coding:Utf-8 -*
"""Space Carrier - Game Script main package."""

import sf, thor
import glob
import os.path
import re
from OiwEngine import State, EntityManager
from script.entities_tools import FpsCounter, loadTexture
from script.frigate import Frigate
from script.camera import Camera
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
	render_window.capture().saveToFile(folder + filename)


class TestState (State):
	"""Test state"""

	def __init__(self, engine):
		State.__init__(self, engine)
		self.engine = engine
		self.entities_mgr = EntityManager()
		globals.camera = Camera()
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
		globals.action_map["keyboard_zoom_in"] = thor.Action(sf.Keyboard.Add)
		globals.action_map["keyboard_zoom_out"] = thor.Action(sf.Keyboard.Subtract)
		globals.action_map["keyboard_zoom_reset"] = thor.Action(sf.Keyboard.Home)
		# Entities
		self.entities_mgr.addEntity(Frigate(self.engine, "frigate"))
		self.entities_mgr.addEntity(FpsCounter(self.engine))
		self.engine.getRenderWindow().setFramerateLimit(0)
		self.engine.getRenderWindow().setVerticalSyncEnabled(False)

		return True

	def update(self, elapsed_time):
		# Game main events
		if globals.action_map.isActive("quit"):
			self.engine.quit()
			return
		elif globals.action_map.isActive("debug"):
			globals.debug = not globals.debug
			print("debug={}".format(globals.debug))
		elif globals.action_map.isActive("print_mouse_pos"):
			print("Mouse relative position is : {}".format(
				sf.Mouse.getPosition(self.engine.getRenderWindow())))
		elif globals.action_map.isActive("screenshot"):
			take_screenshot(self.engine.getRenderWindow())

		# GAME CAMERA : KEYBOARD
		if globals.action_map.isActive("keyboard_zoom_in"):
			globals.camera.zoom(1.0 - elapsed_time.asSeconds())
		elif globals.action_map.isActive("keyboard_zoom_out"):
			globals.camera.zoom(1.0 + elapsed_time.asSeconds())
		elif globals.action_map.isActive("keyboard_zoom_reset"):
			globals.camera.reset()

		# Game entities
		self.entities_mgr.updateAll(elapsed_time)

		globals.action_map.clearEvents()

	def render(self, target):
		target.setView(globals.camera)

		target.clear(sf.Color(100, 200, 100))
		self.entities_mgr.drawAll(target)

		target.setView(target.getDefaultView())

	def handleEvent(self, event):
		# GAME CAMERA : MOUSE
		# Mouse wheel => camera zoom
		if event.type == sf.Event.MouseWheelMoved:
			if event.mouseWheel.delta < 0: # zoom
				globals.camera.zoom(1.1)
			else: # unzoom
				globals.camera.zoom(0.9)
		# middle mouse button => camera reset
		elif event.type == sf.Event.MouseButtonPressed and \
			event.mouseButton.button == sf.Mouse.Middle:
			globals.camera.reset()
		# Other events
		else:
			globals.action_map.pushEvent(event)


def SpaceCarrier_initScript(engine):
	"""
		Script main entry
	"""
	print("Initializing game script...")
	game_mode = TestState(engine)
	# Exception handling from C++ works better in most of the cases
	if not game_mode.init():
		print("Cannot initialize Test State")
		exit(1)
	print("Test State was succesfully initialized!")
	engine.getRenderWindow().setSize(sf.Vector2u(globals.screen_size[0],
		globals.screen_size[1]))
	engine.getRenderWindow().setKeyRepeatEnabled(False)
	engine.addState(game_mode, "Test", True)

	return True
