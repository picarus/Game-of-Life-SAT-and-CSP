from boardutils import readFile, printBoard, generateExtendedBoardFromInput, generateIndexBoard, getN, NAUX
from math import pow
import sys

#####################################################
#
# Global variables and functions to manipulate them
#
#####################################################

clauses=[]      # Set of clauses that generate the auxiliary variables
                # for the Sorting Network
N=0             # Number of cols/rows in the table
                # the extended table will have 2 more rows and cols
SizeTable=0     # SizeTable = (N+2)^2 

SNOUT = {2:37,3:38,4:39}    # the variables that map to the outputs in the
                            # SN that are relevant for the problem
            
def sncnfwa():              # the Tseitin variables in the SN
                            # for a first cell is created
                            # this will be used to generate the variables
                            # for all the cells by adding the required offset to
                            # the variables index

    global clauses
    
    sn = sndef()                    # original SN
    sngates = sndefgates()          # mapping of the SN to the Tseitin vars.

    clauses = []
    
    offset = 0                      # first variable    
    n = len(sn)                     # nr. of levels in the SN

    for lev in xrange(0,n):
        variables = sn[lev]
        gates = sngates[lev]            # for each pair of variables
        for pos in xrange( 0, len(gates) ):
            v1=inputsFor( sn, sngates, variables[pos][0], lev-1) # look for it in the previous levels
            v2=inputsFor( sn, sngates, variables[pos][1], lev-1)
            for idx in xrange( 0, 2 ):  # for each variable in the pair
                value = gates[pos][idx]
                if value>=0:            # if not -1    
                    if idx==0:          # OR
                        cl=orleg(value,v1,v2);
                    else:               # AND
                        cl=andleg(value,v1,v2);
                    clauses.append(cl);
    
    return clauses          #it's a global variable but we return it

##########################################
#
# Generate Tseitin auxiliary variables
#
##########################################

def andleg(a, b, c):    #   a <--> b & c
    cl =[[a,-b,-c],     # right 2 left 
        [-a,b],         # left 2 right
        [-a,c]]
    #print 'and', cl
    return cl
   
def orleg(a, b, c):     #   a <--> b | c
    cl =[[-a,b,c],      # left 2 right
        [-b,a],         # right 2 left
        [-c,a]]
    #print 'or', cl
    return cl

def not3(me, two, three, four): # for dead cells (always requires the cell being processed, me)
    # -me --> -3 | 4
    #print 'p Not 3: -3|4', three,four
    print me, -three, four, 0

def or23(me, two, three, four, alive): # for alive cells or dead cells
    # 2 & -4
    #print 'p 2 or 3: 2 & -4', two, -four
    if not alive:
        # -me --> 2 & -4
        print -me, two, 0
        print -me, -four, 0
    else:
        # 2 & -4
        print two, 0
        print -four, 0
        
def sndef():                    # Original definition of the Sorting Network
    return [[[1,5],[2,6],[3,7],[4,8]],
            [[1,3],[2,4],[5,7],[6,8]],
            [[1,2],[3,5],[4,6],[7,8]],
            [[3,4],[5,6]],
            [[2,5],[4,7]],
            [[2,3],[4,5],[6,7]]]

def sndefgates():   # Mapping of the gates in the SN to the Tseitin variables
    return [[[9,10],[11,12],[13,14],[15,16]],
            [[17,18],[19,20],[21,22],[23,24]],
            [[-1,25],[26,27],[28,29],[30,-1]],
            [[31,32],[33,-1]],
            [[34,35],[36,-1]],
            [[37,38],[39,-1],[-1,-1]]]

#############################################

def getNeighbours(i,j,BI):  # for a cell obtains the neighbors            
    nghbr=[]
    for ix in xrange(i-1,i+2):
        for jx in xrange(j-1,j+2):
            if not ix==i or not jx==j:
                nghbr.append(BI[ix][jx])
    #nghbr=[1,2,3,4,5,6,7,8] # for debugging
    return nghbr

def calculateOffset( i , j ):   # offset for a cell 
    return ( (j - 1) * N + (i - 1) ) * NAUX +  SizeTable - 8
    
def replicateAuxVars(i, j, BI, offset):   # replicate the SN variables for a cell
    neighbours=getNeighbours(i,j, BI)
    replicate(clauses,neighbours,offset)
    return offset

def replicate(clauses,neighbours,offset):
    for clause in clauses:  
        for term in clause:
            clauseOut=[]
            for literal in term:  # for each literal in each clause
                aterm=abs(literal)
                if aterm <9:        # refers to a cell (a neighbour)
                    t=neighbours[aterm-1]
                    if literal<0:
                        t=-t
                else:               # it's an auxiliary variable
                    if literal>0:
                        t=literal+offset
                    else:
                        t=literal-offset
                clauseOut.append(t)
            print ' '.join([str(c) for c in clauseOut]), 0

def inputsFor( sn, sngates, target, n ):
    if n<0:
       return target

    # find the output in the level
    level = sn[n];                  
    pos = -1                        # not found
    gateNr = 0;
    for gate in level:
        if target in gate:
            pos = gate.index(target)
            gateRes = gate;
            gateNrFin = gateNr;
        gateNr = gateNr + 1

    if ( pos >= 0 ):                    
        return sngates[n][gateNrFin][pos]
    else: 
        return inputsFor( sn, sngates, target, n-1 )
      
    
def stableAlive(i,j,offset,B,BI):
    # we know the cell is alive and generate all the constraints for it
    two=SNOUT[2]+offset             # indexes for the outputs in the SN
    three=SNOUT[3]+offset
    four=SNOUT[4]+offset
    me=BI[i][j]                     # index for the cell variable
    #print 'p Alive Cell', i, j, me, offset
    print me, 0                     #the cell is alive
    or23(0,two,three,four,True)

def stableDead(i,j,offset,B,BI):   
    # the cell may be alive or not in the final configuration
    two=SNOUT[2]+offset             # indexes for the outputs in the SN
    three=SNOUT[3]+offset
    four=SNOUT[4]+offset
    me=BI[i][j]                     # index for the cell variable
    #print 'p Dead Cell', i, j, me, offset
    # Depending on the final value, the constraints are one or another
    not3(me,two,three,four)
    or23(me,two,three,four,False)     

def setMarginToZero(BI):
    # horizontal
    #print 'p Horizontal zeros'
    for ix in xrange(0,N+2):
        print -BI[ix][0],0
        print -BI[ix][N+1],0

    #vertical
    #print 'p Vertical zeros'
    for jx in xrange(1,N+1):
        print -BI[0][jx],0
        print -BI[N+1][jx],0


def notThreeInAColumn(BI,row,col):
    print -BI[row-1][col],-BI[row][col],-BI[row+1][col], 0

def notThreeInARow(BI,row,col):
    print -BI[row][col-1],-BI[row][col],-BI[row][col+1], 0

def setCondsOnSideCells(BI):
    #print "p Horizontal sides cells"
    for c in range(2,N):
        notThreeInARow( BI, 1, c)
        notThreeInARow( BI, N, c)

    #print "p Vertical sides cells"
    for r in range(2,N):  
        notThreeInAColumn( BI, r, 1 )
        notThreeInAColumn( BI, r, N )

def generateRules(B,BI):

    sncnfwa() # generate the clauses for the sorting network (auxiliary variables)
    
    # central cells
    for i in xrange(1,N+1):
        for j in xrange(1,N+1):
            offset=calculateOffset(i,j)             # offset for the Tseitin variables for the current cell
            #print 'p', i, j, BI[i][j], offset
            if B[i][j]==1:  # alive cells
                stableAlive(i, j, offset, B, BI)
            else:           # dead cells
                stableDead (i, j, offset, B, BI)
            replicateAuxVars(i,j,BI,offset)         # add the Tseitin vars for the cell

    # margin to zero
    setMarginToZero(BI)         # the cells in the border are dead
    # side cells conditions
    setCondsOnSideCells(BI)     
        

def board2bool(name):       # generate the DIMACS file from the input file
    global N
    global SizeTable
    
    M = readFile(name)      # read the whole file

    # calculate the global variables

    N = getN(M)             # calculate the dimension of the board from the input
    SizeTable = int(pow(N+2,2))  # calculate the size of the extended board
    
    B = generateExtendedBoardFromInput(M)       # Board with initial configuration (0/1)
    BI = generateIndexBoard(N+2)                # Each cell contains its index in the DIMACS file

    nAlives = len(M)-1                          # Nr. of Alive cells
    nCells = int(pow(N,2))                      # Total nr. of cells
    nVars = int( SizeTable + NAUX * nCells )    # number of cells(in the extended board) + NAUX(31) Tseitin variables per cell
    nClauses = NAUX * nCells * 3                # auxiliary variables, NAUX(31) * cell * 3 clauses
    nClauses = nClauses + 4 * N + 4             # margin of zeros, 1 per cell on the margin
    nClauses = nClauses + 4 * ( N - 2 )         # no three in a row, add 1 clause per cell on the side(not corners)
    nClauses = nClauses +(nCells-nAlives) * 3   # dead cells add 3 clauses
    nClauses = nClauses + nAlives * 3           # alive cells add 3 clauses

    print 'c Alive Cells', nAlives
    print 'c', name                             # file used to generate this one
    print 'p cnf', nVars, nClauses              # DIMACS header
   
    generateRules(B, BI)                        # generates all rules

def main():
    if len(sys.argv) >= 2:
        name = sys.argv[1]
        board2bool(name)
    else:
        #sys.stderr.write("The file name is missing: DEFAULT->input1.txt\n")
        board2bool('input1.txt') 

# This is the standard boilerplate that calls the main() function.
if __name__ == '__main__':
    main()
