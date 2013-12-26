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
public:
	GoLOptions(std::string name) : InstanceOptions(name.c_str()){
	}
	IntVarBranch getVar(void) const { return var; }
	IntValBranch getVal(void) const { return val; }
	void setVar(IntVarBranch varParam) { var=varParam; }
	void setVal(IntValBranch valParam) { val=valParam; }
};

IntConLevel GoL_ICL=ICL_DEF;

class GoL : public MaximizeScript {
protected:
	int n;			// side of the board
	IntVarArray X;
	IntVar Cost;
public:
	GoL(const GoLOptions& opt){
		std::ifstream fin;
		std::string filename=opt.instance();
		fin.open(filename);						//string with the file to open

		int nAlive;								// number of initial Alive Cells
		long int boardSize;						// the board is extended with 1 cell of margin around

		fin >> n >> nAlive;						// read n: size of the board
		//std::cout << n << ' ' << nAlive << std::endl;
		boardSize = (n+2)*(n+2);
		long int upperBound = n*n/2+1;

		X = IntVarArray(*this,boardSize,0,1);
		Matrix<IntVarArray> A(X, n+2, n+2);		// because of the minimodel, a matrix can be used from the original vector

		int x, y;
		nAlive=0;
		LinIntExpr r;
		while (!fin.eof()) {
			fin >> x >> y;						// matrix of active cells
			//std::cout << nAlive << ' ' << x << ' ' << y << std::endl;
			nAlive++;
			rel(*this, A(x,y), IRT_EQ, 1);		// the cell is alive
			//linear(*this,A.slice(x-1,x+1,y-1,y+1),IRT_LQ,3);
			//linear(*this,A.slice(x-1,x+1,y-1,y+1),IRT_GQ,4);
			r=expr(*this,A(x-1,y-1)+A(x-1,y)+A(x-1,y+1)+A(x,y-1)+A(x,y+1)+A(x+1,y-1)+A(x+1,y)+A(x+1,y+1),GoL_ICL);
			rel(*this, (r>=2) && (r<=3) );
		} 

		fin.close();

		//std::cout << n << ' ' << --nAlive << std::endl;

		Cost=IntVar(*this,nAlive,		// cost: minimum is the number of nAlive
						//n*n*);
						upperBound);    //		 maximum is the number of cells divided by 2										
		rel(*this,Cost==sum(X));

		// constraints
		// regular cells
		
		for(int i=1; i<=n;i++) {
			for (int j=1; j<=n; j++) {
				LinIntExpr s;
				s=expr(*this,A(x-1,y-1)+A(x-1,y)+A(x-1,y+1)+A(x,y-1)+A(x,y+1)+A(x+1,y-1)+A(x+1,y)+A(x+1,y+1));
				//count(*this,A.slice(x-1,x+1,y-1,y+1),1,IRT_NQ,3);
				rel(*this, (A(i,j)==0) >> (s!=3));
				rel(*this, (A(i,j)==1) >> ((s>=2) && (s<=3)));
				// redundant ???
				//rel(*this, (s>3) >> (A(i,j)==0));
				//rel(*this, (s<2) >> (A(i,j)==0));
			}
		}

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

		// side cells: not 3 consecutive Alive
		for (int i=2; i<n-1; i++){
			// horizontal
			rel(*this, A(1,i-1)+A(1,i)+A(1,i+1)<3);
			rel(*this, A(n,i-1)+A(n,i)+A(n,i+1)<3);
			// vertical
			rel(*this, A(i-1,1)+A(i,1)+A(i+1,1)<3);
			rel(*this, A(i-1,n)+A(i,n)+A(i+1,n)<3);
		}

		IntVarBranch varBranch=opt.getVar(); 
		IntValBranch valBranch=opt.getVal();
		branch(*this, X, varBranch, valBranch);
	}


	GoL(bool share, GoL& s): MaximizeScript(share, s), n(s.n) {
		X.update(*this, share, s.X);
		Cost.update(*this, share, s.Cost);
	}


	virtual MaximizeSpace* copy(bool share) {
		return new GoL(share,*this);
	}

	virtual void print(std::ostream& os) const {

		Matrix<IntVarArray> A(X,n+2,n+2);
		for (int i=1; i<=n; i++) {
			for (int j=1; j<=n; j++)
				os << A(i,j);
			os << std::endl;
		}
		os << std::endl << Cost << std::endl;

	}

	// cost function
	virtual IntVar cost(void) const {return Cost;}    // mandatory for min/max problem. Must return Cost the cost variable
};

// fails if the input is a solution

int main(int argc, char* argv[]) {

	GoLOptions opt("GoL");
	Support::Timer timer;

	if (argc<2){
		opt.instance("sl15_2.txt");	
	} 

	char* strVal []={  
		//"INT_VAL_RND(0)",
		"INT_VAL_MAX()",  
		"INT_VALUES_MAX()", 
		"INT_VAL_RANGE_MAX()",  
		"INT_VAL_SPLIT_MAX()",
		/*
		"INT_VAL_MIN()",
		"INT_VALUES_MIN()",
		"INT_VAL_RANGE_MIN()",
		"INT_VAL_SPLIT_MIN()"
		*/
	};

	IntValBranch valBranch[]={
		//INT_VAL_RND(0),
		INT_VAL_MAX(),  
		INT_VALUES_MAX(), 
		INT_VAL_RANGE_MAX(),
		INT_VAL_SPLIT_MAX(),
		/*
		INT_VAL_MIN(),
		INT_VALUES_MIN(), 
		INT_VAL_RANGE_MIN(),
		INT_VAL_SPLIT_MIN(),
		*/
	};

	char* strVar[]={ 
		"INT_VAR_SIZE_MAX()", 
		"INT_VAR_ACTIVITY_MAX()", 
		"INT_VAR_ACTIVITY_SIZE_MAX()", 
		/*
		"INT_VAR_SIZE_MIN()",
		"INT_VAR_ACTIVITY_MIN()",
		"INT_VAR_ACTIVITY_SIZE_MIN()"
		*/
	};

	IntVarBranch varBranch[]={
		INT_VAR_SIZE_MAX(), 
		INT_VAR_ACTIVITY_MAX(), 
		INT_VAR_ACTIVITY_SIZE_MAX(), 
		/*
		INT_VAR_SIZE_MIN(),
		INT_VAR_ACTIVITY_MIN(),
		INT_VAR_ACTIVITY_SIZE_MIN()
		*/
	};

	std::ofstream fos;
	opt.parse(argc,argv);

	std::string filename=std::string(opt.instance());

	fos.open("out"+filename);

	opt.solutions(0);
	
	int total=0;
	double t;

	opt.setVar(INT_VAR_SIZE_MAX());
	opt.setVal(INT_VAL_MAX());
	timer.start();
	MaximizeScript::run<GoL,BAB,GoLOptions>(opt);
	t=timer.stop();
	fos << std::endl << "VarBranch:" << "INT_VAR_SIZE_MAX()" << " ValBranch:" << "INT_VAL_MAX()" << " Time(msec):" << t <<std::endl ;
	total++;


	/*
	int i=0, j;
	for (IntVarBranch var:varBranch){
		j=0;
		opt.setVar(var);
		for (IntValBranch val: valBranch){ 
			opt.setVal(val);
			timer.start();
			MaximizeScript::run<GoL,BAB,GoLOptions>(opt);
			t=timer.stop();
			fos << "VarBranch:" << strVar[i] << " ValBranch:" << strVal[j] << " Time(msec):"<< t <<std::endl ;
			j++;
			total++;
		}
		i++;
	}
	*/
	/*
	opt.setVar(INT_VAR_SIZE_MAX());

	opt.setVal(INT_VAL_MIN());
	timer.start();
	MaximizeScript::run<GoL,BAB,GoLOptions>(opt);
	t=timer.stop();
	fos << std::endl << "VarBranch:" << "INT_VAR_SIZE_MAX()" << " ValBranch:" << "INT_VAL_MIN()" << " Time(msec):" << t <<std::endl ;
	total++;

	opt.setVal(INT_VAL_RND(0));
	timer.start();
	MaximizeScript::run<GoL,BAB,GoLOptions>(opt);
	t=timer.stop();
	fos << std::endl << "VarBranch:" << "INT_VAR_SIZE_MAX()" << " ValBranch:" << "INT_VAL_RND(0)" << " Time(msec):" << t <<std::endl ;
	total++;
	*/
	fos << "Total:" << total << std::endl;
	fos.close();

	return 0;
}
