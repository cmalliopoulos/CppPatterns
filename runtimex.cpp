#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <cassert>
#include <numeric>

using namespace std;
typedef vector<double> dvec;




// Vectors of T supporting fusion of operations and ufuncs
template<typename T> class Base {
public:
    // Declare them const so that we can call them with const Base objects
    virtual T operator [](size_t i) const = 0;
    virtual size_t size() const = 0;
};


// Thin wrappers around vector to make it Base descendant.
//
// 1. A Vector with copy semantics
template<typename T> class Vector: public Base<T> {
    typedef vector<T> value_type;
    value_type data;

public:
    // Initializers of Vector objects from external data.
    Vector(const value_type &data_): data(data_.size()) { 
        data = data_; 
    }

    Vector(const Base<T> &other_): data(other_.size()) {  
        for (size_t i = 0; i < other_.size(); ++i)
            data[i] = other_[i];
    }

    virtual T operator [](size_t i) const { return data[i]; }
    virtual size_t size() const { return data.size(); }

    // Convert to vector<T>
    operator value_type &() { return data; }
};


/*
 * 2. A Vector (RefVector) with varying semantics based on use:
 *    If created from a concrete container (aka vector<T>), it uses the memory of the
 *    container.
 *    Otherwise allocates its own.
 */
template<typename T> class RefVector: public Base<T> {
    vector<T> *pData;
    bool owns;

public:
    RefVector(vector<T> &other_): pData(&other_), owns(false) { }

    RefVector(const Base<T> &aBase_) {
        pData = new vector<T>(aBase_.size());
 
        for(size_t i = 0; i < aBase_.size(); ++i)
            *(pData->begin() + i) = aBase_[i];
        owns = true;
    }

   ~RefVector() {
        if(owns == true) delete pData;
   }

    virtual T operator [](size_t i) const { return *(pData->begin() + i); }
    virtual size_t size() const { return pData->size(); }

    // Convert to vector<T>
    operator vector<T> &() { return *pData; }
};


// Non-terminal unary expression
// Terminal expressions should be specialization of UEX<Op>
template<typename T, typename Op> class UEX: public Base<T> {
    const Base<T> &expr;
    Op op;

public:
    UEX(const Base<T> &expr_): expr(expr_) { }
    virtual T operator [](size_t i_) { return op(expr[i_]); }
    virtual size_t size() const { return expr.size(); }
};



// Non-terminal node B(inary)E(xpression)
template<typename T, typename Op> class BEX: public Base<T> {
    const Base<T> &l;
    const Base<T> &r;
    Op op;

public:
    BEX(const Base<T> &l_, const Base<T> &r_): l(l_), r(r_) { 
        assert(l_.size() == r_.size());
    }

    virtual T operator [](size_t i) const { return op(l[i], r[i]); }
    virtual size_t size() const { return l.size(); }
};


// It must be Base& because Base is pure virtual
// It must be const for being able to construct higher order sums a + b + c + . . . (we cannot handle temporaries otherwise)
// For simple sums like a + b const is not a constraint though
template<typename T>
BEX<T, plus<T> > operator + (const Base<T> &L, const Base<T> &R) { return BEX<T, plus<T> >(L, R); }

template<typename T>
BEX<T, minus<T> > operator - (const Base<T> &L, const Base<T> &R) { return BEX<T, minus<T> >(L, R); }

template<typename T>
BEX<T, multiplies<T> > operator * (const Base<T> &L, const Base<T> &R) { return BEX<T, multiplies<T> >(L, R); }

template<typename T>
BEX<T, multiplies<T> > operator / (const Base<T> &L, const Base<T> &R) { return BEX<T, divides<T> >(L, R); }


/*
 * C o n s i d e r a t i o n s
 * - - - - - - - - - - - - - - 
 *
 * 1. The type-casting operator to (dvec &) does not work for implicit cast. I cannot use 
 *    accumulate(ret.begin(), ret.end(), 0.). Instead I have to cast to vector<T>& explicitly
 * 
 * 2. operator + does not work for non-(const Base &). 
 */
int main(int argc, char *argv[]) {
    vector<double> data(10);
    for(size_t i = 0; i < data.size(); ++i) {
        data[i] = double(rand())/RAND_MAX - .5;
    }

    Vector<double> a(data), b(data), c(data);
    Vector<double> ret = a + b - c * a;

    RefVector<double> ra(data), rb(data), rc(data);
    RefVector<double> rret = ra + rb - rc * ra;

    for(size_t i = 0; i < ret.size(); ++i) 
        cout << ret[i] << ", " << rret[i] << endl;

    accumulate(static_cast<vector<double> &>(ret).begin(), static_cast<vector<double> &>(ret).end(), 0.);
    return 0;
}
