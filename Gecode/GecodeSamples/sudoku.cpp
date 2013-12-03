#include <gecode/driver.hh>
#include <gecode/int.hh>
#include <gecode/minimodel.hh>

using namespace Gecode;

const int n=3;
const int nn= n*n;
const int instance[nn][nn] = {
  {2, 0, 0, 5, 0, 6, 0, 9, 8},
  {0, 0, 0, 7, 8, 0, 5, 0, 0},
  {5, 0, 4, 0, 0, 0, 0, 0, 6},
  {0, 3, 1, 0, 5, 0, 0, 8, 0},
  {0, 0, 0, 1, 0, 4, 0, 0, 0},
  {0, 9, 0, 0, 7, 0, 6, 1, 0},
  {3, 0, 0, 0, 0, 0, 8, 0, 7},
  {0, 0, 5, 0, 2, 7, 0, 0, 0},
  {1, 7, 0, 8, 0, 3, 0, 0, 2},
};



class SudokuInt : public Script {
protected:
  IntVarArray x;
public:
  SudokuInt(const Options& opt) :x(*this, nn*nn, 1, nn) {
    Matrix<IntVarArray> m(x, nn, nn);  // alias for the x variables as a matrix
    // Constraints for rows and columns
    for (int i=0; i<nn; i++) {
      distinct(*this, m.row(i));
      distinct(*this, m.col(i));
    }

    // Constraints for squares
    for (int i=0; i<nn; i+=n) {
      for (int j=0; j<nn; j+=n) {
        distinct(*this, m.slice(i, i+n, j, j+n)); // a 3x3 in the sudoku
      }
    }
       // Fill-in predefined fields
    for (int i=0; i<nn; i++)
      for (int j=0; j<nn; j++)
        if (instance[i][j]!=0) {
	  rel(*this, m(i,j) == instance[i][j]); // already defined in the instance
	}
     branch(*this, x, INT_VAR_SIZE_MIN(), INT_VAL_SPLIT_MIN());
  }

  SudokuInt(bool share, SudokuInt& s) : Script(share, s) {
    x.update(*this, share, s.x);
  }

  virtual Space*  copy(bool share) {
    return new SudokuInt(share,*this);
  }

  virtual void  print(std::ostream& os) const {
    os << "  ";
    for (int i = 0; i<n*n*n*n; i++) {
      os << x[i] << " ";
      if((i+1)%(n*n) == 0)
        os << std::endl << "  ";
    }
    os << std::endl;
    
  }
};


int main(int argc, char* argv[]) {
  Options opt("SUDOKU");
  opt.solutions(0);
  opt.parse(argc,argv);
  Script::run<SudokuInt,DFS,Options>(opt);
  return 0;
}


