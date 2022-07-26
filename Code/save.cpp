#include <bits/stdc++.h>
using namespace std;

int main(int argc, char* argv[]){
    
    string input_path = argv[1]; //input path
    int topk = stoi(argv[2]);
    string output_file = argv[3];
    
    ifstream in_file;
    in_file.open(input_path+"/user_prediction.bin", std::ios::binary);
    std::ofstream finaltxt(output_file);
    int buff;
    int c = 0;
    
//     cout<<topk<<endl<<endl;
    
    
    while(in_file.read(reinterpret_cast<char*>(&buff),sizeof(buff))){
        c++;
//         cout << buff << " ";
        finaltxt << buff;
        if (c==topk){
            finaltxt << "\n";
//             cout << "\n";
            c=0;
        }
        else{
            finaltxt << " ";
        }
      
    }
    finaltxt.close();
    
//     cout<<endl;
}