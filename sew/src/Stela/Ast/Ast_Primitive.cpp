#include "../Ssa/ParsingContext.h"
#include "../Ssa/Symbol.h"
#include "../Ssa/Room.h"
#include "../Ssa/Conv.h"
#include "../Ssa/Cst.h"
#include "../Ssa/Op.h"
#include "../Ir/Numbers.h"
#include "Ast_Primitive.h"
#include "IrWriter.h"

Ast_Primitive::Ast_Primitive( const char *src, int off, int tok_number ) : Ast( src, off ), tok_number( tok_number ) {
}

void Ast_Primitive::get_potentially_needed_ext_vars( std::set<String> &res, std::set<String> &avail ) const {
    for( int i = 0; i < args.size(); ++i )
        args[ i ]->get_potentially_needed_ext_vars( res, avail );
}

void Ast_Primitive::write_to_stream( Stream &os, int nsp ) const {
    os << "Primitive";
}

void Ast_Primitive::_get_info( IrWriter *aw ) const {
    aw->data << args.size();
    for( int i = 0; i < args.size(); ++i )
        aw->push_delayed_parse( args[ i ].ptr() );
}

#define CHECK_NB_ARGS( N ) if ( p->args.size() != N ) return ip->pc->ret_error( "Expecting " #N " args" )

static Expr parse_info( ParsingContext &context, const Ast_Primitive *p ) {
    for( int i = 0; i < p->args.size(); ++i )
        std::cout << p->args[ i ]->parse_in( context ) << std::endl;
    return Expr();
}
static Expr parse_disp( ParsingContext &context, const Ast_Primitive *p ) {
    Vec<Expr> var( Rese(), p->args.size() );
    for( Past arg : p->args )
        var << arg->parse_in( context )->get( context.cond );
    Inst::display_graph( var );
    return ip->void_var();
}
static Expr parse_rand( ParsingContext &context, const Ast_Primitive *p ) {
    static int num = 0;
    return room( symbol( ip->type_Bool, "rand(/*" + to_string( num++ ) + "*/)" ) );
}
static Expr parse_syscall( ParsingContext &context, const Ast_Primitive *p ) { TODO; return Expr(); }
static Expr parse_set_base_size_and_alig( ParsingContext &context, const Ast_Primitive *p ) {
    CHECK_NB_ARGS( 2 );
    Expr a = p->args[ 0 ]->parse_in( context )->get( context.cond );
    Expr b = p->args[ 1 ]->parse_in( context )->get( context.cond );
    if ( a.error() or b.error() )
        return a;
    if ( a->get_val( &context.base_size, ip->type_SI32 ) == false or b->get_val( &context.base_alig, ip->type_SI32 ) == false )
        return context.ret_error( "set_base_size_and_alig -> SI32/SI64 known values" );
    return ip->void_var();
}
static Expr parse_set_RawRef_dependancy ( ParsingContext &context, const Ast_Primitive *p ) { TODO; return Expr(); }

static Expr parse_reassign_rec( ParsingContext &context, const Ast_Primitive *p ) {
    CHECK_NB_ARGS( 2 );
    Expr dst = p->args[ 0 ]->parse_in( context );
    Expr src = p->args[ 1 ]->parse_in( context );
    Type *dt = dst->ptype();
    Type *st = src->ptype();
    if ( dt != st )
        dst->set( conv( dt, src->get( context.cond ) ), context.cond );
    else
        dst->set( src->get( context.cond ), context.cond );
    return dst;
}

static Expr parse_assign_rec( ParsingContext &context, const Ast_Primitive *p ) { TODO; return Expr(); }
static Expr parse_set_ptr_val( ParsingContext &context, const Ast_Primitive *p ) { TODO; return Expr(); }
static Expr parse_select_SurdefList( ParsingContext &context, const Ast_Primitive *p ) { TODO; return Expr(); }
static Expr parse_ptr_size( ParsingContext &context, const Ast_Primitive *p ) { TODO; return Expr(); }
static Expr parse_ptr_alig( ParsingContext &context, const Ast_Primitive *p ) { TODO; return Expr(); }
static Expr parse_size_of( ParsingContext &context, const Ast_Primitive *p ) { TODO; return Expr(); }
static Expr parse_alig_of( ParsingContext &context, const Ast_Primitive *p ) { TODO; return Expr(); }
static Expr parse_typeof( ParsingContext &context, const Ast_Primitive *p ) { TODO; return Expr(); }
static Expr parse_address( ParsingContext &context, const Ast_Primitive *p ) { TODO; return Expr(); }
static Expr parse_get_slice( ParsingContext &context, const Ast_Primitive *p ) { TODO; return Expr(); }
static Expr parse_pointed_value( ParsingContext &context, const Ast_Primitive *p ) { TODO; return Expr(); }
static Expr parse_pointer_on( ParsingContext &context, const Ast_Primitive *p ) { TODO; return Expr(); }
static Expr parse_block_exec( ParsingContext &context, const Ast_Primitive *p ) { TODO; return Expr(); }
static Expr parse_get_argc( ParsingContext &context, const Ast_Primitive *p ) { TODO; return Expr(); }
static Expr parse_get_argv( ParsingContext &context, const Ast_Primitive *p ) { TODO; return Expr(); }
static Expr parse_apply_LambdaFunc( ParsingContext &context, const Ast_Primitive *p ) { TODO; return Expr(); }
static Expr parse_inst_of( ParsingContext &context, const Ast_Primitive *p ) { TODO; return Expr(); }
static Expr parse_repeat( ParsingContext &context, const Ast_Primitive *p ) {
    CHECK_NB_ARGS( 2 );
    Expr tvar = context.apply( p->args[ 0 ]->parse_in( context ), 0, 0, 0, 0, 0, ParsingContext::APPLY_MODE_PARTIAL_INST );
    Expr func = p->args[ 1 ]->parse_in( context ); ++func.inst->cpt_use;
    SI64 res[ 2 ];
    res[ 0 ] = ST( tvar->ptype() );
    res[ 1 ] = ST( func.inst );
    return room( cst( ip->type_Repeated, 2 * 64, res ) );
}
static Expr parse_code( ParsingContext &context, const Ast_Primitive *p ) {
    if ( p->args.size() == 0 )
        return ip->pc->ret_error( "Expecting at least ine arg" );
    Expr f = p->args[ 0 ]->parse_in( context );
    PRINT( f );
    return Expr();
}


template<class OP>
static Expr parse_una( ParsingContext &context, const Ast_Primitive *p, OP o ) {
    CHECK_NB_ARGS( 1 );
    Expr a = p->args[ 0 ]->parse_in( context );
    return room( op( a->get( context.cond ), o ) );
}

template<class OP>
static Expr parse_bin( ParsingContext &context, const Ast_Primitive *p, OP o ) {
    CHECK_NB_ARGS( 2 );
    Expr a = p->args[ 0 ]->parse_in( context );
    Expr b = p->args[ 1 ]->parse_in( context );
    return room( op( a->get( context.cond ), b->get( context.cond ), o ) );
}

#define DECL_IR_TOK( N ) static Expr parse_##N( ParsingContext &context, const Ast_Primitive *p ) { return parse_una( context, p, Op_##N() ); }
#include "../Ir/Decl_UnaryOperations.h"
#undef DECL_IR_TOK

#define DECL_IR_TOK( N ) static Expr parse_##N( ParsingContext &context, const Ast_Primitive *p ) { return parse_bin( context, p, Op_##N() ); }
#include "../Ir/Decl_BinaryOperations.h"
#undef DECL_IR_TOK


Expr Ast_Primitive::_parse_in( ParsingContext &context ) const {
    switch ( tok_number ) {
    #define DECL_IR_TOK( N ) case IR_TOK_##N: return parse_##N( context, this );
    #include "../Ir/Decl_Primitives.h"
    #include "../Ir/Decl_UnaryOperations.h"
    #include "../Ir/Decl_BinaryOperations.h"
    #undef DECL_IR_TOK
    }
    PRINT( tok_number );
    return context.ret_error( "Not a primitive", false, __FILE__, __LINE__ );
}

PI8 Ast_Primitive::_tok_number() const {
    return tok_number;
}


