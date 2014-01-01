#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>

#include <gecode/driver.hh>
#include <gecode/int.hh>
#include <gecode/minimodel.hh>

using namespace Gecode;

class GoLOptions : public InstanceOptions {
protected:
	IntVarBranch var; //< Var Branch
	IntValBranch val; //< Val Branch
	bool redundant;
public:
	GoLOptions(std::string name) : InstanceOptions(name.c_str()){
	}
	IntVarBranch getVar(void) const { return var; }
	IntValBranch getVal(void) const { return val; }
	
	void setVar(IntVarBranch varParam) { var=varParam; }
	void setVal(IntValBranch valParam) { val=valParam; }

};

IntConLevel GoL_ICL=ICL_DEF;
//-print-last 1 -file-sol sl15_2sol.txt -mode solution -model redundant sl15_2.txt

class GoL : public IntMaximizeScript {
protected:
	IntVarArray X;
	IntVar Cost;
	int n;			// side of the board
public:
	// default parameters -print-last 1 -file-sol sl15_2sol.txt -mode solution -model noredundant sl15_2.txt
	

	GoL(const GoLOptions& opt){
		std::ifstream fin;
		std::string filename=opt.instance();
		bool redundant=(opt.model()==1);
	
		fin.open(filename);						//string with the file to open

		long int nAlive;						// number of initial Alive Cells
		long int boardSize;						// the board is extended with 1 cell of margin around

		fin >> n >> nAlive;						// read n: size of the board, nAlive is ignored
		//std::cout << n << ' ' << nAlive << std::endl;
		boardSize = (n+2)*(n+2);				// the extended size of the board
		long int upperBound = n*n;				// an upper bound for the cost function
		
		X = IntVarArray(*this,boardSize,0,1);
		Matrix<IntVarArray> A(X, n+2);			// using minimodel, a matrix can be used from the original vector

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// read the input file
		int x, y;
		nAlive=0;
		
		while (!fin.eof()) {
			IntVar r=IntVar(*this,0,8);
			fin >> x >> y;						// matrix of active cells
			nAlive++;							// update the nr. of alive cells
			//std::cout << nAlive << ' ' << x << ' ' << y << std::endl;
			rel(*this, A(x,y), IRT_EQ, 1);		// the cell is alive
			if (redundant) {					// redundant constraints based on knowing the cell is alive
				r=expr(*this, A(x-1,y-1)+A(x-1,y)+A(x-1,y+1)+A(x,y-1)+A(x,y+1)+A(x+1,y-1)+A(x+1,y)+A(x+1,y+1));
				rel(*this, (r==2) || (r==3) );
			}
		} 

		fin.close();
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// init cost function
		upperBound=std::max(upperBound,nAlive);
		std::cout << n << ' ' << nAlive << ' ' << upperBound << std::endl;

		Cost=IntVar(*this,nAlive,		// cost: minimum is the number of nAlive
						upperBound);    //		 maximum is the size of the board (not extended)										
		rel(*this,Cost==sum(X));

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// constraints

		// margin to zero
		// horizontal
		for (int i=0; i<=n+1; i++){
			rel(*this, A(0,i), IRT_EQ, 0);
			rel(*this, A(n+1,i), IRT_EQ, 0);
		}
		// vertical: in a different loop to do not repeat corners
		for (int i=1; i<=n; i++){
			rel(*this, A(i,0), IRT_EQ, 0);
			rel(*this, A(i,n+1), IRT_EQ, 0);
		}
		/////////////////////////////////////////////////////////////////////////////////
		// regular cells
		for(int i=1; i<=n;i++) {
			for (int j=1; j<=n; j++) {
				IntVar s; 
				s=expr(*this,A(i-1,j-1)+A(i-1,j)+A(i-1,j+1)+A(i,j-1)+A(i,j+1)+A(i+1,j-1)+A(i+1,j)+A(i+1,j+1));
				rel(*this, (A(i,j)==0) >> (s!=3));
				rel(*this, (A(i,j)==1) >> ((s==2) || (s==3)));
				// redundant constraints
				if (redundant){
					rel(*this, (s>3) >> (A(i,j)==0));
					rel(*this, (s==1) >> (A(i,j)==0));
					rel(*this, (s==0) >> (A(i,j)==0));
					rel(*this, (s==3) >> (A(i,j)==1));
				}
			}
		}
		///////////////////////////////////////////////////////////////////////////////
		// side cells: not 3 consecutive Alive
		for (int i=2; i<=n-1; i++){
			// horizontal
			rel(*this, A(1,i-1)+A(1,i)+A(1,i+1)<3);
			rel(*this, A(n,i-1)+A(n,i)+A(n,i+1)<3);
			// vertical
			rel(*this, A(i-1,1)+A(i,1)+A(i+1,1)<3);
			rel(*this, A(i-1,n)+A(i,n)+A(i+1,n)<3);
		}

		//////////////////////////////////////////////////////////////////////////////
		// calculate result
		IntVarBranch varBranch=opt.getVar(); 
		IntValBranch valBranch=opt.getVal();
		branch(*this, X, varBranch, valBranch);
		
	}

	//////////////////////////////////////////////////////////////////////////////////
	// Other stuff required to define virtual and override methods
	GoL(bool share, GoL& s): IntMaximizeScript(share, s) {
		n=s.n;
		X.update(*this, share, s.X);
		Cost.update(*this, share, s.Cost);
	}


	virtual IntMaximizeSpace* copy(bool share) {
		return new GoL(share,*this);
	}

	////////////////////////////////////////////////////////////////////////////////////////
	// print solution
	virtual void print(std::ostream& os) const {

		Matrix<IntVarArray> A(X,n+2);
		for (int i=1; i<=n; i++) {
			for (int j=1; j<=n; j++)
				os << A(i,j);
			os << std::endl;
		}
		os << std::endl << Cost << std::endl;

	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// cost function
	virtual IntVar cost(void) const {return Cost;}    // mandatory for min/max problem. Must return Cost the cost variable
};


int main(int argc, char* argv[]) {

	GoLOptions opt("GoL");

	if (argc<2){
		opt.instance("sl15_2.txt");	// default input, the most time consuming case
	} 
	 
	/////////////////////////////////////////////////////////////
	// Value and Variable selection
	char* strVal []={  
		"INT_VAL_MAX()",  
		"INT_VALUES_MAX()", 
		"INT_VAL_RANGE_MAX()",  
		"INT_VAL_SPLIT_MAX()",
		"INT_VAL_MIN()",
		"INT_VALUES_MIN()",
		"INT_VAL_RANGE_MIN()",
		"INT_VAL_SPLIT_MIN()",
		"INT_VAL_RND(0)"
	};

	IntValBranch valBranch[]={
		INT_VAL_MAX(),  
		INT_VALUES_MAX(), 
		INT_VAL_RANGE_MAX(),
		INT_VAL_SPLIT_MAX(),
		INT_VAL_MIN(),
		INT_VALUES_MIN(), 
		INT_VAL_RANGE_MIN(),
		INT_VAL_SPLIT_MIN(),
		INT_VAL_RND(0)
	};

	char* strVar[]={ 
		"INT_VAR_SIZE_MAX()", 
		"INT_VAR_ACTIVITY_MAX()", 
		"INT_VAR_ACTIVITY_SIZE_MAX()", 
		"INT_VAR_SIZE_MIN()",
		"INT_VAR_ACTIVITY_MIN()",
		"INT_VAR_ACTIVITY_SIZE_MIN()",
		"INT_VAR_RND(0)"
	};

	IntVarBranch varBranch[]={
		INT_VAR_SIZE_MAX(), 
		INT_VAR_ACTIVITY_MAX(), 
		INT_VAR_ACTIVITY_SIZE_MAX(), 
		INT_VAR_SIZE_MIN(),
		INT_VAR_ACTIVITY_MIN(),
		INT_VAR_ACTIVITY_SIZE_MIN(),
		INT_VAR_RND(0)
	};

	////////////////////////////////////////////////////////////
	// we allow the model to include or not redundant clauses
	// based on example: alpha.cpp
	opt.model(0);				// by default: no redundant
	opt.model(1,"redundant");
	opt.model(0,"noredundant");
	opt.solutions(0);			// retrieve all solutions
	opt.parse(argc,argv);

	//////////////////////////////////////////////////////////////
	// to execute one
	opt.setVar(INT_VAR_SIZE_MAX());
	opt.setVal(INT_VAL_MAX());
	std::cout << std::endl << "VarBranch: INT_VAR_SIZE_MAX() ValBranch: INT_VAL_MAX()" <<std::endl ;
	IntMaximizeScript::run<GoL,BAB,GoLOptions>(opt);

/* 
	/////////////////////////////////////////////////////////////////
	// to execute all configurations of value and variable selection
	int i=0, j;
	for (IntVarBranch var:varBranch){
		j=0;
		opt.setVar(var);
		for (IntValBranch val: valBranch){ 
			opt.setVal(val);
			std::cout << "VarBranch:" << strVar[i] << " ValBranch:" << strVal[j] << std::endl ;
			IntMaximizeScript::run<GoL,BAB,GoLOptions>(opt);
			j++;
		}
		i++;
	}
*/
	return 0;
}
