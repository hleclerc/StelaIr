#include "RefExpr.h"
#include "Var.h"

Var::Var( Interpreter *ip, Var *type, const Expr &expr ) : data( new PRef( ip ) ), type( type->data ), flag( 0 ) {
    data->ptr = new RefExpr( expr );
}

Var::Var( Interpreter *ip, Var *type ) : data( new PRef( ip ) ), type( type->data ), flag( 0 ) {
}

Var::Var() : flag( 0 ) {
}

Var::~Var() {
}

const PI8 *Var::cst_data() const {
    return data and data->ptr ? data->ptr->get().cst_data() : 0;
}
