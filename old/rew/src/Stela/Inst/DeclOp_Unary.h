// name, C++ op, is_oper, return bool, prec
DECL_OP( neg        , "-"  , 1, 0, 20 )
DECL_OP( not_boolean, "not", 1, 1, 17 )

DECL_OP( log        , "log"  , 0, 0, 0 )
DECL_OP( ceil       , "ceil" , 0, 0, 0 )
