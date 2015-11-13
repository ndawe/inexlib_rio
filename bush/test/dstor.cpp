
#include <cstdio>

class IA0 {
public:
  virtual void xxx() = 0;
public:
  //virtual 
  virtual ~IA0() {
    ::printf("IA0::~IA0\n");
  }
};

class A0 
: public IA0 
{
public:
  virtual void xxx() {}
public:
  //virtual 
  ~A0() {
    ::printf("A0::~A0\n");
  }
};

class A1 : public A0 {
public:
  //virtual 
  ~A1() {
    ::printf("A1::~A1\n");
  }
};

class A2 : public A1 {
public:
  //virtual 
  ~A2() {
    ::printf("A2::~A2\n");
  }
};


int main() {
  IA0* a = new A2;
  //IA0* ia = a;
  delete a;
  return 0;
}
