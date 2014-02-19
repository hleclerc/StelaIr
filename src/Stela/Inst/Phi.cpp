#include "InstVisitor.h"
#include "BaseType.h"
#include "Inst_.h"
#include "Phi.h"
#include "Cst.h"

class Phi : public Inst_<1,3> {
public:
    virtual int size_in_bits( int nout ) const { return inp_expr( 1 ).size_in_bits(); }
    virtual void write_dot( Stream &os ) const { os << "phi"; }
    virtual void apply( InstVisitor &visitor ) const { visitor.phi( *this ); }
    virtual int inst_id() const { return Inst::Id_Phi; }
    virtual const BaseType *out_bt( int n ) const { return inp_expr( 1 ).out_bt(); }
};

Expr phi( Expr cond, Expr ok, Expr ko ) {
    // known value ?
    bool v;
    if ( cond.get_val( v ) )
        return v ? ok : ko;

    // the same value in all the cases ?
    if ( ok == ko )
        return ok;

    // cond = bool( ... )
    if ( cond.inst->inst_id() == Inst::Id_Conv and cond.inst->out_bt( 0 ) == bt_Bool )
        cond = cond.inst->inp_expr( 0 );

    // else, create a new inst
    Phi *res = new Phi;
    res->inp_repl( 0, cond );
    res->inp_repl( 1, ok );
    res->inp_repl( 2, ko );
    return Expr( Inst::factorized( res ), 0 );
}

