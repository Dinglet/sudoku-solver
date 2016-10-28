#include "sudoku.h"
#include <iostream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <cmath>
#include <cstring>
#include <iomanip>
#include <vector>
#include <iterator>
using namespace std;
Puzzle::Puzzle()
{
    N = sqrt_N = 0;
    num_answer = 0;
    flag_sat = 0;
}
int Puzzle::get_puzzle(istream &is=cin) // default: cin
{
    int i, counter;
    string first_line;
    istringstream iss_first_line;

    counter = 0;
    while (getline(is, first_line))
    {
        iss_first_line.clear();
        iss_first_line.str(first_line);
        while (iss_first_line >> i)
            counter++;
        if (counter != 0)
            break;
    }
    if (counter == 0)
        return -1;
    // It is an j-by-j puzzle.
    if (counter != N)
    {
        // A new puzzle with the new dimention is needed.
        N = counter;
        puzzle.resize(N*N);
    }
    answer.clear();
    sqrt_N = (int)sqrt((double)N);
    if(N!=sqrt_N*sqrt_N)
        return -1;
    iss_first_line.clear();
    iss_first_line.str(first_line);
    vector<unsigned short>::iterator it_puzzle = puzzle.begin();
    i = N;
    while (i--)
        iss_first_line >> *(it_puzzle++);
    i = N*(N - 1);
    while (i--)
        is >> *(it_puzzle++);
    // The puzzle is gotten.
    flag_sat = 0;
    i = N;
    digit_width = 0;
    while(i>0)
    {
        digit_width+=1;
        i/=10;
    }
    return 0;
}
void Puzzle::put_cnf(ostream &os=cout)
{
    int t1;
    int cell, digit, cell_another;
    ostringstream oss_buffer;
    if(num_answer) // There is already an answer.
    {
        for(cell=0; cell<N*N; cell+=1)
            oss_buffer << "-" << cell*N+answer[num_answer-1][cell] << " ";
        oss_buffer << "0\n";
        str_cnf.append(oss_buffer.str());
        oss_buffer.clear();
        os << "p cnf " << num_var << " " << ++num_clause << "\n";
        os << str_cnf;
    }
    else
    {
        // for the generating rule, see README
        num_var=N*N*N;
        num_clause=0;
        for(cell=0; cell<N*N; cell+=1) // for each cell
        {
            if(puzzle[cell])
            {
                // Prefilled cells.
                for(digit=0; digit<N; digit+=1)
                {
                    if(puzzle[cell]==digit+1)
                        oss_buffer << cell*N+digit+1 << " 0 ";
                    else
                        oss_buffer << "-" << cell*N+digit+1 << " 0 ";
                }
                oss_buffer << "\n";
                num_clause += N;
            }
            else
            {
                // Exactly one digit appears in each unfilled cell.
                for(digit=0; digit<N; digit+=1) // At less one digit appears.
                    oss_buffer << cell*N+digit+1 << " ";
                oss_buffer << "0\n";
                for(digit=0; digit<N; digit+=1) // for each digit
                {
                    for(int digit_another=digit+1; digit_another<N; digit_another+=1) // and it's another one
                    {
                        oss_buffer << "-" << cell*N+digit+1 << " -" << cell*N+digit_another+1 << " 0\n";
                    }
                }
                num_clause += N*(N-1)/2+1;
            }
        }
        for(digit=0; digit<N; digit+=1)
        {
            for(t1=0; t1<N; t1+=1) // for each row, each column, and each block
            {
                for(cell=0; cell<N; cell+=1) // for each cell in a row(t1)
                {
                    oss_buffer << t1*N*N+cell*N+digit+1 << " ";
                }
                oss_buffer << "0\n";
                for(cell=0; cell<N; cell+=1)
                {
                    for(cell_another=cell+1; cell_another<N; cell_another+=1)
                        oss_buffer << "-" << t1*N*N+cell*N+digit+1 << " -" << t1*N*N+cell_another*N+digit+1 << " 0 ";
                }
                oss_buffer << "\n";
                for(cell=0; cell<N; cell+=1) // for each cell in a column
                {
                    oss_buffer << cell*N*N+t1*N+digit+1 << " ";
                }
                oss_buffer << "0\n";
                for(cell=0; cell<N; cell+=1)
                {
                    for(cell_another=cell+1; cell_another<N; cell_another+=1)
                        oss_buffer << "-" << cell*N*N+t1*N+digit+1 << " -" << cell_another*N*N+t1*N+digit+1 << " 0 ";
                }
                oss_buffer << "\n";
                for(cell=0; cell<N; cell+=1)
                {
                    int row = t1/sqrt_N*sqrt_N + cell/sqrt_N;
                    int col = t1%sqrt_N*sqrt_N + cell%sqrt_N;
                    oss_buffer << (row)*N*N+(col)*N+digit+1 << " ";
                }
                oss_buffer << "0\n";
                for(cell=0; cell<N; cell+=1)
                {
                    int row = t1/sqrt_N*sqrt_N + cell/sqrt_N;
                    int col = t1%sqrt_N*sqrt_N + cell%sqrt_N;
                    for(cell_another=cell+1; cell_another<N; cell_another+=1)
                    {
                        int row_another = t1/sqrt_N*sqrt_N + cell_another/sqrt_N;
                        int col_another = t1%sqrt_N*sqrt_N + cell_another%sqrt_N;
                        oss_buffer << "-" << (row)*N*N+(col)*N+digit+1 << " -" << (row_another)*N*N+(col_another)*N+digit+1 << " 0 ";
                    }
                }
                oss_buffer << "\n";
            }
        }
        num_clause += 3*N*N*(1+N*(N-1)/2);
        os << "p cnf " << N*N*N << " " << num_clause << "\n";
        str_cnf = oss_buffer.str();
        os << str_cnf;
        oss_buffer.clear();
    }
}
void Puzzle::get_answer(istream &is)
{
    int var, row, col;
    string str_sat;
    getline(is,str_sat);
    if(str_sat=="SAT")
    {
        flag_sat=1;
        answer.push_back(vector<unsigned short>(N*N));
        for(row=0; row<N; row+=1)
        {
            for(col=0; col<N; col+=1)
            {
                while(is >> var)
                {
                    if(var>0)
                    {
                        answer[num_answer][row*N+col] = (var%N ? var%N : N);
                        break;
                    }
                }
            }
        }
        num_answer += 1;
    }
    else
        flag_sat=0;
}
void Puzzle::print_answer(ostream &os=cout)
{
    int i, j, t;
    if (num_answer)
    {
        for(t=0; t<num_answer; t+=1){
            for (i = 0; i<N; i += 1)
            {
                os << setw(digit_width) << answer[t][i*N];
                for (j = 1; j<N; j += 1)
                    os << ' ' << setw(digit_width) << answer[t][i*N + j];
                os << '\n';
            }
            os << '\n';
        }
    }
    else
        os << "NO\n";
    // os << flush;
}
int Puzzle::is_sat()
{
    return flag_sat;
}