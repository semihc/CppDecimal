#include "DTRunner.hh"
#include <string>
#include <regex>
#include <iostream>
#include <fstream>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;


// Windows
auto path_w = fs::path{"C:\\Users\\Marius\\Documents"};
// POSIX
auto path_p = fs::path{ "/home/marius/docs" };

auto dectests_path{ R"(C:\opt\CPP\CppDecimal\tst\dectests)"};




// https://stackoverflow.com/questions/216823/how-to-trim-an-stdstring
#include <algorithm> 
#include <cctype>
#include <locale>

// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    rtrim(s);
    ltrim(s);
}

// trim from start (copying)
static inline std::string ltrim_copy(std::string s) {
    ltrim(s);
    return s;
}

// trim from end (copying)
static inline std::string rtrim_copy(std::string s) {
    rtrim(s);
    return s;
}

// trim from both ends (copying)
static inline std::string trim_copy(std::string s) {
    trim(s);
    return s;
}




void visit_directory1(fs::path const & dir)
{
  if (fs::exists(dir) && fs::is_directory(dir))
  {
    for (auto const & entry : fs::directory_iterator(dir))
    {
      auto filename = entry.path().filename();
      if (fs::is_directory(entry.status()))
        std::cout << "[+]" << filename << '\n';
      else if (fs::is_symlink(entry.status()))
        std::cout << "[>]" << filename << '\n';
      else if (fs::is_regular_file(entry.status()))
        std::cout << " " << filename << '\n';
      else
        std::cout << "[?]" << filename << '\n';
    }
  }
}

void visit_directory_rec(fs::path const & dir)
{
  if (fs::exists(dir) && fs::is_directory(dir))
  {
    for (auto const & entry :
         fs::recursive_directory_iterator(dir))
    {
      auto filename = entry.path().filename();
      if (fs::is_directory(entry.status()))
        std::cout << "[+]" << filename << '\n';
      else if (fs::is_symlink(entry.status()))
        std::cout << "[>]" << filename << '\n';
      else if (fs::is_regular_file(entry.status()))
        std::cout << " " << filename << '\n';
      else
        std::cout << "[?]" << filename << '\n';
    }
  }
}

void visit_directory(
  fs::path const & dir,
  bool const recursive = false,
  unsigned int const level = 0)
{
  if (fs::exists(dir) && fs::is_directory(dir))
  {
    auto lead = std::string(level*3, ' ');
    for (auto const & entry : fs::directory_iterator(dir))
    {
      auto filename = entry.path().filename();
      if (fs::is_directory(entry.status()))
      {
        std::cout << lead << "[+]" << filename << '\n';
        if(recursive)
          visit_directory(entry, recursive, level+1);
      }
      else if (fs::is_symlink(entry.status()))
        std::cout << lead << "[>]" << filename << '\n';
      else if (fs::is_regular_file(entry.status()))
        std::cout << lead << " " << filename << '\n';
      else
        std::cout << lead << "[?]" << filename << '\n';
    }
  }
}


//
// Regex definitions for dectest file contents
//
regex rxempty {R"(^\s*$)"};
regex rxcomment {R"(--.*$)"};
regex rxcmntline {R"(^(.*)(--.*)$)"};
// Directive
// keyword : value
regex rxdirective {R"(\s*(\w+)\s*:\s*(\S+)\s*$)"};
// Test case
// id operation operand1 [operand2 operand3] -> result [conditions]
regex rxtestcase {R"(\s*(\w+)\s*(\w+)\s*(\S+)\s*(\S+)?\s*(\S+)?\s*->\s*(\S+)\s*(\S+)?)"};




int procTestCaseLine(string& line)
{
  cout << line << endl;
  smatch match;  
  int rv = 0;


  if(line.find("--")>=0)
    line = regex_replace(line, rxcomment, "");
  /*-
  if(regex_search(line, match, rxcmntline)) {
    //-cout << '0' << match.str(0) << endl;
    //-cout << '1' << match.str(1) << endl;
    line = match.str(1);
    cout << "COMMENTED LINE: " << endl;
    // Continue with section of line stripped out of comments
    // Only if line has data in it
  }
  */

  
  trim(line);
  if( line.empty() ) {
    cout << "EMPTY LINE" << endl;
    return rv;
  }
  /*
  if(regex_search(line, match, rxempty)) {
    cout << "EMPTY LINE" << endl;
    return rv;
  }
  */
  if(line.find(":", 1) > 0 && regex_search(line, match, rxdirective)) {
    cout << "DIRECTIVE LINE: (" << match.str(1) << ':' << match.str(2) << ')' << endl;
    return rv;
  }
  if(line.find("->", 1) > 0 && regex_search(line, match, rxtestcase)) {
    cout << "TESTCASE LINE: (" 
      << match.str(1) << ','
      << match.str(2) << ','
      << match.str(3) << ','
      << match.str(4) << ','      
      << match.str(5)
      << "->"
      << match.str(6) << ','
      << match.str(7)
      << ")" << endl;
    return rv;
  }

  rv = 1;
  cout << "UNRECOGNIZED LINE" << endl;

  return rv;
}


int processDecTestFile(string& fileName)
{
  string line;
  ifstream ifile(fileName);
  int rv = 1;

  if(ifile.is_open()) {
    while(getline(ifile, line)) {
      rv = procTestCaseLine(line);
      if(rv) break;
    } // while
    ifile.close();
  }

  return rv;
}






int testProcessDecTestFile()
{
  fs::path dectestFN = dectests_path ;
  dectestFN /= "dectest_sub";
  //dectestFN /= "testall0.decTest";
  dectestFN /= "trim0.decTest";

  string dtfn = dectestFN.string();
  cout << dtfn << endl;

  return processDecTestFile(dtfn);
}

int testVisitDirectory()
{
  visit_directory(dectests_path, true, 2);
  return 0;
}


