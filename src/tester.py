from ctypes import cdll
#import matplotlib.pyplot as plt
lib = cdll.LoadLibrary('./libgame.so')

random = 0
human = 1
minimax = 2
alphabeta = 3

outcome = lib.run_game_wrapper(minimax, random)

print outcome
