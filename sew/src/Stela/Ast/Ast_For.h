#ifndef STELA_AST_For_H
#define STELA_AST_For_H

#include "../System/SplittedVec.h"
#include "../System/AutoPtr.h"
#include "../System/Vec.h"
#include "Ast.h"

/**
*/
class Ast_For : public Ast {
public:
    Ast_For( int off );
    virtual void write_to_stream( Stream &os, int nsp = 0 ) const;

    Vec<String>                 names;
    SplittedVec<AutoPtr<Ast>,2> objects;
    AutoPtr<Ast>                block;
};

#endif // STELA_AST_For_H

