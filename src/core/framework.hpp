#include <iostream>

namespace framework {
class Filter {
 public:
  class Retry {
   public:
    virtual const char* message() { return 0; }
  };
  virtual void start() {}
  virtual int read() = 0;
  virtual void write() {}
  virtual void compute() {}
  virtual int result() = 0;

  virtual int retry(Retry& m) {
    std::cout << m.message() << '\n';
    return 2;
  }

  //int main_loop(Filter* p);

  virtual ~Filter() {}
int main_loop(Filter * p);
};
};  // namespace framework
