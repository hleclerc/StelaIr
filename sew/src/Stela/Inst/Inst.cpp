#include <fstream>
#include "Inst.h"
#include "Ip.h"

PI64 Inst::cur_op_id = 0;

Inst::Inst() {
    ext_par   = 0;
    when      = 0;

    op_id_vis = 0;
    op_id     = 0;
    op_mp     = 0;

    flags     = 0;

    cpt_use   = 0;
}

Inst::~Inst() {
    rem_ref_to_this();
    // delete when;
}

void Inst::set( Expr obj, const BoolOpSeq &cond ) {
    ip->disp_error( "attempting to set an object that is not a pointer" );
}

Expr Inst::get( const BoolOpSeq &cond ) {
    if ( type() == ip->type_Error )
        return ip->error_var();
    return ip->ret_error( "attempting to get the pointed value of an object that is not a pointer" );
}

Expr Inst::simplified( const BoolOpSeq &cond ) {
    return this;
}

bool Inst::same_cst( const Inst *inst ) const { return false; }
bool Inst::emas_cst( const Inst *inst ) const { return false; }

void Inst::write_to_stream( Stream &os, int prec ) {
    write_dot( os );
    if ( inp.size() ) {
        for( int i = 0; i < inp.size(); ++i )
            os << ( i ? "," : "(" ) << inp[ i ];
        os << ")";
    }
}


Type *Inst::type() {
    return 0;
}


void Inst::add_dep( const Expr &val ) {
    val->par << Parent{ this, TPAR_DEP };
    dep << val;
}

void Inst::add_inp( const Expr &val ) {
    val->par << Parent{ this, int( inp.size() ) };
    inp << val;
}

void Inst::add_ext( const Expr &val ) {
    val->ext_par = this;
    ext << val;
}

void Inst::mod_inp( const Expr &val, int num ) {
    if ( inp.size() <= num )
        inp.resize( num + 1 );
    inp[ num ]->par.remove_first_unordered( Parent{ this, num } );
    val->par << Parent{ this, num };
    inp[ num ] = val;
}


void Inst::rem_ref_to_this() {
    for( int num = 0; num < inp.size(); ++num )
        inp[ num ]->par.remove_first_unordered( Parent{ this, num } );
    for( int num = 0; num < dep.size(); ++num )
        dep[ num ]->par.remove_first_unordered( Parent{ this, TPAR_DEP } );
    for( int num = 0; num < ext.size(); ++num )
        ext[ num ]->ext_par = 0;
}

void Inst::mark_children() {
    if ( op_id == cur_op_id )
        return;
    op_id = cur_op_id;
    for( Expr &e : inp )
        e->mark_children();
    for( Expr &e : ext )
        e->mark_children();
}

void Inst::clone( Vec<Expr> &created ) const {
    if ( op_id == cur_op_id )
        return;
    op_id = cur_op_id;

    // clone the children
    for( const Expr &i : inp )
        i->clone( created );
    for( const Expr &i : dep )
        i->clone( created );
    for( const Expr &i : ext )
        i->clone( created );

    // basic clone
    Expr res = forced_clone( created );

    // add cloned children
    for( const Expr &i : inp )
        res->add_inp( reinterpret_cast<Inst *>( i->op_mp ) );
    for( const Expr &i : dep )
        res->add_dep( reinterpret_cast<Inst *>( i->op_mp ) );
    for( const Expr &i : ext )
        res->add_ext( reinterpret_cast<Inst *>( i->op_mp ) );

    // register
    op_mp = res.inst;
    created << res;
}

int Inst::display_graph( Vec<Expr> outputs, const char *filename ) {
    ++cur_op_id;

    std::ofstream f( filename );
    f << "digraph Instruction {\n";
    f << "  node [shape = record];\n";

    Vec<Inst *> ext_buf;
    for( int i = 0; i < outputs.size(); ++i )
        outputs[ i ]->write_graph_rec( ext_buf, f );

    for( Inst *ch : ext_buf )
        if ( ch )
            ch->write_sub_graph_rec( f );

    f << "}";
    f.close();

    return system( ( "dot -Tps " + std::string( filename ) + " > " + std::string( filename ) + ".eps && gv " + std::string( filename ) + ".eps" ).c_str() );
}

void Inst::write_sub_graph_rec( Stream &os ) {
    os << "    node" << ext_par << " -> node" << this << " [color=\"green\"];\n";
    os << "    subgraph cluster_" << this <<" {\ncolor=yellow;\nstyle=dotted;\n";
    Vec<Inst *> ext_buf;
    write_graph_rec( ext_buf, os );
    for( Inst *nch : ext_buf )
        nch->write_sub_graph_rec( os );
    os << "    }\n";
}

void Inst::write_graph_rec( Vec<Inst *> ext_buf, Stream &os ) {
    if ( op_id_vis == cur_op_id )
        return;
    op_id_vis = cur_op_id;

    // label
    std::ostringstream ss;
    write_dot( ss );

    //if ( when )
    //    when->write_to_stream( ss << "\n" );
    //if ( IIC( this )->out_reg )
    //    IIC( this )->out_reg->write_to_stream( ss << " " );

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
    if ( inp.size() > 1 )
        for( int i = 0; i < inp.size(); ++i )
            os << "|<f" << i << ">i";
    os << "\"];\n";

    // children
    for( int i = 0; i < inp.size(); ++i ) {
        os << "    node" << this;
        if ( inp.size() > 1 )
            os << ":f" << i;
        os << " -> node" << inp[ i ].inst << ";\n";

        inp[ i ]->write_graph_rec( ext_buf, os );
    }

    // dependencies
    for( int i = 0; i < dep.size(); ++i ) {
        os << "    node" << this << " -> node" << dep[ i ].inst << " [style=dotted];\n";
        dep[ i ]->write_graph_rec( ext_buf, os );
    }

    // ext
    for( int i = 0; i < ext_disp_size(); ++i )
        if ( Inst *ch = ext[ i ].inst )
            ext_buf << ch;
}

int Inst::ext_disp_size() const {
    return ext.size();
}


int Inst::always_checked() const {
    return false;
}

bool Inst::has_inp_parent() const {
    for( int i = 0; i < par.size(); ++i )
        if ( par[ i ].ninp >= 0 )
            return true;
    return false;
}

bool Inst::uninitialized() const {
    return false;
}

bool Inst::get_val( Type *type, void *data ) const {
    return false;
}
