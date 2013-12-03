#include <gecode/int.hh>
#include <gecode/minimodel.hh>
#include <gecode/search.hh>

using namespace Gecode;

class SendMoreMoney : public Space {
protected:
  IntVarArray l;
public:
  SendMoreMoney(void) : l(*this, 8, 0, 9) {
    IntVar s(l[0]), e(l[1]), n(l[2]), d(l[3]),
           m(l[4]), o(l[5]), r(l[6]), y(l[7]);
    rel(*this, s != 0);
    rel(*this, m != 0);
    distinct(*this, l);
    
     rel(*this,             1000*s + 100*e + 10*n + d
                         + 1000*m + 100*o + 10*r + e
              == 10000*m + 1000*o + 100*n + 10*e + y);

    
    /*    
    IntArgs c(4+4+5); IntVarArgs x(4+4+5);
    c[0]=1000; c[1]=100; c[2]=10; c[3]=1;
    x[0]=s;    x[1]=e;   x[2]=n;  x[3]=d;
    c[4]=1000; c[5]=100; c[6]=10; c[7]=1;
    x[4]=m;    x[5]=o;   x[6]=r;  x[7]=e;
    c[8]=-10000; c[9]=-1000; c[10]=-100; c[11]=-10; c[12]=-1;
    x[8]=m;      x[9]=o;     x[10]=n;    x[11]=e;   x[12]=y;
    linear(*this, c, x, IRT_EQ, 0);  // c, x, comparator, value= c*x comp value
    */


    branch(*this, l, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
  }
  SendMoreMoney(bool share, SendMoreMoney& s) : Space(share, s) {
    l.update(*this, share, s.l);
  }
  virtual Space* copy(bool share) {
    return new SendMoreMoney(share,*this);
  }
  void print(void) const {
    std::cout << l << std::endl;
  }
};

int main(int argc, char* argv[]) {
  SendMoreMoney* m = new SendMoreMoney;
  DFS<SendMoreMoney> e(m);
  delete m;
  while (SendMoreMoney* s = e.next()) {
    s->print(); delete s;
  }
  return 0;
}
