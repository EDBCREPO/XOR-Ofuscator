#include <nodepp/nodepp.h>
#include <nodepp/fs.h>

using namespace nodepp;

void onMain() {

    if( process::env::get("out").empty() ){ process::error("no output file found"); }
    if( process::env::get("sec").empty() ){ process::error("no secret key found"); }
    if( process::env::get("in").empty()  ){ process::error("no input file found"); }

    auto fout = fs::writable( process::env::get("out") );
    auto file = fs::readable( process::env::get("in") );
    auto sec  = process::env::get("sec");

    file.onClose([](){ console::log("done"); });

    file.onData([=]( string_t data ){
        ulong pos = 0; forEach( x, data ){
            x = x ^ sec[pos]; pos++; 
            pos %= sec.size() + 1;
        }   fout.write( data );
    });

    stream::pipe( file );

}