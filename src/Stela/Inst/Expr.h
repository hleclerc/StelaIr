#ifndef EXPR_H
#define EXPR_H

#include "Inst.h"

/**
*/
class Expr {
public:
    typedef Vec<Inst::Out::Item,-1,1> TPar;

    Expr( Ptr<Inst> inst, int nout = 0 );
    Expr();

    const PI8 *cst_data() const;
    void write_to_stream( Stream &os ) const;
    operator bool() const { return inst; }
    int size_in_bits() const;
    int size_in_bytes() const;

    const TPar &parents() const { return inst->out_expr( nout ).parents; }
    TPar &parents() { return inst->out_expr( nout ).parents; }

    bool conv( SI32 &val ) const;

    Ptr<Inst> inst;
    int       nout; ///< num output of inst to use
};

#endif // EXPR_H
