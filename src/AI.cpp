#include <cassert>
#include <iostream>

#include "AI.h"

std::map<AI::Answer, std::string> answerMap()
{
  std::map<AI::Answer, std::string> result;
  result[AI::Answer::YES] = "YES";
  result[AI::Answer::NO] = "NO";
  result[AI::Answer::DK] = "DK";
  return result;
}

AI::Answer operator!(AI::Answer ans)
{
  switch(ans)
  {
    case AI::Answer::YES:
      return AI::Answer::NO;
    case AI::Answer::NO:
      return AI::Answer::YES;
    default:
      return AI::Answer::DK;
  }
}

AI::Answer operator||(AI::Answer f, AI::Answer s)
{
  if(f == AI::Answer::YES || s == AI::Answer::YES)
    return AI::Answer::YES;

  if(f == AI::Answer::NO || s == AI::Answer::NO)
    return AI::Answer::NO;

  return AI::Answer::DK;
}

AI::Answer operator&&(AI::Answer f, AI::Answer s)
{
  if(f == AI::Answer::YES && s == AI::Answer::YES)
    return AI::Answer::YES;

  if(f == AI::Answer::NO || s == AI::Answer::NO)
    return AI::Answer::NO;

  return AI::Answer::DK;
}

AI::Answer operator^(AI::Answer f, AI::Answer s)
{
  if(f == AI::Answer::YES && s == AI::Answer::NO)
    return AI::Answer::YES;

  if(f == AI::Answer::NO && s == AI::Answer::YES)
    return AI::Answer::YES;

  if(f == s && f != AI::Answer::DK)
    return AI::Answer::NO;

  return AI::Answer::DK;
}


AI::AI(Knowledge& knowledgeBase)
  : knowledgeBase(knowledgeBase)
{
}

AI::Answer AI::question(const InputStruct& is)
{
  switch(is.op)
  {
    case LogicOperator::AND:
      return (question(is.childs.front()) && question(is.childs.back()));
    case LogicOperator::OR:
      return (question(is.childs.front()) || question(is.childs.back()));
    case LogicOperator::NOT:
      return !question(is.childs.front());
    case LogicOperator::NONE:
      return sentenceQuestion(is);
    default:
      assert(false);
  }
}

AI::Answer AI::sentenceQuestion(const InputStruct& is)
{
  assert(is.op == LogicOperator::NONE);

  FactList facts = knowledgeBase.findBySentence(is.text);
  for(FactList::const_iterator i = facts.begin(); i != facts.end(); ++i)
  {
    const InputStruct& currFact = *i;
    if(currFact.st == SentenceType::RULE)
    {
      if(currFact.childs.back().getTextFromSimpleSentence() == is.text)
      {
        Answer ans = question(currFact.childs.front());
        if(ans != Answer::DK)
          return ans;
      }
    }
    else
    {
      Answer ans = claimAnswer(is, currFact);
      if(ans != Answer::DK)
        return ans;
    }
  }
  return Answer::DK;
}

AI::Answer AI::claimAnswer(const InputStruct& is, const InputStruct& claim)
{
  std::cout << "Claim: ";
  claim.printInline();
  std::cout << "Prove: ";
  is.printInline();

  switch(claim.op)
  {
  case LogicOperator::NONE:
    if(is.text == claim.text)
    {
      std::cout << "YES" << std::endl;
      return Answer::YES;
    }
    else
    {
      std::cout << "DK" << std::endl;
      return Answer::DK;
    }
  case LogicOperator::NOT:
    return !claimAnswer(is, claim.childs.front());
  case LogicOperator::AND:
    return (claimAnswer(is, claim.childs.front())
            || claimAnswer(is, claim.childs.back()));
  case LogicOperator::OR:
    return (claimAnswer(is, claim.childs.front())
           ^ claimAnswer(is, claim.childs.back()));
  default:
    assert(false);
  }
}

