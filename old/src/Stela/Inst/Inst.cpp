#include <fstream>
#include "Cst.h"

PI64 Inst::cur_op_id = 0;
static bool full_dot = true;


Inst::Inst() {
    ext_parent = 0;

    op_id_vis  = 0;
    op_id      = 0;
    op_mp      = 0;
}

Inst::~Inst() {
}

int Inst::size_in_bytes( int nout ) const {
    return ( size_in_bits( nout ) + 7 ) / 8;
}

void Inst::write_to_stream( Stream &os ) const {
    write_dot( os );
    os << "(";
    for( int i = 0; i < inp_size(); ++i ) {
        if ( i )
            os << ",";
        os << inp_expr( i );
    }
    os << ")";
}

void Inst::mark_children() const {
    if ( op_id == cur_op_id )
        return;
    op_id = cur_op_id;
    for( int i = 0; i < inp_size(); ++i )
        inp_expr( i ).inst->mark_children();
    for( int i = 0; i < ext_size(); ++i )
        ext_inst( i )->mark_children();
}

const PI8 *Inst::cst_data( int nout, int beg, int end ) const {
    return 0;
}

const PI8 *Inst::vat_data( int nout, int beg, int end ) const {
    return 0;
}

bool Inst::undefined() const {
    return false;
}

Expr Inst::clone( Expr *ch, int nout ) const {
    TODO;
    return Expr();
}

const BaseType *Inst::out_bt( int n ) const {
    return 0;
}

int Inst::ext_size_disp() const {
    return ext_size();
}

bool Inst::equal( const Inst *b ) const {
    if ( inst_id() != b->inst_id() or inp_size() != b->inp_size() )
        return false;
    for( int i = 0; i < inp_size(); ++i )
        if ( inp_expr( i ) != b->inp_expr( i ) )
            return false;
    return true;
}

int Inst::sizeof_additionnal_data() const {
    return 0;
}

void Inst::copy_additionnal_data_to( PI8 *dst ) const {
}

int Inst::display_graph( const Vec<ConstPtr<Inst> > &outputs, const char *filename ) {
    ++cur_op_id;

    std::ofstream f( filename );
    f << "digraph Instruction {\n";
    if ( full_dot )
        f << "  node [shape = record];\n";
    else
        f << "  node [shape = none];\n";

    Vec<const Inst *> ext_buf;
    for( int i = 0; i < outputs.size(); ++i )
        outputs[ i ]->write_graph_rec( ext_buf, f );

    for( const Inst *ch : ext_buf )
        if ( ch )
            ch->write_sub_graph_rec( f );

    f << "}";
    f.close();

    return system( ( "dot -Tps " + std::string( filename ) + " > " + std::string( filename ) + ".eps && gv " + std::string( filename ) + ".eps" ).c_str() );
}

void Inst::write_sub_graph_rec( Stream &os ) const {
    os << "    node" << ext_parent << " -> node" << this << " [color=\"green\"];\n";
    os << "    subgraph cluster_" << this <<" {\ncolor=yellow;\nstyle=dotted;\n";
    Vec<const Inst *> ext_buf;
    write_graph_rec( ext_buf, os );
    for( const Inst *nch : ext_buf )
        nch->write_sub_graph_rec( os );
    os << "    }\n";
}

void Inst::write_graph_rec( Vec<const Inst *> &ext_buf, Stream &os ) const {
    if ( op_id_vis == cur_op_id )
        return;
    op_id_vis = cur_op_id;

    // label
    std::ostringstream ss;
    write_dot( ss );

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
    if ( full_dot ) {
        if ( out_size() > 1 )
            for( int i = 0; i < out_size(); ++i )
                os << "|<f" << i << ">o";
        if ( inp_size() > 1 )
            for( int i = 0; i < inp_size(); ++i )
                os << "|<f" << out_size() + i << ">i";
    }
    os << "\"];\n";

    // children
    for( int i = 0, n = inp_size(); i < n; ++i ) {
        const Expr &ch = inp_expr( i );
        if ( full_dot ) {
            os << "    node" << this;
            if ( inp_size() > 1 )
                os << ":f" << out_size() + i;
            os << " -> node" << ch.inst.ptr();
            if ( ch.inst->out_size() > 1 )
                os << ":f" << ch.nout;
            os << ";\n";
        } else
            os << "    node" << this << " -> node" << ch.inst.ptr() << ";\n";

        if ( ch.inst )
            ch.inst->write_graph_rec( ext_buf, os );
    }

    // ext
    for( int i = 0, n = ext_size_disp(); i < n; ++i )
        if ( const Inst *ch = ext_inst( i ) )
            ext_buf << ch;

    // parents
    //    for( int nout = 0; nout < out_size(); ++nout ) {
    //        VPar &p = parents( nout );
    //        for( int i = 0; i < p.size(); ++i ) {
    //            os << "    node" << p[ i ] << " -> node" << this << ":f" << nout << " [color=\"red\"];\n";
    //            // parents[ i ]->write_graph_rec( ext_buf, os );
    //        }
    //    }
}

Inst *Inst::factorized( Inst *inst ) {
    if ( inst->inp_size() ) {
        const Vec<Inst::Out::Parent,-1,1> &p = inst->inp_expr( 0 ).parents();
        for( int i = 0; i < p.size(); ++i ) {
            if ( p[ i ].inst != inst and p[ i ].inst->equal( inst ) ) {
                delete inst;
                return p[ i ].inst;
            }
        }
    }
    return inst;
}

Expr Inst::_smp_slice( int nout, int beg, int end ) {
    if ( beg == 0 and end == size_in_bits( nout ) )
        return Expr( this, nout );
    if ( beg == end )
        return cst();
    return Expr();
}

Expr Inst::_smp_val_at( int nout, int beg, int end ) {
    return Expr();
}

Expr Inst::_smp_pointer_on( int nout ) {
    return Expr();
}

Expr Inst::_smp_setval( int nout, const Expr &b, int beg ) {
    return Expr();
}


