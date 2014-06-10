#include "../Ir/CallableFlags.h"
#include "Class.h"
#include "Type.h"
#include "Cst.h"
#include "Ip.h"

Class::Class() {
}

void Class::write_to_stream( Stream &os ) {
    os << ip->str_cor.str( name );
}

void Class::read_bin( Scope *scope, BinStreamReader &bin ) {
    Callable::read_bin( scope, bin );

    for( int i = 0, nb_anc = bin.read_positive_integer(); i < nb_anc; ++i )
        ancestors << Code( sf, bin.read_offset() );
}

Class::Trial *Class::test( int nu, Expr *vu, int nn, int *names, Expr *vn, int pnu, Expr *pvu, int pnn, int *pnames, Expr *pvn, Expr self ) {
    TrialClass *res = new TrialClass( this );

    if ( flags & IR_HAS_COMPUTED_PERT ) return res->wr( "TODO: computed pertinence" );

    // nb arguments
    if ( pnu + pnn < min_nb_args() ) return res->wr( "no enough arguments" );
    if ( pnu + pnn > max_nb_args() ) return res->wr( "To much arguments" );

    Scope scope = Scope( &ip->main_scope, 0, "trial_class_" + to_string( name ) );
    res->args.resize( arg_names.size() );

    if ( has_varargs() ) {
        TODO;
    } else {
        // unnamed args
        for( int i = 0; i < pnu; ++i )
            scope.local_vars << pvu[ i ];
        // named args
        Vec<bool> used_arg( Size(), pnn, false );
        for( int i = pnu; i < arg_names.size(); ++i ) {
            int arg_name = arg_names[ i ];
            for( int n = 0; ; ++n ) {
                if ( n == pnn ) {
                    // unspecified arg
                    int j = i - ( arg_names.size() - arg_defaults.size() );
                    if ( j < 0 )
                        return res->wr( "unspecified mandatory argument" );
                    scope.local_vars << scope.parse( arg_defaults[ j ].sf, arg_defaults[ j ].tok, "making default value" );
                    break;
                }
                if ( arg_name == pnames[ n ] ) {
                    scope.local_vars << pvn[ i ];
                    used_arg[ n ] = true;
                    break;
                }
            }
        }
        for( int n = 0; n < pnn; ++n ) {
            if ( not used_arg[ n ] ) {
                for( int m = 0; m < n; ++m )
                    if ( pnames[ n ] == pnames[ m ] )
                        ip->disp_error( "arg assigned twice", true );
                return res->wr( "name=... does not appear in def args" );
            }
        }
    }

    // condition
    if ( condition ) {
        res->cond = *scope.parse( condition.sf, condition.tok, "parsing condition" )->get( scope.cond );
        if ( res->cond.always( false ) )
            return res->wr( "condition = false" );
        if ( not res->cond.always( true ) ) {
            ip->disp_error( "class conditions must be known at compile time" );
            return res->wr( "condition not known at compile time" );
        }
    }

    return res;
}

Expr Class::TrialClass::call( int nu, Expr *vu, int nn, int *names, Expr *vn, int pnu, Expr *pvu, int pnn, int *pnames, Expr *pvn, Expr self, int apply_mode ) {
    Type *type = orig->type_for( args );

    // start with a unknown cst
    Expr ret( type );
    if ( apply_mode == Scope::APPLY_MODE_NEW )
        TODO;

    // call init
    if ( apply_mode == Scope::APPLY_MODE_STD )
        ip->main_scope.apply( ip->main_scope.get_attr( ret, STRING_init_NUM ), nu, vu, nn, names, vn, Scope::APPLY_MODE_STD );

    return ret;
}

Expr Class::const_or_copy( Expr &var ) {
    if ( var->type()->orig == ip->class_Callable ) {
        Expr i = ip->main_scope.apply( var, 0, 0, 0, 0, 0, Scope::APPLY_MODE_PARTIAL_INST );
        Expr v = ip->make_type_var( i->type() );
        return const_or_copy( v );
    }
    if ( var.inst->flags & Inst::CONST )
        return var;
    Expr res = ip->main_scope.copy( var );
    res.inst->flags |= Inst::CONST;
    return res;
}

Type *Class::type_for( Vec<Expr> &args ) {
    for( Type *t : types )
        if ( t->parameters == args )
            return t;
    Type *res = new Type( this );
    for( int i = 0; i < args.size(); ++i )
        res->parameters << const_or_copy( args[ i ] );

    types << res;
    return res;
}
