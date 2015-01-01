#include "../Ssa/ParsingContext.h"
#include "../Ir/Numbers.h"
#include "IrWriter.h"
#include "Ast_NdList.h"

Ast_NdList::Ast_NdList( const char *src, int off ) : Ast( src, off ) {
}

void Ast_NdList::get_potentially_needed_ext_vars( std::set<String> &res, std::set<String> &avail ) const {
    for( int i = 0; i < lst.size(); ++i )
        lst[ i ]->get_potentially_needed_ext_vars( res, avail );
}

void Ast_NdList::write_to_stream( Stream &os, int nsp ) const {
    os << "nb_dim=" << nb_dim << " sizes=" << sizes;
    for( int i = 0; i < lst.size(); ++i )
        lst[ i ]->write_to_stream( os << "\n" << String( nsp + 2, ' ' ), nsp + 2 );
}


void Ast_NdList::_get_info( IrWriter *aw ) const {
    TODO;
}

Expr Ast_NdList::_parse_in( ParsingContext &context ) const {
    return context.ret_error( "TODO: _parse_in", false, __FILE__, __LINE__ );
}

PI8 Ast_NdList::_tok_number() const {
    return IR_TOK_LIST;
}
