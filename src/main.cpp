#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/spirit/include/qi_and_predicate.hpp>
#include <boost/spirit/include/qi_hold.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/qi_repeat.hpp>
#include <boost/fusion/tuple.hpp>

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <list>

using namespace boost::spirit;

enum class LogicOperator { NOT, AND, IMPL, OR };

struct InputStruct
{
  std::string text;
  LogicOperator op;
  std::list<InputStruct> childs;  
};

template <typename Iterator>
struct TKOMLawGrammar : public qi::grammar<Iterator, std::string()>
{
  TKOMLawGrammar() : TKOMLawGrammar::base_type(zdanie)
  {
    using ascii::char_;

    zdanie = 
      (
        zdanie_twierdzace | regula 
      );

    regula =
      (
        "Jesli " >>
        suma_logiczna >> 
        " to " >>
        zdanie_proste >>
        "."
      );
    
    zdanie_twierdzace =
      (
        suma_logiczna >>
        "."
      );

    suma_logiczna =
      (
        ( 
          iloczyn_logiczny >> 
          " lub " >>
          suma_logiczna
        )
        |
        (
          iloczyn_logiczny
        )
      );

    iloczyn_logiczny =
      (
        (
          zdanie_proste >>
          " i " >>
          iloczyn_logiczny
        )
        |
        (
          zdanie_proste
        )
      );

    zdanie_proste =
      (
        (
          "nie " >>
          zdanie_proste
        )
        |
        (
          zdanie_jezykowe
        )
      );

    zdanie_jezykowe =
      (
        lexeme['"' >> +(char_ - '"') >> '"']
      );
  }

  qi::rule<Iterator, std::string()> zdanie;
  qi::rule<Iterator, std::string()> regula;
  qi::rule<Iterator, std::string()> zdanie_twierdzace;
  qi::rule<Iterator, std::string()> suma_logiczna;
  qi::rule<Iterator, std::string()> iloczyn_logiczny;
  qi::rule<Iterator, std::string()> zdanie_proste;
  qi::rule<Iterator, std::string()> zdanie_jezykowe;
};

typedef TKOMLawGrammar<std::string::iterator> TKOMLawGrammarString;

void show_string(const std::vector<char>& s)
{
  std::string thisstring(s.begin(), s.end());
  std::cout << "Zdanie: " << thisstring << std::endl;
}

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
    std::string parsed;
    bool result = phrase_parse(inputLine.begin(), inputLine.end(), grammar, space, parsed);
    if(result)
      std::cout << "Sparsowane " << parsed << std::endl;
    else
      std::cout << ":-(" << std::endl;
    getline(std::cin, inputLine);  
  } while (inputLine != "");
}
