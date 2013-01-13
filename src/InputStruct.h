#pragma once
#ifndef _INPUTSTRUCT_H_
#define _INPUTSTRUCT_H_

#include <string>
#include <list>
#include <vector>
#include <map>

#include <boost/fusion/include/adapt_struct.hpp>

enum class LogicOperator { NONE, NOT, AND, IMPL, OR };
enum class SentenceType { NONE, RULE, CLAIM, QUESTION, SEARCH };

struct InputStruct
{
  InputStruct(const std::vector<char>& s);
  InputStruct();

  bool containsSentence(const std::string& sentence) const;
  std::string getTextFromSimpleSentence() const;
  bool getIfNegativeFromSimpleSentence() const;

  std::string toString() const;

  SentenceType st;
  std::string text;
  LogicOperator op;
  std::list<InputStruct> childs;
};

BOOST_FUSION_ADAPT_STRUCT(
    InputStruct,
    (SentenceType, st)
    (std::string, text)
    (LogicOperator, op)
    (std::list<InputStruct>, childs)
)

std::map<LogicOperator, std::string> logicOperatorMap();
std::map<SentenceType, std::string> sentenceTypeMap();

#endif

