#ifndef MATH_H
#define MATH_H

#include "TypeInformation.h"
#include "TypeConfig.h"
#include "Assert.h"
#include "N.h"
#include "S.h"
#include <cmath>

template<class T>
T ceil( T a, T m ) {
    if ( not m )
        return a;
    return ( a + m - 1 ) / m * m;
}

template<class T>
T gcd( T a, T b ) {
    if ( b == 1 )
        return 1;

    T old;
    while ( b ) {
        old = b;
        b = a % b;
        a = old;
    }
    return a;
}

template<class T>
T ppcm( T a, T b ) {
    return a * b / gcd( a, b );
}

// generic conv
template<class A,class B>
bool conv( A &res, B src ) { res = src; return true; }

//template<class B>
//bool conv( void *&res, B src ) { res = (void*)(ST)src; return true; }

//template<class A>
//bool conv( A &res, void *src ) { res = (ST)src; return true; }

// error
template<class A>
bool conv( A &, Error ) { return false; }

// bool
template<class B>
bool conv( bool &res, B src ) { res = src; return true; }

inline bool conv( bool &, Error ) { return false; }



//template<class A,class B>
//bool conv( A &res, B *src ) { return false; }

//
template<class T1,class T2,class TR>
TR mod( T1 a, T2 b, N<0>, N<0>, S<TR> ) {
    return a % b;
}

template<class T1,class T2,int n1,int n2,class TR>
TR mod( T1 a, T2 b, N<n1>, N<n2>, S<TR> ) {
    return std::fmod( TR( a ), TR( b ) );
}

template<class T1,class T2>
typename TypePromote<T1,T2>::T mod( T1 a, T2 b ) {
    return mod( a, b,
                N<TypeInformation<T1>::float_type>(),
                N<TypeInformation<T2>::float_type>(),
                S<typename TypePromote<T1,T2>::T>()
                );
}

template<class T2>
ST mod( void *a, T2 b ) {
    return 0;
}

template<class T2>
ST mod( T2 a, void *b ) {
    return 0;
}


#endif // MATH_H
