#include "Knowledge.h"
#include "AI.h"

bool Knowledge::addFact(InputStruct fact, AnswerStack* toSaveResultTrack)
{
  AI ai(*this);
  if(ai.ask(fact, toSaveResultTrack) != AI::Answer::NO)
  {
    knowledge.push_back(fact);
    return true;
  }
  else
    return false;
}

bool Knowledge::addFact(InputStruct fact, AnswerStack& stack, AnswerStack* toSaveResultTrack)
{
  AI ai(*this);
  if(ai.ask(fact, stack, toSaveResultTrack) != AI::Answer::NO)
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
                             AnswerStack& stack,
                             AnswerStack* toSaveResultTrack)
  : knowledgeBase(kw)
{
  //TODO - better exception then std.
  if(!knowledgeBase.addFact(fact, stack, toSaveResultTrack))
  {
    throw std::exception();
  }
}

TmpFactPusher::~TmpFactPusher()
{
  knowledgeBase.popFact();
}

