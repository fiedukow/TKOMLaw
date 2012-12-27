#ifndef AI_H
#define AI_H

#include <stack>

#include "Knowledge.h"

class AI
{
public:
  enum class Answer { YES, NO, DK };
  typedef std::list<const InputStruct*> AnswerStack;

public:
  AI(Knowledge& knowledgeBase);
  Answer ask(const InputStruct& is);
private:
  Answer question(const InputStruct& is,
                  AnswerStack& stack);
  Answer sentenceQuestion(const InputStruct& is,
                          AnswerStack& stack);
  Answer claimAnswer(const InputStruct& is, const InputStruct& claim);

private:
  const Knowledge& knowledgeBase;
};

std::map<AI::Answer, std::string> answerMap();
AI::Answer operator!(AI::Answer ans);
AI::Answer operator||(AI::Answer f, AI::Answer s);
AI::Answer operator&&(AI::Answer f, AI::Answer s);

#endif // AI_H
