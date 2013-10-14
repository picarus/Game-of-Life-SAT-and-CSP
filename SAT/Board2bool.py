
#count number of constraints  --> not needed
#corners
#sorted network
#minisat2
#other solvers
#solver parameters

from boardutils import readFile, printBoard, generateBoardFromInput, generateIndexBoard, generateHowManyBoard
import sys

def main():
    if len(sys.argv) >= 2:
        name = sys.argv[1]
        board2bool(name)
    else:
        print "The file name is missing: DEFAULT->input1.txt"
        board2bool('input1.txt')  

def board2bool(name):
    
    M = readFile(name)

    B = generateBoardFromInput(M)

    n = len(B)
    
    BI = generateIndexBoard(len(B))
    BHM = generateHowManyBoard(B)
    BDC = generateBoardCellsAround(B,BI,n,0)
    
   # printBoard(B)
   # printBoard(BI)
   # printBoard(BHM)
   # printBoard(BDC)

    generateRules(B, BI, BHM, BDC)

def generateBoardCellsAround(B,BI,n,value):
    BCA= [[[] for _ in range(n)] for _ in range(n)]

    for r in range(0,n):
        for c in range(0,n):
            if B[r][c]==value: # cell has the looked up value
                addCell(BCA,r,c, BI[r][c],n)
   
    return BCA

    
def addCell(BCA, i, j, index,n):
    for r in range(max(0,i-1),min(i+2,n)) :
        for c in range(max(0,j-1),min(j+2,n)) :
            if r<>i or c<>j:
                BCA[r][c].append(index) 

def notChange(B, BI, BHM, BDC, r, c):
    if B[r][c]==1:# alive
        print 'p Alive Cell', BI[r][c], r, c
        print BI[r][c], 0
        # two or three
        generate2or3(BI[r][c],BHM[r][c],BDC[r][c])
    else: # dead
        print 'p Dead Cell', BI[r][c], r, c
        # not three
        generateNot3(BI[r][c],BHM[r][c],BDC[r][c])

def generate2or3(me,hm,ei):
    # Alive cell
    if hm <= 3 :
        if hm < 2 :
            print 'p 2<= N <= 3, we have', hm
            generateMoreThanN(2-hm,list(ei));
        else:
            print 'p already 2 or more: N <= 3, we have', hm 
        generateLessThanN(3-hm,list(ei));
    else:
        print 'p DEAD BY OVERCROWDED: hm=',hm,'>3 -- not SAT !!!!!'

def generateNot3(me,hm,ei):
    
    if hm <=3:
        k = 4 - hm
        if hm==3:
            print 'p 4 <= N, we have', hm
            if k > 0 and len(ei)>=k:
                # we must add at least 1
                generateMoreThanN(k,ei)
            else:
                # we must get alive (for this cell to be stable)
                print me, 0
        # for the rest of the cases
        # the 2 blocks must be conjugated,
        # as the constraints must be simultaneously satisfied
        else:
            generateAllCombinationsAndNegate(k,ei)
            pass
    else:
        # the cell adds no constraints
        # whatever happen will not change its state
        print 'p DEAD BY OVERCROWDED: hm=',hm,',OK, since already dead'

def generateAllCombinationsAndNegate(k,ei):
    # we generate all DNF combinations that add K
    # when negating, to obtain the combinations that are not k,
    # we will obtain CNF
    genAllInner(k,ei,[])

def genAllInner(k,ei,acc):
    if k==0:
        # concatenate all and print
        for val in ei:
            acc.append(val)
        print ' '.join([str(item) for item in acc]),  0
    elif k==len(ei): # we need all we have
        # concatenate all and print
        for val in ei:
            acc.append(-val)
        print ' '.join([str(item) for item in acc]),  0
    elif k < len(ei):
        val = ei.pop();
        l=list(acc);
        l.append(val);
        genAllInner(k, list(ei), l)
        l=list(acc);
        l.append(-val);
        genAllInner(k-1, list(ei), l)
    else: # k>len(ei)
        # we have taken too many options
        pass
    
def generateMoreThanN(k,ei):
    k = len(ei) - k + 1
    if k > 0:
        print 'p Take', k,'out of', len(ei), 'positive'
        takeK(k,list(ei),[],1);

def generateLessThanN(k,ei):
    k = k + 1;
    print 'p Take', k,'out of', len(ei), 'negative'
    if k > 0:
        takeK(k,list(ei),[],-1)
    
def takeK(k,ei,acc,sign):
    if k==1:
        # concatenate each to the acc and print
        s = ' '.join([str(item) for item in acc])
        for val in ei:
            print s,  val*sign, 0
    elif k==len(ei): # we need all we have
        # concatenate all and print
        for val in ei:
            acc.append(val*sign)
        print ' '.join([str(item) for item in acc]),  0
    elif k < len(ei):
        val = ei.pop();  
        takeK(k, list(ei), acc[:],sign)
        acc.append(val*sign)
        takeK(k-1, list(ei), acc[:],sign)
    else: # k>len(ei)
        print 'p K =',k,'> dead neighbours=',len(ei),' -- not sat'
           
def generateRules(B, BI, BHM, BDC):
    n = len(B)

    print 'p cnf', n*n, 0

    print 'p Central cells'
    
    # central cells:
    for r in range(1,n-1):
        for c in range(1,n-1):
            notChange(B, BI, BHM, BDC, r, c)  

    print "p Vertical sides cells"
                       
    # vertical sides
    for r in range(1,n-1):
        notChange( B, BI, BHM, BDC, r, 0 )
        notThreeInAColumn( BI, r, 0 )
        notChange( B, BI, BHM, BDC, r, n-1 )
        notThreeInAColumn( BI, r, n-1 )

    print "p Horizontal sides cells"

    # horizontal sides
    for c in range(1,n-1):
        notChange( B, BI, BHM, BDC, 0, c )
        notThreeInARow(BI,0,c)
        notChange( B, BI, BHM, BDC, n-1, c )
        notThreeInARow(BI,n-1,c)

    print "p Corners"
    
    notChange( B, BI, BHM, BDC, 0, 0 )
    notChange( B, BI, BHM, BDC, 0, n-1 )
    notChange( B, BI, BHM, BDC, n-1, 0 )
    notChange( B, BI, BHM, BDC, n-1, n-1 )

def notThreeInAColumn(BI,row,col):
    print 'p Not three in a column'
    print -BI[row-1][col],-BI[row][col],-BI[row+1][col], 0

def notThreeInARow(BI,row,col):
    print 'p Not three in a row'
    print -BI[row][col-1],-BI[row][col],-BI[row][col+1], 0


# This is the standard boilerplate that calls the main() function.
if __name__ == '__main__':
  main()
