#include <iostream>
#include <string>

#include <boost/spirit/include/qi.hpp>

#include "InputStruct.h"
#include "TKOMLawGrammar.hpp"

int main()
{
  using qi::parse;

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
