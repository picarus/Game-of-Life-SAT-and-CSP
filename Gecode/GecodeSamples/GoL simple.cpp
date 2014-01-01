#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>

#include <gecode/driver.hh>
#include <gecode/int.hh>
#include <gecode/minimodel.hh>

using namespace Gecode;

//-print-last 1 -file-sol input_4sol.txt -mode solution input_4.txt

class GoL : public IntMaximizeScript {
protected:
	IntVarArray X;
	IntVar density;
	int n;			// side of the board
public:

	// http://dl.acm.org/citation.cfm?id=2308553&CFID=274794194&CFTOKEN=59290478

	GoL(const InstanceOptions& opt){
		std::ifstream fin;
		std::string filename=opt.instance();
		fin.open(filename);						//string with the file to open

		long int nAlive;							// number of initial Alive Cells
		long int boardSize;						// the board is extended with 1 cell of margin around

		fin >> n >> nAlive;						// read n: size of the board
		//std::cout << n << ' ' << nAlive << std::endl;
		boardSize = (n+2)*(n+2);
		long int upperBound = n*n;
		
		X = IntVarArray(*this,boardSize,0,1);
		Matrix<IntVarArray> A(X, n+2);		// using minimodel, a matrix can be used from the original vector

		int x, y;
		nAlive=0;
		
		while (!fin.eof()) {
			fin >> x >> y;						// matrix of active cells
			nAlive++;
			std::cout << nAlive << ' ' << x << ' ' << y << std::endl;
			rel(*this, A(x,y), IRT_EQ, 1);		// the cell is alive			
		} 

		fin.close();

		upperBound=std::max(upperBound,nAlive);
		std::cout << n << ' ' << nAlive << ' ' << upperBound << std::endl;

		density=IntVar(*this,nAlive,		// cost: minimum is the number of nAlive
						upperBound);    //		 maximum is the number of cells divided by 2										
		rel(*this,density==sum(X));
		
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

		// regular cells
		
		for(int i=1; i<=n;i++) {
			for (int j=1; j<=n; j++) {
				Slice<IntVarArray> ngbr=A.slice(i-1,i+2,j-1,j+2);
				IntVar r=expr(*this, sum(ngbr));
				rel(*this, (A(i,j)==0) >> (r!=3));
				rel(*this, (A(i,j)==1) >> ((r==3) || (r==4)));
			}
		}

		// side cells: not 3 consecutive Alive
		for (int i=2; i<n; i++){
			// horizontal
			rel(*this, A(1,i-1)+A(1,i)+A(1,i+1)<3);
			rel(*this, A(n,i-1)+A(n,i)+A(n,i+1)<3);
			// vertical
			rel(*this, A(i-1,1)+A(i,1)+A(i+1,1)<3);
			rel(*this, A(i-1,n)+A(i,n)+A(i+1,n)<3);
		}

		branch(*this, X, INT_VAR_SIZE_MAX(), INT_VAL_MAX());
	}


	GoL(bool share, GoL& s): IntMaximizeScript(share, s) {
		n=s.n;
		X.update(*this, share, s.X);
		density.update(*this, share, s.density);
	}


	virtual IntMaximizeSpace* copy(bool share) {
		return new GoL(share,*this);
	}

	virtual void print(std::ostream& os) const {

		Matrix<IntVarArray> A(X,n+2);
		for (int i=1; i<=n; i++) {
			for (int j=1; j<=n; j++)
				os << A(i,j);
			os << std::endl;
		}
		os << std::endl << density << std::endl;

	}

	// cost function
	virtual IntVar cost(void) const {return density;}    
};


int main(int argc, char* argv[]) {

	InstanceOptions opt("GoL");

	if (argc<2){
		opt.instance("input_4.txt");	
	} 

	opt.parse(argc,argv);

	std::string filename=std::string(opt.instance());

	opt.solutions(0);
	
	IntMaximizeScript::run<GoL,BAB,InstanceOptions>(opt);

	return 0;
}
