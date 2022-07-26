/* data.cpp
- converts vect.txt to vect.bin
- converts index.txt to index.bin
- calculates dimension of embeddings
*/

#include <bits/stdc++.h>

using namespace std;

int main(int argc, char const *argv[]){
    
    string input_path = argv[1];
    string output_path = argv[2];
    
    string vect_file_in = input_path + "/vect.txt";
    string index_file_in = input_path + "/index.txt";
    
    string vect_file_out = output_path + "/vect.bin";
    string index_file_out = output_path + "/index.bin";
    
    // vect -- read
    ifstream in_file;
    in_file.open(vect_file_in);
    ofstream vect_out_file(vect_file_out, std::ios::binary);

    // dimension -- count
    string first_line;
    getline(in_file, first_line);

    int col_dim = 0;
    int i = 0;
    while(i < first_line.size()){
        if(first_line[i] == ' '){
            col_dim++;
        }
        i++;
    }
    
    col_dim++;
//     cout<<"Dim: "<<col_dim<<"\n";

    in_file.close();
    in_file.open(vect_file_in);
    
    float embd;

    while (in_file >> embd){
        vect_out_file.write(reinterpret_cast<const char*>(&embd), sizeof(embd));
    }
    
    in_file.close();
    vect_out_file.close();

    ofstream dim_file(output_path + "/dimension.txt");
    dim_file<<col_dim;
    dim_file.close();

    //index -- read
    int ind;
    ofstream index_out_file(index_file_out, std::ios::binary );
    in_file.open(index_file_in);
    
    while (in_file >> ind){
        index_out_file.write(reinterpret_cast<const char*>(&ind), sizeof(ind)) ;
    }
    
    in_file.close();
    index_out_file.close();

    return 0;
}
