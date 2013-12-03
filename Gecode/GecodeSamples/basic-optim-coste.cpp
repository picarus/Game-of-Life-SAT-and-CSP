#include <gecode/int.hh>
#include <gecode/minimodel.hh>
#include <gecode/search.hh>

using namespace Gecode;

class Basic : public MaximizeSpace {
protected:
  IntVarArray X;
  IntVar Cost;
public:
  Basic(void) : X(*this, 4, 0, 5), Cost(*this,0,1000) {

    //constraints

    rel(*this, X[0]+X[1]+X[2]+X[3] == 8);
    rel(*this, Cost == X[1]+X[2]);

    //Search
    branch(*this, X, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
  }

  Basic(bool share, Basic& s) : MaximizeSpace(share, s) {
    X.update(*this, share, s.X);
    Cost.update(*this, share, s.Cost);
  }
  virtual Space* copy(bool share) {
    return new Basic(share,*this);
  }
  void print(void) const {
    std::cout << X << ", " << Cost << std::endl;
  }
  // cost function
  virtual IntVar cost(void) const{return Cost;}    // mandatory for min/max problem. Must return Cost the cost variable
};

int main(int argc, char* argv[]) {
  Basic* m = new Basic;
  BAB<Basic> e(m);    // branch and bound (for min/max problems)
  delete m;
  while (Basic* s = e.next()){s->print(); delete s;}  // show new solutions better than the previously found
  return 0;
}
