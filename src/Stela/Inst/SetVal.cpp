#include "InstVisitor.h"
#include "SetValData.h"
#include "SetVal.h"
#include "Inst_.h"
#include "Cst.h"
#include "Op.h"

///
class SetVal : public Inst_<1,3> {
public:
    virtual int size_in_bits( int nout ) const { return inp_expr( 0 ).size_in_bits(); }
    virtual void write_dot( Stream &os ) const { os << "setval"; }
    virtual void apply( InstVisitor &visitor ) const { visitor.setval( *this ); }
    virtual int inst_id() const { return Inst::Id_SetVal; }
    virtual int sizeof_additionnal_data() const { return sizeof( data ); }
    virtual void copy_additionnal_data_to( PI8 *dst ) const { memcpy( dst, &data, sizeof( data ) ); }
    SetValData data;
};

Expr setval( Expr a, Expr b, Expr beg, bool beg_in_bits ) {
    if ( a.size_in_bits() == 0 or b.size_in_bits() == 0 )
        return a;

    // simplifications ?
    int off;
    if ( beg.get_val( off ) )
        return setval( a, b, beg, beg_in_bits );

    // else create a new inst
    if ( beg_in_bits ) {
        if ( beg.inst->inst_id() == Inst::Id_Op_mul ) {
            SI64 m;
            const BaseType *bt = beg.inst->out_bt( 0 );
            if ( beg.inst->inp_expr( 0 ).get_val( m ) and m % 8 == 0 )
                return setval( a, b, op_mul( bt, cst( bt, m / 8 ), beg.inst->inp_expr( 1 ) ), false );
            if ( beg.inst->inp_expr( 1 ).get_val( m ) and m % 8 == 0 )
                return setval( a, b, op_mul( bt, cst( bt, m / 8 ), beg.inst->inp_expr( 0 ) ), false );
        }
    }

    SetVal *res = new SetVal;
    res->data.size_is_in_bytes = not beg_in_bits;
    res->data.off_size_in_bits = beg.size_in_bits();
    res->inp_repl( 0, a );
    res->inp_repl( 1, b );
    res->inp_repl( 2, beg );
    return Expr( Inst::factorized( res ), 0 );
}

Expr setval( Expr a, Expr b, int beg, bool beg_in_bits ) {
    if ( a.size_in_bits() == 0 or b.size_in_bits() == 0 )
        return a;

    if ( beg % 8 == 0 and beg_in_bits )
        return setval( a, b, beg / 8, false );

    // else create a new inst
    SetVal *res = new SetVal;
    res->data.size_is_in_bytes = not beg_in_bits;
    res->data.off_size_in_bits = 32;
    res->inp_repl( 0, a );
    res->inp_repl( 1, b );
    res->inp_repl( 2, cst( beg ) );
    return Expr( Inst::factorized( res ), 0 );
}