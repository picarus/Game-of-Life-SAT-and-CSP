#include <iostream>
#include <fstream>
#include <stdlib.h>

#include <gecode/driver.hh>
#include <gecode/int.hh>
#include <gecode/minimodel.hh>

/*
n: g * s
g: number of groups
s: group size
w: number of weeks
*/

using namespace Gecode;

class Golf : public Space {
protected:
  IntVarArray X;
  int w;
  int g;
  int s;
public:
  Golf(const int g, const int s, const int w) : w(w), g(g), s(s),   
	  X( *this, g*s*w, 0, g*s-1 ) {

	int n=g*s;

    Matrix<IntVarArray> A(X,w,n);   // because of the minimodel, a matrix can be used from the original vector
   
	for (int k=0;k<w;k++)
		distinct(*this, A.col(k));	// all the players must be different (appear only once)
									// and by construction all will appear at least once

	for (int k=0;k<w-1;k++)
		for (int j=0;j<g;j++)				// for every group, check groups in other weeks are different
			for (int kp=k+1;kp<w;kp++)		// for every par of weeks (not the same/order does not matter)
				for (int jp=0;jp<g;jp++)	// for every group
											// no two players are in group k,j and kp, jp
					for (int i=0; i<s; i++)
						for (int i2=0; i2<s; i2++)
							for (int ip=i+1; ip<s; ip++)
								for (int i2p=i2+1; i2p<s; i2p++) {
									//std::cout << i << i2 << ip << i2p << ' ' << (i+j*s) << (i2+jp*s) << (i+jp*s) << (i2p+jp*s) <<std::endl;
									rel(*this, ( A(k,i+j*s) != A(kp,i2+jp*s) ) || ( A(k,ip+j*s) != A(kp,i2p+jp*s) ) );
								}
									
	for (int i=0;i<n;i++)
		rel(*this, A(0,i), IRT_EQ, i);						// the first week the players follow the natural order

	for (int k=0;k<w;k++){
		rel(*this, A(k,0), IRT_EQ,0);						// the first one will always be zero
		for (int j=0; j<(g-1); j++)
			rel(*this, A(k,j*s), IRT_LQ, A(k,(j+1)*s));		// the first of every group will be lesser than the first of the next group
		for (int j=0; j<g; j++)
			for (int i=0; i<s-1; i++)
				rel(*this, A(k,i+j*s), IRT_LQ, A(k,i+1+j*s)); // the groups are ordered too
	}

	for (int k=0;k<w-1;k++)									// the second element in the first group is ordered week to week
		rel(*this, A(k,1), IRT_LQ, A(k+1,1));		
				
    branch(*this, X, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
  }

  Golf(bool share, Golf& s) : Space(share, s), g(s.g), s(s.s), w(s.w) {
    X.update(*this, share, s.X);
  }

  virtual Space* copy(bool share) {
    return new Golf(share,*this);
  }
  
  void print() const {
	int n=g*s;
	Matrix<IntVarArray> A(X,w,n);
	for (int i=0; i<n; i++){
		std::cout << A.row(i) << std::endl;
	}
	std::cout << std::endl;
  }
};


int main(int argc, char* argv[]) {
#ifdef _DEBUG
	InstanceOptions opt("Golf CSP - DEBUG");
#else
	InstanceOptions opt("Golf CSP - RELEASE");
#endif
  Golf* golf=new Golf(3,3,4); 
  DFS<Golf> e(golf);
  delete golf;

  int c=0;
  /* all solutions */
  while (Golf* s = e.next()) {
    s->print(); delete s;
	c++;
  }

  std::cout << c << std::endl;

  return 0;
}
