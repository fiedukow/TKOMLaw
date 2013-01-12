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

AI::Answer AI::ask(const InputStruct &is, AnswerStack& stack)
{
  return question(is, stack);
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

      /*
       * jesli nie jestesmy w stanie ustalic wartosci logicznej ktoregos
       * ze zdan, mozemy sprobowac zalozyc ze jedno z nich jest faktem
       * jesli wtedy wyniknie nam, że drugie jest falszywe to znaczy
       * że całe zdanie jest fałszywe - w kazdym innym wypadku nadal nie wiemy
       * nic. Uzywamy wnioskowania nie wprost.
       */

      {
        TmpFactPusher f(knowledgeBase, is.childs.front(), stack);
        ans = question(is.childs.back(), stack);
      }
      if(ans == Answer::NO)
        return ans;

      {
        TmpFactPusher f(knowledgeBase, is.childs.back(), stack);
        ans = question(is.childs.front(), stack);
      }
      if(ans == Answer::NO)
        return ans;

      return Answer::DK;
    }
    case LogicOperator::OR:
    {
      Answer ans;
      Answer leftAns = question(is.childs.front(), stack);
      Answer rightAns = question(is.childs.back(), stack);
      ans = (leftAns || rightAns);
      if(ans != Answer::DK)
        return ans;

      /*
       * Jesli nie ustalimy wyniku klasycznie tj. ani lewe ani prawe podzdanie
       * nie da wyniku pozytywnego a takze przynajmniej jedno da wynik DK
       * istnieje szansa ze zdania laczy wewnętrzna zależność.
       * Szansa ta może się pojawić tylko wtedy gdy oba podzdania mają wartość
       * DK, ponieważ w przeciwnym razie objawiła by się w normalnym wnioskowaniu.
       * Dlatego gdy oba zdania mają wartość DK i po dodaniu jednego z tych zdan
       * do bazy wiedzy drugie staje się fałszywe - alternatywa jest prawdziwa.
       */
      if ((leftAns && rightAns) == Answer::DK)
      {
        {
          TmpFactPusher f(knowledgeBase, is.childs.front(), stack);
          ans = question(is.childs.back(), stack);
        }
        if(ans == Answer::NO)
          return Answer::YES;

        {
          TmpFactPusher f(knowledgeBase, is.childs.back(), stack);
          ans = question(is.childs.front(), stack);
        }
        if(ans == Answer::NO)
          return Answer::YES;
      }

      return Answer::DK;
    }
    case LogicOperator::NOT:
      return !question(is.childs.front(), stack);
    case LogicOperator::IMPL:
    {
      TmpFactPusher f(knowledgeBase, is.childs.front(), stack);
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

  FactPtrList facts = knowledgeBase.findBySentence(is.text); //fakty powiazane
                                                             // ze zdaniem
                                                             // o ktore pytamy
  for(FactPtrList::const_iterator i = facts.begin(); i != facts.end(); ++i)
  {
    const InputStruct* currFact = *i;
    if(std::find(stack.begin(), stack.end(), currFact) != stack.end())
    {
      continue; //nie wykorzystuj faktow ktore probujesz udowodnic
    }

    if(currFact->st == SentenceType::RULE)
    {
      //korzystamy z reguly tylko gdy nastepnik dotyczy nas
      if(currFact->childs.back().getTextFromSimpleSentence() == is.text)
      {
        stack.push_back(currFact);
        Answer ans = question(currFact->childs.front(), stack);
        stack.pop_back();
        bool neg = currFact->childs.back().getIfNegativeFromSimpleSentence();
        if(neg)
          ans = !ans;
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
  {
    return Answer::DK;
  }

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

    //TODO: May be optymalized by renumbering which one is the one we want to
    // claim

    /*
     * sprawdzamy czy mozemy stwierdzic negatywnosc podzdania ktore nie zawiera
     * naszego pytania - jesli mozemy to znaczy ze wystarczy ze udowodnimy
     * nasza teze tym drugim pod zdaniem bo ono jest na pewno prawdziwe
     */
    ans = (!question(claim.childs.front(), stack));
    if(ans == Answer::YES)
    {
      ans = claimAnswer(is, claim.childs.back(), stack);
      if(ans != Answer::DK)
      {
        stack.pop_back();
        return ans;
      }
    }

    ans = (!question(claim.childs.back(), stack));
    if(ans == Answer::YES)
    {
      ans = claimAnswer(is, claim.childs.front(), stack);
      if(ans != Answer::DK)
      {
        stack.pop_back();
        return ans;
      }
    }

    //sprawdzenie rownowaznosci
    {
      TmpFactPusher f(knowledgeBase, claim.childs.front(), stack);
      ans = question(is, stack);
    }
    if(ans != Answer::YES)
    {
      stack.pop_back();
      return Answer::DK;
    }

    {
      TmpFactPusher f(knowledgeBase, claim.childs.back(), stack);
      ans = question(is, stack);
    }
    //Koniec sprawdzenia rownowaznosci

    stack.pop_back();

    if(ans == Answer::YES)
      return ans;

    return Answer::DK;
  default:
    assert(false);
  }
}

