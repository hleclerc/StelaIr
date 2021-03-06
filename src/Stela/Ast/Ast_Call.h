#ifndef STELA_AST_Call_H
#define STELA_AST_Call_H

#include "../System/SplittedVec.h"
#include "../System/AutoPtr.h"
#include "../System/Vec.h"
#include "Ast.h"

/**
*/
class Ast_Call : public Ast {
public:
    Ast_Call( const char *src, int off );

    virtual void get_potentially_needed_ext_vars( std::set<String> &res, std::set<String> &avail ) const;
    virtual void write_to_stream( Stream &os, int nsp = 0 ) const;
    virtual void write_name( Stream &os ) const = 0;

protected:
    friend class AstMaker;

    virtual Expr _parse_in( ParsingContext &context ) const;
    virtual void _get_info( IrWriter *aw ) const;

    Past                 f;
    SplittedVec<Past,4 > args;
    Vec<String>          names;
};

#endif // STELA_AST_Call_H

