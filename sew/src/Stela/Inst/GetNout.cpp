#include "GetNout.h"

/**
*/
struct GetNout : Inst {
    GetNout( int nout ) : nout( nout ) {}
    virtual void write_dot( Stream &os ) { os << "nout_" << nout; }
    virtual Expr forced_clone( Vec<Expr> &created ) const { return new GetNout( nout ); }
    virtual Type *ptype() { return inp[ 0 ]->ptype( nout ); }
    virtual Type *type() { return inp[ 0 ]->type( nout ); }
    int nout;
};

Expr get_nout( Expr inp, int nout ) {
    GetNout *res = new GetNout( nout );
    res->add_inp( inp );
    return res;
}

