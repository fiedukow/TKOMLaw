#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <string>
#include <map>

#define BOOST_BIND_NO_PLACEHOLDERS
#include <boost/noncopyable.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "InputStruct.h"

class IO;
class Knowledge;

class Interpreter : boost::noncopyable
{
  typedef boost::function<bool (InputStruct&)> Action;
  typedef std::map<SentenceType, Action> ActionMap;

public:
  Interpreter(Knowledge& knowledge);
  bool parseLine(std::string toParse);
  void setOutput(IO* io);

private:
  bool unknownTypeAction(InputStruct& /*is*/);
  bool ruleAction(InputStruct& is);
  bool claimAction(InputStruct& is);
  bool questionAction(InputStruct& is);
  bool searchAction(InputStruct& is);

  ActionMap createActionMap();
  /*const*/ ActionMap actionMap;
  Knowledge& knowledgeBase;
  const IO* io;
};

#endif // INTERPRETER_H
