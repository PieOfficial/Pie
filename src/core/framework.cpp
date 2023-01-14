
/* 
EXAMPLE:
class My_filter : public Filter {
istream& is;
ostream& os;
int nchar;
public:
int read() { char c; is.get(c); return is.good(); }
void compute() { nchar++; }
int result() { os << nchar << " characters read\n"; return 0;}
void start() {cout << "started";}

My_filter(istream& ii, ostream& oo) : is(ii), os(oo), nchar(0) {}
};

MAIN:
My_filter f(cin,cout);
return main_loop(&f);
*/
#include "framework.hpp" // header in local directory
#include <iostream> // header in standard library

using namespace framework;
using namespace std;



int Filter::main_loop(Filter* p) {
  for(;;) {
    try {
      p->start(); 
      // while (p->read()) {
      //   p->compute();
      //   p->write();
      // }
      return p->result();
    }
    catch (Filter::Retry& m) {
      if (int i = p->retry(m)) return i;
    }
    catch (...) {
        cout << "Fatal filter error\n";
        return 1;
    }
  }
}