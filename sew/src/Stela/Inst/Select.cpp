#include "BoolOpSeq.h"
#include "Select.h"

/**
*/
struct Select : Inst {
    virtual void write_dot( Stream &os ) { os << "Select"; }
    virtual Expr forced_clone( Vec<Expr> &created ) const { return new Select; }
    virtual Type *type() { return inp[ 1 ]->type(); }
    Vec<Vec<Bool> > pos;
};

Expr select( const BoolOpSeq &cond, Expr ok, Expr ko ) {
    if ( cond.always( true  ) ) return ok;
    if ( cond.always( false ) ) return ko;
    if ( ok == ko ) return ok;

    Select *res = new Select;
    res->add_inp( ok );
    res->add_inp( ko );
    for( int i = 0; i < cond.or_seq.size(); ++i ) {
        Vec<Bool> *v = res->pos.push_back();
        for( int j = 0; j < cond.or_seq[ i ].size(); ++j ) {
            res->add_inp( cond.or_seq[ i ][ j ].expr );
            *v << cond.or_seq[ i ][ j ].pos;
        }
    }
    return res;
}

