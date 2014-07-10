#ifndef AST_H
#define AST_H

#include "../System/Stream.h"
#include <set>
class ErrorList;
class IrWriter;
class Lexem;

/***/
class Ast {
public:
    Ast( int off );
    virtual ~Ast();

    virtual void get_potentially_needed_ext_vars( std::set<String> &res, std::set<String> &avail ) const;
    virtual void write_to_stream( Stream &os, int nsp = 0 ) const = 0;
    void write_to( IrWriter *aw ) const;

protected:
    virtual void _get_info( IrWriter *aw ) const;
    virtual PI8  _tok_number() const = 0;

    int _off; ///< offset in sourcefile
};

Ast *make_ast( ErrorList &e, const Lexem *l, bool sibling );

#endif // AST_H