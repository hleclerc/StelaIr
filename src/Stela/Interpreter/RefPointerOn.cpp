#include "../Inst/PointerOn.h"
#include "../Inst/Arch.h"
#include "RefPointerOn.h"
#include "Interpreter.h"

RefPointerOn::RefPointerOn( Var var ) : var( var ) {
}

Expr RefPointerOn::expr() const {
    return pointer_on( var.expr() );
}

bool RefPointerOn::indirect_set( Expr expr, Scope *set_scope, const Expr &sf, int off, Expr ext_cond ) {
    //Expr res = setval( var.expr(), expr, beg );
    //set_scope->set( var, res, sf, off, ext_cond );
    TODO;
    return true;
}

void RefPointerOn::write_to_stream( Stream &os ) const {
    os << "&" << var;
}

Var pointer_on( Var var ) {
    Var type_var = ip->type_of( var );
    return Var( ip->type_for( ip->class_info( ip->class_Ptr.expr() ), &type_var ), new RefPointerOn( var ) );
}
