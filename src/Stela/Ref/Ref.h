#ifndef REF_H
#define REF_H

#include "../System/Ptr.h"
#include "RefItem.h"
#include "Type.h"

/**
   Var pour pointer sur Ref...
   Pourquoi ne pas utiliser directement des Ref, avec des RefItemRef
     -> parce que de toutes façons, il faut utiliser des Ptr<Ref>
     -> sauf si on dit que ~Ref déplace le RefItemRef. Pour y arriver, il faut que chaque Ref
        contienne par exemple une liste chainée de références

*/
class Ref : public GarbageCollectedObject {
public:
    enum {
        CONST = 1
    };

    Ref( Type *type, RefItem *ref_item );
    virtual ~Ref();

    void write_to_stream( Stream &os ) const;

protected:
    Type    *type;
    RefItem *ref_item; ///<
    int      flags;
};

#endif // REF_H
