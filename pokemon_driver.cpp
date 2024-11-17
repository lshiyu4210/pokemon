// Project Identifier: 5949F553E20B650AB0FB2266D3C0822B13D248B0
#include "pokemon.h"
#include <getopt.h>
//#include <chrono>

void Pokemon::get_mode(int argc, char** argv) {
    int option_index = 0, option = 0;
    
    // Don't display getopt error messages about options
    opterr = false;

    struct option longOpts[] = {{ "help", no_argument,       nullptr, 'h' },
                                { "mode", required_argument, nullptr, 'm' },
                                { nullptr, 0,                nullptr, '\0' },};

    while ((option = getopt_long(argc, argv, "hm:", longOpts, &option_index)) != -1) {
        switch (option) {
            case 'h':
                cout << "Valid Syntax: -m, --mode {MST|FASTTSP|OPTTSP}\n";
                exit(0);
        
            case 'm': {  
                if (optarg == nullptr) {
                    cerr << "Error: No mode specified\n";
                    exit(1);
                }

                string arg{optarg};
                if (arg != "MST" && arg != "FASTTSP" && arg != "OPTTSP") {
                    cerr << "Error: Invalid mode\n";
                    exit(1);
                }  

                if (arg[0] == 'M') {
                    mode = Mode::MST;
                } else if (arg[0] == 'F') {
                    mode = Mode::FASTTSP;
                } else {
                    mode = Mode::OPTTSP;
                }  
                break;
            }  

            case ':':       
                cerr << "Error: No mode specified\n";
                exit(1);

            default:
                cerr << "Error: Invalid command line option\n";
                exit(1);
                
    }
  }
}

void Pokemon::select_mode() {
    if (mode == Mode::MST) {
        Prims_MST();
    }
    else if (mode == Mode::FASTTSP) {
        fast_tsp();
    }
    else {
        opt_tsp();
    }
}

int main(int argc, char *argv[]) {

    ios_base::sync_with_stdio(false);
    //auto start = chrono::high_resolution_clock::now();
    Pokemon poke;
    poke.get_mode(argc, argv);
    poke.read_vertices();
    poke.select_mode();
    poke.output();
    // auto stop = chrono::high_resolution_clock::now();
    // auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
    // cout << "Elapsed time: " << duration.count() << " milliseconds\n";
}