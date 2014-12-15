#include "../Ir/Numbers.h"
#include "IrWriter.h"
#include "Ast_Class.h"

Ast_Class::Ast_Class( int off ) : Ast_Callable( off ) {
}

void Ast_Class::_get_potentially_needed_ext_vars( std::set<String> &res, std::set<String> &avail ) const {
    for( int i = 0; i < inheritance.size(); ++i )
        inheritance[ i ]->get_potentially_needed_ext_vars( res, avail );
}

void Ast_Class::_get_info( IrWriter *aw ) const {
    Ast_Callable::_get_info( aw );

    // extends
    aw->data << inheritance.size();
    for( int i = 0; i < inheritance.size(); ++i )
        aw->push_delayed_parse( inheritance[ i ].ptr() );
}

PI8 Ast_Class::_tok_number() const {
    return IR_TOK_CLASS;
}

int Ast_Class::_spec_flags() const {
    return 0;
}
