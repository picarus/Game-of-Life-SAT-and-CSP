#include <iostream>
#include <fstream>
#include <stdlib.h>

#include <gecode/driver.hh>
#include <gecode/int.hh>
#include <gecode/minimodel.hh>

/*
3 4 0 10	--> (3 eq, 4vars) size of the problem + (min 0,max 10) bound for variables
1 2 3 4 30 -->
2 2 2 2 20 -->
4 3 2 1 20 -->
*/


using namespace Gecode;

class Arithm : public Script {
protected:
  IntVarArray X;
public:
  Arithm(const InstanceOptions& opt){
    std::ifstream fin;
    fin.open(opt.instance()); //string with the file to open
    int fil;
    int col;
    int L;
    int U;

    fin >> fil >> col >> L >> U;

    X = IntVarArray(*this,col,L,U);
    IntArgs AL(col*fil);			// no matrixes can be managed, vectors instead 
    Matrix<IntArgs> A(AL,col,fil);   // because of the minimodel, a matrix can be used from the original vector
    IntArgs B(fil);

    for(int j=0;j<fil;j++){
      for(int i=0; i<col; i++) fin >> A(i,j); // matrix
      fin >> B[j];					   // b cell	
    }
    fin.close();

    for(int j=0;j<fil;j++){
      for(int i=0; i<col; i++) std::cout<<A(i,j);
      std::cout<<std::endl;
    }
    for(int j=0;j<fil;j++){
      linear(*this,A.row(j),X,IRT_EQ,B[j]);
    }

    branch(*this, X, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
  }
  Arithm(bool share, Arithm& s): Script(share, s) {
    X.update(*this, share, s.X);
  }
  virtual Space* copy(bool share) {
    return new Arithm(share,*this);
  }

  virtual void print(std::ostream& os) const {
    os << X << std::endl;
  }
};


// command line argumentsN
// execute from command line

int main(int argc, char* argv[]) {
	if (argc>0)
		printf("%d %s\n",argc, argv[1]);
#ifdef _DEBUG
	InstanceOptions opt("Arithmetic CSP - DEBUG");
#else
	InstanceOptions opt("Arithmetic CSP - RELEASE");
#endif
  
  opt.instance("arith3.pb");	// can be overwritten from the command-line: -instance otherinstance
  opt.solutions(0);
  
  opt.parse(argc,argv);
  Script::run<Arithm,DFS,InstanceOptions>(opt);
  return 0;
}
