#include <iostream>
#include <fstream>
#include <cstdlib>
#include <climits>

using namespace std;

int getRandom(int min, int max)
{
   return (rand() % static_cast<int>(max + 1 - min)) + min;
}

int main(int argc, char * argv[])
{
   if (argc < 4) {
      cout << "Usage: generate <filename> <number_of_rows> <number_of_cols>" << endl;
      return -1;
   }
   string filename = argv[1];
   int rows = atoi(argv[2]);
   int cols = atoi(argv[3]);

   ofstream fout(filename);
   if (!fout.good()) {
      cerr << "Error while the opening the output file!" << endl;
      return -1;
   }

   srand(static_cast<unsigned int>(time(nullptr)));
   string funcs[] = {"min", "max", "avg", "p90"};
   for(int i = 0; i < rows; i++) {
      fout << funcs[getRandom(0,3)] << ":";
      for(int j = 0; j < cols; j++) {
         fout << getRandom(INT_MIN/4, INT_MAX/4);
         if(j != cols -1) {
            fout << ",";
         }
      }
      fout << endl;
   }
}
