
from math import sqrt

NAUX = 31

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

def generateExtendedBoardFromInput(M):
    return generateBoardFromInputCore(M,1)

def generateBoardFromInput(M):
    return generateBoardFromInputCore(M,0)


def getN(M): 
    return int(M[0][0])

def generateBoardFromInputCore(M,margin):
    # from the list of positions with checkers
    # we generate a matrix with 0/1

    n = getN(M)                 # number of cells

    nExtended = n + margin * 2

    B = [[]]*nExtended;           # we generate n empty rows
  
    for i in xrange(0, nExtended):
        B[i] = [0] * nExtended    # to each row we add n 'dead' columns 

    for i in xrange(1,len(M)):
        x = int(M[i][0]) + margin - 1
        y = int(M[i][1]) + margin - 1
        B[x][y] = 1       # for each position in the file we set a 1, 'alive' position
        
    return B


def calculateN(nVars, NAUX):
    A = NAUX+1
    B = 4 
    C = 4 - nVars

    dscr = B*B - 4 * A * C
    if dscr==0:
        result=-B/(2*A)
    elif dscr>0:
        d=sqrt(dscr)
        result=(-B+d)/(2*A)
    return int(result)

def generateBoardFromMinisatExtendedOutput(M):
    # from the results file
    # we generate a matrix with 0/1 

    nVars = len( M[1] )-1
    N = calculateN( nVars, NAUX )
    SizeTable = int(pow(N+2,2))

    results = [int(cell) for cell in M[1][0:SizeTable]]

    #print results

    B = [[0 for _ in xrange(N)] for _ in xrange(N)]# we generate an n x n dead matrix

    BI = generateIndexBoard(N+2)

    for ix in xrange(1,N+1):
        for jx in xrange(1,N+1):
            var=BI[ix][jx]
            if results[var-1]>0:
                B[ix-1][jx-1]=1
    
    return B

def generateBoardFromMinisatOutput(M):
    # from the results file
    # we generate a matrix with 0/1 

    ncells =len(M[1])-1
    n = int(sqrt(ncells));

    results = [int(cell) for cell in M[1]]

    B= [[0 for _ in xrange(n)] for _ in xrange(n)]# we generate an n x n dead matrix
  
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
    B = [[]]*n;                         # we generate n empty rows
    for i in xrange(0, n):
        B[i] = range(i*n+1,i*n+n+1)     # index for the cells in the row 
    return B

def howMany(B, i, j, n):   
    # how many alive positions around
    how = 0

    # this is not efficient, we are reading the board 9 times
    # the good solution would be to generate this while generating the board
    # by adding one to all neighbours when an alive position is found
    
    for r in xrange(max(0,i-1),min(i+2,n)) :
        for c in xrange(max(0,j-1),min(j+2,n)) :
            how = how + B[r][c]
    how = how - B[i][j]
    
    return how
