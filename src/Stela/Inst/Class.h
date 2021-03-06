#ifndef CLASS_H
#define CLASS_H

#include "Callable.h"
#include "Scope.h"
class Type;

/**
*/
class Class : public Callable {
public:
    struct TrialClass : Trial {
        TrialClass( Class *orig, Scope *caller );
        virtual Expr call( int nu, Expr *vu, int nn, int *names, Expr *vn, int pnu, Expr *pvu, int pnn, int *pnames, Expr *pvn, int apply_mode, Scope *caller, const BoolOpSeq &cond, Expr self );
        Class *orig;
        Scope ns;
    };
    struct Attribute {
        SI32 type; ///< CALLABLE_ATTR_...
        SI32 name;
        Code code;
        int  num;
    };

    Class();
    void write_to_stream( Stream &os );
    virtual void read_bin( Scope *scope, BinStreamReader &bin );
    virtual Trial *test( int nu, Expr *vu, int nn, int *names, Expr *vn, int pnu, Expr *pvu, int pnn, int *pnames, Expr *pvn, Scope *caller, Expr self );

    Type *type_for( Vec<Expr> &args );

    static Expr const_or_copy( Expr &Expr );

    Vec<Type *>    types;
    Vec<Code>      ancestors;
    Vec<std::pair<int,Vec<Code> > > methods; ///< surdef lists for each method name
    Vec<Attribute> attributes;
};

#endif // CLASS_H
