#include <gecode/int.hh>
#include <gecode/minimodel.hh>
#include <gecode/search.hh>

using namespace Gecode;

class Basic : public Space {
protected:
  IntVarArray X;
public:
  Basic(void) : X(*this, 4, 0, 1) {

    //constraints
    for(int i=0;i<3;i++) {rel(*this,X[i]!= X[i+1]);}

    //Search
    branch(*this, X, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
  }

  Basic(bool share, Basic& s) : Space(share, s) {
    X.update(*this, share, s.X);
  }
  virtual Space* copy(bool share) {
    return new Basic(share,*this);
  }
  void print(void) const {
    std::cout << X << std::endl;
  }
};


int main(int argc, char* argv[]) {
  Basic* m = new Basic;
  DFS<Basic> e(m);
  delete m;
  /* one solution*/
  //if(Basic* s = e.next()){s->print(); delete s;}

  /* all solutions */
  while (Basic* s = e.next()) {
    s->print(); delete s;
  }
  
}
