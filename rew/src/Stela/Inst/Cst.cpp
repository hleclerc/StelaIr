#include "Cst.h"

class Cst;
static Vec<Cst *> cst_list;

/**
*/
class Cst : public Inst {
public:
    Cst( int len, const PI8 *data, const PI8 *kwnw ) : len( len ) {
        this->data.resize( 2 * sb() );
        memcpy( d(), data, sb() );
        memcpy( k(), kwnw, sb() );
    }
    int sb() const { return ( len + 7 ) / 8; }
    const PI8 *d() const { return data.ptr() + 0 * sb(); }
    const PI8 *k() const { return data.ptr() + 1 * sb(); }
    PI8 *d() { return data.ptr() + 0 * sb(); }
    PI8 *k() { return data.ptr() + 1 * sb(); }

    virtual void write_dot( Stream &os ) const {
        if ( len == 0 )
            os << "";
        else if ( len <= 8 )
            os << (int)*reinterpret_cast<const PI8 *>( d() );
        else if ( len == 16 )
            os << *reinterpret_cast<const SI16 *>( d() );
        else if ( len == 32 )
            os << *reinterpret_cast<const SI32 *>( d() );
        else if ( len == 64 )
            os << *reinterpret_cast<const SI64 *>( d() );
        else {
            const char *c = "0123456789ABCDEF";
            for( int i = 0; i < std::min( sb(), 4 ); ++i ) {
                if ( i )
                    os << ' ';
                os << c[ data[ i ] >> 4 ] << c[ data[ i ] & 0xF ];
            }
            if ( sb() > 4 )
                os << "...";
        }

    }

    virtual Expr forced_clone( Vec<Expr> &created ) const {
        return new Cst( len, d(), k() );
    }

    virtual int size() const {
        return len;
    }

    virtual const PI8 *data_ptr( int offset ) const {
        return d();
    }
    bool equal( int l, const PI8 *nd, const PI8 *nk ) const {
        if ( l != len )
            return false;
        for( int i = 0; i < sb(); ++i )
            if ( nd[ i ] != d()[ i ] )
                return false;
        for( int i = 0; i < sb(); ++i )
            if ( nk[ i ] != k()[ i ] )
                return false;
        return true;
    }
    virtual int checked_if( Expr cond ) {
        if ( ( k()[ 0 ] & 1 ) and cond->always_checked() != -1 )
            return d()[ 0 ] & 1 ? 1 : -1;
        return 0;
    }
    virtual int always_checked() const {
        if ( k()[ 0 ] & 1 )
            return d()[ 0 ] & 1 ? 1 : -1;
        return 0;
    }

    int len;
    Vec<PI8> data;
};

Expr cst( int len, const PI8 *data, const PI8 *kwnw ) {
    int sb = ( len + 7 ) / 8;
    if ( not data ) {
        PI8 nd[ sb ];
        for( int i = 0; i < sb; ++i )
            nd[ i ] = 0x00;
        return cst( len, nd, nd );
    }
    if ( not kwnw ) {
        PI8 nk[ sb ];
        for( int i = 0; i < sb; ++i )
            nk[ i ] = 0xFF;
        return cst( len, data, nk );
    }
    // normalize data
    PI8 nd[ sb ];
    for( int i = 0; i < sb; ++i )
        nd[ i ] = data[ i ] & kwnw[ i ];
    // already created ?
    for( Cst *c : cst_list )
        if ( c->equal( len, nd, kwnw ) )
            return c;
    // else, create a new one
    return new Cst( len, nd, kwnw );
}