#include <boost/spirit/include/qi.hpp>
#include <boost/bind/arg.hpp>

#include "Interpreter.h"
#include "InputStruct.h"
#include "TKOMLawGrammar.hpp"
#include "Knowledge.h"
#include "AI.h"
#include "IO.h"

using namespace boost;

Interpreter::Interpreter(Knowledge& knowledgeBase)
  : actionMap(createActionMap()),
    knowledgeBase(knowledgeBase),
    io(NULL)
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
  {
    if(io)
      io->unknownCommand(toParse);
    return false; /*parsing failor*/
  }

  return actionMap[parsed.st](parsed);
}

void Interpreter::setOutput(IO* io)
{
  this->io = io;
}


bool Interpreter::unknownTypeAction(InputStruct& /*is*/)
{
  assert(false);
}

bool Interpreter::ruleAction(InputStruct& is)
{
  AnswerStack stack;
  std::list<std::string> conflict;
  if(knowledgeBase.addFact(is, &stack))
  {
    if(io)
      io->addedRule(is.toString());
  }
  else
  {
    for(auto& i : stack)
      conflict.push_back(i->toString());
    if(io)
      io->conflictRule(is.toString(), conflict);
  }
  return true;
}

bool Interpreter::claimAction(InputStruct& is)
{
  AnswerStack stack;
  std::list<std::string> conflict;
  if(knowledgeBase.addFact(is, &stack))
  {
    if(io)
      io->addedRule(is.toString());
  }
  else
  {
    for(auto& i : stack)
      conflict.push_back(i->toString());
    if(io)
      io->conflictRule(is.toString(), conflict);
  }
  return true;
}

bool Interpreter::questionAction(InputStruct& is)
{
  AI ai(knowledgeBase);
  if(io)
    io->answer(ai.ask(is));
  return true;
}

bool Interpreter::searchAction(InputStruct& is)
{
  std::list<std::string> connected;
  const FactPtrList found = knowledgeBase.findBySentence(is.text);
  for(FactPtrList::const_iterator i = found.begin(); i != found.end(); ++i)
  for(auto& i : found)
    connected.push_back(i->toString());

  if(io)
    io->informationsAbout(is.toString(), connected);
  return true;
}
