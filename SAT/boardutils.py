
from math import sqrt

def readFile(name):
    f = open(name, 'r')
    M = []
    for line in f:
        l=line.rstrip().split()
        M.append(l)      
    f.close()
    return M

def printBoard(B):
    n = len(B)
    for i in xrange(0,n):
        print ' '.join([str(item) for item in B[i]])

def generateBoardFromInput(M):
    # from the list of positions with checkers
    # we generate a matrix with 0/1

    n = int(M[0][0])      # number of cells

    B = [[]]*n;           # we generate n empty rows
  
    for i in xrange(0, n):
        B[i] = [0] * n    # to each row we add n 'dead' columns 

    for i in xrange(1,len(M)):
        x = int(M[i][0]) - 1
        y = int(M[i][1]) - 1
        B[x][y] = 1       # for each position in the file we set a 1, 'alive' position
        
    return B

def generateBoardFromMinisatOutput(M):
    # from the results file
    # we generate a matrix with 0/1 

    ncells =len(M[1])-1
    n = int(sqrt(ncells));

    results = [int(cell) for cell in M[1]]

    B= [[0 for _ in range(n)] for _ in range(n)]# we generate an n x n dead matrix
  
    for cell in results:
        if cell > 0:
            set(cell, B, 1, n)
    
    return B

def set(cell,B, value, n):
    cell = cell - 1
    r = cell // n
    c = cell % n
    B[r][c]=value;

def generateHowManyBoard(B):
    n = len(B)
    BHM = [[]]*n;                           # we generate n empty rows
    for r in xrange(0, n):
        row = []
        for c in xrange(0,n):
            row.append(howMany(B, r, c, n)) # count the number of alive neighbours
        BHM[r] = row        
    return BHM

def generateIndexBoard(n):
    B = [[]]*n;                     # we generate n empty rows
    for i in xrange(0, n):
        B[i] = range(i*n+1,i*n+n+1)     # index for the cells in the row 
    return B

def howMany(B, i, j, n):   
    # how many alive positions around
    how = 0

    # this is not efficient, we are reading the board 9 times
    # the good solution would be to generate this while generating the board
    # by adding one to all neighbours when an alive position is found
    
    for r in range(max(0,i-1),min(i+2,n)) :
        for c in range(max(0,j-1),min(j+2,n)) :
            how = how + B[r][c]
    how = how - B[i][j]
    
    return how
