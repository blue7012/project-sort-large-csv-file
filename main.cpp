#include <iostream>
#include <list>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <queue>
using namespace std;

class Compare
{
public:
    //Ascending order
    bool operator() (pair< vector<string>, int> pair1, pair< vector<string>, int> pair2)
    {
        alphabeticallyOrder(pair1.first[0], pair2.first[0]);
        return pair1.first > pair2.first;
    }
};
string ToString(int val) //Convert int to string
{
    stringstream stream;
    stream << val;
    return stream.str();
}
bool checkNumber(string a); //Check if a string contains all numeric characters.
bool alphabeticallyOrder(string a, string b); //Check if two strings are alphabetically ordered
void MergeSortA(int low, int high); 
void MergeA(int low, int mid, int high);
void SortnWrite(int numberOfchunnk);
void MergeFile(int counter);

vector<vector<string>> content;
int main()
{
    int chunkSize = 107374182/4; //Max size for 1 file csv
    int TotalByte = 0; //Actually it is a length of string
    int numberOfchunk = 1;
    bool unprocessedData = true; //Check if data had been written or not
    string fname = "Books_rating.csv"; //file path
    //Get a line of file and push to vector 2D content[row][column]
    //if TotalByte >= chunkSize then write content to file and reset variables
    vector<string> row;
    string line, word;
    fstream file(fname, ios_base::in | ios_base::binary);
    if (file.is_open())
    {
       getline(file, line); //Remove from taking header data;
       while (getline(file, line))
       {
           unprocessedData = true;
           row.clear();
           stringstream str(line);
           while (getline(str, word, ','))
           {
               row.push_back(word);
               TotalByte += row.size();
           }
           content.push_back(row);
           if (TotalByte >= chunkSize)
           {
               SortnWrite(numberOfchunk);
               //Clean up and reset
               for (int i = 0; i < content.size(); i++)
                   content[i].clear();
               content.clear();
               TotalByte = 0;
               numberOfchunk++;
               unprocessedData = false;
           }
       }
    }
    else
       cout << "Could not open the file\n";
    //if still have data in content
    if (unprocessedData)
    {
       SortnWrite(numberOfchunk);
       for (int i = 0; i < content.size(); i++)
            content[i].clear();
        content.clear();
    }
    else
    {
       numberOfchunk--;
    }

    MergeFile(numberOfchunk);
    return 0;
}

void SortnWrite(int numberOfchunk)
{
    MergeSortA(0, content.size() - 1);
    string outputFileName = "output" + ToString(numberOfchunk) + ".csv"; //name file
    ofstream myfile;
    myfile.open(outputFileName.c_str());
    //write content to file
    for (int i = 0; i < content.size(); i++)
    {
        for (int j = 0; j < content[i].size(); j++)
        {
            myfile << content[i][j] << ",";
        }
        myfile << "\n";
    }
    myfile.close();
}

void MergeSortA(int low, int high)
{
    int mid = 0;
    if (low < high)
    {
        mid = ((low + high) / 2);
        MergeSortA(low, mid);
        MergeSortA(mid + 1, high);
        MergeA(low, mid, high);
    }
}
void MergeA(int low, int mid, int high)
{
    int i = low, j = mid + 1, k = low;

    vector<vector<string>> Temp;

    while (i <= mid && j <= high)
    {
        if (alphabeticallyOrder(content[i][0], content[j][0]))
        {
            Temp.push_back(content[i]);
            i++;
        }
        else
        {
            Temp.push_back(content[j]);
            j++;
        }
        k++;
    }
    if (i > mid)
    {
        for (int h = j; h <= high; h++)
        {
            Temp.push_back(content[h]);
            k++;
        }
    }
    else
    {
        for (int h = i; h <= mid; h++)
        {
            Temp.push_back(content[h]);
            k++;
        }
    }
    int count = 0;
    for (int i = low; i <= high; i++) //swap data from Temp to content
    {
        swap(Temp[count], content[i]);
        count++;
    }
}

bool checkNumber(string a)
{
    for (int i = 0; i < a.length(); i++)
    {
        if (a[i] > 57 || a[i] < 48)
        {
            return false;
        }
    }
    return true;
}

bool alphabeticallyOrder(string a, string b)
{
    if (a.length() > b.length())
    {
        return false;
    }
    else
    {
        if (checkNumber(a) == true && checkNumber(b) == true)
        {
            if (stoull(a) > stoull(b))
            {
                return false;
            }
            else
            {
                return true;
            }
        }
        if (checkNumber(a) == false || checkNumber(b) == false)
        {
            if (checkNumber(b) == true)
            {
                return false;
            }
            if (checkNumber(a) == true)
            {
                return true;
            }
        }
        if (checkNumber(a) == false && checkNumber(b) == false)
        {
            if (lexicographical_compare(a.begin(), a.end(), b.begin(), b.end()) == false)
            {
                return false;
            }
            else
            {
                return true;
            }
        }
    }
    return true;
}

void MergeFile(int counter)
{
    //Create a priority queue to push smallest data to top of queue
    //   A data in queue will contain a pair data:
    //        vector string contain data in file
    //        int contain file order
    priority_queue<pair< vector<string>, int>, std::vector<pair< vector<string>, int> >, Compare> minHeap;

    ifstream* handles = new ifstream[counter];
    int numFile = 0;
    //Take first value in each file (minimum in the file) and push to minHeap
    for (int i = 1; i <= counter; i++)
    {
        string sortedInputFileName = "output" + ToString(i) + ".csv";
        handles[i - 1].open(sortedInputFileName.c_str(),ios_base::binary);

        vector<string> firstValue;
        string line,word;
        getline(handles[numFile], line);
        stringstream str(line);
        while (getline(str, word, ','))
        {
            firstValue.push_back(word);
        }
        numFile++;

        minHeap.push(pair< vector<string>, int>(firstValue, i - 1));
    }

    string outputFileName = "sorted_books_rating.csv";
    ofstream outputFile(outputFileName.c_str());
    outputFile << "Id,Title,Price,User_id,profileName,review/helpfulness,review/score,review/time,review/summary,review/text\n"; //Add back header data
    //Take a top value in minHeap and write to output file
    //  then get next value in the file that top value was in 
    while (minHeap.size() > 0)
    {
        pair< vector<string>, int> minPair = minHeap.top();
        minHeap.pop();


        for (int i = 0; i < minPair.first.size(); i++)
        {
            outputFile << minPair.first[i];
            if(i < minPair.first.size() - 1)
                outputFile << ",";
        }

        vector<string> nextValue;
        string line, word;
        getline(handles[minPair.second], line);
        stringstream str(line);
        while (getline(str, word, ','))
        {
            nextValue.push_back(word);
        }

        if (nextValue.size() > 0)
        {
            minHeap.push(pair < vector<string>, int>(nextValue, minPair.second));
        }

    }
}

