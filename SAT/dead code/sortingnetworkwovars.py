
from aima.logic import to_cnf, pl_true, Expr
import itertools

def test():

    snCnf=sncnf()

    sol=[];
    sol.append(to_cnf(snCnf[1] & ~snCnf[3]))
    sol.append(to_cnf(~snCnf[2] & snCnf[3]))
    
    lCnf=[] 
    for i in range(0,len(snCnf)):
        lCnf.append( cnfstr2list( snCnf[i] ) )

    lCnfSol=[] 
    for i in range(0,len(sol)):
        lCnf.append( cnfstr2list( sol[i] ) )    
    
##    values = {}
##    for a in [False,True]:
##        values[0]=a
##        for b in [False,True]:
##            values[1]=b
##            for c in [False,True]:
##                values[2]=c
##                for d in [False,True]:
##                    values[3]=d
##                    for e in [False,True]:
##                        values[4]=e
##                        for f in [False,True]:
##                            values[5]=f
##                            for g in [False,True]:
##                                values[6]=g
##                                for h in [False,True]:
##                                    values[7]=h
##                                    snTest(snCnf,values)

def countTrue(values):
    count=0;
    for v in values:  
        if values[v]: count=count+1
    
    return count    

def integrity(evalua):
    i=0;
    while i<len(evalua) and evalua[i]:
        i=i+1;

    for j in range(i,len(evalua)) :
        if evalua[j]:
            return False # error

    return True

def snTest(snCnfL,values):
    
    count=countTrue(values)
    if count>4: count=4 # the maximum we require is 4
    evalua = []
    
    for i in range(0,len(snCnf)):
        evalua.append( evalCNFlist(snCnfL[i],values) )

    if not integrity(evalua):
        print 'No integrity:', ' '.join([str(item) for item in evalua])
        return False

    if count==4:
        if not evalua[3]:
            print 'No count match: count==',count,'and eval[3]=',evalua[3]
            return False
        
    if count==0:
        if evalua[0]:
            print 'No count match: count==',count,'and eval[0]=',evalua[0]
            return False
            
    if count<4:
        if evalua[count] or not evalua[count-1]:
            print 'No count match: count==',count,'and eval[',count,']=',evalua[count],'and eval[',count-1,']=',evalua[count-1] 
            return False
        
    return True

def evalCNFlist(cnflist,values):
    
    #instantiate with the model
    model = []
    for cl in cnflist:
        f = []
        f.append([values[literal] for literal in cl])
        model.append(f)

    #each clause must contain True
    res = [ True in cl for cl in model ]
    
    return not False in res
        
def cnfstr2list(cnformula):
 
    if cnformula.op == '|':
        cnflist=[]
        cnflist.append(cnformula.args)
    else:
        aux=[]
        for cl in cnformula.args:
            l = list(set(cl.args))
            aux.append( l )
        aux.sort()
        cnflist=list(aux for aux,_ in itertools.groupby(aux))
        
    print len(cnflist)
    return cnflist

#############################################################################################
#
#   Without auxiliary variables (and recursively)
#   It has repetitions (removed later on) but still generates 7000+ clauses per cell
#
#############################################################################################

def sncnf():

    sn = sndef() 

    n = len(sn) - 1 # number of levels

    cnformula = []
    formula = []

    for output in xrange(0,4):
        formula.append( snF(sn, n, output ) )
        cnformula.append( to_cnf( formula[output] ) )
        #print output, '-->',  cnformula[output]

    return formula
    

def snF(sn, n, output):
    
    # direct case
    if n<0:
        return Expr(output)

    # find the output in the level
    level = sn[n];
    pos = -1                        # not found
    gateRes= [];
    for gate in level:
        if output in gate:
            pos = gate.index(output)
            gateRes = gate;

    if (pos==0):                    # first element is OR
        return snF(sn,n-1,gateRes[0]) | snF(sn,n-1,gateRes[1]) 
    elif (pos==1):                  # second element is AND
        return snF(sn,n-1,gateRes[0]) & snF(sn,n-1,gateRes[1])
    else: 
        return snF(sn,n-1,output)

    #print pos, output,  ' '.join([str(item) for item in level])
