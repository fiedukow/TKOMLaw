#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/spirit/include/qi_and_predicate.hpp>
#include <boost/spirit/include/qi_hold.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/qi_repeat.hpp>
#include <boost/fusion/tuple.hpp>
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/foreach.hpp>


#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <list>
#include <map>

using namespace boost::spirit;
using namespace boost;

enum class LogicOperator { NONE, NOT, AND, IMPL, OR };
std::map<LogicOperator, std::string> logicOperatorMap()
{
  std::map<LogicOperator, std::string> result;
  result[LogicOperator::NONE] = "NONE";
  result[LogicOperator::NOT] = "NOT";
  result[LogicOperator::AND] = "AND";
  result[LogicOperator::IMPL] = "IMPL";
  result[LogicOperator::OR] = "OR";
  return result;
}
/*const*/ std::map<LogicOperator, std::string> LoToStr = logicOperatorMap();

struct InputStruct
{
  InputStruct(const std::string& v)
    : text(v)
  {}  

  InputStruct(const std::vector<char>& s)
  {
    text = std::string(s.begin(), s.end());
  }

  InputStruct()
  {}
  
  void print()
  {
    std::cout << "OPERATOR = " << LoToStr[op] << std::endl;
    std::cout << "TEXT = " << text << std::endl;
    std::cout << "Dzieci " << childs.size() << ":" << std::endl;
    std::cout << "{" << std::endl;
    for(auto is : childs)
      is.print();
    std::cout << "}" << std::endl; 
  }

  std::string text;
  LogicOperator op;
  std::list<InputStruct> childs;  
};

BOOST_FUSION_ADAPT_STRUCT(
    InputStruct,
    (std::string, text)
    (LogicOperator, op)
    (std::list<InputStruct>, childs)
)

void show_string(const std::vector<char>& s)
{
  std::string thisstring(s.begin(), s.end());
  std::cout << "Zdanie: " << thisstring << std::endl;
}

void show_str(const std::string& s)
{
  std::cout << "Reg: " << s << std::endl;
}

template <typename Iterator>
struct TKOMLawGrammar : public qi::grammar<Iterator, InputStruct()>
{
  TKOMLawGrammar() : TKOMLawGrammar::base_type(zdanie)
  {
    using ascii::char_;
    using qi::_1;
    using qi::_val;
    using phoenix::at_c;
    using phoenix::push_back;

    zdanie = 
      (
        (zdanie_twierdzace)[_val=_1]
        |
        (regula)
      );

    regula =
      (
        qi::string("Jesli ")[at_c<1>(_val) = LogicOperator::IMPL] >>
        suma_logiczna >> 
        qi::string(" to ") >>
        zdanie_proste >>
        qi::string(".")
      );
    
    zdanie_twierdzace =
      (
        suma_logiczna[_val=_1] >>
        qi::string(".")
      );

    suma_logiczna =
      (
        ( 
          iloczyn_logiczny >> 
          qi::string(" lub ") >>
          suma_logiczna
        )
        |
        (
          iloczyn_logiczny[_val=_1]
        )
      );

    iloczyn_logiczny =
      (
        (
          zdanie_proste [push_back(at_c<2>(_val), _1)] >>
          qi::string(" i ") [at_c<1>(_val) = LogicOperator::AND] >>
          iloczyn_logiczny [push_back(at_c<2>(_val), _1)] 
        )
        |
        (
          zdanie_proste[_val=_1]
        )
      );

    zdanie_proste =
      (
        (
          qi::string("nie ") [at_c<1>(_val) = LogicOperator::NOT] >>
          zdanie_proste [push_back(at_c<2>(_val), _1)]
        )
        |
        (
          zdanie_jezykowe[_val=_1]
        )
      );

    zdanie_jezykowe =
      (
        lexeme['"' >> +(char_ - '"') >> '"'][_val=_1]
      );
  }

  qi::rule<Iterator, InputStruct()> zdanie;
  qi::rule<Iterator, InputStruct()> regula;
  qi::rule<Iterator, InputStruct()> zdanie_twierdzace;
  qi::rule<Iterator, InputStruct()> suma_logiczna;
  qi::rule<Iterator, InputStruct()> iloczyn_logiczny;
  qi::rule<Iterator, InputStruct()> zdanie_proste;
  qi::rule<Iterator, InputStruct()> zdanie_jezykowe;
};

typedef TKOMLawGrammar<std::string::iterator> TKOMLawGrammarString;


void show_int(int i)
{
  std::cout << "Int: " << i << std::endl;
}

int main()
{
  using qi::parse;
  using ascii::char_;

  std::string inputLine;
  TKOMLawGrammarString grammar;

  getline(std::cin, inputLine);  
  do {
    using boost::spirit::ascii::space;
    InputStruct parsed;
    bool result = phrase_parse(inputLine.begin(), inputLine.end(), grammar, space, parsed);
    if(result)
    {
      std::cout << "Sparsowane " << std::endl;
      parsed.print();
    } 
    else
      std::cout << ":-(" << std::endl;
    getline(std::cin, inputLine);  
  } while (inputLine != "");
}
