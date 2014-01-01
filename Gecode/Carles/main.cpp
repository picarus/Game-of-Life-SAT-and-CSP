#include <iostream> //To be able to read
#include <fstream>
#include <stdlib.h>

#include <gecode/driver.hh>
#include <gecode/int.hh>
#include <gecode/minimodel.hh>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

using namespace Gecode;


class Life : public MaximizeScript {//If you use driver the parent class must be of type script
protected:
  BoolVarArray grid; //Stores the matrix of the grid
  IntVar density; //Stores the number of cells alive
  int n;
public:
  Life(const InstanceOptions& opt) {
    //Read input file
	  /*
    std::ifstream fin;
    fin.open(opt.instance());
    std::string line;
    if (!fin.is_open()){
      while(getline(fin, line)){
        std::cout << line << '\n';
      }
      fin.close();
    }else{
        return;
      //throw std::runtime_error("File not found or invalid.");
    }
	*/
    n = 4;
    int nn = n*n;
    int m = 2;
    int input[2][2] ={{1,1},{2,2}};

    grid = BoolVarArray(*this, nn, 0, 1);
    Matrix<BoolVarArray> mat(grid, n);
    density = IntVar(*this, 0, nn);

    //Grid initial state
    for (int i = 0; i < m; i++){
        rel(*this, mat(input[i][0], input[i][1]) == 1);
    }

    for (int i = 0; i < n; i++){
        for (int j = 0; j < n; j++){
            //Neighbourhood computing. Neighbourhood contains the center cell
            //all constraints take this into consideration
            BoolVar alive = expr(*this, mat(i,j) == 1);
            int i_a = MAX(i-1, 0);
            int i_b = MIN(n-1, i+1);
            int j_a = MAX(j-1, 0);
            int j_b = MIN(n-1, j+1);

            //+1 because the open bounds in the right side
            //Look at: http://www.gecode.org/doc-latest/reference/
            //classGecode_1_1Matrix.html#a5e32ad0b25d8a0f7606d2ac9607220db
            Slice<BoolVarArray> neighbourhood = mat.slice(i_a, i_b+1, j_a, j_b+1);
            IntVar neircount = expr(*this, sum(neighbourhood));

            //CONSTRAINTS
            //1. If alive
                //1.1 neighbours alive >= 2 and <= 3

                //As m(i,j) is in the neighbourhood and it's alive, +1 to the limits
                //rel(*this, alive >> (sum(neighbourhood)));
                rel(*this, alive >> ((neircount == 3) || (neircount == 4)));

            //2. If dead
                //2.1 neighbours alive != 3

                //as m(i,j) is dead, it is not needed to sum +1.
                rel(*this, !alive >> (neircount != 3));

            //3. Boundary conditions
                //TODO: Sequence constraint
                if((i == 0 || i == n-1) && (j != 0 && j != n-1)){
                    rel(*this,  mat(i_a, j) == 0 || !alive || mat(i_b, j) == 0);
                }
                if((j == 0 || j == n-1) && (i != 0 && i != n-1)){
                    rel(*this, mat(i, j_a) == 0 || !alive || mat(i, j_b) == 0);
                }
        }
    }



    //2. If dead

    rel(*this, sum(grid) == density);//Optimization problem definition
    Rnd rnd = Rnd();
    rnd.time();
	 //branch(*this, grid, INT_VAR_RND(rnd), INT_VAL_MAX());
    branch(*this, grid, INT_VAR_SIZE_MAX(), INT_VAL_MAX());
  }

  Life(bool share, Life& s)
    : MaximizeScript(share, s) {
	n=s.n;
    grid.update(*this, share, s.grid);
    density.update(*this, share, s.density);
  }

  virtual Space* copy(bool share) {
    return new Life(share,*this);
  }

  virtual IntVar cost(void) const {//cost of a given solution
    return density;
  }

  virtual void print(std::ostream& os) const {
    for(int i = 0; i < n; i++){
        for (int j = 0; j < n; j++){
            os <<grid[i + (j*n)] << ", ";
        }
        os << std::endl;
    }
    os << "Density: " << density << std::endl << std::endl;
  }
};




int main(int argc, char* argv[]) {
    try{
        InstanceOptions opt("DENSE GAME OF LIFE"); //Allows to get options in runtime
        opt.solutions(0);// Number of solutions
       // opt.instance("input<.pb");
        //opt.parse(argc,argv);
        Script::run<Life,BAB,InstanceOptions>(opt);
    }catch(Exception e){
        std::cout <<  "Gecode exception: " << e.what() << std::endl;
        return 1;
    }
    std::cout << "DONE" << std::endl;
    return 0;
}


