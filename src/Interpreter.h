#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <string>

#include <boost/noncopyable.hpp>

class Interpreter : boost::noncopyable
{
public:
  static Interpreter& getInstance();
  bool parseLine(std::string toParse);

private:
  Interpreter();
};

#endif // INTERPRETER_H
