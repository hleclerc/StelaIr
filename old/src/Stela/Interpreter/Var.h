#ifndef VAR_H
#define VAR_H

#include "PRef.h"
class ClassInfo;
class TypeInfo;
class Type;

/**
  Several variables may point to the same Ref, and they are also able to change the type of Ref,
  explaining the need for a double pointer
*/
class Var {
public:
    enum {
        WEAK_CONST = 1, ///< this variable should be changed, but nothing can guarantee that it won't be changed elsewhere
        SURDEF     = 2,
        IS_A_REF   = 4 ///< no destruction
    };

    // Où stocker ip ??
    Var( Ptr<PRef> type, Ptr<PRef> data, int flags );
    Var( Ptr<PRef> type, const Expr &expr );
    Var( Ptr<PRef> type, Ptr<Ref> ref );
    Var( Var *type, const Expr &expr );
    Var( Var *type, Ptr<PRef> ref );
    Var( Var *type, Ptr<Ref> ref );
    Var( Var *type );
    Var();

    ~Var();

    void write_to_stream( Stream &os ) const;
    String type_disp() const;

    operator bool() const { return type and data and data->ptr; }
    const PI8 *cst_data() const; ///< return 0 or a pointer on data if known
    bool referenced_more_than_one_time() const;

    Var add_ref_from( const Var &var ); ///< add all refs stored in var
    Var add_ref( int offset, const Var &var ); ///< return *this
    Var get_ref( int offset );

    Var pointed_value() const;

    ClassInfo *class_info() const;
    TypeInfo  *type_info() const;

    bool is_weak_const() const;
    bool is_full_const() const;
    bool is_surdef() const;

    Expr expr() const;
    Expr type_expr() const;

    Ptr<PRef> data;
    Ptr<PRef> type;
    int       flags;
};

Var constified( const Var &var );

#endif // VAR_H
