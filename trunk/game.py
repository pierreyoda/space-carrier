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

class ParticleTestState (State):
	"""Particle test state"""

	def __init__(self, engine):
		State.__init__(self, engine)
		self.engine = engine

	def init(self):
		self.paused = False
		self.engine.getRenderWindow().setFramerateLimit(0)
		self.engine.getRenderWindow().setVerticalSyncEnabled(False)

		# particle emitter
		self.emitter = thor.UniversalEmitter.create()
		self.emitter.setEmissionRate(30)
		self.emitter.setLifetime(sf.seconds(5))
		self.emitter.setPositionCallback(self.mousePosition)

		# particle system
		particle = loadTexture("data/particle.png", self.engine)
		self.particle_system = thor.ParticleSystem(thor.noDeletePtr(particle))
		self.particle_system.addEmitter(self.emitter)

		# particle affectors
		gradient = thor.createGradient(
			sf.Color(0, 150, 0), [1,
			sf.Color(0, 150, 100), 1,
			sf.Color(0, 0, 150),  ])
		self.particle_system.addAffector(thor.ColorAffector.create(gradient))
		self.particle_system.addAffector(thor.FadeInAffector.create(0.1))
		self.particle_system.addAffector(thor.FadeOutAffector.create(0.1))
		self.particle_system.addAffector(thor.TorqueAffector.create(500))
		self.particle_system.addAffector(thor.ForceAffector.create(sf.Vector2f(0, 100)))

		# particle parameters
		self.velocity = thor.PolarVector2f(200, -90)
		# text
		from script.entities_tools import loadFont
		self.font = loadFont("data/Crimson-Roman.otf", self.engine)
		self.text = sf.Text(
			"Left click: Enable/disable glow effect\n"
			"Right click: Pause\n"
			"Mouse wheel: Change direction\n",
			self.font)
		self.text.setCharacterSize(14)
		self.text.setColor(sf.Color(255, 255, 255))

		return True

	def mousePosition(self):
		pos = sf.Mouse.getPosition(self.engine.getRenderWindow())
		return sf.Vector2f(pos.x, pos.y)

	def update(self, elapsed_time):
		if not self.paused:
			self.emitter.setVelocity(
				thor.Distributions.deflect(self.velocity.to_vec2f(), 10).get())
			self.particle_system.update(elapsed_time)

	def render(self, target):
		target.clear(sf.Color(30, 30, 30))
		self.particle_system.draw(target)
		target.draw(self.text)

	def handleEvent(self, event):
		if event.type == sf.Event.KeyPressed:
			if event.key.code == sf.Keyboard.Escape:
				self.engine.quit()
			elif event.key.code == sf.Keyboard.F12:
				take_screenshot(self.engine.getRenderWindow())
		elif event.type == sf.Event.MouseButtonPressed:
			if event.mouseButton.button == sf.Mouse.Left:
				self.particle_system.setGlowing(not self.particle_system.isGlowing())
			elif event.mouseButton.button == sf.Mouse.Right:
				self.paused = not self.paused
		elif event.type == sf.Event.MouseWheelMoved:
			self.velocity.phi += 12 * event.mouseWheel.delta

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
	#game_mode = TestState(engine)
	game_mode = ParticleTestState(engine)
	# Exception handling from C++ works better in most of the cases
	if not game_mode.init():
		print("Cannot initialize Test State")
		return False
	print("Test State was succesfully initialized!")
	engine.getRenderWindow().setSize(sf.Vector2u(globals.screen_size[0],
		globals.screen_size[1]))
	engine.getRenderWindow().setKeyRepeatEnabled(False)
	engine.addState(game_mode, "Test", True)

	return True
