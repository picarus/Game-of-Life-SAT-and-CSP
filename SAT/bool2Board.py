
import sys
from boardutils import readFile, printBoard, generateBoardFromMinisatOutput
from math import sqrt

def main():
    if len(sys.argv) >= 2:
        name = sys.argv[1]
        bool2board(name)
    else:
        print "The file name is missing: DEFAULT->input1.txt"
        bool2board('outcnf.txt')  

def bool2board(name):
    
    M = readFile(name)

    B = generateBoardFromMinisatOutput(M)
    
    printBoard(B)

def set(cell,B, value, n):
    cell = cell - 1
    r = cell // n
    c = cell % n
    B[r][c]=value;

# This is the standard boilerplate that calls the main() function.
if __name__ == '__main__':
    main()
