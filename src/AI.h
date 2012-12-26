#ifndef AI_H
#define AI_H

#include <stack>

#include "Knowledge.h"

class AI
{
public:
  enum class Answer { YES, NO, DK };

public:
  AI(Knowledge& knowledgeBase);
  Answer question(const InputStruct& is);
  Answer sentenceQuestion(const InputStruct& is);
  Answer claimAnswer(const InputStruct& is, const InputStruct& claim);

private:
  const Knowledge& knowledgeBase;
};

std::map<AI::Answer, std::string> answerMap();
AI::Answer operator!(AI::Answer ans);
AI::Answer operator||(AI::Answer f, AI::Answer s);
AI::Answer operator&&(AI::Answer f, AI::Answer s);

#endif // AI_H
