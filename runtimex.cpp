#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <cassert>
#include <numeric>

// #include <random>

using namespace std;
typedef vector<double> dvec;


// Vectors of doubles supporting fusion of +
class Base {
public:

    // Declare them const so that we can call them with const Base objects
    virtual double operator [](size_t i) const = 0;
    virtual size_t size() const = 0;
};


// Terminal node I(tem)E(xpression)
class IX: public Base {
    dvec data;

public:
    // An initializer of IX objects from non-X data.
    // Used for creating terminal expression nodes
    IX(const dvec &other_): data(other_.size()) { data = other_; }

    // this is an lvalue (assignment) constructor. Creates an IX terminal node from the result of an 
    // expression on the right.
    IX(const Base &other_): data(other_.size()) {
        for(size_t i = 0; i < other_.size(); ++i) { data[i] = other_[i]; }
    }

    virtual double operator [](size_t i) const { return data[i]; }
    virtual size_t size() const { return data.size(); }

    // Convert to vector<double>
    operator dvec &() { return data; }
};



// Non-terminal node B(inary)E(xpression)
class BEX: public Base {
    const Base &l;
    const Base &r;

public:
    BEX(const Base &l_, const Base &r_): l(l_), r(r_) { 
        assert(l_.size() == r_.size());
    }

    virtual double operator [](size_t i) const { return l[i] + r[i]; }
    virtual size_t size() const { return l.size(); }
};


// It must be Base & because Base is pure virtual
// It must be const for being able to construct higher order sums a + b + c + . . . (we cannot handle temporaries otherwise)
// For simple sums like a + b const is not a constraint
BEX operator +(const Base &L, const Base &R) { return BEX(L, R); }



// Issues requiring explanation:
// -----------------------------
// 1. The type-casting operator to (dvec &) does not work for implicit cast. I cannot use 
//    accumulate(ret.begin(), ret.end(), 0.). Instead I have to cast to dvec& explicitly
// 
// 2. operator + does not work for non-const Bases (prob due to Base being pure virtual?)
int main(int argc, char *argv[]) {
    dvec data(10);
    for(size_t i = 0; i < data.size(); ++i) {
        data[i] = double(rand())/RAND_MAX - .5;
    }

    IX a(data), b(data), c(data);
    // IX ret = a + b + c + b + b + a;
    IX ret = a + b;

    for(size_t i = 0; i < ret.size(); ++i) 
        cout << ret[i] << endl;

    accumulate(static_cast<dvec &>(ret).begin(), static_cast<dvec &>(ret).end(), 0.);
    return 0;
}
