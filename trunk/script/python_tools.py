# -*-coding:Utf-8 -*
"""Utilities functions"""

def enum(*sequential, **named):
	"""Create an enum from a list of values
		ex. : Numbers = enum('ZERO', 'ONE', 'TWO')
			  one = Numbers.ONE
	"""
	enums = dict(zip(sequential, range(len(sequential))), **named)
	return type('Enum', (), enums)
