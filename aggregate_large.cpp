#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <unordered_map>
#include <climits>
#include <chrono>

using namespace std;

enum ERROR_CODE {
   NO_ERROR,
   PARSE_ERROR_EMPTY_LINE,
   PARSE_ERROR_INVALID_NUMBER_OF_CHARACTERS,
   PARSE_ERROR_INVALID_FUNCTION_DELIMITER,
   PARSE_ERROR_INVALID_NUMBER_DELIMITER,
   PARSE_ERROR_INTEGER_OVERFLOW,
   PARSE_ERROR_INTEGER_UNDERFLOW,
   PARSE_ERROR_INVALID_INTEGER,
   PARSE_ERROR_UNSUPPORTED_FUNCTION_NAME,
};

typedef double(*funcPointer)(vector<int> &, ERROR_CODE &);
typedef unordered_map<string, funcPointer> functionMap;

/* Parsing Routines */
bool parseLine (string & line, string & function, vector<int> & args,
      functionMap & supportedFunctions, ERROR_CODE & error);
string parseFunction (const string & line, functionMap &
      supportedFunctions, ERROR_CODE & error);
vector<string> parseArguments (const string & line, ERROR_CODE & error);
int parseInteger (const char * s, ERROR_CODE & error);

/* Supported mathematical functions */
double getMax(vector<int> & args, ERROR_CODE & error);
double getMin(vector<int> & args, ERROR_CODE & error);
double getAvg(vector<int> & args, ERROR_CODE & error);
double getP90(vector<int> & args, ERROR_CODE & error);

/* Calculate the result of applying the passed function to its arguments */
double apply (functionMap & supportedFunctions, const string & function,
      vector<int> & argv, ERROR_CODE & error);

/* Helper functions */
void printError(const int lineNumber, const ERROR_CODE error);
string getErrorMessage(const ERROR_CODE error);

int main (int argc, char* argv[])
{
   typedef chrono::high_resolution_clock clock;
   typedef chrono::milliseconds milliseconds;
   clock::time_point t0 = clock::now();

   if (argc != 2) {
      cerr << "Invalid parameters" << endl;
      return -1;
   }

   string filename = argv[1];
   ifstream fin(filename.c_str());
   if (!fin) {
      cerr << "Error opening " << filename << " for reading" << endl;
      return -1;
   }

   int parseErrorCount;
   double result;
   string line, function;
   vector<int> args(1);
   ERROR_CODE error;

   unordered_map<string, funcPointer> supportedFunctions = {
      {"min", &getMin},
      {"max", &getMax},
      {"avg", &getAvg},
      {"p90", &getP90},
   };

   cout.precision(numeric_limits<double>::digits10);

   int size = 1024;
   unsigned long long fileSize, lineNumber = 0;

   //Get file size
   fin.seekg(0, ios::end);
   fileSize = fin.tellg();

   //Reset seek pointer
   fin.seekg(0, ios::beg);

   cout << "File size: " << fileSize << endl;

   do {
      vector<char> * bytes = new vector<char>(size);
      fin.read(&(*bytes)[0], size);

      for(int i = 0; i < size; i++) {
         char elem = bytes->at(i);
         if (elem == '\n') {
            //New line, clear line
            lineNumber++;
            //cout << "Line " << lineNumber << ": " << line << endl;
            cout << "Line " << lineNumber << endl;
            line.clear();
            continue;
         }
         line.push_back(elem);
      }

      delete bytes;

      if(fin.tellg() == -1) {
         break;
      }

   } while(true);


   clock::time_point t1 = clock::now();
   milliseconds ms = chrono::duration_cast<milliseconds>(t1 - t0);
   cout << "Total time is: " << ms.count() << "ms" << endl;

   return 0;
}


/**
 * Main parse routine, it will parse a line into functionName and argument list. In case of error it will
 * update the error flag
 */
bool parseLine (string & line, string & function, vector<int> & args,
      functionMap & supportedFunctions, ERROR_CODE & error)
{
   if (line.empty()) {
      error = PARSE_ERROR_EMPTY_LINE;
      return false;
   }

   //Check if it least we have 5 characters, such as max:3
   if (line.size() < 5) {
      error = PARSE_ERROR_INVALID_NUMBER_OF_CHARACTERS;
      return false;
   }

   //Clear arguments, in case it was populated in the pervious call
   args.erase(args.begin(), args.end());

   //Get function name
   function = parseFunction(line, supportedFunctions, error);
   if (function.empty()) {
      return false;
   }

   //Remove the function & its delimiter
   line.erase(0, 4);

   //Get arguments
   vector<string> argsList = parseArguments(line, error);
   if(argsList.empty()) {
      return false;
   }

   //Convert argment list from string to integer
   int num = 0;
   for (auto item: argsList) {
      num = parseInteger(item.c_str(), error);
      if (error != NO_ERROR) {
         return false;
      }
      args.push_back(num);
   }

   return true;
}

/**
 * This function will parse the line by reading the first 4 characters
 * to deterimne the function name. It will assume function delimiter is ':'
 * such as min:23,23
 */
string parseFunction (const string & line, functionMap &
      supportedFunctions, ERROR_CODE & error)
{
   //Assumption for our current app, could be passed as arguments if needed later
   unsigned short nameLength = 3;
   char expectedDelimiter = ':';

   //Assume function length is 3, such as "min:", then delimiter will be at position '3'
   char actualDelimiter = line.at(nameLength);
   if (actualDelimiter != expectedDelimiter) {
      error = PARSE_ERROR_INVALID_FUNCTION_DELIMITER;
      return "";
   }

   error = NO_ERROR;
   string function = line.substr(0, nameLength);
   if (supportedFunctions.find(function) == supportedFunctions.end()) {
      error = PARSE_ERROR_UNSUPPORTED_FUNCTION_NAME;
      return "";
   }

   return function;
}

/**
 * This function will parse a line starting by number. It will assume comma
 * separated items and parse them into list such as 3,12,2543
 */
vector<string> parseArguments (const string & line, ERROR_CODE & error)
{
   vector<string> args;
   string delimiter = ",";
   unsigned int start = 0;

   auto pos = line.find_first_of(delimiter, start);
   while(pos != string::npos) {
      if(pos != start)
         args.push_back(line.substr(start, pos - start));
      start = pos + 1;
      pos = line.find_first_of(delimiter, start);
   }

   if(start < line.length())
      args.push_back(line.substr(start));

   if (args.empty()) {
      error = PARSE_ERROR_INVALID_NUMBER_DELIMITER;
   }

   return args;
}

/**
 * This function will parse string into integer using strtol, it will return
 * the correct error code in case of overflow or conversion error
 */
int parseInteger (const char * str, ERROR_CODE & error)
{
   char * end;
   long num;
   errno = 0;
   num = strtol(str, &end, 10);

   if ((errno == ERANGE && num == LONG_MAX) || num > INT_MAX) {
      error = PARSE_ERROR_INTEGER_OVERFLOW;
      return 0;
   }

   if ((errno == ERANGE && num == LONG_MIN) || num < INT_MIN) {
      error = PARSE_ERROR_INTEGER_UNDERFLOW;
      return 0;
   }

   if(*str == '\0' || *end != '\0') {
      error = PARSE_ERROR_INVALID_INTEGER;
      return 0;
   }

   error = NO_ERROR;
   return num;
}

double apply (functionMap & supportedFunctions, const string & function,
      vector<int> & argv, ERROR_CODE & error)
{
   return (*supportedFunctions.at(function))(argv, error);
}

double getMax (vector<int> & argv, ERROR_CODE & error)
{
   vector<int>::iterator result;
   result = max_element(argv.begin(), argv.end());
   return *result;
}

double getMin (vector<int> & argv, ERROR_CODE & error)
{
   vector<int>::iterator result;
   result = min_element(argv.begin(), argv.end());
   return *result;
}

double getAvg (vector<int> & argv, ERROR_CODE & error)
{
   double avg = argv.at(0);
   argv.erase(argv.begin());

   for(auto num: argv)
      avg = avg/2.0 + num/2.0;

   return avg;
}

double getP90 (vector<int> & argv, ERROR_CODE & error)
{
   sort(argv.begin(), argv.end());
   int i = 0.9 * argv.size() - 1;
   return argv[i];
}

void printError(const int lineNumber, const ERROR_CODE error)
{
   cout << "Unable to parse line: " << lineNumber << ", because: " << getErrorMessage(error) << endl;
}

string getErrorMessage(ERROR_CODE error)
{
   string msg;

   switch(error)
   {
      case NO_ERROR:
         msg = "no errors!";
         break;
      case PARSE_ERROR_INVALID_NUMBER_OF_CHARACTERS:
         msg = "invalid number of characters";
         break;
      case PARSE_ERROR_EMPTY_LINE:
         msg = "line is empty";
         break;
      case PARSE_ERROR_INVALID_FUNCTION_DELIMITER:
         msg = "function name has invalid delimiter";
         break;
      case PARSE_ERROR_INVALID_NUMBER_DELIMITER:
         msg = "required tokens not found";
         break;
      case PARSE_ERROR_INTEGER_OVERFLOW:
         msg = "integer overflow happened while parsing the number";
         break;
      case PARSE_ERROR_INTEGER_UNDERFLOW:
         msg = "integer underflow happened while parsing the number";
         break;
      case PARSE_ERROR_INVALID_INTEGER:
         msg = "can't parse string to integer";
         break;
      case PARSE_ERROR_UNSUPPORTED_FUNCTION_NAME:
         msg = "unsupported function name";
         break;
      default:
         msg = "unknown error";
         break;
   }

   return msg;
}

