#include <gecode/driver.hh>			// includes search, then is not required to be included
#include <gecode/int.hh>
#include <gecode/minimodel.hh>

using namespace Gecode;

class SendMostMoney : public MaximizeScript {		// if driver is used MaximizeScript (not MaximizeSpace)
protected:
  IntVarArray l;
  IntVar money;
public:
  SendMostMoney(const Options& opt)
    : l(*this, 8, 0, 9), money(*this,0,100000) {		// huge domain to make sure we do not loose the optimal solution
    IntVar s(l[0]), e(l[1]), n(l[2]), d(l[3]), m(l[4]), 
           o(l[5]), t(l[6]), y(l[7]);
    rel(*this, s != 0);
    rel(*this, m != 0);
    distinct(*this, l);
    rel(*this,             1000*s + 100*e + 10*n + d
                           + 1000*m + 100*o + 10*s + t
                == 10000*m + 1000*o + 100*n + 10*e + y);
 
    /*
    IntArgs c(4+4+5); IntVarArgs x(4+4+5);
    c[0]=1000; c[1]=100; c[2]=10; c[3]=1;
    x[0]=s;    x[1]=e;   x[2]=n;  x[3]=d;
    c[4]=1000; c[5]=100; c[6]=10; c[7]=1;
    x[4]=m;    x[5]=o;   x[6]=s;  x[7]=t;
    c[8]=-10000; c[9]=-1000; c[10]=-100; c[11]=-10; c[12]=-1;
    x[8]=m;      x[9]=o;     x[10]=n;    x[11]=e;   x[12]=y;
    linear(*this, c, x, IRT_EQ, 0);
    */

    rel(*this, money == 10000*m + 1000*o + 100*n + 10*e + y);
    branch(*this, l, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
  }
  SendMostMoney(bool share, SendMostMoney& s)
    : MaximizeScript(share, s) {
    l.update(*this, share, s.l);
    money.update(*this, share, s.money);
  }
  virtual Space* copy(bool share) {
    return new SendMostMoney(share,*this);
  }
  virtual IntVar cost(void) const {
    return money;
  }
  virtual void print(std::ostream& os) const {
    os << l <<" "<<money<<std::endl;
  }
};

int main(int argc, char* argv[]) {
  Options opt("SEND + MOST = MONEY"); // just a name
  opt.solutions(0);				 // options to execute the problem: 1/all solutions --> 0: 1 solution
  opt.parse(argc,argv);		      // options specified at execution time
  Script::run<SendMostMoney,BAB,Options>(opt);	// options passed to the script 
  return 0;
}
