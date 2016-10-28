#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include "sudoku.h"
using namespace std;
const char *cnf_fn = "tmp/minisat_if";
const char *minisat_ofn = "tmp/minisat_of";
int main(int argc, char** argv)
{
    int num_answer=0;
    int row, col, digit;
    string sys_arg = (string)"./" + argv[3] + " " + cnf_fn + " " + minisat_ofn;
    // The program should take three arguments: <input puzzle>, <output puzzle>, <MiniSatExe>.
    if(argc<=3)
    {
        cerr << "Usage: " << argv[0] << " <input puzzle> <output puzzle> <MiniSatExe>" << endl;
        exit(EXIT_FAILURE);
    }
    // Read <input-file>.
    Puzzle puzzle;
    ifstream ifs_puzzle(argv[1], ifstream::in);
    if(!ifs_puzzle.is_open())
    {
        perror(argv[1]);
        exit(EXIT_FAILURE);
    }
    if(puzzle.get_puzzle(*(istream*)&ifs_puzzle)==-1)
    {
        cerr << argv[1] << " is in a bad format\n";
        ifs_puzzle.close();
    }
    ifs_puzzle.close();

    // Creat a tmp folder
    if(mkdir("tmp", ACCESSPERMS)==-1)
    {
        perror("tmp");
    }
    do{
        // Encode the puzzle into CNF and Write to a DIMACS format file.
        ofstream ofs_cnf(cnf_fn, ofstream::out|ofstream::trunc);
        if(!ofs_cnf.is_open())
        {
            perror(cnf_fn);
            exit(EXIT_FAILURE);
        }
        puzzle.put_cnf(*(ostream*)&ofs_cnf);
        ofs_cnf.close();
        
        // Call MiniSat.
        // ./<MiniSatExe> <input-file> <result-output-file> >/dev/null
        cout << "Call " << argv[3] << " for the " << num_answer+1 << "'s time.\n";
        system(sys_arg.c_str());
        
        // Read minisat's output file and store it.
        ifstream ifs_minisat_result(minisat_ofn, ifstream::in);
        if(!ifs_minisat_result.is_open())
        {
            perror(minisat_ofn);
            exit(EXIT_FAILURE);
        }
        puzzle.get_answer(*(istream*)&ifs_minisat_result);
        ifs_minisat_result.close();
        if(puzzle.is_sat()){
            cout << "The " << ++num_answer << "'s answer is found.\n\n";
        }
        // break; /** This line must be made a comment for finding multiple answers. **/
    } while(puzzle.is_sat());
    cout << "There is no other answer.\n";

    // Save the answer to the output file.
    ofstream ofs_answer(argv[2], ofstream::out|ofstream::trunc);
    if(!ofs_answer.is_open())
    {
        perror(argv[2]);
        exit(EXIT_FAILURE);
    }
    puzzle.print_answer(*(ostream*)&ofs_answer);
    ofs_answer.close();

    unlink(cnf_fn);
    unlink(minisat_ofn);
    rmdir("tmp");
    return 0;
}