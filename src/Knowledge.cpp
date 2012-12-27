#include "Knowledge.h"

void Knowledge::addFact(InputStruct fact)
{
  knowledge.push_back(fact);
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

