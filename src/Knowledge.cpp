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

