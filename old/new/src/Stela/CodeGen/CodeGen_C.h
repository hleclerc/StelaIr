#ifndef CODEGEN_C_H
#define CODEGEN_C_H

#include "../System/StreamSep.h"
#include "CodeGen.h"

/**
*/
class CodeGen_C : public CodeGen {
public:
    struct C_Code {
        void write_to_stream( Stream &os ) const;
        CodeGen_C  *cc;
        const Inst *inst;
    };

    CodeGen_C();

    virtual void write_to( Stream &os );
    virtual void exec();

    Stream &decl_if_nec( const Inst *inst );
    Stream &bdef_if_nec( const Inst *inst ); ///< beg def ("  Type Rx = ") if necessary
    C_Code code( const Ptr<Inst> &inst );

    StreamSepMaker<std::ostringstream> on;
    std::ostringstream main_os;
    std::ostringstream *os;

protected:
    Ptr<Inst> get_next_inst_in_front( Vec<Ptr<Inst> > &front, Ptr<Inst> current_cond );
    void make_code();
    int nb_regs;
};

#endif // CODEGEN_C_H
