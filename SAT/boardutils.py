
from math import sqrt

NAUX = 31   # number of auxiliary variables in the sorting netwok
            # that are added for each cell in the board

def readFile(name):             # load the input file with the initial config 
    f = open(name, 'r')
    M = []
    for line in f:
        l=line.rstrip().split()
        M.append(l)      
    f.close()
    return M

def printBoard(B):              # print the board game (whatever dimensions)
    n = len(B)
    for i in xrange(0,n):
        print ' '.join([str(item) for item in B[i]])

def generateExtendedBoardFromInput(M):      # generates an (N+1)x(N+1) board
    return generateBoardFromInputCore(M,1)

def generateBoardFromInput(M):              # generates an NxN board
    return generateBoardFromInputCore(M,0)

def getN(M):                                # obtains the dimension of the board
    return int(M[0][0])

def generateBoardFromInputCore(M,margin):
    # from the list of positions with checkers
    # we generate a matrix with 0/1
    # for each checker in the config file we add a 1

    n = getN(M)                 # number of cells

    nExtended = n + margin * 2

    B = [[]]*nExtended;           # we generate nExtended wide empty rows 
  
    for i in xrange(0, nExtended):
        B[i] = [0] * nExtended    # to each row we add nExtended 'dead' columns 

    for i in xrange(1,len(M)):
        x = int(M[i][0]) + margin - 1   #adjust offsets to the position in the extended board
        y = int(M[i][1]) + margin - 1
        B[x][y] = 1             # for each position in the file we set a 1, 'alive' position
        
    return B

def calculateN(nVars):
    # from the output we calculate the original N size of the board
    # N is calculated from a second order equation Ax2+Bx+C=0
    A = NAUX+1
    B = 4 
    C = 4 - nVars

    dscr = B*B - 4 * A * C      # discriminant
    if dscr==0:
        result=-B/(2*A)
    elif dscr>0:                # if there are two solutions we get the bigger one (the other is negative)
        d=sqrt(dscr)
        result=(-B+d)/(2*A)
    return int(result)

def generateBoardFromMinisatExtendedOutput(M):
    # from the results file
    # we generate a matrix with 0/1
    
    nVars = len( M[1] )-1
    N = calculateN( nVars )
    SizeTable = int(pow(N+2,2))

    results = [int(cell) for cell in M[1][0:SizeTable]]

    B = [[0 for _ in xrange(N)] for _ in xrange(N)]         # we generate an n x n dead matrix

    BI = generateIndexBoard(N+2)                            # an index board is generated to easily translate from
                                                            # DIMACS indexed variable to the matrix based index

    for ix in xrange(1,N+1):
        for jx in xrange(1,N+1):
            var=BI[ix][jx]
            if results[var-1]>0:                            # for each positive variable
                B[ix-1][jx-1]=1                             # a checker is added to the board
    
    return B

def generateBoardFromMinisatOutput(M):
    # from the results file
    # we generate a matrix with 0/1 

    ncells =len(M[1])-1
    n = int(sqrt(ncells));

    results = [int(cell) for cell in M[1]]

    B= [[0 for _ in xrange(n)] for _ in xrange(n)]          # we generate an n x n dead matrix
  
    for cell in results:
        if cell > 0:                                        # for each positive variable
            set(cell, B, 1, n)                              # a checker is added to the board
    
    return B

def set(cell,B, value, n):                                  # the indexing is inverted (for the NxN board)
    cell = cell - 1
    r = cell // n
    c = cell % n
    B[r][c]=value;

def generateHowManyBoard(B):                # for each cell we count the number of neighbours
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
        B[i] = range(i*n+1,i*n+n+1)     # index for the cells in the row, left to right, top down 
    return B

def howMany(B, i, j, n):                # for each cell, count how many neighbours are alive
    # how many alive positions around
    how = 0

    # this is not efficient, we are reading the board 9 times
    # the good solution would be to generate this while generating the board
    # by adding one to all neighbours when an alive position is found
    
    for r in xrange(max(0,i-1),min(i+2,n)) :        # the min/max is required to avoid exceeding the limits
        for c in xrange(max(0,j-1),min(j+2,n)) :
            how = how + B[r][c]
    how = how - B[i][j]                     
    
    return how
