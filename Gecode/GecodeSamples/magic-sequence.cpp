#include <gecode/driver.hh>
#include <gecode/int.hh>
#include <gecode/minimodel.hh>

using namespace Gecode;

class MagicSequence : public Script {
private:
  const int n;
  IntVarArray s;
public:
  MagicSequence(const SizeOptions& opt)
    : n(opt.size()), s(*this,n,0,n-1) {
      for (int i=n; i--; )
        count(*this, s, i, IRT_EQ, s[i]);
      //Redundant constraint
      //linear(*this, s, IRT_EQ, n);

      //counting multiple values simultaneously
      //count(*this, s, s, opt.icl());

      //Redundant constraint
      //linear(*this, IntArgs::create(n,0,1), s, IRT_EQ, n);
    branch(*this, s, INT_VAR_NONE(), INT_VAL_MAX());
  }

  MagicSequence(bool share, MagicSequence& e) : Script(share,e), n(e.n) {
    s.update(*this, share, e.s);
  }
  virtual Space*  copy(bool share) {
    return new MagicSequence(share,*this);
  }

  virtual  void print(std::ostream& os) const {
    os << "\t";
    for (int i = 0; i<n; i++) {
      os << s[i] << ", ";
      if ((i+1) % 20 == 0)
        os << std::endl << "\t";
    }
    os << std::endl;
  }

};

int main(int argc, char* argv[]) {
  SizeOptions opt("MagicSequence");
  opt.solutions(0);
  opt.size(500);
  opt.parse(argc,argv);
  Script::run<MagicSequence,DFS,SizeOptions>(opt);
  return 0;
}

