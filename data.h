#ifndef __MLLIB_DATA_H_
#define __MLLIB_DATA_H_
#include "itensor/all.h"
#include "mllib/datatype.h"

using std::vector;
using std::string;

namespace mllib {

template<typename value_type_, int NL_>
struct Data
    {
    static const int NL = NL_;
    using value_type = value_type_;

    int n = -1;     //which data set element this is
    int label = -1; //label of which class it belongs to
    double y = 0.;    //target output value for scalar classifiers
    DataType type = DataType("None");
    vector<value_type> data;
    string name;

    Data() { }

    Data(int n_, int label_ = -1)
        : n(n_), label(label_)
        { }

    Data(int n_, int label_, int size_)
        : n(n_), label(label_), data(size_)
        { }

    Data(int n_, DataType type_, int label_ = -1)
        : n(n_), label(label_), type(type_)
        { }

    Data(int n_, DataType type_, int label_, int size_)
        : n(n_), label(label_), type(type_), data(size_)
        { }

    value_type static
    default_value() { return value_type{}; }

    int
    size() const { return data.size(); }

    //operator[] is 0-indexed
    value_type&
    operator[](int n);
    value_type
    operator[](int n) const;

    //operator() is 1-indexed
    value_type&
    operator()(int n);
    value_type
    operator()(int n) const;

    };

//
// Implementations
//

//operator[] is 0-indexed
inline
value_type& Data::
operator[](int n) 
    { 
#ifdef DEBUG
    return data[n]; 
#else
    return data.at(n); 
#endif
    }

inline
value_type Data::
operator[](int n) const 
    { 
#ifdef DEBUG
    return data[n]; 
#else
    return data.at(n); 
#endif
    }

//operator() is 1-indexed
inline
value_type& Data::
operator()(int n) 
    { 
#ifdef DEBUG
    return data[n-1]; 
#else
    return data.at(n-1); 
#endif
    }
inline
value_type Data::
operator()(int n) const 
    { 
#ifdef DEBUG
    return data[n-1]; 
#else
    return data.at(n-1); 
#endif
    }

} //namespace mllib

#endif
