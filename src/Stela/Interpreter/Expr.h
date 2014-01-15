#ifndef EXPR_H
#define EXPR_H

#include "Inst.h"

/**
*/
class Expr {
public:
    Expr( Ptr<Inst> inst, int nout = 0 );
    Expr();

    const PI8 *cst_data() const;
    void write_to_stream( Stream &os ) const;

    Ptr<Inst> inst;
    int       nout; ///< num output of inst to use
};

// bad hack
inline const Expr &Inst::InpList::operator[]( int ind ) const { return _data[ ind ]; }

#endif // EXPR_H
