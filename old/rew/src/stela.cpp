/**
  main file for the stela interpreter / compiler
*/
// #include <Stela/Interpreter/Interpreter.h>
// #include <Stela/Compilers/CppCompiler.h>
#include <Stela/System/InstallDir.h>
#include <Stela/System/ReadFile.h>
#include <Stela/Inst/Codegen_C.h>
#include <Stela/Met/IrWriter.h>
#include <Stela/Met/Lexer.h>
#include <Stela/Inst/Ip.h>
#include <fstream>
#include <math.h>


#define PREPARG_FILE <args.h>
#include <PrepArg/usage.h>

int main( int argc, char **argv ) {
    // args
    #include <PrepArg/declarations.h>
    #include <PrepArg/parse.h>
    if ( beg_files < 0 )
        return usage( argv[ 0 ], "Please specify an input file", 2 );
    bool add_base_files = not ( disp_lexems or disp_tokens );

    // particular case
    if ( disp_tokens or disp_lexems ) {
        // -> source data
        ReadFile r( argv[ beg_files ] );
        if ( not r ) {
            std::cerr << "Impossible to open " << argv[ beg_files ] << std::endl;
            return 2;
        }

        // -> lexical data
        ErrorList e;
        Lexer l( e );
        l.parse( r.data, argv[ beg_files ] );
        if ( e )
            return 3;
        if ( disp_lexems )
            l.display();

        if ( disp_tokens )
            TODO;

        return 0;
    }

    // input files
    Vec<String> input_files;
    if ( add_base_files and false )
        input_files << String( base_met_files ) + "/base.met";
    for( int i = beg_files; i < argc; ++i )
        input_files << argv[ i ];

    // context
    Ip ip_inst; ip = &ip_inst;
    ip->add_inc_path( base_met_files );

    // parse
    for( int i = 0; i < input_files.size(); ++i )
        ip->import( input_files[ i ] );

    // compile
    Codegen_C cr;
    cr.disp_inst_graph_wo_phi = disp_inst_g_wo_phi;
    cr.disp_inst_graph = disp_inst_g;
    cr << ip->sys_state.get_val();

    std::ofstream out_file( "out.cpp" );
    cr.write_to( out_file );

    return ip->error_list;
}

