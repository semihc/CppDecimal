#include "DTRunner.hh"
#include <string>
#include <regex>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <algorithm>
#include "DecContext.hh"
#include "DecNumber.hh"


using namespace std;
namespace fs = std::filesystem;
using namespace dec;


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

static void lowerCase1(string& s) {
  std::transform(s.begin(), s.end(), s.begin(),
                  [](unsigned char c){ return std::tolower(c); });
}

static void lowerCase(string& s)
{
  for(auto& c : s)
    c = tolower(c);
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
// Fologing regex doesn't work due to non-greedy match
//regex rxtestcase {R"(\s*(\S+)\s*(\S+)\s*(\S+)\s*(\S+)?\s*(\S+)?\s*->\s*(\S+)\s*(\S+)?)"};
regex rxtestcase {R"(\s*(\w+)\s*(\w+)\s*(\S+|'\w+')\s*(\S*)\s*(\S*)\s*->\s*(\S+)\s*(.*))"};
//                      (id )   (op )   (op1)         (op2)   (op3)   ->   (res)   (cnd)
regex rxtestcase_lhs {R"(\s*(\w+)\s*(\S+)\s*(\S+)\s*(\S*)\s*(\S*))"};
regex rxtestcase_rhs {R"(->\s*(\S+)\s*(.*))"};


bool iequals(const string& a, const string& b)
{
    return std::equal(a.begin(), a.end(),
                      b.begin(), b.end(),
                      [](char a, char b) {
                          return tolower(a) == tolower(b);
                      });
}

// Global DecContext
static thread_local DecContext Ctx;


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
      clog << "Precison value conversion failed: " << val;
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
      clog << "Unknown value for rounding: " << val;
    }
  }
  else if(iequals(dir, "maxexponent")) {
    int32_t emax = (int32_t)std::stoi(val);
    if(ok) {
      rv = 0;
      ctx.setEmax(emax);
    }
    else {
      clog << "Unrecognized maxexponent: " << val;
    }
  }
  else if(dir == "minexponent") {
    int32_t emin = (int32_t)std::stoi(val);
    if(ok) {
      rv = 0;
      ctx.setEmin(emin);
    }
    else {
      clog << "Unrecognized minexponent: " << val;
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
      clog << "Unrecognized extended: " << val;
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
    rv = 0;
    //+TODO: Include the file
  }


  // Check if dir/value pair is recognized
  if(rv != 0)
    clog << "Unknown directive " << dir << endl;
  else {
    //-m_curDirectives.insert(dir, val);
    clog << "dir=" << dir << " val=" << val;
    clog << " ctx=" << ctx << endl;
  }

  
  return rv;
}





// Perform (do) a DecNumber operation
DecNumber opDo(const string& op,
               DecNumber& n1, DecNumber& n2, DecNumber& n3,
               DecContext& c, string& rs)
{
  //- clog << "Operation: " << op << endl;

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

  
  clog << "Unrecognized operation: " << op << endl;
  return DecNumber{};
}

/*
int QDecNumberTests::opTest(const QStringList& tokens)
{
  QString id = tokens.at(0);
  QString op = tokens.at(1).toLower();
  QString opd1 = tokens.at(2);
  QString opd2 = tokens.at(3);
  QString opd3 = tokens.at(4);
  QString res = tokens.at(5);
  QString cond = tokens.at(6);
  bool ret = false;
  
  QDecNumber n1,n2,n3,e;
  // Conversion Context - needs high precision
  QDecContext cc(DEC_INIT_DECIMAL128);
  // Operation Context
  QDecContext oc(DEC_INIT_DECIMAL128);
  QString rs; // Result String
  bool op_precision_needed = false;
  bool is_rs_used = false; // Is result string used?


  // Skip a testcase with # as any of the operands
  for(int i=2; i<=4; i++)
    if(QString('#')==tokens.at(i)) {
      qDebug() << "SKIP(operand#): " << tokens.join(",");
      return 0;
    }
  
  // Expected result will get maximum allowable precision
  cc.setEmax(QDecMaxExponent); 
  cc.setEmin(QDecMinExponent); 
  // Expected result should not be affected by current context
  if(res != "?") {
    ret = token2QDecNumber(res, cc, e); // Expected result
    qDebug() << "cc: " << cc;
  }
  cc.zeroStatus(); // Clear status flag for next operation
  
  // Apply current context to operands now
  if(op=="tosci" ||
     op=="toeng" ||
     op=="apply") {
    op_precision_needed = true;
    is_rs_used = true;
    res.remove(QChar('\''));
  }
  getDirectivesContext(cc, op_precision_needed);

  ret = token2QDecNumber(opd1, cc, n1);
  cc.zeroStatus(); // Clear status flag for next operation

  if(is_binary_op(op) ||
     is_ternary_op(op)) {
    ret = token2QDecNumber(opd2, cc, n2);
    cc.zeroStatus();
  }
  if(is_ternary_op(op)) {
    ret = token2QDecNumber(opd3, cc, n3);
    cc.zeroStatus();
  }

  // Get context directives including precision
  getDirectivesContext(oc, true);
  // Perform the operation, obtain the result
  QDecNumber r = op_do(op,n1,n2,n3,oc,rs);
  
  if(res=="?") { 
    ret = true;
    if(oc.status()) {
      qDebug() << "runTestCase ctx=" << oc.statusToString()
               << "flg=" << oc.statusFlags();
    }
  }
  else {
    if(op == "tosci" ||
       op == "toeng" ||
       op == "class" ) {
      ret = (0==res.trimmed().compare(rs));
      if(!ret)
        // If false check the result is identical
        // This is also acceptable as there might be more than
        // one representation of same number
        ret = r.compare(e, &oc).isZero();
    }
    else {
      ret = r.compare(e, &oc).isZero();
      if(r.isNaN() && e.isNaN()) ret = true;
    }
  }
  qDebug() << "oc: " << oc;
  if(ret) {
    qDebug() << "PASS: " << tokens.join(",");
    // Uncomment to receive more information about passing test cases: 
    qDebug() << "n1=" << n1.toString().data()
             << "n2=" << n2.toString().data()
             << "r="
             << (is_rs_used ? rs.toLocal8Bit().data() : r.toString().data())
             << "e=" << e.toString().data()
             << "prc=" << oc.digits()
             << "ctx=" << (oc.status() ? oc.statusToString() : 0)
             << (is_rs_used ?  res + "|" + rs : (const char*)0);

    return 0; // Success
  }
  else {
    qDebug() << "FAIL: " << tokens.join(",");
    qDebug() << "n1=" << n1.toString().data()
             << "n2=" << n2.toString().data()
             << "n3=" << n3.toString().data()
             << "r="
             << (is_rs_used ? rs.toLocal8Bit().data() : r.toString().data())
             << "e=" << e.toString().data()
             << "prc=" << oc.digits()
             << "ctx=" << (oc.status() ? oc.statusToString() : 0)
             << (is_rs_used ?  res + "|" + rs : (const char*)0);

    // Print out operation context
    qDebug() << "oc: " << oc;
    // Print out prevailing context settings
    displayDirectivesContext();
    // Uncomment this if you want to stop the test cases after failure
    //qFatal("End");                    
    return 1; // Failure
  }
  
  return 0;
}
*/


bool token2DecNumber(const string& token, const DecContext& ctx, DecNumber& num)
{
  string tt = token;
  char sq = '\''; // Single quote
  char dq = '\"'; // Double quote

  if(tt.find(sq) != string::npos)
    std::erase(tt, sq); 

  if(tt.find(dq) != string::npos)
    std::erase(tt, dq); 
  
  //clog << "token=" << token << " tt=" << tt << endl;

  /*
  // Deal with quotes, double quotes and escaped quotes
  if(tt.contains("''")) {
    tt.replace("''","'");
    tt = tt.remove(0,1);
    tt.chop(1);
  }
  else
    tt.remove(QChar('\''));

  if(tt.contains("\"\"")) {
    tt.replace("\"\"","\"");
    tt = tt.remove(0,1);
    tt.chop(1);   
  }
  else
    tt.remove(QChar('\"'));

  if(token.contains('#')) {
    regex expl{"#([0-9a-fA-F]+)"}; // explicit notation
    regex altn{"([0-9]+)#(.+)"};   // alternative notation

    if(expl.exactMatch(token)) {
      QString hexval = expl.cap(1); // get hex value
      switch(hexval.size()) {
        case 8: {
          QDecSingle ds;
          ds.fromHexString(hexval.toLocal8Bit().data());
          num = ds.toQDecNumber();
          return true;
          }      
        case 16: {
          QDecDouble dd;
          dd.fromHexString(hexval.toLocal8Bit().data());
          num = dd.toQDecNumber();
          return true;
        }
        case 32: {
          QDecQuad dq;
          dq.fromHexString(hexval.toLocal8Bit().data());
          num = dq.toQDecNumber();
          return true;
        }
      } // end switch
    } // expl.

    if(altn.exactMatch(token)) {
      QString fmt = altn.cap(1); // get format size
      QString val = altn.cap(2); // get number value in string

      uint fmtsize = fmt.toUInt();
      switch(fmtsize) {
        case 32: {
          qDebug() << "fmt=" << fmt << "val=" << val;
          QDecSingle ds(val.toLocal8Bit().data());
          num = ds.toQDecNumber();
          return true;
        }

        case 64: {
          qDebug() << "fmt=" << fmt << "val=" << val;
          QDecDouble dd(val.toLocal8Bit().data());
          num = dd.toQDecNumber();
          return true;
        }

        case 128: {
          qDebug() << "fmt=" << fmt << "val=" << val;
          QDecQuad dq(val.toLocal8Bit().data());
          num = dq.toQDecNumber();
          return true;
        }

      } // end switch

    } // altn.

    
    // '#' in a token by itself
    num.fromString("NaN");
    return true;
      
  } // contains #
  */

  //-qDebug() << "ctx=" << ctx;
  DecContext c(ctx);

  DecNumber tnum;
  tnum.fromString(tt.data(), &c);
  num = tnum;

  //TODO: Check if warning is necessary
  if(c.getStatus()) {
    clog << "token2DecNumber"
         << " tkn=" << token
         << " ctx=" << c.statusToString()
         << c.statusFlags()
         << " val=" << tnum.toString()
         << endl;

    //- clog << "c=" << c << endl;
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
  /*ERASE
  QMapIterator<QString, QString> i(m_curDirectives);
  QStringList tokens;
  
  while(i.hasNext()) {
    i.next();
    if(!precision) {
      if(i.key() == "precision")
        continue; // Ignore precison directives if not wanted
    }
    tokens.clear();
    tokens << i.key() << i.value();
    applyTestDirective(tokens, ctx);
  }

  // If precision is not wanted, pick largest exponent values
  // to avoid rounding
  if(!precision) {
    ctx.setEmax(QDecMaxExponent); 
    ctx.setEmin(QDecMinExponent); 
  }

  if(ctx.status())
    qWarning() << "getDirectivesContext ctx=" << ctx.statusToString();
  //qDebug() << "getDirectivesContext ctx=" << ctx;
  
  return 0;
  */
  return 0;
}



void displayDirectivesContext()
{
  /*ERASE
  QMapIterator<QString, QString> i(m_curDirectives);

  qDebug() << "Currently valid context settings:";
  while(i.hasNext()) {
    i.next();
    qDebug() << i.key() << '=' << i.value();
  }
  */
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

  lowerCase(tc_op); // Normalize the operation to lowercases
  
  DecNumber n1,n2,n3,e;
  // Conversion Context - needs high precision
  DecContext cc(DEC_INIT_DECIMAL128);
  // Operation Context
  DecContext oc(DEC_INIT_DECIMAL128);
  string erv = tc_rv; // Expected result value
  string rs; // Result String
  bool op_precision_needed = false;
  bool is_rs_used = false; // Is result string used?


  // Skip a testcase with # as any of the operands
  string hashstr = "#";
  if(tc_a1==hashstr || tc_a2==hashstr || tc_a3==hashstr) {
    clog << "SKIP(operand#)" << endl;
    return 0;
  }
  /*ERASE
  for(int i=2; i<=4; i++)
    if(QString('#')==tokens.at(i)) {
      qDebug() << "SKIP(operand#): " << tokens.join(",");
      return 0;
    }
  */
  
  // Expected result will get maximum allowable precision
  cc = ctx;
  cc.setEmax(DecMaxExponent); 
  cc.setEmin(DecMinExponent); 
  // Expected result should not be affected by current context
  if(tc_rv != "?") {
    ret = token2DecNumber(tc_rv, cc, e); // Expected result
    //clog << "cc: " << cc << endl;
  }
  cc.zeroStatus(); // Clear status flag for next operation
  
  // Apply current context to operands now
  if(tc_op=="tosci" ||
     tc_op=="toeng" ||
     tc_op=="apply") {
    op_precision_needed = true;
    is_rs_used = true;
    std::erase(erv, '\''); 
    //erv.erase(std::remove(erv.begin(), erv.end(), '\''), erv.end());
    //-res.remove(QChar('\''));
  }

  getDirectivesContext(cc, op_precision_needed);

  ret = token2DecNumber(tc_a1, cc, n1);
  cc.zeroStatus(); // Clear status flag for next operation
  if(!tc_a2.empty()) {
    ret = token2DecNumber(tc_a2, cc, n2);
    cc.zeroStatus(); // Clear status flag for next operation
  }
  if(!tc_a3.empty()) {
    ret = token2DecNumber(tc_a3, cc, n3);
    cc.zeroStatus(); // Clear status flag for next operation
  }

  /*ERASE
  if(is_binary_op(op) ||
     is_ternary_op(op)) {
    ret = token2DecNumber(tc_a2, cc, n2);
    cc.zeroStatus();
  }
  if(is_ternary_op(op)) {
    ret = token2QDecNumber(tc_a3, cc, n3);
    cc.zeroStatus();
  }
  */

  // Get context directives including precision
  getDirectivesContext(oc, true);
  oc = ctx;
  //clog << "oc: " << oc << endl;

  // Perform the operation, obtain the result
  DecNumber r = opDo(tc_op,n1,n2,n3,oc,rs);
  
  if(tc_rv=="?") { 
    ret = true;
    if(oc.getStatus()) {
      clog << "runTestCase ctx=" << oc.statusToString()
           << "flg=" << oc.statusFlags();
    }
  }
  else {
    if(tc_op == "tosci" ||
       tc_op == "toeng" ||
       tc_op == "class" ) {
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
      if(r.isNaN() && e.isNaN()) ret = true;
    }
  }
  //- clog << "oc: " << oc;
  if(ret) {
    //-clog << "PASS: " << tokens.join(",");
    clog << "PASS: " << tc_id << ' ';
    // Uncomment to receive more information about passing test cases: 
    clog << " n1=" << n1.toString().data()
             << " n2=" << n2.toString().data()
             << " r="
             << (is_rs_used ? rs.data() : r.toString().data())
             << " e=" << e.toString().data()
             << " prc=" << oc.getDigits()
             << " ctx=" << (oc.getStatus() ? oc.statusToString() : "")
             << (is_rs_used ?  erv + "|" + rs : "")
             << endl;

    return 0; // Success
  }
  else {
    //-clog << "FAIL: " << tokens.join(",");
    clog << "FAIL: " << tc_id << ' ';
    clog << " n1=" << n1.toString().data()
             << " n2=" << n2.toString().data()
             << " n3=" << n3.toString().data()
             << " r="
             << (is_rs_used ? rs.data() : r.toString().data())
             << " e=" << e.toString().data()
             << " prc=" << oc.getDigits()
             << " ctx=" << (oc.getStatus() ? oc.statusToString() : "")
             << (is_rs_used ?  erv + "|" + rs : "")
             << endl;

    // Print out operation context
    //- clog << "oc: " << oc;
    // Print out prevailing context settings
    displayDirectivesContext();
    // Uncomment this if you want to stop the test cases after failure
    //qFatal("End");                    
    return 1; // Failure
  }
  


  return rv;
}


int procTestCaseLine(string& line)
{
  cout << line << endl;
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
      void; // NOP, Not really a comment
    else
      line = regex_replace(line, rxcomment, ""); // Remove comment for easy parsing
  }
  
  // EMPTY LINE
  trim(line);
  if( line.empty() ) {
    cout << "EMPTY LINE" << endl; 
    return rv;
  }

  // DIRECTIVE  
  if(line.rfind(':') > 0 && regex_search(line, match, rxdirective)) {
    string dir = match.str(1);
    string val = match.str(2);
    cout << "DIRECTIVE LINE: (" << dir << ':' << val << ')' << endl;
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
    cout << "TESTCASE LINE: (" 
      << tc_id << ','
      << tc_op << ','
      << tc_a1 << ','
      << tc_a2 << ','      
      << tc_a3
      << "->"
      << tc_rv << ','
      << tc_cd
      << ")" << endl;

    rv = applyTestCase(tc_id, tc_op, tc_a1, tc_a2, tc_a3, tc_rv, tc_cd, ctx);
    return rv;
   }
  }

  
  rv = 1;
  cout << "UNRECOGNIZED LINE " << dpos << '|' << line << '|' << endl;

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
  //dectestFN /= "trim0.decTest"; 
  //dectestFN /= "rounding0.decTest"; 

  //dectestFN /= "abs0.decTest"; 
  //dectestFN /= "add0.decTest"; 
  //dectestFN /= "base0.decTest";
  //dectestFN /= "compare0.decTest";
  //dectestFN /= "comparetotal0.decTest";
  dectestFN /= "divide0.decTest";

  string dtfn = dectestFN.string();
  cout << dtfn << endl;

  return processDecTestFile(dtfn);
}

int testVisitDirectory()
{
  visit_directory(dectests_path, true, 2);
  return 0;
}


