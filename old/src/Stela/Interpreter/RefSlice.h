#ifndef REFSLICE_H
#define REFSLICE_H

#include "Ref.h"
#include "Var.h"
class Scope;

/**
*/
class RefSlice : public Ref {
public:
    RefSlice( const Var &var, int beg, int end );
    virtual Expr expr() const;
    virtual void write_to_stream( Stream &os ) const;
    virtual bool contains_var_referenced_more_than_one_time() const;
    virtual bool indirect_set( const Var &src, Scope *set_scope, const Expr &sf, int off, Expr ext_cond );
    virtual Ptr<Ref> copy();

    Var var;
    int beg;
    int end;
};

#endif // REFSLICE_H
