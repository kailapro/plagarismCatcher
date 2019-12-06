#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <queue>
#include <cmath>
#include <algorithm>

using namespace std;

// all of this code is copied.....it is supposed to help with getting the file names from the directory
//input is a set of plain-text documents and a number n
//output is a representation showing the number of n-word sequences each document has in common with
//every other document in the set

//inputs: command line => ./plagarismCatcher path/to/files 6 200
//^^ this would return a list (in order) of all the pairs of files in path/to/docs that shared more
//than 200 6-word sequences in common


/*function... might want it in some class?*/
int getdir (string dir, vector<string> &files)
{
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL) {
        cout << "Error(" << errno << ") opening " << dir << endl;
        return errno;
    }

    while ((dirp = readdir(dp)) != NULL) {
        files.push_back(string(dirp->d_name));
    }
    closedir(dp);
    return 0;
}

int hashfunc(string str, int sizeofhashtable){
    size_t hash = 0;
    size_t base = 7;
    size_t exponent = 1;
    for(int x = 0; x<str.length(); x++){
        char c = str[x];
        hash = (hash + (int)c * exponent) % sizeofhashtable; // summation of all of them
        exponent = (exponent*base) % sizeofhashtable; // increment the exponent value
    }
    return hash;
}

struct item{
    int count;
    string filex;
    string filey;
};

bool compareByCount(const item &a, const item &b)
{
    return a.count < b.count;
}


int main(int argc, char *argv[])
{
    //there's 3 args
    // argv[1] = path/to/files
    // argv[2] = 6
    // argv[3] = 200
    string dir = argv[1];
    //cout<<"dir "<<dir<<endl;
    //string dir = string("/Users/kailaprochaska/Desktop/sanity check");
    int sequencelen = stoi(argv[2]);
//    cout<<"sequencelen "<< sequencelen<<endl;
    int threshold = stoi(argv[3]);
//    cout<<"threshold "<< threshold<<endl;
    vector<string> files = vector<string>();

    getdir(dir,files);

    //int sequencelen = 6; // change this to be from user input later

//    int threshold = 200;

    int array[files.size()][files.size()]; // a 2D array with 2d size based on the number of files we have

    // intiailiaze to 0
    for (int x = 0; x<files.size(); x++){
        for (int y = 0; y<files.size(); y++){
            array[x][y] = 0;
        }
    }

    int sizeOfHashTable = 211093;

    vector<int> hashTable[sizeOfHashTable];// creaing a hash table of vectors

    for (unsigned int i = 2;i < files.size();i++) { // iterating through all the files that we have in our small doc set
        deque <string> words;
        vector <string> sequences;
        string str;
        string filepath = dir + "/" + files[i];
        //cout<<files[i]<<endl;
        ifstream myfile (filepath);
        if(myfile.is_open()){
            while(myfile>>str){ // this is iterating through each word of the file
                //add to the queue
                words.push_back(str);
                if(words.size() == sequencelen) { //if there are 6 words in the queue
                    string combined = "";
                    //iterate through the queue and add them to this string
                    for(int y = 0; y<words.size(); y++){
                        combined += words.at(y);
                    }

                    sequences.push_back(combined); // add the string to the vector of n length sequences
                    words.pop_front();// pop the first element out

                }
            }
            // printing out all the 6 word sequences in the file
        }

        //  sequences is now populated with the chunks from this particular file
        //cout<<"======= what file we are on: "<< i<< endl;

        for(int j = 0; j<sequences.size(); j++){ // go through all the chunks stored in the vector named sequences
            string chunk = sequences.at(j);
            string cleanedchunk = "";

            for(int k = 0; k<chunk.length(); k++){ // go through characters of the chunk
                char c = chunk[k];
                int ascii = (int)c;
                if((ascii>=65 and ascii<=90) or (ascii>=97 and ascii<=122)){// it's a valid letter
                    cleanedchunk += tolower(c);
                }
            }
            // outside of the for loop - we now have the current cleanedchunk
            // find the hash key of the cleaned chunk
            int key = hashfunc(cleanedchunk, sizeOfHashTable); // so we are generating this key for each chunk
            // add it to the hash table
            if(hashTable[key].size() != 0){// its not an empty vector
                if(hashTable[key].back() != i){ // if we don't already have an instance of this chunk in the table
                    hashTable[key].push_back(i);
                }
            }else{
                hashTable[key].push_back(i);
            }
//            cout<<"chunk " << cleanedchunk<<endl;
//            cout<<"key " << key<<endl;
        }


    }
    // ========= now all of our files have been processed and the hash table has been filled ========

    // go through hash table
    int count = 0;
    for(int index = 0; index<sizeOfHashTable; index++){ // iterate through all indexes of the hash table
        // current vector = hashTable[index]
        if(hashTable[index].size() > 1){ // if there's more than one element in the vector, then we know that some collisions occurred
            //cout<<"this is the vector at index " << index << endl;
            for(int a = 0; a<hashTable[index].size()-1; a++){ // this current vector's length
                //cout<<"contents of hashtable: "<<hashTable[index].at(a)<<endl;
                for(int b = a+1; b<hashTable[index].size(); b++){
                    int filea = hashTable[index].at(a);
                    int fileb = hashTable[index].at(b);
//                    cout<<"filea "<<filea<<endl;
//                    cout<<"fileb "<<fileb<<endl;
                    array[filea][fileb] = array[filea][fileb] + 1;
//                    cout<<array[filea][fileb]<<endl;


                }
            }

        }
    }
    //cout<<"count "<<count<<endl;


    vector<item> finalprint = vector<item>(); // a vector of items ^^ each item has the count, and the two string files

    for (int x = 0; x<files.size(); x++){
        for(int y = 0; y<files.size(); y++){
//            cout<<x<<","<<y<<" "<<array[x][y]<<endl;
            if(array[x][y] > threshold){
                struct item it;
                it.count = array[x][y];
                it.filex = files.at(x);
                it.filey = files.at(y);
                finalprint.push_back(it);
                sort(finalprint.begin(), finalprint.end(),compareByCount);
            }
        }
    }
    for(int w = finalprint.size()-1; w>=0; w--){
        cout<<finalprint.at(w).count<< ": " << finalprint.at(w).filex << ", " << finalprint.at(w).filey <<endl;
    }
//    for(int l = 0; l<files.size(); l++){
//        cout<<l<<": "<<files.at(l)<<endl;
//    }


    // now the array has been updated with the number of collisions associated with the

    // first we want to get a vector of the 6 word chunks.
    // remove the punctuation and combine it all into 1 string
    // then calculate the hash key for that string and add it to the hash table with entry being the name of the file that it came from

    return 0;
}


