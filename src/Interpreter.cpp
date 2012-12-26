#include <boost/spirit/include/qi.hpp>
#include <boost/bind/arg.hpp>

#include "Interpreter.h"
#include "InputStruct.h"
#include "TKOMLawGrammar.hpp"
#include "Knowledge.h"

using namespace boost;

Interpreter::Interpreter(Knowledge& knowledgeBase)
  : actionMap(createActionMap()),
    knowledgeBase(knowledgeBase)
{
}

Interpreter::ActionMap Interpreter::createActionMap()
{
  ActionMap result;
  result[SentenceType::NONE]
      = boost::bind(&Interpreter::unknownTypeAction, this, arg<1>());
  result[SentenceType::RULE]
      = boost::bind(&Interpreter::ruleAction,        this, arg<1>());
  result[SentenceType::CLAIM]
      = boost::bind(&Interpreter::claimAction,       this, arg<1>());
  result[SentenceType::QUESTION]
      = boost::bind(&Interpreter::questionAction,    this, arg<1>());
  result[SentenceType::SEARCH]
      = boost::bind(&Interpreter::searchAction,      this, arg<1>());
  return result;
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
  actionMap[parsed.st](parsed);

  /*
   * Do sth with parsed InputStruct
   */
  return true;
}

bool Interpreter::unknownTypeAction(InputStruct& is)
{
  assert(false);
}

bool Interpreter::ruleAction(InputStruct& is)
{
  std::cout << "New rule added" << std::endl;
  knowledgeBase.addFact(is);
  return true;
}

bool Interpreter::claimAction(InputStruct& is)
{
  std::cout << "New claim added" << std::endl;
  knowledgeBase.addFact(is);
  return true;
}

bool Interpreter::questionAction(InputStruct& is)
{
  std::cout << "I will answer later." << std::endl;
  return true;
}

bool Interpreter::searchAction(InputStruct& is)
{
  std::cout << "Cant find it :(" << std::endl;
  return true;
}
