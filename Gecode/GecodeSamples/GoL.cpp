#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>

#include <gecode/driver.hh>
#include <gecode/int.hh>
#include <gecode/minimodel.hh>

#define _MAXIMIZE_ 1

using namespace Gecode;


class GoLOptions : public InstanceOptions {
protected:
	Driver::StringValueOption _var; //< Var Branch
	Driver::StringValueOption _val; //< Val Branch

public:
   GoLOptions(std::string name)
     : InstanceOptions("GoL"),
       _var("-var","INT_VAR","INT_VAR_SIZE_MIN"),
       _val("-val","INT_VAL","INT_VAL_MIN") {
     add(_var);
     add(_val);
   }
   IntVarBranch var(void) const { return INT_VAR_SIZE_MIN(); }
   IntValBranch val(void) const { return INT_VAL_MIN(); }
};


#if _MAXIMIZE_
class GoL : public MaximizeScript {
#else
class GoL : public MinimizeScript {
#endif
protected:
  int n;			// side of the board
  IntVarArray X;
  IntVar Cost;
public:
  GoL(const InstanceOptions& opt){
    std::ifstream fin;
    fin.open(opt.instance()); //string with the file to open

	// alpha.cpp --> uses branching
	// opt.branching
									
    int nAlive;								// number of initial Alive Cells
    long int boardSize;						// the board is extended with 1 cell of margin around

    fin >> n >> nAlive;						// read n: size of the board
	std::cout << n << ' ' << nAlive << std::endl;
	boardSize = (n+2)*(n+2);

    X = IntVarArray(*this,boardSize,0,1);
    Matrix<IntVarArray> A(X, n+2, n+2);		// because of the minimodel, a matrix can be used from the original vector

	int x, y;
	nAlive=0;
	LinIntExpr r;
	do {
      fin >> x >> y;						// matrix of active cells
	  std::cout << nAlive << ' ' << x << ' ' << y << std::endl;
	  nAlive++;
	  rel(*this, A(x,y), IRT_EQ, 1);		// the cell is alive
	  r=A(x-1,y-1)+A(x-1,y)+A(x-1,y+1)+A(x,y-1)+A(x,y+1)+A(x+1,y-1)+A(x+1,y)+A(x+1,y+1);
	  rel(*this, 2<=r && r<=3);	
	} while (!fin.eof());
     
    fin.close();

	std::cout << n << ' ' << --nAlive << std::endl;

	Cost=IntVar(*this,nAlive,boardSize/2+1);    // maximum cost: minimum is the number of nAlive --> accelerates processing
	rel(*this,Cost==sum(X));

    // constraints

	// margin to zero
	// horizontal
	for (int i=0; i<n+2; i++){
		rel(*this, A(0,i), IRT_EQ, 0);
		rel(*this, A(n+1,i), IRT_EQ, 0);
	}
	// vertical: in a different loop to do not repeat corners
	for (int i=1; i<n+1; i++){
		rel(*this, A(i,0), IRT_EQ, 0);
		rel(*this, A(i,n+1), IRT_EQ, 0);
	}

	// side cells: not 3 in a row Alive
	for (int i=2; i<n-1; i++){
		// horizontal
		rel(*this, A(1,i)+A(1,i+1)+A(1,i+2)<3);
		rel(*this, A(n,i)+A(n,i+1)+A(n,i+2)<3);
		// vertical
		rel(*this, A(i,1)+A(i+1,1)+A(i+2,1)<3);
		rel(*this, A(i,n)+A(i+1,n)+A(i+2,n)<3);
	}
	
	LinIntExpr s;
	
	// regular cells
	for(int i=1; i<=n;i++)
		for (int j=1; j<=n; j++) {
			s=A(x-1,y-1)+A(x-1,y)+A(x-1,y+1)+A(x,y-1)+A(x,y+1)+A(x+1,y-1)+A(x+1,y)+A(x+1,y+1);
			rel(*this, (A(i,j)==0) >> (s!=3));  		
		}

    branch(*this, X, INT_VAR_SIZE_MIN(), INT_VAL_MAX());
  }

#ifdef _MAXIMIZE_
  GoL(bool share, GoL& s): MaximizeScript(share, s), n(s.n) {
#else
  GoL(bool share, GoL& s): MinimizeScript(share, s), n(s.n) {
#endif
    X.update(*this, share, s.X);
	Cost.update(*this, share, s.Cost);
  }

#ifdef _MAXIMIZE_
  virtual MaximizeSpace* copy(bool share) {
#else
  virtual MinimizeSpace* copy(bool share) {
#endif
    return new GoL(share,*this);
  }

  virtual void print(std::ostream& os) const {
	
	Matrix<IntVarArray> A(X,n+2,n+2);
	for (int i=1; i<=n; i++) {
		for (int j=1; j<=n; j++)
			os << A(i,j);
		os << std::endl;
	}
	os << Cost << std::endl;

  }

  // cost function
  virtual IntVar cost(void) const{return Cost;}    // mandatory for min/max problem. Must return Cost the cost variable
};


// command line argumentsN
// execute from command line

int main(int argc, char* argv[]) {
/*	if (argc>0)
		printf("%d %s\n",argc, argv[1]);*/
	
  InstanceOptions opt("Arithmetic CSP");

  opt.instance("sl15_2.txt");	// can be overwritten from the command-line: -instance otherinstance
  opt.solutions(0);
  
  opt.parse(argc,argv);
#if _MAXIMIZE_
  MaximizeScript::run<GoL,BAB,InstanceOptions>(opt);
#else
  MinimizeScript::run<GoL,BAB,InstanceOptions>(opt);
#endif
  
  return 0;
}
