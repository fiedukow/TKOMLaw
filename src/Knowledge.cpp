#include "Knowledge.h"
#include "AI.h"
#include <iostream>

bool Knowledge::addFact(InputStruct fact)
{
  AI ai(*this);
  if(ai.ask(fact) != AI::Answer::NO)
  {
    knowledge.push_back(fact);
    return true;
  }
  else
    return false;
}

bool Knowledge::addFact(InputStruct fact, AnswerStack& stack)
{
  AI ai(*this);
  if(ai.ask(fact, stack) != AI::Answer::NO)
  {
    knowledge.push_back(fact);
    return true;
  }
  return false;
}

FactList::const_iterator Knowledge::begin() const
{
  return knowledge.begin();
}

FactList::const_iterator Knowledge::end() const
{
  return knowledge.end();
}

FactPtrList Knowledge::findBySentence(const std::string& sentence) const
{
  FactPtrList result;
  for(FactList::const_iterator i = begin(); i != end(); ++i)
  {
    if(i->containsSentence(sentence))
      result.push_back(&(*i));
  }
  return result;
}

void Knowledge::popFact()
{
  knowledge.pop_back();
}

TmpFactPusher::TmpFactPusher(Knowledge& kw,
                             InputStruct fact,
                             AnswerStack& stack)
  : knowledgeBase(kw)
{
  //TODO - better exception then std.
  if(!knowledgeBase.addFact(fact, stack))
  {
    throw std::exception();
  }
}

TmpFactPusher::~TmpFactPusher()
{
  knowledgeBase.popFact();
}

