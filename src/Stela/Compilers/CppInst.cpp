#include "../Interpreter/Interpreter.h"
#include "../Inst/BaseType.h"
#include "../System/rcast.h"
#include "CppCompiler.h"
#include "CppInst.h"
#include <fstream>

enum {
    PREC_phi =  1,
    PREC_add = 10,
    PREC_mul = 20
};

PI64 CppInst::cur_op_id = 0;

CppInst::CppInst( int inst_id, int nb_outputs ) : inst_id( inst_id ), out( Size(), nb_outputs ), op_id( 0 ), op_id_vis( 0 ) {
    ext_parent = 0;
    ext_ds = -1;
}

void CppInst::DeclWriter::write_to_stream( Stream &os ) const {
    const BaseType *bt = info->out[ nout ].bt_hint;
    ASSERT( info->out[ nout ].num < 0, "weird" );
    ASSERT( bt, "bad" );

    info->out[ nout ].num = cc->get_free_reg( bt );
    os << *bt << " R" << info->out[ nout ].num;
    if ( equ_sgn )
        os << " = ";
}

void CppInst::InstWriter::write_to_stream( Stream &os ) const {
    if ( info->inst_id == CppInst::Id_IfInp ) {
        InstWriter n = *this;
        n.info = info->ext_parent->inp[ nout ].inst;
        n.nout = info->ext_parent->inp[ nout ].nout;
        return n.write_to_stream( os );
    }

    if ( info->out[ nout ].num >= 0 )
        os << "R" << info->out[ nout ].num;
    else
        info->write_code( cc, precedance );
}

CppInst::DeclWriter CppInst::decl( CppCompiler *cc, int nout, bool equ_sgn ) {
    DeclWriter res;
    res.equ_sgn = equ_sgn;
    res.info = this;
    res.nout = nout;
    res.cc = cc;
    return res;
}

CppInst::InstWriter CppInst::inst( CppCompiler *cc, int nout, int precedance ) {
    InstWriter res;
    res.precedance = precedance;
    res.info = this;
    res.nout = nout;
    res.cc = cc;
    return res;
}

CppInst::InstWriter CppInst::disp( CppCompiler *cc, CppExpr expr, int precedance ) {
    return expr.inst->inst( cc, expr.nout, precedance );
}

int CppInst::add_child( CppExpr expr ) {
    int res = inp.size();
    if ( expr.inst )
        expr.inst->out[ expr.nout ].parents << Out::Parent{ this, int( inp.size() ) };
    inp << expr;
    return res;
}

void CppInst::set_child( int ninp, CppExpr expr ) {
    if ( inp[ ninp ].inst )
        inp[ ninp ].inst->out[ inp[ ninp ].nout ].parents.remove_first_unordered( Out::Parent{ this, ninp } );
    if ( expr.inst )
        expr.inst->out[ expr.nout ].parents << Out::Parent{ this, ninp };
    inp[ ninp ] = expr;
}

int CppInst::add_out() {
    int res = out.size();
    out.push_back();
    return res;
}

void CppInst::check_out_size( int n ) {
    if ( out.size() < n )
        out.resize( n );
}

void CppInst::add_ext( CppInst *inst ) {
    ASSERT( not inst->ext_parent, "..." );
    inst->ext_parent = this;
    ext << inst;
}

void CppInst::set_out_bt_hint( int nout, const BaseType *bt, bool force ) {
    if ( force ? out[ nout ].bt_hint == bt : out[ nout ].bt_hint != 0 )
        return;
    out[ nout ].bt_hint = bt;

    // parent propagation
    for( const CppInst::Out::Parent &p : out[ nout ].parents )
        p.inst->set_inp_bt_hint( p.ninp, bt );

    // downward propagation
    switch ( inst_id ) {
    case CppInst::Id_WhileInp: {
        CppExpr ch = ext_parent->inp[ nout ];
        ch.inst->set_out_bt_hint( ch.nout, bt );
        break;
    }
    }
}

void CppInst::set_inp_bt_hint( int ninp, const BaseType *bt ) {
    switch ( inst_id ) {
    case CppInst::Id_WhileOut:
        if ( ninp < inp.size() - 1 )
            ext_parent->set_out_bt_hint( ninp, bt );
        break;
    }
}

void CppInst::bt_hint_propagation() {
    switch ( inst_id ) {
    // case Op_...:
        #define DECL_IR_TOK( INST ) case CppInst::Id_Op_##INST:
        #include "../Ir/Decl_Operations.h"
        #undef DECL_IR_TOK
        set_out_bt_hint( 0, reinterpret_cast<const BaseType **>( additionnal_data )[ 0 ] );
        for( int i = 0; i < inp.size(); ++i )
            inp[ i ].inst->set_out_bt_hint( inp[ i ].nout, reinterpret_cast<const BaseType **>( additionnal_data )[ 0 ] );
        break;
    case CppInst::Id_Syscall:
        set_out_bt_hint( 0, bt_Void );
        set_out_bt_hint( 1, ip->bt_ST );
        inp[ 0 ].inst->set_out_bt_hint( inp[ 0 ].nout, bt_Void );
        for( int i = 1; i < inp.size(); ++i )
            inp[ i ].inst->set_out_bt_hint( inp[ i ].nout, ip->bt_ST );
        break;
    case CppInst::Id_Conv:
        set_out_bt_hint( 0, reinterpret_cast<const BaseType **>( additionnal_data )[ 0 ] );
        break;
    case CppInst::Id_Rand:
        set_out_bt_hint( 0, bt_PI64, true );
        break;
    case CppInst::Id_WhileOut: {
        CppExpr ch = inp.back();
        ch.inst->set_out_bt_hint( ch.nout, bt_Bool );
        break;
    }
    }
}


void CppInst::write_to_stream( Stream &os ) const {
    // particular cases
    if ( inst_id == CppInst::Id_Cst ) {
        if ( out[ 0 ].bt_hint )
            return out[ 0 ].bt_hint->write_to_stream( os, additionnal_data + sizeof( int ) );

        // else
        int size = *reinterpret_cast<int *>( additionnal_data );

        const PI8 *data = additionnal_data + sizeof( int );
        const char *c = "0123456789ABCDEF";
        for( int i = 0; i < std::min( size / 8, 4 ); ++i ) {
            if ( i )
                os << ' ';
            os << c[ data[ i ] >> 4 ] << c[ data[ i ] & 0xF ];
        }
        if ( size / 8 > 4 )
            os << "...";
        return;
    }

    switch ( inst_id ) {
        #define DECL_INST( INST ) case CppInst::Id_##INST: os << #INST; break;
        #include "../Inst/DeclInst.h"
        #undef DECL_INST
        default: ERROR( "?" );
    }
}

static bool inlinable( const CppInst::Out &out ) {
    return out.parents.size() == 1 and out.parents[ 0 ].inst->inst_id != Inst::Id_WhileInst;
}

static bool declable( CppExpr expr ) {
    return expr.inst->out[ expr.nout ].bt_hint and expr.inst->out[ expr.nout ].bt_hint != bt_Void;
}

/// binary operation
void CppInst::write_code_bin_op( CppCompiler *cc, int prec, const char *op_str, int prec_op ) {
    if ( prec >= 0 or not inlinable( out[ 0 ] ) ) {
        if ( prec < 0 ) cc->on.write_beg() << decl( cc, 0 );
        if ( prec > prec_op ) cc->os << "( ";
        cc->os << inp[ 0 ].inst->inst( cc, inp[ 0 ].nout, prec_op ) << op_str << inp[ 1 ].inst->inst( cc, inp[ 1 ].nout, prec_op );
        if ( prec > prec_op ) cc->os << " )";
        if ( prec < 0 ) cc->on.write_end( ";" );
    }
}

void CppInst::write_code( CppCompiler *cc, int prec ) {
    switch ( inst_id ) {
    case CppInst::Id_Cst:
        if ( prec >= 0 )
            out[ 0 ].bt_hint->write_to_stream( cc->os, additionnal_data + sizeof( int ) );
        else if ( not inlinable( out[ 0 ] ) ) {
            cc->on.write_beg() << decl( cc, 0 );
            out[ 0 ].bt_hint->write_to_stream( cc->os, additionnal_data + sizeof( int ) );
            cc->on.write_end( ";" );
        }
        break;
    case CppInst::Id_Rand:
        cc->on << decl( cc, 0 ) << "rand();";
        break;
    case CppInst::Id_Conv:
        if ( prec >= 0 )
            cc->os << *out[ 0 ].bt_hint << "( " << disp( cc, inp[ 0 ] ) << " )";
        else if ( not inlinable( out[ 0 ] ) )
            cc->on << decl( cc, 0 ) << disp( cc, inp[ 0 ] ) << ";";
        break;
    case CppInst::Id_Phi:
        if ( prec >= 0 or not inlinable( out[ 0 ] ) ) {
            if ( prec < 0 )
                cc->on.write_beg() << decl( cc, 0 );
            cc->os << disp( cc, inp[ 0 ], PREC_phi ) << " ? "
                   << disp( cc, inp[ 1 ], PREC_phi ) << " : "
                   << disp( cc, inp[ 2 ], PREC_phi );
            if ( prec < 0 )
                cc->on.write_end( ";" );
        }
        break;
    case CppInst::Id_Syscall:
        cc->on.write_beg();
        if ( out[ 1 ].parents.size() )
            cc->os << decl( cc, 1 );
        cc->os << "syscall( ";
        for( int i = 1; i < inp.size(); ++i ) {
            if ( i > 1 )
                cc->os << ", ";
            cc->os << disp( cc, inp[ i ], 0 );
        }
        cc->on.write_end( " );" );
        break;

    case CppInst::Id_If: {
        for( int i = 0; i < out.size(); ++i )
            if ( declable( ext[ 0 ]->inp[ i ] ) )
                cc->on << decl( cc, i, false );

        Vec<CppInst *> va[ 2 ];
        for( int n = 0; n < 2; ++n )
            for( int i = 0; i < ext[ n ]->inp.size(); ++i )
                va[ n ] << ext[ n ]->inp[ i ].inst;

        cc->on << "if ( " << disp( cc, inp[ 0 ] ) << " ) {";
        cc->on.nsp += 4;
        cc->output_code_for( va[ 0 ] );
        cc->on.nsp -= 4;
        cc->on << "} else {";
        cc->on.nsp += 4;
        cc->output_code_for( va[ 1 ] );
        cc->on.nsp -= 4;
        cc->on << "}";
        break;
    }

    case CppInst::Id_IfOut: {
        CppInst *ii = ext_parent;
        for( int nout = 0; nout < ii->out.size(); ++nout )
            if ( declable( inp[ nout ] ) )
                cc->on << ii->inst( cc, nout ) << " = " << disp( cc, inp[ nout ] ) << ";";
        break;
    }

    case CppInst::Id_IfInp: {
        break;
    }

    case CppInst::Id_WhileInst: {
        const int *corr_inp = rcast( additionnal_data );
        for( int i = 0; i < out.size(); ++i ) {
            if ( declable( ext[ 0 ]->inp[ i ] ) ) {
                if ( corr_inp[ i ] >= 0 )
                    cc->on << decl( cc, i ) << disp( cc, inp[ corr_inp[ i ] ] ) << ";";
                else
                    cc->on << decl( cc, i, false ) << ";";
            }
        }

        cc->on << "while ( true ) {";
        cc->on.nsp += 4;

        Vec<CppInst *> out;
        for( const CppExpr &ch : ext[ 0 ]->inp )
            out << ch.inst;
        cc->output_code_for( out );
        cc->on.nsp -= 4;
        cc->on << "}";
        break;
    }

    case CppInst::Id_WhileInp: {
        const int *corr_inp = rcast( ext_parent->additionnal_data );
        for( int i = 0; i < out.size(); ++i ) {
            for( int j = 0; ; ++j ) {
                if ( j == ext_parent->out.size() ) {
                    // variable not modified during the loop
                    CppExpr ch = ext_parent->inp[ i ];
                    out[ i ].num = ch.inst->out[ ch.nout ].num;
                    break;
                }
                if ( corr_inp[ j ] == i ) {
                    out[ i ].num = ext_parent->out[ j ].num;
                    break;
                }
            }
        }
        break;
    }

    case CppInst::Id_WhileOut: {
        for( int i = 0; i < inp.size() - 1; ++i )
            cc->on << "R" << ext_parent->out[ i ].num << " = "
                   << disp( cc, inp[ i ], 0 ) << ";";
        break;
    }

    case CppInst::Id_Op_add: write_code_bin_op( cc, prec, " + ", PREC_add ); break;
    case CppInst::Id_Op_mul: write_code_bin_op( cc, prec, " * ", PREC_mul ); break;

    default:
        #define DECL_INST( INST ) if ( inst_id == CppInst::Id_##INST ) std::cout << #INST << std::endl;
        #include "../Inst/DeclInst.h"
        #undef DECL_INST
        // TODO;
    }
}

void CppInst::mark_children_wo_ext() {
    if ( op_id == cur_op_id )
        return;
    op_id = cur_op_id;

    for( int i = 0, nch = inp.size(); i < nch; ++i )
        inp[ i ].inst->mark_children_wo_ext();
}

void CppInst::get_insts_rec( Vec<CppInst *> &res, int id ) {
    if ( op_id == cur_op_id )
        return;
    op_id = cur_op_id;

    if ( inst_id == id )
        res << this;

    for( int i = 0, nch = inp.size(); i < nch; ++i )
        inp[ i ].inst->get_insts_rec( res, id );
    for( int i = 0; i < ext_ds; ++i )
        ext[ i ]->get_insts_rec( res, id );
}

int CppInst::display_graph( const Vec<CppInst *> &res, const char *filename ) {
    ++cur_op_id;

    std::ofstream f( filename );
    f << "digraph Instruction {";
    f << "  node [shape = record];";
    for( int i = 0; i < res.size(); ++i )
        res[ i ]->write_graph_rec( f );
    f << "}";
    f.close();

    return system( ( "dot -Tps " + std::string( filename ) + " > " + std::string( filename ) + ".eps && gv " + std::string( filename ) + ".eps" ).c_str() );
}

void CppInst::write_graph_rec( Stream &os, void *omd ) const {
    if ( op_id_vis == cur_op_id )
        return;
    op_id_vis = cur_op_id;

    // label
    std::ostringstream ss;
    write_to_stream( ss );

    // node
    std::string ls = ss.str();
    os << "    node" << this << " [label=\"";
    for( unsigned i = 0; i < ls.size(); ++i ) {
        switch ( ls[ i ] ) {
        case '<':
        case '>':
        case '\\':
            os << '\\';
        }
        os << ls[ i ];
    }
    // os << '(' << ( op_mp != 0 ) << ')';
    for( int i = 0; i < out.size(); ++i ) {
        os << "|<f" << i << ">";
        if ( out[ i ].bt_hint )
            os << *out[ i ].bt_hint;
        else
            os << "o";
    }
    for( int i = 0; i < inp.size(); ++i )
        os << "|<f" << out.size() + i << ">i";
    os << "\"];\n";

    // children
    for( int i = 0, n = inp.size(); i < n; ++i ) {
        const CppExpr &ch = inp[ i ];
        os << "    node" << this << ":f" << out.size() + i << " -> node" << ch.inst << ":f" << ch.nout << ";\n";
        if ( ch.inst )
            ch.inst->write_graph_rec( os, omd );
    }

    // ext
    for( int i = 0, n = ext_disp_size(); i < n; ++i ) {
        const CppInst *ch = ext[ i ];
        os << "    node" << this << " -> node" << ch << " [color=\"green\"];\n";
        if ( ch ) {
            os << "    subgraph cluster_" << ch <<" {\ncolor=yellow;\nstyle=dotted;\n";
            ch->write_graph_rec( os, omd );
            os << "    }\n";
       }
    }

    // parents
    //    for( int nout = 0; nout < out_size(); ++nout ) {
    //        VPar &p = parents( nout );
    //        for( int i = 0; i < p.size(); ++i ) {
    //            os << "    node" << p[ i ] << " -> node" << this << ":f" << nout << " [color=\"red\"];\n";
    //            // parents[ i ]->write_graph_rec( os );
    //        }
    //    }
}

int CppInst::ext_disp_size() const {
    return ext_ds >= 0 ? ext_ds : ext.size();
}
