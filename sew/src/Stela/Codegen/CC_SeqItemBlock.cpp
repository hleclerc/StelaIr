#include "../Inst/CppRegConstraint.h"
#include "../Inst/Type.h"

#include "CC_SeqItemContinueOrBreak.h"
#include "CC_SeqItemBlock.h"
#include "Codegen_C.h"
#include "CppOutReg.h"

CC_SeqItemBlock::CC_SeqItemBlock() : CC_SeqItem( 0, 0 ), sibling( 0 ) {
}

CC_SeqItemBlock::~CC_SeqItemBlock() {
}

void CC_SeqItemBlock::get_glo_cond_and_seq_of_sub_blocks( Vec<CC_SeqItemBlock *> &res, const BoolOpSeq &cond ) {
    glo_cond = cond;
    res << this;
    for( int i = 0; i < seq.size(); ++i )
        seq[ i ]->get_glo_cond_and_seq_of_sub_blocks( res, cond );
}

bool CC_SeqItemBlock::ch_followed_by_something_to_execute( int &nb_evicted_blocks, CC_SeqItem *ch, const BoolOpSeq &cond ) {
    bool found = false;
    for( int i = 0; i < seq.size(); ++i ) {
        if ( found ) {
            if ( seq[ i ]->non_void() ) {
                if ( not cond.imply( not seq[ i ]->glo_cond ) )
                    return true;
                ++nb_evicted_blocks;
            }
        } else if ( seq[ i ] == ch )
            found = true;
    }
    return parent ? parent->ch_followed_by_something_to_execute( nb_evicted_blocks, this, cond ) : false;
}

bool CC_SeqItemBlock::non_void() {
    for( int i = 0; i < seq.size(); ++i )
        if ( seq[ i ]->non_void() )
            return true;
    return false;
}

void CC_SeqItemBlock::write( Codegen_C *cc ) {
    std::map<Type *,Vec<CppOutReg *> > by_type;
    for( CppOutReg *r : reg_to_decl )
        by_type[ r->type ] << r;
    for( auto it : by_type ) {
        cc->on.write_beg();
        cc->write( it.first );
        for( int i = 0; i < it.second.size(); ++i )
            *cc->os << ( i ? ", R" : " R" ) << it.second[ i ]->num;
        cc->on.write_end( ";" );
    }

    for( int i = 0; i < seq.size(); ++i )
        seq[ i ]->write( cc );
}

void CC_SeqItemBlock::get_constraints( CppRegConstraint &reg_constraints ) {
    for( int n = 0; n < seq.size(); ++n )
        seq[ n ]->get_constraints( reg_constraints );
}

void CC_SeqItemBlock::assign_reg( Codegen_C *cc, CppRegConstraint &reg_constraints ) {
    for( int n = 0; n < seq.size(); ++n )
        seq[ n ]->assign_reg( cc, reg_constraints );
}

bool CC_SeqItemBlock::contains_a_cont_or_break() {
    for( int n = 0; n < seq.size(); ++n )
        if ( dynamic_cast<CC_SeqItemContinueOrBreak *>( seq[ n ].ptr() ) )
            return true;
    return false;
}