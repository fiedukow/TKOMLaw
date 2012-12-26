#include <iostream>

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

void InputStruct::print(int tab)
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
    for(auto is : childs)
      is.print(tab + 1);
    tabs(tab);
    std::cout << "}" << std::endl; 
  }
  tabs(tab);
  std::cout << "*** END ***" << std::endl;
}

void InputStruct::tabs(int tab)
{
  while(tab--)
    std::cout << "\t";
}

