#include <iostream>
#include <fstream>
#include <stdlib.h>

#include <gecode/driver.hh>
#include <gecode/int.hh>
#include <gecode/minimodel.hh>

using namespace Gecode;

class RoundRobin : public Space {
protected:
  IntVarArray X;
  int n;
  int n_rounds;
public:
  RoundRobin(const int num) {
	n=num;
	int odd = n % 2;  
	n_rounds=n-1+odd;
	X=IntVarArray( *this, n*n_rounds, 0, n_rounds ); // if odd there will be a team that does not play(opponent n) every round
	Matrix<IntVarArray> A(X,n_rounds,n);

	for (int r=0;r<n_rounds;r++){ // every round
		// each team plays only once or rests
		distinct(*this,A.col(r));	
		// if team A plays against B, B plays against A
		for (int t=0;t<n;t++) {
			for (int i=0;i<n_rounds;i++){ 
				rel(*this,(A(r,t)==i)==(A(r,i)==t));
			}
		}
	}

	for (int t=0;t<n;t++){ // every team
		// each team plays only once against every other team
		distinct(*this,A.row(t));
		// a team can not play against himself
		rel(*this,A.row(t),IRT_NQ,t);
	}

	if (odd) { // team i rests at round i
		for (int i=0;i<n;i++){
			rel(*this,A(i,i)==n);
		}
	}

    branch(*this, X, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
  }

  RoundRobin(bool share, RoundRobin& s) : Space(share, s), n(s.n), n_rounds(s.n_rounds) {
    X.update(*this, share, s.X);
  }

  virtual Space* copy(bool share) {
    return new RoundRobin(share,*this);
  }
  
  void print() const {
	Matrix<IntVarArray> A(X,n_rounds,n);
	for (int i=0; i<n; i++){
		std::cout << A.row(i) << std::endl;
	}
	std::cout << std::endl;
  }
};


int main(int argc, char* argv[]) {

  RoundRobin* rr=new RoundRobin(4); 
  DFS<RoundRobin> e(rr);
  delete rr;

  int c=0;
  /* all solutions */
  while (RoundRobin* s = e.next()) {
    s->print(); 
	delete s;
	c++;
  }

  std::cout << c << std::endl;

  return 0;
}


//////////// RESULTS
// FOR N=3
/*
{3, 2, 1}
{2, 3, 0}
{1, 0, 3}

1
*/
//////////////////////
// FOR N=2
/*
{1}
{0}

1
*/
////////////////////
// FOR N=4 
/*
{1, 2, 3}
{0, 3, 2}
{3, 0, 1}
{2, 1, 0}

{1, 3, 2}
{0, 2, 3}
{3, 1, 0}
{2, 0, 1}

{2, 1, 3}
{3, 0, 2}
{0, 3, 1}
{1, 2, 0}

{2, 3, 1}
{3, 2, 0}
{0, 1, 3}
{1, 0, 2}

{3, 1, 2}
{2, 0, 3}
{1, 3, 0}
{0, 2, 1}

{3, 2, 1}
{2, 3, 0}
{1, 0, 3}
{0, 1, 2}

6
*/
// N=5 has 6 solutions
// N=6 has 720 solutions
// N=7 has 6240 solutions

