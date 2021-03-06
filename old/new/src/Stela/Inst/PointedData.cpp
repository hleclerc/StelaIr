#include "PointedData.h"
#include "Ip.h"

/**
*/
class PointedData : public Inst {
public:
    PointedData( int len ) : len( len ) {
    }
    virtual void write_dot( Stream &os ) const {
        os << "@";
    }
    virtual void write_to_stream( Stream &os ) const {
        os << "@" << inp[ 0 ];
    }
    virtual int size() const {
        return len;
    }
    virtual void set( Ptr<Inst> val ) {
        if ( Ptr<Inst> ptr = inp[ 0 ]->_pointer_on( 0, len ) )
            return ptr->set( val );
        IP_ERROR( "inp[ 0 ] is not a pointer data" );
    }
    virtual Ptr<Inst> _simplified() {
        if ( Ptr<Inst> ptr = inp[ 0 ]->_pointer_on( 0, len ) )
            return simplified( ptr );
        return 0;
    }
    virtual Ptr<Inst> forced_clone( Vec<Ptr<Inst> > &created ) const {
        return new PointedData( len );
    }
    virtual void write_1l_to( CodeGen_C *cc ) const {
        TODO;
    }

    int len;
};

Ptr<Inst> pointed_data( Ptr<Inst> ptr, int size ) {
    PointedData *res = new PointedData( size );
    res->add_inp( ptr );
    return res;
}
