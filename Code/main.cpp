#include <string>
#include <mpi.h>
#include <assert.h>
#include <omp.h>
#include <bits/stdc++.h>

using namespace std;

// returns cosine similarly b/w two float arrays with length = _length
float cosine_similarity(vector<float>& A, vector<float>& B, int _length){
    float dot = 0.0;
    float _a = 0.0;
    float _b = 0.0;

     for(unsigned int i = 0; i < _length; ++i){
        dot += A[i] * B[i] ;
        _a += A[i] * A[i] ;
        _b += B[i] * B[i] ;
    }
    _a = sqrt(_a);
    _b = sqrt(_b);
    
    return 1-(dot/(_a * _b)) ;
}

//SearchLayer function
priority_queue<pair<float,int>> SearchLayer(int topk, vector<float>& q, priority_queue<pair<float,int>>& candidates, vector<int>& indptr, vector<int>& index, vector<int>& level_offset, int lc, unordered_set<int>& visited, vector<vector<float>>& vect, int dim){
    
    priority_queue<pair<float,int>> top_k(candidates);
    
    while(candidates.size()>0){
        
        pair<float,int> temp = candidates.top();
        candidates.pop();
        
        int ep = temp.second;
        int start = indptr[ep] + level_offset[lc];
        int end = indptr[ep] + level_offset[lc+1];
        
        for(int i=start;i<end;i++){
            int px = index[i];
            if (visited.find(px) != visited.end() || px==-1)
                continue;
            
            visited.insert(px);
            float _dist = cosine_similarity(q, vect[px],dim);
            
            if(_dist>top_k.top().first && top_k.size() >= topk)
                continue;
            
            top_k.push({_dist,px});
            
            while(top_k.size()>topk){
                top_k.pop();
            }
            
            candidates.push({_dist,px});
        }
    }
    return top_k;
}

//QueryHNSW function
priority_queue<pair<float,int>> QueryHNSW(vector<float>& q, int topk, int ep, vector<int>& indptr, vector<int>& index, vector<int>& level_offset, int max_level, vector<vector<float>>& vect, int dim){
    
    priority_queue<pair<float,int>> top_k;
    top_k.push({cosine_similarity(q,(vect)[ep],dim),ep});
    
    unordered_set<int> visited;
    visited.insert(ep);
    
    for(int level = max_level; level >=0;level--){
        top_k = SearchLayer(topk,q,top_k,indptr,index,level_offset,level,visited,vect,dim);
    }
    
    return top_k;
} 

//main 
int main(int argc, char* argv[]){
    
    string input_path = argv[1]; //input path
    int topk = stoi(argv[2]);    //num of predictions
    string user_file = argv[3];  //user file path
    string output_file = argv[4];
    
    string vect_file = input_path + "/vect.bin";
    string index_file = input_path + "/index.bin";
    string ep_file = input_path + "/ep.txt";
    string indptr_file = input_path + "/indptr.txt";
    string level_file = input_path + "/level.txt";
    string level_offset_file = input_path + "/level_offset.txt";
    string max_level_file = input_path + "/max_level.txt";
    string dimension_file = input_path + "/dimension.txt";
    
    int ep;              //entry_point
    int dim;             //column dimension of embeddings
    int max_level;       //max levels
    vector<int> level;   //levels of all nodes
    vector<int> level_offset; 
    vector<vector<float>> user; //users
    vector<int> indptr;  //indptr
    vector<int> index;
    vector<vector<float>> vect;
    
    ifstream in_file;
    
    //ep file
    in_file.open(ep_file);
    in_file >> ep;
    in_file.close();
    
    //dim file
    in_file.open(dimension_file);
    in_file >> dim;
    in_file.close();
    
    //max_level file
    in_file.open(max_level_file);
    in_file >> max_level;
    in_file.close();
    
    //level file
    in_file.open(level_file);
    int temp;
    while(in_file >> temp){
        level.push_back(temp);
    }
    in_file.close();
    
    //level_offset file
    in_file.open(level_offset_file);
    int tmp;
    while(in_file >> tmp){
        level_offset.push_back(tmp);
    }
    in_file.close();
    
    //user file
    in_file.open(user_file);
    float temp1;
    
    while(in_file >> temp1){
        //float* us = new float[dim];
        vector<float> us(dim);
        us[0] = temp1;
        for(int i = 1; i < dim;i++){
            in_file >> us[i];
        }
        user.push_back(us);
    }
    in_file.close();
    
    
    //indptr file
    in_file.open(indptr_file);
    int tem;
    while(in_file >> tem){
        indptr.push_back(tem);
    }
    in_file.close();
    
    //index file
    in_file.open(index_file, ios::binary);
    int embd; 
    
    while(in_file.read(reinterpret_cast<char*>(&embd),sizeof(embd))){
        index.push_back(embd);
    }
    in_file.close();
    
    //vect file
    in_file.open(vect_file, ios::binary);
    float emb; 
    int count = 0;
    while(in_file.read(reinterpret_cast<char*>(&emb),sizeof(emb))){
        //float* row = new float[dim];
        vector<float> row(dim);
        row[0] = emb;
        for(int i = 1; i < dim;i++){
            in_file.read(reinterpret_cast<char*>(&row[i]),sizeof(row[i]));
        }
        vect.push_back(row);
    }
    
//     cout<<user.size()<<endl;
    
//     for(auto x : user){
//         for(int i=0;i<dim;i++) cout<<x[i]<<" ";
//         cout<<"\n";
//     }
    
    // MPI Pipeline
    int rank;
    int size;
    std::fstream writefile(input_path + "/user_prediction.bin", std::ios::out|std::ios::binary);
    MPI_Init(NULL, NULL);
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    
    
    int num_users = user.size();
    int low = (rank*num_users)/size;
    int high = ((rank+1)*num_users)/size;
    
//     cout<<"Rank: "<<rank<<" Size: "<<size<<" "<<num_users<<endl;
    
//     cout<<low<<" "<<high<<" "<<num_users<<endl;
    vector<vector<int>> rec_users(high-low);
    //int** rec_users = new int[num_users][topk];
    
//  this rank predicts for user between high and low
    #pragma omp parallel 
    {
        #pragma omp for
        for(int u=low; u<high; u++){
            //int* recom = new int[topk];
            vector<int> recom(topk);
            priority_queue<pair<float, int>> res = QueryHNSW(user[u], topk, ep, indptr, index, level_offset, max_level, vect, dim);
            int j = 0;
            while(!res.empty()){
                recom[topk-j-1] = res.top().second;
                res.pop();
                j++;
            }

            rec_users[u-low] = recom;
        }
    }
    
//     for(int i=low; i<high; i++){
//         cout<<i<<" : ";
//         for(int j=0;j<topk;j++) cout<<rec_users[i-low][j]<<" ";
//         cout<<"\n";
//     }
    
    writefile.seekg(4*low*topk, std::ios::beg);
    for(int u=low; u<high;u++){
        writefile.write(reinterpret_cast<const char*>(&rec_users[u-low][0]), topk*sizeof(int));
    }
    
    
    
    MPI_Finalize();
    writefile.close();
    
    return 0;
}

/*
//     cout<<ep<<"\n";              
//     cout<<dim<<"\n";             
//     cout<<max_level<<"\n";       
//     for(auto x : level) cout<<x<<" ";
//     cout<<"\n";
    
//     for(auto x : level_offset) cout<< x <<" ";
//     cout<<"\n";
    
//     for(auto x : user){
//         for(int i=0;i<dim;i++) cout<<x[i]<<" ";
//         cout<<"\n";
//     }
    
//     for(auto x : indptr) cout<<x<<" ";
//     cout<<"\n";
    
//     for(auto x : index) cout<<x<<" ";
//     cout<<"\n";
    
//     for(auto x : vect){
//         for(int i=0;i<dim;i++) cout<<x[i]<<" ";
//         cout<<"\n";
//     }
*/
