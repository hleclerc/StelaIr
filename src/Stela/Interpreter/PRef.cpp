#include "Var.h"

struct VRF {
    Var var;
    int off; ///< offset in bits
};


PRef::~PRef() {
    delete refs;
}

void PRef::add_ref( int offset, const Var &var ) {
    get_vrf( offset )->var = var;
}

Var PRef::get_ref( int offset ) {
    return get_vrf( offset )->var;
}

VRF *PRef::get_vrf( int offset ) {
    if ( not refs )
        refs = new Vec<VRF>;
    for( int i = 0; i < refs->size(); ++i )
        if ( refs->operator[]( i ).off == offset )
            return &refs->operator[]( i );
    VRF *res = refs->push_back();
    res->off = offset;
    return res;
}
