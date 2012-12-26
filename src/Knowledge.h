#ifndef _KNOWLEDGE_H_
#define _KNOWLEDGE_H_

#include <list>

#include "InputStruct.h"

typedef std::list<InputStruct> FactList;

class Knowledge
{
public:  
  void addFact(InputStruct);
  FactList::const_iterator begin() const;
  FactList::const_iterator end() const;

private:
  FactList knowledge;
};

#endif
