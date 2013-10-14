
import sys
from boardutils import readFile, printBoard, generateBoardFromMinisatOutput, generateBoardFromInput
from math import sqrt

def main():
    if len(sys.argv) >= 2:
        name1 = sys.argv[1]
        name2 = sys.argv[2]
        checkResults(name1,name2)
    else:
        print "The file name is missing: DEFAULT->input1.txt"
        checkResults('input.txt','outcnf.txt')  

def checkResults(name1, name2):
    
    M1 = readFile(name1)
    B1 = generateBoardFromInput(M1)
    M2 = readFile(name2)
    B2 = generateBoardFromMinisatOutput(M2)

    compare(B1,B2)

    printBoard(B1)
    print '-----------------------'
    print '  '
    print '-----------------------'
    printBoard(B2)

def compare(B1,B2):
    n = len(B2)
    r = 0
    for row in B1:
        c = 0
        for cell in row:
            if cell<>B2[r][c]:
                if cell==0:
                    B2[r][c]='*'
                else:
                    B2[r][c]='!'
            c=c+1
        r=r+1

        
# This is the standard boilerplate that calls the main() function.
if __name__ == '__main__':
    main()
