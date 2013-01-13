#include <cassert>
#include <algorithm>
#include <iostream>

#include "AI.h"

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

AI::Answer AI::ask(const InputStruct &is,
                   AnswerStack* toSaveResultTrack)
{
  AnswerStack stack;
  return ask(is, stack, toSaveResultTrack);
}

AI::Answer AI::ask(const InputStruct &is,
                   AnswerStack& stack,
                   AnswerStack* toSaveResultTrack)
{
  return question(is, stack, toSaveResultTrack);
}

AI::Answer AI::question(const InputStruct& is,
                        AnswerStack& stack,
                        AnswerStack* toSaveResultTrack)
{
  switch(is.op)
  {
    case LogicOperator::AND:
    {
      Answer ans;
      ans = (question(is.childs.front(), stack, toSaveResultTrack)
             && question(is.childs.back(), stack, toSaveResultTrack));
      if(ans != Answer::DK)
        return ans;

      /*
       * jesli nie jestesmy w stanie ustalic wartosci logicznej ktoregos
       * ze zdan, mozemy sprobowac zalozyc ze jedno z nich jest faktem
       * jesli wtedy wyniknie nam, że drugie jest falszywe to znaczy
       * że całe zdanie jest fałszywe - w kazdym innym wypadku nadal nie wiemy
       * nic. Uzywamy wnioskowania nie wprost.
       */
      try
      {
        TmpFactPusher f(knowledgeBase, is.childs.front(), stack);
        ans = question(is.childs.back(), stack, NULL);
      }
      catch(...)
      {
        assert(false);
      }

      if(ans == Answer::NO)
      {
        return ans;
      }

      try
      {
        TmpFactPusher f(knowledgeBase, is.childs.back(), stack);
        ans = question(is.childs.front(), stack, NULL);
      }
      catch(...)
      {
        assert(false);
      }

      if(ans == Answer::NO)
      {
        return ans;
      }

      return Answer::DK;
    }
    case LogicOperator::OR:
    {
      Answer ans;
      Answer leftAns = question(is.childs.front(), stack, toSaveResultTrack);
      Answer rightAns = question(is.childs.back(), stack, toSaveResultTrack);
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
        try
        {
          TmpFactPusher f(knowledgeBase, is.childs.front(), stack);
          ans = question(is.childs.back(), stack, NULL);
        }
        catch(...)
        {
          ans = Answer::DK;
        }

        if(ans == Answer::NO)
          return Answer::YES;

        try
        {
          TmpFactPusher f(knowledgeBase, is.childs.back(), stack);
          ans = question(is.childs.front(), stack, NULL);
        }
        catch(...)
        {
          ans = Answer::DK;
        }
        if(ans == Answer::NO)
          return Answer::YES;
      }

      return Answer::DK;
    }
    case LogicOperator::NOT:
      return !question(is.childs.front(), stack, toSaveResultTrack);
    case LogicOperator::IMPL:
    try
    {
      TmpFactPusher f(knowledgeBase, is.childs.front(), stack);
      return question(is.childs.back(), stack, NULL);
    }
    catch(...)
    {
      return Answer::DK;
    }

    case LogicOperator::NONE:
      return sentenceQuestion(is, stack, toSaveResultTrack);
    default:
      assert(false);
  }
}

AI::Answer AI::sentenceQuestion(const InputStruct& is,
                                AnswerStack& stack,
                                AnswerStack* toSaveResultTrack)
{
  if(isOnStack(stack, &is))
    return Answer::DK;

  assert(is.op == LogicOperator::NONE);

  FactPtrList facts = knowledgeBase.findBySentence(is.text); //fakty powiazane
                                                             // ze zdaniem
                                                             // o ktore pytamy
  for(FactPtrList::const_iterator i = facts.begin(); i != facts.end(); ++i)
  {
    const InputStruct* currFact = *i;

    if(isOnStack(stack, currFact))
    {
      continue; //nie wykorzystuj faktow ktore probujesz udowodnic
    }

    if(toSaveResultTrack)
    {
      resetStackFromLvl(toSaveResultTrack, stack.size());
      toSaveResultTrack->push_back(currFact);
    }

    if(currFact->st == SentenceType::RULE)
    {
      //korzystamy z reguly tylko gdy nastepnik dotyczy nas
      if(currFact->childs.back().getTextFromSimpleSentence() == is.text)
      {
        stack.push_back(currFact);
        Answer ans = question(currFact->childs.front(), stack, toSaveResultTrack);
        stack.pop_back();
        bool neg = currFact->childs.back().getIfNegativeFromSimpleSentence();
        if(ans == Answer::YES)
        {
          if(neg)
            ans = !ans;
          return ans;
        }
      }

      //mozemy tez z niej skorzystac jesli nastepnik jest falszywy
      //wtedy wiemy ze poprzednik jest falszywy
      stack.push_back(currFact);
      if(question(currFact->childs.back(), stack, toSaveResultTrack)
         == Answer::NO)
      {
        Answer ans = claimAnswer(is, currFact->childs.front(), stack, toSaveResultTrack);
        if(ans == Answer::YES)
        {
          stack.pop_back();
          return !ans;
        }
      }
      stack.pop_back();
    }
    else
    {
      Answer ans = claimAnswer(is, *currFact, stack, toSaveResultTrack);
      if(ans != Answer::DK)
        return ans;
    }
  }
  return Answer::DK;
}

AI::Answer AI::claimAnswer(const InputStruct& is,
                           const InputStruct& claim,
                           AnswerStack& stack,
                           AnswerStack* toSaveResultTrack)
{
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
    return !claimAnswer(is, claim.childs.front(), stack, toSaveResultTrack);
  case LogicOperator::AND:
    return anyKnown(claimAnswer(is, claim.childs.front(), stack, toSaveResultTrack),
                    claimAnswer(is, claim.childs.back(), stack, toSaveResultTrack));
  case LogicOperator::OR:
    Answer ans;
    stack.push_back(&claim);

    /*
     * sprawdzamy czy mozemy stwierdzic negatywnosc podzdania ktore nie zawiera
     * naszego pytania - jesli mozemy to znaczy ze wystarczy ze udowodnimy
     * nasza teze tym drugim pod zdaniem bo ono jest na pewno prawdziwe
     */
    ans = (!question(claim.childs.front(), stack, toSaveResultTrack));
    if(ans == Answer::YES)
    {
      ans = claimAnswer(is, claim.childs.back(), stack, toSaveResultTrack);
      if(ans != Answer::DK)
      {
        stack.pop_back();
        return ans;
      }
    }

    ans = (!question(claim.childs.back(), stack, toSaveResultTrack));
    if(ans == Answer::YES)
    {
      ans = claimAnswer(is, claim.childs.front(), stack, toSaveResultTrack);
      if(ans != Answer::DK)
      {
        stack.pop_back();
        return ans;
      }
    }

    //sprawdzenie rownowaznosci
    try
    {
      TmpFactPusher f(knowledgeBase, claim.childs.front(), stack);
      ans = question(claim.childs.back(), stack, NULL);
    }
    catch(...)
    {
      ans = Answer::NO;
    }

    if(ans != Answer::YES)
    {
      stack.pop_back();
      return Answer::DK;
    }

    try
    {
      TmpFactPusher f(knowledgeBase, claim.childs.back(), stack);
      ans = question(claim.childs.front(), stack, NULL);
    }
    catch(...)
    {
      ans = Answer::NO;
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

void AI::resetStackFromLvl(AnswerStack* stack, int lvl)
{
  assert(lvl >= 0);
  int toRm = stack->size() - lvl;
  while(toRm-- > 0)
  {
    stack->pop_back();
  }
}

bool AI::isOnStack(AnswerStack& stack, const InputStruct* is)
{
  /*std::string isStr = is->toString();
  for(auto& i : stack)
  {
    if(isStr == i->toString())
      return true;
  }
  return false;*/
  return (std::find(stack.begin(), stack.end(), is) != stack.end());
}

ScopeStackAdder::ScopeStackAdder(AnswerStack& stack, const InputStruct* is)
  : stack(stack)
{
  stack.push_back(is);
}

ScopeStackAdder::~ScopeStackAdder()
{
  stack.pop_back();
}
