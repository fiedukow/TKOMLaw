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
  bool addFact(const InputStruct&, AnswerStack* toSaveResultTrack);
  bool addFact(const InputStruct&, AnswerStack&, AnswerStack* toSaveResultTrack);
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
  TmpFactPusher(Knowledge&,
                const InputStruct& fact,
                AnswerStack&);
  ~TmpFactPusher();

private:
  Knowledge& knowledgeBase;
};

#endif
