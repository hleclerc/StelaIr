/****************************************************************************
**
** Copyright (C) 2014 SocaDB
**
** This file is part of the SocaDB toolkit/database.
**
** SocaDB is free software. You can redistribute this file and/or modify
** it under the terms of the Apache License, Version 2.0 (the "License").
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** You should have received a copy of the Apache License, Version 2.0
** along with this program. If not, see
** <http://www.apache.org/licenses/LICENSE-2.0.html>.
**
**
** Commercial License Usage
**
** Alternatively, licensees holding valid commercial SocaDB licenses may use
** this file in accordance with the commercial license agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and SocaDB.
**
**
****************************************************************************/


#ifndef TYPE_H
#define TYPE_H

#include "../System/Stream.h"
#include "Inst.h"
class Class;

/**
*/
class Type {
public:
    struct Attr {
        Expr          off_expr; ///< Expr() -> static
        Expr          val; ///< flags may contain to Inst::PART_INST
        String        name;
        int           off;
        const String *src;
    };
    struct AsVar {
        void write_to_stream( Stream &os ) const { type->as_var( os ); }
        Type *type;
    };

    Type( Class *orig );

    void  write_to_stream( Stream &os ) const;
    void  as_var( Stream &os, bool und = true ) const;
    int   alig(); ///< needed alignement in bits
    int   size(); ///< size in bits, or -1 if not known
    int   sb(); ///< size in bytes, or -1 if not known
    Expr  size( Expr obj ); ///< size in bits (return an expr to be computed if not known)

    bool  pod(); ///< size in bits, or -1 if not known

    void  find_attr( Vec<Attr *> &attr_list, String name );
    Attr *find_attr( String name );
    Expr  find_static_attr( String name );
    Expr  attr_expr( Expr self, Attr &a ); ///< get self.a
    Type *ptype();

    // numeric/known values
    virtual bool get_val( void *res, Type *type, const PI8 *data, const PI8 *knwn );
    virtual bool always( bool val, const PI8 *data, const PI8 *knwn );
    virtual bool always_equal( Type *t, const void *d, const PI8 *data, const PI8 *knwn );

    //


    virtual void write_val( Stream &os, const PI8 *data, const PI8 *knwn = 0 );
    void parse();

    Class    *orig;
    Vec<Expr> parameters;

    Expr      _symbol;    ///< symbol to use for _len_expr and off_expr substitution
    Expr      _len_expr;
    int       _len;       ///< sie in bits, -1 if depends on the underlying value
    int       _ali;       ///< needed alignment in bits
    int       _pod;       ///< plain old data
    bool      _parsed;    ///< true is already parsed
    bool      aryth;      ///< true if basic arythmetic type
    Vec<Attr> attributes; ///< static and dynamic attributes
};

#endif // TYPE_H
