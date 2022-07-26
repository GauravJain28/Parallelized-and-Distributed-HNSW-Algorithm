#compile data.cpp file for file conversion
g++ -std=c++17 -o dat data.cpp
#compile main.cpp
mpic++ -std=c++17 -fopenmp -o mn main.cpp
#convert bin to txt
g++ -std=c++17 -o sv save.cpp