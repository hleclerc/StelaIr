#ifndef PREF_H
#define PREF_H

#include "Ref.h"
class Interpreter;
class VRF;
class Var;

/**
  Pointer on a ref object
*/
struct PRef : ObjectWithCptUse {
    enum {
        CONST = 1 ///< this variable cannot be changed (and it's definitive)
    };

    PRef( Ref *ref ) : ptr( ref ), refs( 0 ), date( cur_date ), flags( 0 ) {}
    PRef() : refs( 0 ), date( cur_date ), flags( 0 ) {}
    ~PRef();

    void write_to_stream( Stream &os ) const { os << ptr; }
    Expr expr() const { return ptr ? ptr->expr() : Expr(); }

    void add_ref_from( const PRef *var );
    void add_ref( int offset, const Var &var );
    Var  get_ref( int offset );
    VRF *get_vrf( int offset );

    bool is_const() const { return flags & CONST; }


    Ptr<Ref>     ptr;
    Vec<VRF>    *refs; ///< holded references
    PI64         date; ///< creation date
    int          flags;

    static PI64  cur_date;
};

#endif // PREF_H
