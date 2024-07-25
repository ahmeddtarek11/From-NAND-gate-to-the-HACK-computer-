#include <iostream>
#include <queue>
#include <map>
#include <fstream>
#include <string>
#include <bitset>
using namespace std;

void initialize_symbol_table();
void first_pass(ifstream &input_file, queue<string> &q1);
void A_instruction_decode(string &instrc, ofstream &output_file);
void C_instruction_decode(string &instrc, ofstream &output_file);
bool is_decimal(string num);
string remove_empty_space(string &line);

map<string, string> symbol_table;
map<string, string> JMP_table;
map<string, string> C_instruction_table;
map<string, string> C_dest_table;
queue<string> q1;

int main(int argc, char *argv[])
{
    initialize_symbol_table();
    ofstream output_file;
    ifstream input_file;

    if (argc < 3)
    {
        cerr << "main(): please specify input .asm file and output .txt file and try again " << endl;
        return 1;
    }
    else
    {
        input_file.open(argv[1]);
        output_file.open(argv[2]);

        if (!input_file.is_open())
        {
            cerr << "Error opening input file: " << argv[1] << endl;
            return 1;
        }
        if (!output_file.is_open())
        {
            cerr << "Error opening output file: " << argv[2] << endl;
            return 1;
        }
    }

    first_pass(input_file, q1);

    // reading queue instruction,decoding then printing
    while (!q1.empty())
    {
        string instrc;

        instrc = q1.front();

        if (instrc[0] == '@')
        {
            A_instruction_decode(instrc, output_file);
        }
        else
        {
            C_instruction_decode(instrc, output_file);
        }

        q1.pop();
    }

    input_file.close();
    output_file.close();
    return 0;
}

void first_pass(ifstream &input_file, queue<string> &q1)
{
    string line;
    int line_count = 0;

    while (!input_file.eof())
    {
        getline(input_file, line);
        remove_empty_space(line);
        // there's an error here with the whitespace checks
        if (line.empty() || line[0] == '/' || line[0] == '(')
        {

            if (line[0] == '(')
            {
                string label = "";
                for (int i = 1; i < line.size(); i++)
                { // storing the string label char by char in the string vriable label
                    if (line[i] == ')')
                        break;
                    label += line[i];
                }
                // converting the next line value from decimal to a 15 bit binary
                bitset<15> bin(line_count);
                string binary = bin.to_string();
                symbol_table[label] = binary;
            }
            continue;
        }

        q1.push(line);
        line_count++;
    }
}

void A_instruction_decode(string &instrc, ofstream &output_file)
{
    string ans = "0";   // A-instructions start with '0'
    instrc.erase(0, 1); // Remove the '@' character

    // Check if it's a symbol or a direct address
    if (is_decimal(instrc))
    {
        bitset<15> bin(stoi(instrc));
        ans += bin.to_string();
    }
    else
    {
        // If it's a symbol, check if it exists in the symbol table
        if (symbol_table.find(instrc) == symbol_table.end())
        {
            // Allocate new address if symbol not found
            static int adrs = 16;
            bitset<15> bin(adrs);
            symbol_table[instrc] = bin.to_string();
            adrs++;
        }
        ans += symbol_table[instrc];
    }

    output_file << ans << endl;
}

// void A_instruction_decode(string &instrc, ofstream &output_file)
// {
//     // storing a instruction the table
//     string ans = "";
//     static int adrs = 16;
//     instrc.erase(0, 1);
//     if (symbol_table.find(instrc) == symbol_table.end())
//     {

//         if (is_decimal(instrc))
//         {
//             bitset<16> bin(stoi(instrc));
//             ans = bin.to_string();
//         }

//         else
//         {
//             bitset<15> bin(adrs);
//             string binary = bin.to_string();
//             symbol_table[instrc] = binary;
//             ans += '0' + binary;
//             adrs++;
//         }
//     }

//     else
//     {
//         ans = '0' + symbol_table[instrc];
//     }

//     output_file << ans << endl;
// }

void C_instruction_decode(string &instrc, ofstream &output_file)
{
    string C_header = "111";
    string ret = "";
    // Concatenating each field of the C-Instruction
    string comp = "", des = "", jump = "";
    bool equal_sign = false, semi_colon = false;
    int equal_sign_index = -1, semi_colon_index = -1;

    for (int i = 0; i < instrc.size(); i++)
    {
        if (instrc[i] == '=')
        {
            equal_sign = true;
            equal_sign_index = i;
            break;
        }
    }
    for (int i = 0; i < instrc.size(); i++)
    {
        if (instrc[i] == ';')
        {
            semi_colon = true;
            semi_colon_index = i;
            break;
        }
    }

    if (equal_sign)
    {
        if (semi_colon)
        {
            des = instrc.substr(0, equal_sign_index);
            comp = instrc.substr(equal_sign_index + 1, semi_colon_index - equal_sign_index - 1);
            jump = instrc.substr(semi_colon_index + 1);
        }
        else
        {
            des = instrc.substr(0, equal_sign_index);
            comp = instrc.substr(equal_sign_index + 1);
            jump = "null";
        }
    }
    else
    {
        if (semi_colon)
        {
            des = "null";
            comp = instrc.substr(0, semi_colon_index);
            jump = instrc.substr(semi_colon_index + 1);
        }
        else
        {
            des = "null";
            comp = instrc;
            jump = "null";
        }
    }
    C_header += C_instruction_table[comp];
    C_header += C_dest_table[des];
    C_header += JMP_table[jump];

    output_file << C_header << endl;
}

bool is_decimal(string num)
{
    int cnt = 0;
    for (int i = 0; i < num.size(); i++)
        if (isdigit(num[i]))
            cnt++;

    if (cnt == num.size())
        return true;
    return false;
}

string remove_empty_space(string &line)
{
    string nospace = "";
    bool comment = false;

    for (int i = 0; i < line.size(); i++)
    {
        if (line[i] == '/' && i + 1 < line.size() && line[i + 1] == '/')
        {
            break; // Stop processing if a comment is found
        }
        if (line[i] != ' ' && line[i] != '\t' && !comment)
        {
            nospace += line[i];
        }
    }

    line = nospace;
    return line;
}

void initialize_symbol_table()
{

    symbol_table["R0"] = "000000000000000";
    symbol_table["R1"] = "000000000000001";
    symbol_table["R2"] = "000000000000010";
    symbol_table["R3"] = "000000000000011";
    symbol_table["R4"] = "000000000000100";
    symbol_table["R5"] = "000000000000101";
    symbol_table["R6"] = "000000000000110";
    symbol_table["R7"] = "000000000000111";
    symbol_table["R8"] = "000000000001000";
    symbol_table["R9"] = "000000000001001";
    symbol_table["R10"] = "000000000001010";
    symbol_table["R11"] = "000000000001011";
    symbol_table["R12"] = "000000000001100";
    symbol_table["R13"] = "000000000001101";
    symbol_table["R14"] = "000000000001110";
    symbol_table["R15"] = "000000000001111";
    symbol_table["SP"] = "000000000000000";
    symbol_table["LCL"] = "000000000000001";
    symbol_table["ARG"] = "000000000000010";
    symbol_table["THIS"] = "000000000000011";
    symbol_table["THAT"] = "000000000000100";
    symbol_table["SCREEN"] = "100000000000000";
    symbol_table["KBD"] = "110000000000000";

    // Jump table
    JMP_table["JGT"] = "001";
    JMP_table["JEQ"] = "010";
    JMP_table["JGE"] = "011";
    JMP_table["JLT"] = "100";
    JMP_table["JNE"] = "101";
    JMP_table["JLE"] = "110";
    JMP_table["JMP"] = "111";
    JMP_table["null"] = "000";

    C_instruction_table["0"] = "0101010";
    C_instruction_table["1"] = "0111111";
    C_instruction_table["-1"] = "0111010";
    C_instruction_table["D"] = "0001100";
    C_instruction_table["A"] = "0110000";
    C_instruction_table["M"] = "1110000";
    C_instruction_table["!D"] = "0001101";
    C_instruction_table["!A"] = "0110001";
    C_instruction_table["!M"] = "1110001";
    C_instruction_table["-D"] = "0001111";
    C_instruction_table["-A"] = "0110011";
    C_instruction_table["-M"] = "1110011";
    C_instruction_table["D+1"] = "0011111";
    C_instruction_table["1+D"] = "0011111";
    C_instruction_table["A+1"] = "0110111";
    C_instruction_table["M+1"] = "1110111";
    C_instruction_table["1+A"] = "0110111";
    C_instruction_table["1+M"] = "1110111";
    C_instruction_table["D-1"] = "0001110";
    C_instruction_table["-1+D"] = "0001110";
    C_instruction_table["A-1"] = "0110010";
    C_instruction_table["M-1"] = "1110010";
    C_instruction_table["-1+A"] = "0110010";
    C_instruction_table["-1+M"] = "1110010";
    C_instruction_table["D+A"] = "0000010";
    C_instruction_table["D+M"] = "1000010";
    C_instruction_table["A+D"] = "0000010";
    C_instruction_table["M+D"] = "1000010";
    C_instruction_table["D-A"] = "0010011";
    C_instruction_table["D-M"] = "1010011";
    C_instruction_table["A-D"] = "0000111";
    C_instruction_table["M-D"] = "1000111";
    C_instruction_table["D&A"] = "0000000";
    C_instruction_table["D&M"] = "1000000";
    C_instruction_table["D|A"] = "0010101";
    C_instruction_table["D|M"] = "1010101";
    C_instruction_table["A|D"] = "0010101";
    C_instruction_table["M|D"] = "1010101";

    // Destination field table
    C_dest_table["M"] = "001";
    C_dest_table["D"] = "010";
    C_dest_table["MD"] = "011";
    C_dest_table["A"] = "100";
    C_dest_table["AM"] = "101";
    C_dest_table["AD"] = "110";
    C_dest_table["AMD"] = "111";
    C_dest_table["null"] = "000";
}