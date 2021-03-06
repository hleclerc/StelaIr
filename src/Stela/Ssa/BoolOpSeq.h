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


#ifndef BOOLOPSEQ_H
#define BOOLOPSEQ_H

#include "../System/Stream.h"
class Expr;

/**
*/
class BoolOpSeq {
public:
    BoolOpSeq( bool default_val = true );

    void write_to_stream( Stream &os ) const;
    bool always( bool val ) const;

    bool default_val;
};

/// simplify and/or stuff in cond
Expr simplified_cond( const Expr &cond );

#endif // BOOLOPSEQ_H
