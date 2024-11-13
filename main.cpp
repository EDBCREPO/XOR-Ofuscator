#include <nodepp/nodepp.h>
#include <nodepp/fs.h>

/*─────────────────────────────────────────────────────────────────*/

using namespace nodepp;

/*─────────────────────────────────────────────────────────────────*/

void normal() {

    auto fout = fs::writable( process::env::get("out") );
    auto file = fs::readable( process::env::get("in") );
    auto sec  = process::env::get("sec");

    file.onData([=]( string_t data ){
        ulong pos = 0; forEach( x, data ){
            x = x ^ sec[pos]; pos++; 
            pos %= sec.size() + 1;
        }   fout.write( data );
    });

    stream::pipe( file );
    
}

/*─────────────────────────────────────────────────────────────────*/

void abnormal() {

    auto file = fs::readable( process::env::get("in") );
    auto sec  = process::env::get("sec");
    auto data = stream::await( file );

    ulong pos = 0; forEach( x, data ){
        x = x^sec[pos]; pos++;
        pos %= sec.size() + 1;
    }

    auto fout = fs::writable( process::env::get("out") );
    fout.write( data );

}

/*─────────────────────────────────────────────────────────────────*/

void subnormal() {

    file_t out;

    if( !process::env::get("out").empty() )
      { out = fs::writable( process::env::get("out") ); }

    auto sec  = process::env::get("sec");
    auto file = fs::std_input();

    file.onData([=]( string_t data ){

        ulong pos = 0; forEach( x, data ){
            x = x ^ sec[pos]; pos++; 
            pos %= sec.size() + 1;
        }   
        
        if( !out.is_available() ){
            console::log( data );
        } else {
            out.write( data );
        }

    });

    stream::pipe( file );

}

/*─────────────────────────────────────────────────────────────────*/

void onMain() {

    if( process::env::get("sec").empty() ){ process::error("no secret key found"); }

    if( process::env::get("file").empty() && process::env::get("in").empty() )
      { subnormal(); return; }

    if( process::env::get("file").empty() ){
    if( process::env::get("out").empty() ){ process::error("no output file found"); }
    if( process::env::get("in").empty()  ){ process::error("no input file found"); }
    } else { 
        process::env::set( "out", process::env::get("file") );
        process::env::set( "in" , process::env::get("file") );
    }

    if( process::env::get("out") == process::env::get("in") )
      { abnormal(); return; } else { normal(); return; }

}

/*─────────────────────────────────────────────────────────────────*/