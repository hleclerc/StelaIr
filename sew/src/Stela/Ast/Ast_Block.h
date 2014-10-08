#ifndef STELA_AST_Block_H
#define STELA_AST_Block_H

#include "../System/SplittedVec.h"
#include "../System/AutoPtr.h"
#include "Ast.h"

/**
*/
class Ast_Block : public Ast {
public:
    Ast_Block( int off );

    virtual void get_potentially_needed_ext_vars( std::set<String> &res, std::set<String> &avail ) const;
    virtual void prep_get_potentially_needed_ext_vars( std::set<String> &avail ) const;
    virtual void write_to_stream( Stream &os, int nsp = 0 ) const;

protected:
    friend class AstMaker;

    virtual Past _parse_in( ConvScope &scope );

    virtual void _get_info( IrWriter *aw ) const;
    virtual PI8  _tok_number() const;

    SplittedVec<Past,8> lst;
};

#endif // STELA_AST_Block_H

