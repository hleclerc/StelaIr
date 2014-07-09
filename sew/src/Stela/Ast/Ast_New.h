#ifndef STELA_AST_New_H
#define STELA_AST_New_H

#include "Ast_Call.h"

/**
*/
class Ast_New : public Ast_Call {
public:
    Ast_New( int off );
    virtual void write_name( Stream &os ) const { os << "new"; }
};

#endif // STELA_AST_New_H

