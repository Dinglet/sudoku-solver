#ifndef _SUDOKU_H
#define _SUDOKU_H
#include <iostream>
#include <vector>
#include <string>
using namespace std;
class Puzzle{
public:
    Puzzle();
    int get_puzzle(istream &);
    void put_cnf(ostream &);
    void get_answer(istream &);
    void print_answer(ostream &);
    int is_sat();
protected:
    int N, sqrt_N, digit_width;
    int flag_sat, num_answer;
    int num_var, num_clause;
    string str_cnf;
    vector<unsigned short> puzzle;
    vector<vector<unsigned short> > answer;
    vector<vector<bool> > check_list_row, check_list_col, check_list_matrix;
};
#endif