#include <cassert>
#include <sstream>

#include "InputStruct.h"

/*const*/ std::map<LogicOperator, std::string> LoToStr = logicOperatorMap();
/*const*/ std::map<SentenceType, std::string> StToStr = sentenceTypeMap();

std::map<LogicOperator, std::string> logicOperatorMap()
{
  std::map<LogicOperator, std::string> result;
  result[LogicOperator::NONE] = "NONE";
  result[LogicOperator::NOT] = "NOT";
  result[LogicOperator::AND] = "AND";
  result[LogicOperator::IMPL] = "IMPL";
  result[LogicOperator::OR] = "OR";
  return result;
}

std::map<SentenceType, std::string> sentenceTypeMap()
{
  std::map<SentenceType, std::string> result;
  result[SentenceType::NONE] = "NONE";
  result[SentenceType::RULE] = "RULE";
  result[SentenceType::CLAIM] = "CLAIM";
  result[SentenceType::QUESTION] = "QUESTION";
  result[SentenceType::SEARCH] = "SEARCH";
  return result;
}

InputStruct::InputStruct(const std::vector<char>& s)
  : InputStruct()
{
  text = std::string(s.begin(), s.end());
}

InputStruct::InputStruct()
  : st(SentenceType::NONE),
    op(LogicOperator::NONE)
{}

bool InputStruct::containsSentence(const std::string& sentence) const
{
  assert(sentence != "");

  if(text == sentence)
    return true;

  for(auto& is : childs)
  {
    if(is.containsSentence(sentence))
      return true;
  }

  return false;
}

std::string InputStruct::getTextFromSimpleSentence() const
{
  const InputStruct* current = this;
  while(current->op == LogicOperator::NOT)
    current = &(current->childs.front());

  assert(current->op == LogicOperator::NONE);

  return current->text;
}

bool InputStruct::getIfNegativeFromSimpleSentence() const
{
  bool result = false;
  const InputStruct* current = this;
  while(current->op == LogicOperator::NOT)
  {
    current = &(current->childs.front());
    result = !result;
  }

  assert(current->op == LogicOperator::NONE);
  return result;
}

std::string InputStruct::toString() const
{
  std::stringstream ss;
  switch(op)
  {
    case LogicOperator::NONE:
      ss << '\"' << text << '\"';
      break;
    case LogicOperator::NOT:
      ss << "nie ";
      ss << childs.front().toString();
      break;
    case LogicOperator::AND:
      ss << childs.front().toString();
      ss << " i ";
      ss << childs.back().toString();
      break;
    case LogicOperator::OR:
      ss << childs.front().toString();
      ss << " lub ";
      ss << childs.back().toString();
      break;
    case LogicOperator::IMPL:
      ss << "Jesli ";
      ss << childs.front().toString();
      ss << " to ";
      ss << childs.back().toString();
      break;
    default:
      assert(false);
  }

  return ss.str();
}

/*
void InputStruct::print(int tab) const
{
  tabs(tab);
  std::cout << "*** BEGIN ***" << std::endl;
  if(st != SentenceType::NONE)
  {
    tabs(tab);
    std::cout << "SENTENCE_TYPE = " << StToStr[st] << std::endl;
  }
  if(op != LogicOperator::NONE)
  {
    tabs(tab);
    std::cout << "OPERATOR = " << LoToStr[op] << std::endl;
  }
  if(text != "")
  {
    tabs(tab);
    std::cout << "TEXT = " << text << std::endl;
  }
  if(childs.size() > 0)
  {
    tabs(tab);
    std::cout << "Dzieci " << childs.size() << ":" << std::endl;
    tabs(tab);
    std::cout << "{" << std::endl;
    for(auto& is : childs)
      is.print(tab + 1);
    tabs(tab);
    std::cout << "}" << std::endl;
  }
  tabs(tab);
  std::cout << "*** END ***" << std::endl;
}

void InputStruct::printInline(bool newline) const
{
  switch(op)
  {
    case LogicOperator::NONE:
      std::cout << '\"' << text << '\"';
      break;
    case LogicOperator::NOT:
      std::cout << "nie ";
      childs.front().toString();
      break;
    case LogicOperator::AND:
      childs.front().toString();
      std::cout << " i ";
      childs.back().toString();
      break;
    case LogicOperator::OR:
      childs.front().toString();
      std::cout << " lub ";
      childs.back().toString();
      break;
    case LogicOperator::IMPL:
      std::cout << "Jesli ";
      childs.front().toString();
      std::cout << " to ";
      childs.back().toString();
      break;
    default:
      assert(false);
  }

  if(newline)
    std::cout << "." << std::endl;
}


void InputStruct::tabs(int tab)
{
  while(tab--)
    std::cout << "\t";
}*/

