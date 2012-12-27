#include <cassert>
#include <iostream>
#include <algorithm>

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

  if(f == AI::Answer::NO && s == AI::Answer::NO)
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

AI::Answer anyKnown(AI::Answer f, AI::Answer s)
{
  assert(!(f != AI::Answer::DK && s != AI::Answer::DK && f!=s));

  if(f == AI::Answer::YES || s == AI::Answer::YES)
    return AI::Answer::YES;

  if(f == AI::Answer::NO || s == AI::Answer::NO)
    return AI::Answer::NO;

  return AI::Answer::DK;
}

AI::AI(Knowledge& knowledgeBase)
  : knowledgeBase(knowledgeBase)
{
}

AI::Answer AI::ask(const InputStruct &is)
{
  AnswerStack stack;
  AI::Answer answer = question(is, stack);
  return answer;
}

AI::Answer AI::question(const InputStruct& is, AnswerStack& stack)
{
  switch(is.op)
  {
    case LogicOperator::AND:
    {
      Answer ans;
      ans = (question(is.childs.front(), stack)
             && question(is.childs.back(), stack));
      if(ans != Answer::DK)
        return ans;

      {
        TmpFactPusher f(knowledgeBase, is.childs.front());
        ans = question(is.childs.back(), stack);
      }
      if(ans == Answer::NO)
        return ans;

      {
        TmpFactPusher f(knowledgeBase, is.childs.back());
        ans = question(is.childs.front(), stack);
      }
      if(ans == Answer::NO)
        return ans;

      return Answer::DK;
    }
    case LogicOperator::OR:
    {
      Answer ans;
      ans = (question(is.childs.front(), stack)
            || question(is.childs.back(), stack));
      if(ans != Answer::DK)
        return ans;

      {
        TmpFactPusher f(knowledgeBase, is.childs.front());
        ans = question(is.childs.back(), stack);
      }
      if(ans == Answer::NO)
        return Answer::YES;

      {
        TmpFactPusher f(knowledgeBase, is.childs.back());
        ans = question(is.childs.front(), stack);
      }
      if(ans == Answer::NO)
        return Answer::YES;

      return Answer::DK;
    }
    case LogicOperator::NOT:
      return !question(is.childs.front(), stack);
    case LogicOperator::IMPL:
    {
      TmpFactPusher f(knowledgeBase, is.childs.front());
      return question(is.childs.back(), stack);
    }
    case LogicOperator::NONE:
      return sentenceQuestion(is, stack);
    default:
      assert(false);
  }
}

AI::Answer AI::sentenceQuestion(const InputStruct& is, AnswerStack& stack)
{
  assert(is.op == LogicOperator::NONE);

  FactPtrList facts = knowledgeBase.findBySentence(is.text);
  for(FactPtrList::const_iterator i = facts.begin(); i != facts.end(); ++i)
  {
    const InputStruct* currFact = *i;
    if(std::find(stack.begin(), stack.end(), currFact) != stack.end())
      continue;
    if(currFact->st == SentenceType::RULE)
    {
      if(currFact->childs.back().getTextFromSimpleSentence() == is.text)
      {
        stack.push_back(currFact);
        Answer ans = question(currFact->childs.front(), stack);
        stack.pop_back();
        if(ans != Answer::DK)
          return ans;
      }
    }
    else
    {
      Answer ans = claimAnswer(is, *currFact, stack);
      if(ans != Answer::DK)
        return ans;
    }
  }
  return Answer::DK;
}

AI::Answer AI::claimAnswer(const InputStruct& is,
                           const InputStruct& claim,
                           AnswerStack& stack)
{
  if(std::find(stack.begin(), stack.end(), &claim) != stack.end())
    return Answer::DK;

  switch(claim.op)
  {
  case LogicOperator::NONE:
    if(is.text == claim.text)
    {
      return Answer::YES;
    }
    else
    {
      return Answer::DK;
    }
  case LogicOperator::NOT:
    return !claimAnswer(is, claim.childs.front(), stack);
  case LogicOperator::AND:
    return anyKnown(claimAnswer(is, claim.childs.front(), stack),
                    claimAnswer(is, claim.childs.back(), stack));
  case LogicOperator::OR:
    Answer ans;

    stack.push_back(&claim);
    ans = (!question(claim.childs.front(), stack)
           && claimAnswer(is, claim.childs.back(), stack));
    if(ans == Answer::YES)
    {
      return ans;
    }

    ans = (!question(claim.childs.back(), stack)
           && claimAnswer(is, claim.childs.front(), stack));
    if(ans == Answer::YES)
    {
      return ans;
    }

    //Sprawdzenie rownowaznosci
    {
      TmpFactPusher f(knowledgeBase, claim.childs.front());
      ans = question(is, stack);
    }
    if(ans != Answer::YES)
    {
      stack.pop_back();
      return Answer::DK;
    }

    {
      TmpFactPusher f(knowledgeBase, claim.childs.back());
      ans = question(is, stack);
    }
    //Koniec sprawdzenia rownowaznosci

    if(ans == Answer::YES)
      return ans;

    stack.pop_back();
    return Answer::DK;
  default:
    assert(false);
  }
}

