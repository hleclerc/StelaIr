#ifndef AST_VARIABLE_H
#define AST_VARIABLE_H

#include "Ast.h"

/**
*/
class Ast_Variable : public Ast {
public:
    Ast_Variable( int off, String str );

    virtual void get_potentially_needed_ext_vars( std::set<String> &res, std::set<String> &avail ) const;
    virtual void write_to_stream( Stream &os, int nsp = 0 ) const;

protected:
    virtual void _get_info( IrWriter *aw ) const;
    virtual PI8  _tok_number() const;
    virtual Past _parse_in( ConvScope &scope );

    String str;
};

#endif // AST_VARIABLE_H
