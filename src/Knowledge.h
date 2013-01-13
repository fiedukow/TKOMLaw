#ifndef _KNOWLEDGE_H_
#define _KNOWLEDGE_H_

#include <list>

#include "InputStruct.h"

typedef std::list<const InputStruct*> AnswerStack;

typedef std::list<InputStruct> FactList;
typedef std::list<const InputStruct*> FactPtrList;

class TmpFactPusher;

class Knowledge
{
public:
  bool addFact(InputStruct, AnswerStack* toSaveResultTrack);
  bool addFact(InputStruct, AnswerStack&, AnswerStack* toSaveResultTrack);
  FactList::const_iterator begin() const;
  FactList::const_iterator end() const;
  FactPtrList findBySentence(const std::string&) const;

private:
  void popFact();
  friend class TmpFactPusher;

private:
  FactList knowledge;
};

class TmpFactPusher
{
public:
  TmpFactPusher(Knowledge&, InputStruct fact,
                AnswerStack&,
                AnswerStack* toSaveResultTrack);
  ~TmpFactPusher();

private:
  Knowledge& knowledgeBase;
};

#endif
