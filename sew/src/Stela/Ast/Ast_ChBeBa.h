#ifndef STELA_AST_ChBeBa_H
#define STELA_AST_ChBeBa_H

#include "Ast_Call.h"

/**
*/
class Ast_ChBeBa : public Ast_Call {
public:
    Ast_ChBeBa( int off );
    virtual void write_name( Stream &os ) const { os << "chbeba"; }
};

#endif // STELA_AST_ChBeBa_H

