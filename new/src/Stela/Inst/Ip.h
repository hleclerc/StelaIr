#ifndef IP_H
#define IP_H

#include "../System/ErrorList.h"
#include "../System/NstrCor.h"
#include "Type.h"

/**
*/
class Ip {
public:
    Ip();

    // errors
    void disp_error( String msg, bool warn = false, const char *file = 0, int line = -1 );
    ErrorList::Error &error_msg( String msg, bool warn = false, const char *file = 0, int line = -1 );

    void add_cond( const Var &cond );
    void add_cond( Ptr<Inst> cond );
    void pop_cond();

    // base type
    Type type_SI32;
    Type type_RawPtr;

    // context
    Vec<Ptr<Inst> > conds; ///< current conditions (use add_cond)

    //
    ErrorList error_list;
    NstrCor   str_cor;
};

#define IP_ERROR( MSG ) ip->disp_error( MSG, false, __FILE__, __LINE__ );


extern Ip *ip;

#endif // IP_H
