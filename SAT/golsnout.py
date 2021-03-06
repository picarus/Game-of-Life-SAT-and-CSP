
import sys
from boardutils import readFile, printBoard, generateIndexBoard, generateBoardFromMinisatExtendedOutput
from math import sqrt


def main():
    if len(sys.argv) >= 2:
        name = sys.argv[1]
        bool2board(name)
    else:
        print "The file name is missing: DEFAULT->outputcnf1.txt"
        bool2board('outputcnf1.txt')  


def bool2board(name):
    
    M = readFile(name)

    if M[0][0]=='UNSAT':
        print 'UNSAT PROBLEM'
        return

    B = generateBoardFromMinisatExtendedOutput(M)

    # check no 1's in margin
    
    printBoard(B)

# This is the standard boilerplate that calls the main() function.
if __name__ == '__main__':
    main()
