#include "Stela/System/UsualStrings.h"
#include "Ip.h"

Ip::Ip() :
    type_SI32  ( STRING_SI32_NUM   ),
    type_RawPtr( STRING_RawPtr_NUM ) {

    type_SI32  ._size = 32;
    type_RawPtr._size = 8 * sizeof( void * );
}

void Ip::disp_error( String msg, bool warn, const char *file, int line ) {
    std::cerr << error_msg( msg, warn, file, line );
}

ErrorList::Error &Ip::error_msg( String msg, bool warn, const char *file, int line ) {
    ErrorList::Error &res = error_list.add( msg, warn );
    if ( file )
        res.caller_stack.push_back( line, file );

    //    if ( sf )
    //        res.ac( sf_info( sf )->filename, off );
    //    for( Scope *s = sc; s; s = s->caller )
    //        if ( s->instantiated_from_sf )
    //            res.ac( sf_info( s->instantiated_from_sf )->filename, s->instantiated_from_off );
    return res;
}


static Ip ip_inst;
Ip *ip = &ip_inst;
