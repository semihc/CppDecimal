#include "DTRunner.hh"
// Std includes
#include <string>
#include <regex>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <vector>
#include <stack>
#include <unordered_set>
#include <unordered_map>
#include <cctype>
#include <locale>
#include <algorithm>
#include <source_location>
// Prj includes
#include <gtest/gtest.h>
#include "absl/log/log.h"
#include "absl/flags/flag.h"
#include "absl/flags/declare.h"
#include <spdlog/spdlog.h>

#include "DecContext.hh"
#include "DecNumber.hh"
#include "DecSingle.hh"
#include "DecDouble.hh"
#include "DecQuad.hh"


using namespace std;
namespace fs = std::filesystem;
using namespace dec;


// Enumerated values for spdlog levels
enum level_enum_local : int
{
    trace = SPDLOG_LEVEL_TRACE,
    debug = SPDLOG_LEVEL_DEBUG,
    info = SPDLOG_LEVEL_INFO,
    warn = SPDLOG_LEVEL_WARN,
    err = SPDLOG_LEVEL_ERROR,
    critical = SPDLOG_LEVEL_CRITICAL,
    off = SPDLOG_LEVEL_OFF,
    n_levels
};


// Utility class to allow stream inserters on spdlog
struct slog : public std::stringstream
{
  spdlog::level::level_enum m_lvl;
  explicit slog(int lvl) : m_lvl(static_cast<spdlog::level::level_enum>(lvl)) {}

  ~slog() { spdlog::log(m_lvl, this->str()); }

  private:
    slog();
};


// Current source location
const source_location Loc = source_location::current();

// Path/directory that DecTest files reside
fs::path DecTestsDir = fs::path(Loc.file_name()).remove_filename();

ABSL_DECLARE_FLAG(int, debug);
int Dbg = absl::GetFlag(FLAGS_debug);

ABSL_FLAG(std::string, dectests_dir, "", "dectests directory");
ABSL_FLAG(std::string, dectest_file, "", "dectest file");
ABSL_FLAG(bool, extended, false, "Use extended tests");


// https://stackoverflow.com/questions/216823/how-to-trim-an-stdstring
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

static void lowerCase1(string& s) {
  std::transform(s.begin(), s.end(), s.begin(),
                  [](unsigned char c){ return std::tolower(c); });
}

static void lowerCase(string& s)
{
  for(auto& c : s)
    c = tolower(c);
}

bool iequals(const string& a, const string& b)
{
    return std::equal(a.begin(), a.end(),
                      b.begin(), b.end(),
                      [](char a, char b) {
                          return tolower(a) == tolower(b);
                      });
}


// https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
std::vector<std::string> split (const std::string &s, char delim) {
    std::vector<std::string> result;
    std::stringstream ss (s);
    std::string item;

    while (getline (ss, item, delim)) {
        result.push_back (item);
    }

    return result;
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
      string filename = entry.path().filename().string();
      if (fs::is_directory(entry.status()))
      {
        spdlog::info("{} [+] {}", lead, filename);
        if(recursive)
          visit_directory(entry, recursive, level+1);
      }
      else if (fs::is_symlink(entry.status()))
        spdlog::info("{} [>] {}", lead, filename);
      else if (fs::is_regular_file(entry.status()))
        spdlog::info("{} {}", lead, filename);
      else
        spdlog::info("{} [?] {}", lead, filename);
    }
  }
}



//
// Regex definitions for dectest file contents
//
regex rxempty {R"(^\s*$)"};
regex rxcomment {R"(--.*)"};
regex rxcmntline {R"(^(.*)(--.*)$)"};
// Directive
// keyword : value
regex rxdirective {R"(\s*(\w+)\s*:\s*(\S+)\s*$)"};
// Test case
// id operation operand1 [operand2 operand3] -> result [conditions]
// Following regex is abandoned to simplify
//regex rxtestcase {R"(\s*(\S+)\s*(\S+)\s*(\S+)\s*(\S+)?\s*(\S+)?\s*->\s*(\S+)\s*(\S+)?)"};
regex rxtestcase {R"(\s*(\w+)\s*(\w+)\s*(\S+|'\w+')\s*(\S*)\s*(\S*)\s*->\s*(\S+)\s*(.*))"};
//                      (id )   (op )   (op1)         (op2)   (op3)   ->   (res)   (cnd)
regex rxtestcase_lhs {R"(\s*(\w+)\s*(\S+)\s*('[^']*'|\S+)\s*(\S*)\s*(\S*))"};
regex rxtestcase_rhs {R"(->\s*(\S+)\s*(.*))"};




// Global DecContext
static thread_local DecContext Ctx(DEC_INIT_DECIMAL128);
static thread_local stack<DecContext> CtxStack;

// Global Directives Map
static thread_local unordered_map<string, string> DirvMap; 


// Forward declarations
int processDecTestFile(string& fileName);

// Initiase the set of test cases to be skipped
unordered_set<string> SkipSet {
  // Invalid operations due to restrictions, or other limitations
  "basx559", "basx716", "basx720", "basx724", "basx744",
  "addx1130", "cotx9990", "cotx9991",
  "ctmx9990", "ctmx9991",
  "expx260", "expx901", "expx902",
  "fmax2784", "fmax2785",
  "lnx116", "lnx732", "lnx901", "lnx902",
  "logx901", "logx902",
  "maxx900", "maxx901",
  "mxgx900", "mxgx901",
  "powx4001", "powx4002", "powx4302", "powx4303", "powx4342", "powx4343",
  "dsbas559", "ddbas559", "dqbas559"
};


int applyTestDirective(string& dir, string& val, DecContext& ctx)
{
  int rv = -1; // Return value error by default

  // Flags required to construct context object
  bool ok = true;

  lowerCase(dir); // Convert to lower case for easy comparison

  // Clear any status value left before
  ctx.zeroStatus();
  
  //
  // Mandatory directives
  //
  if(dir == "precision") {
    // No operation to be done on context
    unsigned pval = std::stoul(val);
    ok = (pval > 0);
    // Check if conversion is ok
    if(ok) {
      rv = 0;
      if(pval < static_cast<unsigned>(DecNumDigits))
        ctx.setDigits(pval);
    }
    else {
      slog(err) << "Precison value conversion failed: " << val;
    }    
  }
  else if(dir == "rounding") {
    rv = 0; // Assume success
    if(val == "ceiling") {
      ctx.setRound(DEC_ROUND_CEILING);
    }
    else if(val == "down") {
      ctx.setRound(DEC_ROUND_DOWN);
    }
    else if(val == "floor") {
      ctx.setRound(DEC_ROUND_FLOOR);
    }
    else if(val == "half_down") {
      ctx.setRound(DEC_ROUND_HALF_DOWN);
    }
    else if(val == "half_even") {
      ctx.setRound(DEC_ROUND_HALF_EVEN);
    }
    else if(val == "half_up") {
      ctx.setRound(DEC_ROUND_HALF_UP);
    }
    else if(val == "up") {
      ctx.setRound(DEC_ROUND_UP);
    }
    else if(val == "05up") {
      ctx.setRound(DEC_ROUND_05UP);
    }
    else {
      rv = -1;
      slog(err) << "Unknown value for rounding: " << val;
    }
  }
  else if(iequals(dir, "maxexponent")) {
    int32_t emax = (int32_t)std::stoi(val);
    if(ok) {
      rv = 0;
      ctx.setEmax(emax);
    }
    else {
      slog(err) << "Unrecognized maxexponent: " << val;
    }
  }
  else if(dir == "minexponent") {
    int32_t emin = (int32_t)std::stoi(val);
    if(ok) {
      rv = 0;
      ctx.setEmin(emin);
    }
    else {
      slog(err) << "Unrecognized minexponent: " << val;
    }
  }

  //
  // Optional directives
  //
  if(dir == "version") {
    // No operation for version, just store it
    rv = 0;
  }
  else if(dir == "extended") {
    uint8_t ext = static_cast<uint8_t>(std::stoi(val));
    if(ok) {
      rv = 0;
      ctx.setExtended(ext); 
    }
    else
      slog(err) << "Unrecognized extended: " << val;
  }
  else if(dir == "clamp") {
    uint8_t clp = static_cast<uint8_t>(std::stoi(val));
    if(ok) {
      rv = 0;
      ctx.setClamp(clp); 
    }
    else
      clog << "Unrecognized clamp: " << val;

  }
  else if(dir == "dectest") {
    // Process the test cases in a file
    fs::path dectestFN = DecTestsDir;
    dectestFN /= val ;
    string dtfn = dectestFN.string();
    string dtext = ".decTest";
    if(dtfn.find(dtext) == string::npos)
      dtfn += dtext; // if test file extension is not specifed, add it
    rv = processDecTestFile(dtfn);
    if(rv) slog(err) << "Unable to process decTest file: " <<  dtfn;
  }


  // Check if dir/value pair is recognized
  if(rv != 0)
    clog << "Unknown directive " << dir << endl;
  else {
    // If successful parsing occurred, then register dir/val pair
    auto it = DirvMap.find(dir);
    if(it != DirvMap.end())
      it->second = val;
    else
      DirvMap.insert({dir, val});
    slog(debug) << "dir=" << dir << " val=" << val << " ctx=" << ctx;
  }

  return rv;
}





// Perform (do) a DecNumber operation
DecNumber opDo(const string& op,
               DecNumber& n1, DecNumber& n2, DecNumber& n3,
               DecContext& c, string& rs)
{

  //
  // Unary operations
  //
  if("abs" == op)
    return n1.abs(&c);
  if("apply" == op)
    return n1.plus(&c);
  // canonical is similar to apply
  if("canonical" == op)
    return n1.plus(&c);
  if("class" == op) {
    enum decClass dc = n1.toClass(&c);
    rs = n1.ClassToString(dc);
    return n1;
  }
  // Copy operation modifies the callee, thus operation
  // is done on unused operand
  if("copy" == op) 
    return n2.copy(n1);
  if("copyabs" == op)
    return n2.copyAbs(n1);
  if("copynegate" == op)
    return n2.copyNegate(n1);
  if("copysign" == op)
    return n3.copySign(n1,n2);
  if("exp" == op)
    return n1.exp(&c);
  if("invert" == op)
    return n1.invert(&c);
  if("ln" == op)
    return n1.ln(&c);
  if("log10" == op)
    return n1.log10(&c);
  if("logb" == op)
    return n1.logB(&c);
  if("minus" == op)
    return n1.minus(&c);
  if("nextminus" == op)
    return n1.nextMinus(&c);
  if("nextplus" == op)
    return n1.nextPlus(&c);
  if("plus" == op)
    return n1.plus(&c);
  if("reduce" == op)
    return n1.reduce(&c);
  if("squareroot" == op)
    return n1.squareRoot(&c);
  if("tosci" == op) {
    rs = n1.toString(); //-.data();
    return n1;
  }
  if("toeng" == op) {
    rs = n1.toEngString(); //-.data();
    return n1;
  }
  if("tointegral" == op)
    return n1.toIntegralValue(&c);
  if("tointegralx" == op)
    return n1.toIntegralExact(&c);
  if("trim" == op)
    return n1.trim();
  
  //
  // Binary operations
  //
  if("add" == op)
    return n1.add(n2, &c);
  if("and" == op)
    return n1.digitAnd(n2, &c);
  if("compare" == op)
    return n1.compare(n2, &c);
  if("comparesig" == op)
    return n1.compareSignal(n2, &c);  
  if("comparetotal" == op)
    return n1.compareTotal(n2, &c);
  if("comparetotalmag" == op ||
     "comparetotmag" == op)
    return n1.compareTotalMag(n2, &c);
  if("divide" == op)
    return n1.divide(n2, &c);
  if("divideint" == op)
    return n1.divideInteger(n2, &c);
  if("max" == op)
    return n1.max(n2, &c);
  if("min" == op)
    return n1.min(n2, &c);
  if("maxmag" == op)
    return n1.maxMag(n2, &c);
  if("minmag" == op)
    return n1.minMag(n2, &c);
  if("multiply" == op)
    return n1.multiply(n2, &c);
  if("nexttoward" == op)
    return n1.nextToward(n2, &c);
  if("or" == op)
    return n1.digitOr(n2, &c);
  if("power" == op)
    return n1.power(n2, &c);
  if("quantize" == op)
    return n1.quantize(n2, &c);
  if("remainder" == op)
    return n1.remainder(n2, &c);
  if("remaindernear" == op)
    return n1.remainderNear(n2, &c);
  if("rescale" == op)
    return n1.rescale(n2, &c);
  if("rotate" == op)
    return n1.rotate(n2, &c);
  if("samequantum" == op) {
    if(n1.sameQuantum(n2)) return "1";
    else return "0";
  }
  if("scaleb" == op)
    return n1.scaleB(n2, &c);
  if("shift" == op)
    return n1.shift(n2, &c);
  if("subtract" == op)
    return n1.subtract(n2, &c);
  if("xor" == op)
    return n1.digitXor(n2, &c);

  //
  // Ternary operations
  //
  if("fma" == op)
    return n1.fma(n2, n3, &c);

  
  slog(err) << "Unrecognized operation: " << op ;
  return DecNumber{};
}



bool token2DecNumber(const string& token, DecContext& ctx, DecNumber& num)
{
  string tt = token;
  
  // Remote quotes
  char sq = '\''; // Single quote
  char dq = '\"'; // Double quote

  if(tt.find(sq) != string::npos)
    std::erase(tt, sq); 

  if(tt.find(dq) != string::npos)
    std::erase(tt, dq); 
  
  DecContext& c = ctx;
  //clog << "token=" << token << " tt=" << tt << endl;

  
  if(token.find('#') != string::npos) {
    static regex expl{"#([0-9a-fA-F]+)"}; // explicit notation
    static regex altn{"([0-9]+)#(.+)"};   // alternative notation
    smatch match;

    if(regex_match(token, match, expl)) {
      string hexval = match.str(1); // get hex value
      switch(hexval.size()) {
        case 8: {
          DecSingle ds;
          ds.fromHexString(hexval.data());
          num = ds.toDecNumber();
          return true;
        }      
        case 16: {
          DecDouble dd;
          dd.fromHexString(hexval.data());
          num = dd.toDecNumber();
          return true;
        }
        case 32: {
          DecQuad dq;
          dq.fromHexString(hexval.data());
          num = dq.toDecNumber();
          return true;
        }
      } // end switch
    } // expl.

    if(regex_match(token, match, altn)) {
      string fmt = match.str(1); // get format size
      string val = match.str(2); // get number value in string

      int fmtsize = stoi(fmt.data());
      switch(fmtsize) {
        case 32: {
          clog << "fmt=" << fmt << "val=" << val;
          DecSingle ds(val.data());
          num = ds.toDecNumber();
          return true;
        }

        case 64: {
          clog << "fmt=" << fmt << "val=" << val;
          DecDouble dd(val.data());
          num = dd.toDecNumber();
          return true;
        }

        case 128: {
          clog << "fmt=" << fmt << "val=" << val;
          DecQuad dq(val.data());
          num = dq.toDecNumber();
          return true;
        }

      } // end switch

    } // altn.

    
    // '#' in a token by itself
    num.fromString(tt, &c);
    return true;
      
  } // contains #

  if(tt.find('?') != string::npos) {  
    // ? in a token by itself
    num.fromString(tt, &c);
    return true;
  }

  // Anything else
  DecNumber tnum;
  tnum.fromString(tt.data(), &c);
  num = tnum;

  //TODO: Check if warning is necessary
  if(c.getStatus()) {
    slog(warn) 
         << "token2DecNumber"
         << " tkn=" << token
         //<< " ctx=" << c.statusToString()
         << " ctx_sts=" << c.getStatus()
         << ' ' << c.statusFlags()
         << " val=" << tnum.toString();

  }
  
  return true;
}


bool DecNumber2token(string& token, const DecNumber& num)
{
  string numstr = num.toString();
  token = numstr;
  return true;
}



int getDirectivesContext(DecContext& ctx, bool precision)
{
  return 0;
}



void displayDirectivesContext()
{
  slog(info) << "Directives in effect: ";
  for( auto& mi : DirvMap) 
    slog(info) << '\t' << mi.first << ':' << mi.second ;
}



int applyTestCase(string& tc_id, 
                  string& tc_op, 
                  string& tc_a1,
                  string& tc_a2,
                  string& tc_a3,
                  string& tc_rv,
                  string& tc_cd,
                  DecContext& ctx)
{
  int rv = -1; // Return value error by default
  bool ret = false;

  // Check if the test case is to be skipped
  if(SkipSet.find(tc_id) != SkipSet.end()) {
    slog(info) << "IGNORE: " << tc_id ;
    return 0;
  }

  lowerCase(tc_op); // Normalize the operation to lowercases
  
  DecNumber n1,n2,n3,e;
  // Conversion Context - needs high precision
  DecContext cc(DEC_INIT_DECIMAL128);
  uint32_t cc_sts; // Accumulated CC status code
  // Operation Context
  DecContext oc(DEC_INIT_DECIMAL128);
  uint32_t oc_sts; // Accumulated OC status code

  string erv = tc_rv; // Expected result value
  string rs; // Result String
  bool op_precision_needed = false;
  bool is_rs_used = false; // Is result string used?

 
  // Apply current context to operands now
  if(tc_op=="tosci" ||
     tc_op=="toeng" ||
     tc_op=="apply") {
    op_precision_needed = true;
    is_rs_used = true;
    if(tc_op=="apply") is_rs_used = false;
    std::erase(erv, '\''); 
  }

  // Expected result will get maximum allowable precision
  cc = ctx;
  // If precision is not wanted, pick largest exponent values
  // to avoid rounding
  if(!op_precision_needed) {
      cc.setDigits(DecNumDigits);
      cc.setEmax(DecMaxExponent); 
      cc.setEmin(DecMinExponent); 
  }

  // Expected result should not be affected by current context
  DecContext evc(DEC_INIT_DECIMAL128); // Expected value context
  evc.setDigits(DecNumDigits);
  evc.setEmax(DecMaxExponent); 
  evc.setEmin(DecMinExponent); 
  ret = token2DecNumber(tc_rv, evc, e); // Expected result


  cc.zeroStatus(); // Clear status flag for next operation
  cc_sts = 0;


  getDirectivesContext(cc, op_precision_needed);

  ret = token2DecNumber(tc_a1, cc, n1);
  //cc.zeroStatus(); // Clear status flag for next operation
  cc_sts = cc.getStatus();
  if(!tc_a2.empty()) {
    ret = token2DecNumber(tc_a2, cc, n2);
    //cc.zeroStatus(); // Clear status flag for next operation
    cc_sts |= cc.getStatus();
  }
  if(!tc_a3.empty()) {
    ret = token2DecNumber(tc_a3, cc, n3);
    //cc.zeroStatus(); // Clear status flag for next operation
    cc_sts |= cc.getStatus();
  }

  // Get context directives including precision
  getDirectivesContext(oc, true);
  oc = ctx; // Get current global context by copying
  oc.zeroStatus(); // Clear status flag for next operation
  //-clog << "oc: " << oc << endl;

  // Perform the operation, obtain the result
  DecNumber r = opDo(tc_op,n1,n2,n3,oc,rs);
  
  if(tc_rv=="?") { 
    // Result is undefined, therefore is true by default
    ret = true;
  }
  else {
    if(tc_op == "tosci" ||
       tc_op == "toeng" ||
       tc_op == "class" ) {
      // Test the expected result and actual result
      //-ret = (0==tc_rv.trimmed().compare(rs));
      ret = (0==tc_rv.compare(rs));
      if(!ret)
        // If false check the result is identical
        // This is also acceptable as there might be more than
        // one representation of same number
        ret = r.compare(e, &oc).isZero();
    }
    else {
      ret = r.compare(e, &oc).isZero();
      if(!ret && r.isNaN() && e.isNaN()) ret = true;
      if(!ret && (tc_a1.find('#')!=string::npos || tc_a2.find('#')!=string::npos) && e.isNaN()) ret = true;
    }
  }    
  
  if(tc_cd.size() && tc_rv.find('?')==string::npos) {
    // Only inspect the test cases where result is not ? and conditions are not empty
    DecContext ec{oc};
    ec.zeroStatus();
    //clog << tc_cd << endl;
    vector<string> strvec = split(tc_cd, ' ');
    for (auto cd : strvec) {
      //clog << '\t' << cd << endl;
      for(auto& c : cd) if(c=='_') c = ' '; // Replace _ with space in condition strings
      ec.setStatusFromString(cd.data());
    }
    uint32_t ec_sts = ec.getStatus();  // Expected status code
    cc_sts = cc.getStatus();
    oc_sts = oc.getStatus();
    uint32_t ac_sts = oc_sts | cc_sts; // All status code = Operation | Conversion codes
    if(ac_sts != ec_sts)
      slog(warn) << "WARNING: " << tc_cd << ' ' << '(' << cc_sts << '|' << oc_sts << ") "
           << ac_sts << "!=" << ec_sts;
  }


  EXPECT_TRUE(ret);
  if(ret) {
    //-clog << "PASS: " << tokens.join(",");
    //-clog << "PASS: " << tc_id << ' ';
    slog(info) << "PASS: " << tc_id << ' '
    // Uncomment to receive more information about passing test cases: 
             << " n1=" << n1.toString().data()
             << " n2=" << n2.toString().data()
             << " r="
             << (is_rs_used ? rs.data() : r.toString().data())
             << " e=" << e.toString().data()
             << " prc=" << oc.getDigits()
             //<< " ctx=" << (oc.getStatus() ? oc.statusToString() : "")
             << " ctx_sts=" << oc.getStatus()
             << (is_rs_used ?  string(" rs=") + erv + "?=" + rs + "|": "") ;


    return 0; // Success
  }
  else {

    slog(info) << "cc: " << cc << " cc_sts:" << cc_sts;
    if (cc_sts) slog(info) << " flg=" << cc.statusFlags() ;

    oc_sts = oc.getStatus();
    slog(info) << "oc: " << oc << " oc_sts:" << oc_sts;
    if (oc_sts) slog(info) << " flg=" << oc.statusFlags();

    //-clog << "FAIL: " << tokens.join(",");
    slog(err) 
            << "FAIL: " << tc_id << ' '
            << " n1=" << n1.toString().data()
            << " n2=" << n2.toString().data()
            << " n3=" << n3.toString().data()
            << " r="
            << (is_rs_used ? rs.data() : r.toString().data())
            << " e=" << e.toString().data()
            << " prc=" << oc.getDigits()
            //<< " ctx=" << (oc.getStatus() ? oc.statusToString() : "")
            << " ctx_sts=" << oc.getStatus()
            << (is_rs_used ?  string(" rs=") + erv + "?=" + rs + "|": "") ;

    

    // Print out operation context
    //- clog << "oc: " << oc;
    // Print out prevailing context settings
    displayDirectivesContext();
    return 1; // Failure
  }
  
  return rv;
}


int procTestCaseLine(string& line)
{
  slog(debug) << line ;
  smatch match;  
  smatch match_lhs;  
  smatch match_rhs;  
  int rv = 0;
  DecContext& ctx = Ctx;


  // COMMENTS
  // Delimiter position
  int dpos = line.find("--"); 
  if( dpos >= 0) {
    int qpos = line.rfind('\'', dpos); // Check if -- is inside quote
    if( qpos > 0)
      (void)0; // NOP, Not really a comment
    else
      line = regex_replace(line, rxcomment, ""); // Remove comment for easy parsing
  }
  
  // EMPTY LINE
  trim(line);
  if( line.empty() ) {
    slog(trace) << "EMPTY LINE" ; 
    return rv;
  }

  // DIRECTIVE  
  if(line.rfind(':') > 0 && regex_search(line, match, rxdirective)) {
    string dir = match.str(1);
    string val = match.str(2);
    slog(info) << "DIRECTIVE LINE: (" << dir << ':' << val << ')' ;
    rv = applyTestDirective(dir, val, ctx);
    return rv;
  }

  // TEST CASE
  // delimiter position
   dpos = line.rfind("->");
  if( dpos > 0 ) {
    string line_lhs = line.substr(0, dpos);
    string line_rhs = line.substr(dpos);
    bool matched_lhs = regex_search(line_lhs, match_lhs, rxtestcase_lhs);
    bool matched_rhs = regex_search(line_rhs, match_rhs, rxtestcase_rhs);
    //clog << "LHS " << matched_lhs << '|' << line_lhs << '|' << endl;
    //clog << "RHS " << matched_rhs << '|' << line_rhs << '|' << endl; 
    if(matched_lhs && matched_rhs) {
      string tc_id = match_lhs.str(1);  // Test case id
      string tc_op = match_lhs.str(2);  // Test case operation
      string tc_a1 = match_lhs.str(3);  // Argument #1 to the test case operation
      string tc_a2 = match_lhs.str(4);  // Argument #2 to the test case operation
      string tc_a3 = match_lhs.str(5);  // Argument #3 to the test case operation
      string tc_rv = match_rhs.str(1);  // Test case result or expected value
      string tc_cd = match_rhs.str(2);  // Test case conditions
    slog(info) << "TESTCASE LINE: (" 
      << tc_id << ','
      << tc_op << ','
      << tc_a1 << ','
      << tc_a2 << ','      
      << tc_a3
      << "->"
      << tc_rv << ','
      << tc_cd
      << ")";

    rv = applyTestCase(tc_id, tc_op, tc_a1, tc_a2, tc_a3, tc_rv, tc_cd, ctx);
    return rv;
   }
  }

  
  rv = 1;
  slog(err) << "UNRECOGNIZED LINE " << dpos << '|' << line << '|';

  return rv;
}





int processDecTestFile(string& fileName)
{
  int rv = 1;
  if(! std::filesystem::exists(fileName)) {
    slog(err) << "Unable to locate file " << fileName ;
    return rv;
  }

  ifstream ifile(fileName);  
  if(!ifile.good()) {
    slog(err) << "Unable to open file " << fileName ;
    return rv;
  }

  string line;
  if(ifile.is_open()) {
    while(getline(ifile, line)) {
      rv = procTestCaseLine(line);
      if(rv) break;
    } // while
    ifile.close();
  }
  else {
    slog(err) << "File " << fileName << " is not open";
  }

  return rv;
}






int testProcessDecTestFile()
{
  // Specfied directory that decTest files are in
  string dtdir = absl::GetFlag(FLAGS_dectests_dir);
  // Individual decTest file name
  string dtfn = absl::GetFlag(FLAGS_dectest_file);
  // Extended tests versus subset?
  bool extended = absl::GetFlag(FLAGS_extended);

  fs::path cwd = std::filesystem::current_path();
  fs::path dectestFN;

  if(dtdir.empty())
    dectestFN = DecTestsDir ;
  else
    dectestFN = dtdir;

  if(extended) {
    dectestFN /= "dectest_ext";
    if(dtfn.empty()) dtfn = "testall.decTest";

  }
  else {
    dectestFN /= "dectest_sub";
    if(dtfn.empty()) dtfn = "testall0.decTest";
  }
  
  // The path that decTest file reside
  DecTestsDir = dectestFN;

  // Now construct the full path of the test file
  dectestFN /= dtfn;
  // in string
  dtfn = dectestFN.string();
  // exececute the tests and return the outcome
  return processDecTestFile(dtfn);
}



int testVisitDirectory()
{
  fs::path dtpath = fs::current_path();
  // Specfied directory that decTest files are in
  string dtdir = absl::GetFlag(FLAGS_dectests_dir);
  if(dtdir.size())
    dtpath = dtdir;
  

  visit_directory(dtpath, true, 2);
  return 0;
}


