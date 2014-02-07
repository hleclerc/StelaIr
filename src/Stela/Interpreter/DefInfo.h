#ifndef DefInfo_H
#define DefInfo_H

#include "../System/BinStreamReader.h"
#include "CallableInfo_WT.h"

class DefInfo : public CallableInfo_WT {
public:
    struct TrialDef : Trial {
        TrialDef( DefInfo *orig );
        virtual ~TrialDef();
        virtual Var call( int nu, Var *vu, int nn, int *names, Var *vn, int pnu, Var *pvu, int pnn, int *pnames, Var *pvn, const Var &self, const Expr &sf, int off, Scope *caller );
        DefInfo *orig;
        Scope *scope;
    };

    struct AttrInit {
        SI32 name;
        SI32 nu, na;
        Vec<Code> args;
        Vec<SI32> names; ///< named arguments
    };

    DefInfo( const Expr &sf, int src_off, BinStreamReader bin );
    virtual Trial *test( int nu, Var *vu, int nn, int *names, Var *vn, int pnu, Var *pvu, int pnn, int *pnames, Var *pvn, const Var &self, const Expr &sf, int off, Scope *caller );

    Code block_with_ret;
    // Code return_type;
    Vec<AttrInit> attr_init;

    int get_of;
    int set_of;
    int sop_of;
};

#endif // DefInfo_H
