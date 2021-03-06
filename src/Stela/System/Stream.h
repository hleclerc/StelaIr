#ifndef STREAM_H
#define STREAM_H

#include "TensorOrder.h"
#include "TypeConfig.h"
#include "EnableIf.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

template<class O,class R> struct ShouldBeDisplayedWriteToStream {
    typedef R T;
};

template<class R> struct ShouldBeDisplayedWriteToStream<Bool,R> {};
template<class R> struct ShouldBeDisplayedWriteToStream<char,R> {};

template<class R> struct ShouldBeDisplayedWriteToStream<SI8 ,R> {};
template<class R> struct ShouldBeDisplayedWriteToStream<SI16,R> {};
template<class R> struct ShouldBeDisplayedWriteToStream<SI32,R> {};
template<class R> struct ShouldBeDisplayedWriteToStream<SI64,R> {};

template<class R> struct ShouldBeDisplayedWriteToStream<PI8 ,R> {};
template<class R> struct ShouldBeDisplayedWriteToStream<PI16,R> {};
template<class R> struct ShouldBeDisplayedWriteToStream<PI32,R> {};
template<class R> struct ShouldBeDisplayedWriteToStream<PI64,R> {};

template<class R> struct ShouldBeDisplayedWriteToStream<FP32,R> {};
template<class R> struct ShouldBeDisplayedWriteToStream<FP64,R> {};
template<class R> struct ShouldBeDisplayedWriteToStream<FP80,R> {};

template<class O,class R> struct ShouldBeDisplayedWriteToStream<O *,R> {};
template<ST n,class R> struct ShouldBeDisplayedWriteToStream<char [n],R> {};

template<class R> struct ShouldBeDisplayedWriteToStream<std::string,R> {};


template<class T>
typename EnableIf<
  TensorOrder<T>::res == 0,
  typename ShouldBeDisplayedWriteToStream<T,std::ostream>::T
>::T &operator<<( std::ostream &os, const T &val ) {
    const_cast<T &>( val ).write_to_stream( os );
    return os;
}



template<class T>
typename EnableIf<
  TensorOrder<T>::res == 1,
  std::ostream
>::T &operator<<( std::ostream &os, const T &val ) {
    if ( int s = val.size() ) {
        os << val[ 0 ];
        for( int i = 1; i < s; ++i )
            os << ' ' << val[ i ];
    }
    return os;
}

#ifndef PRINT
    #define PRINT( A ) \
        std::cout << "  " << #A << " -> " << (A) << std::endl
    #define PRINTN( A ) \
        std::cout << "  " << #A << " ->\n" << (A) << std::endl
    #define PRN( A ) \
        std::cout << "  " << #A << " => " << ip->str_cor.str( A ) << std::endl
#endif

typedef std::string  String;
typedef std::ostream Stream;

template<class T>
String to_string( const T &val ) {
    std::ostringstream ss;
    ss << val;
    return ss.str();
}

String to_string( const PI8 *val );


#endif // STREAM_H
