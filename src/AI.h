#ifndef AI_H
#define AI_H

#include <stack>

#include "Knowledge.h"

typedef std::list<const InputStruct*> AnswerStack;

class AI
{
public:
  enum class Answer { YES, NO, DK };

public:
  AI(Knowledge& knowledgeBase);
  Answer ask(const InputStruct& is,
             AnswerStack* toSaveResultTrack = NULL);
  Answer ask(const InputStruct& is,
             AnswerStack& stack,
             AnswerStack* toSaveResultTrack = NULL);

private:
  Answer question(const InputStruct& is,
                  AnswerStack& stack,
                  AnswerStack* toSaveResultTrack);
  Answer sentenceQuestion(const InputStruct& is,
                          AnswerStack& stack,
                          AnswerStack* toSaveResultTrack);
  Answer claimAnswer(const InputStruct& is,
                     const InputStruct& claim,
                     AnswerStack &stack,
                     AnswerStack* toSaveResultTrack);
  static void resetStackFromLvl(AnswerStack* stack,
                                int lvl);

private:
  Knowledge& knowledgeBase;
};

AI::Answer operator!(AI::Answer ans);
AI::Answer operator||(AI::Answer f, AI::Answer s);
AI::Answer operator&&(AI::Answer f, AI::Answer s);

#endif // AI_H
