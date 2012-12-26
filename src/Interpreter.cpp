#include <boost/spirit/include/qi.hpp>

#include "Interpreter.h"
#include "InputStruct.h"
#include "TKOMLawGrammar.hpp"

Interpreter::Interpreter()
{
}

Interpreter& Interpreter::getInstance()
{
  static Interpreter instance;
  return instance;
}

bool Interpreter::parseLine(std::string toParse)
{
  using qi::parse;
  using boost::spirit::ascii::space;

  TKOMLawGrammarString grammar;
  InputStruct parsed;
  bool r = phrase_parse(toParse.begin(), toParse.end(), grammar, space, parsed);
  if(!r)
    return false; /*parsing failor*/

  parsed.print();
  /*
   * Do sth with parsed InputStruct
   */
  return true;
}
