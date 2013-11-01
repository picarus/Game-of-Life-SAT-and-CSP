
import sys
from boardutils import readFile, printBoard, generateBoardFromMinisatOutput, generateBoardFromInput, generateBoardFromMinisatExtendedOutput
from math import sqrt

def main():
    if len(sys.argv) >= 2:
        name1 = sys.argv[1]
        name2 = sys.argv[2]
        checkResults(name1,name2)
    else:
        print "The file name is missing: DEFAULT->input1.txt outputcnf1.txt"
        checkResults('input1.txt','outputcnf1.txt')  

def checkResults(name1, name2):
    
    M1 = readFile(name1)
    M2 = readFile(name2)

    if M2[0][0]=='UNSAT':
        print 'UNSAT PROBLEM'
        return
    
    B1 = generateBoardFromInput(M1)
    
    B2 = generateBoardFromMinisatExtendedOutput(M2)

    compare(B1,B2)
    n=len(B2)*2

    print '-'*n
    printBoard(B1)
    print '-'*n
    print '  '
    print '-'*n
    printBoard(B2)
    print '-'*n

def compare(B1,B2):
    n = len(B2)
    r = 0
    for row in B1:
        c = 0
        for cell in row:
            if cell<>B2[r][c]:
                if cell==0:
                    B2[r][c]='*' # added checker, a 0 becomes a 1
                else:
                    B2[r][c]='!' # error: a 1 has become a 0
            c=c+1
        r=r+1

        
# This is the standard boilerplate that calls the main() function.
if __name__ == '__main__':
    main()
