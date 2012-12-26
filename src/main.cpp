#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include <iostream>
#include <string>
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


enum class SentenceType { NONE, RULE, CLAIM, QUESTION, SEARCH };
std::map<SentenceType, std::string> sentenceTypeMap()
{
  std::map<SentenceType, std::string> result;
  result[SentenceType::NONE] = "NONE";
  result[SentenceType::RULE] = "RULE";
  result[SentenceType::CLAIM] = "CLAIM";
  result[SentenceType::QUESTION] = "QUESTION";
  result[SentenceType::SEARCH] = "SEARCH";
  return result;
}
/*const*/ std::map<SentenceType, std::string> StToStr = sentenceTypeMap();

struct InputStruct
{
  InputStruct(const std::vector<char>& s)
    : InputStruct()
  {
    text = std::string(s.begin(), s.end());
  }

  InputStruct()
    : st(SentenceType::NONE),
      op(LogicOperator::NONE)
  {}
  
  void print(int tab = 0)
  {
    tabs(tab);
    std::cout << "*** BEGIN ***" << std::endl;
    if(st != SentenceType::NONE)
    {
      tabs(tab);
      std::cout << "SENTENCE_TYPE = " << StToStr[st] << std::endl;
    }
    if(op != LogicOperator::NONE)
    {
      tabs(tab);
      std::cout << "OPERATOR = " << LoToStr[op] << std::endl;
    }
    if(text != "")
    {
      tabs(tab);
      std::cout << "TEXT = " << text << std::endl;
    }
    if(childs.size() > 0)
    {
      tabs(tab);
      std::cout << "Dzieci " << childs.size() << ":" << std::endl;
      tabs(tab);
      std::cout << "{" << std::endl;
      for(auto is : childs)
        is.print(tab + 1);
      tabs(tab);
      std::cout << "}" << std::endl; 
    }
    tabs(tab);
    std::cout << "*** END ***" << std::endl;
  }

  static void tabs(int tab)
  {
    while(tab--)
      std::cout << "\t";
  }

  SentenceType st;
  std::string text;
  LogicOperator op;
  std::list<InputStruct> childs;
};

BOOST_FUSION_ADAPT_STRUCT(
    InputStruct,
    (SentenceType, st)
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
        (zdanie_twierdzace)[_val=_1, at_c<0>(_val) = SentenceType::CLAIM]
        |
        (regula)[_val=_1, at_c<0>(_val) = SentenceType::RULE]
        |
        (pytanie)[_val=_1, at_c<0>(_val) = SentenceType::QUESTION]
        |
        (wyszukiwanie)[_val=_1, at_c<0>(_val) = SentenceType::SEARCH]
      );

    regula =
      (
        qi::string("Jesli ") >>
        suma_logiczna [push_back(at_c<3>(_val), _1)] >> 
        qi::string(" to ") [at_c<2>(_val) = LogicOperator::IMPL] >>
        zdanie_proste [push_back(at_c<3>(_val), _1)] >>
        qi::string(".")
      );
    
    zdanie_twierdzace =
      (
        suma_logiczna[_val=_1] >>
        qi::string(".")
      );

    pytanie =
      (
        zdanie_proste[_val=_1] >>
        qi::string("?")
      );

    wyszukiwanie =
      (
        qi::string("Co wiesz o ") >>
        zdanie_jezykowe[_val=_1] >>
        qi::string("?")
      );

    suma_logiczna =
      (
        ( 
          iloczyn_logiczny[push_back(at_c<3>(_val), _1)] >> 
          qi::string(" lub ")[at_c<2>(_val) = LogicOperator::OR] >>
          suma_logiczna[push_back(at_c<3>(_val), _1)]
        )
        |
        (
          iloczyn_logiczny[_val=_1]
        )
      );

    iloczyn_logiczny =
      (
        (
          zdanie_proste [push_back(at_c<3>(_val), _1)] >>
          qi::string(" i ") [at_c<2>(_val) = LogicOperator::AND] >>
          iloczyn_logiczny [push_back(at_c<3>(_val), _1)] 
        )
        |
        (
          zdanie_proste[_val=_1]
        )
      );

    zdanie_proste =
      (
        (
          qi::string("nie ") [at_c<2>(_val) = LogicOperator::NOT] >>
          zdanie_proste [push_back(at_c<3>(_val), _1)]
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
  qi::rule<Iterator, InputStruct()> pytanie;
  qi::rule<Iterator, InputStruct()> wyszukiwanie;
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
