#include "InstVisitor.h"
#include "PointerOn.h"
#include "Slice.h"
#include "Inst_.h"
#include "Arch.h"
#include "Cst.h"
#include "Op.h"

///
class Slice : public Inst_<1,1> {
public:
    virtual int size_in_bits( int nout ) const { return end - beg; }
    virtual void write_dot( Stream &os ) const { os << "slice[" << beg << "," << end << "]"; }
    virtual void apply( InstVisitor &visitor ) const { visitor.slice( *this, beg, end ); }
    virtual int inst_id() const { return Inst::Id_Slice; }
    virtual bool equal( const Inst *b ) const {
        return Inst::equal( b ) and 
            beg == static_cast<const Slice *>( b )->beg and 
            end == static_cast<const Slice *>( b )->end;
    }
    virtual const PI8 *cst_data( int nout, int lbeg, int lend ) const {
        return inp_expr( 0 ).cst_data( beg + lbeg, beg + lend );
    }
    virtual int sizeof_additionnal_data() const {
        return 2 * sizeof( int );
    }
    virtual void copy_additionnal_data_to( PI8 *dst ) const {
        memcpy( dst + 0 * sizeof( int ), &beg, sizeof( int ) );
        memcpy( dst + 1 * sizeof( int ), &end, sizeof( int ) );
    }
    virtual Expr _smp_pointer_on( int nout ) {
        if ( Expr res = Inst::_smp_pointer_on( nout ) )
            return res;
        if ( beg % 8 )
            TODO;
        return op_add( arch->bt_ptr(), pointer_on( inp_expr( 0 ) ), arch->cst_ptr( beg / 8 ) );
    }
    int beg, end;
};


///
class SliceUnk : public Inst_<1,2> {
public:
    virtual int size_in_bits( int nout ) const { return len; }
    virtual void write_dot( Stream &os ) const { os << "slice[" << inp[ 0 ] << ",len=" << len << "]"; }
    virtual void apply( InstVisitor &visitor ) const { visitor.slice( *this, len ); }
    virtual int inst_id() const { return Inst::Id_SliceUnk; }
    virtual bool equal( const Inst *b ) const { return Inst::equal( b ) and len == static_cast<const SliceUnk *>( b )->len; }
    int len;
};

Expr slice( Expr expr, const Expr &beg, int len ) {
    SliceUnk *res = new SliceUnk;
    res->inp_repl( 0, expr );
    res->inp_repl( 1, beg  );
    res->len = len;
    return Expr( Inst::factorized( res ), 0 );
}

Expr slice( Expr expr, int beg, int end ) {
    ASSERT( beg >= 0 and beg <= expr.size_in_bits(), "Wrong size" );
    ASSERT( end >= 0 and end <= expr.size_in_bits(), "Wrong size" );
    ASSERT( beg <= end, "Wrong size" );

    // simplifications ?
    if ( Expr res = expr.inst->_smp_slice( expr.nout, beg, end ) )
        return res;

    // else create a new inst
    Slice *res = new Slice;
    res->inp_repl( 0, expr );
    res->beg = beg;
    res->end = end;
    return Expr( Inst::factorized( res ), 0 );
}
