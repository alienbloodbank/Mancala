#author: Rich Magnotti and Soubhik Ghoush
from ctypes import cdll
import matplotlib.pyplot as plt
import numpy as np

lib = cdll.LoadLibrary("./libgame.so")

#hash table representing possible players for p1 and p2
playerHash = {'random'      : 0,
              'human'       : 1,
              'minimax'     : 2,
              'alphabeta'   : 3}

#hash table to keep track of num winsper player/draws
winsHash = {1   : 0,
            2   : 0,
            3   : 0}

playerSelection1 = input("Choose player 1: \n") #string to accept usr input as selection
player1 = playerHash[playerSelection1] #using player selection str as key to get value to pass to C++ wrapper args
playerSelection2 = input("\nChoose player 2: \n")
player2 = playerHash[playerSelection2]

random = 0
human = 1
minimax = 2
alphabeta = 3
totalCalls = 20

for i in range(totalCalls):
    print('Game number ', i+1)
    outcome = lib.run_game_wrapper(player1, player2)
    winsHash[outcome] = winsHash[outcome] + 1

print(winsHash)

plt.bar('P1 %s Wins' %(playerSelection1), winsHash[1], align='center', alpha=0.5)
plt.bar('P2 %s Wins' %(playerSelection2), winsHash[2], align='center', alpha=0.5)
plt.bar('Draws', winsHash[3], align='center', alpha=0.5)
plt.xlabel('Result of Matches')
plt.ylabel('Times Won')
plt.ylim(top=totalCalls)
plt.title('Num Times Won | %s vs %s, %s Matches' %(playerSelection1, playerSelection2, totalCalls))

plt.show()


