#ifndef REF_H
#define REF_H

#include "../Inst/Expr.h"

/**
*/
class Ref : public ObjectWithCptUse {
public:
    Ref();
    virtual ~Ref();

    virtual Expr expr() const = 0;
    virtual void set( Expr expr ) = 0;
    virtual void write_to_stream( Stream &os ) const = 0;
};

#endif // REF_H
