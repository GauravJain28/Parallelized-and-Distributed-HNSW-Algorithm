# DataSetup.sh
# make output path directory if doesn't exist
if [ ! -d "$2" ]
then
    mkdir $2
fi

# copy files from input path to output path
cp $1/ep.txt $2/ep.txt
cp $1/indptr.txt $2/indptr.txt
cp $1/level.txt $2/level.txt
cp $1/level_offset.txt $2/level_offset.txt
cp $1/max_level.txt $2/max_level.txt

# run the data.cpp file for conversion to bin file
./dat $1 $2