#include <gecode/int.hh>
#include <gecode/minimodel.hh>
#include <gecode/search.hh>

using namespace Gecode;

class Magic : public Space {
protected:
  IntVarArray l;
public:
  Magic(int n) : l(*this, n, 0, n) {

	for (int i=0;i<n;i++)
		count(*this, l, i, IRT_EQ, l[i]);
   
	// redundant constraints
	// the sum of elements is equal to N
	linear(*this, l,IRT_EQ, n);

	linear(*this, IntArgs::create(n,0,1), l, IRT_EQ, n);

    branch(*this, l, 
		INT_VAR_NONE(),		// VARIABLES ORDER
		INT_VAL_MAX());	    // VALUE_ORDERING
	
  }
  Magic(bool share, Magic& s) : Space(share, s) {
    l.update(*this, share, s.l);
  }
  virtual Space* copy(bool share) {
    return new Magic(share,*this);
  }
  void print(void) const {
    std::cout << l << std::endl;
  }
};

int main(int argc, char* argv[]) {
// SizeOptions as an alternative
  for (int size=1000; ;size+=1000){

	  Magic* m = new Magic(size);
	  DFS<Magic> e(m);
	  delete m;
	  if (Magic* s = e.next()) { // there is only one solution, not look for a second one
		std::cout << size ;
		//s->print(); 
		delete s;
	  }
  }
  return 0;
}
