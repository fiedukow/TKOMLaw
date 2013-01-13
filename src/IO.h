#ifndef IO_H
#define IO_H
#include "AI.h"

class Interpreter;

class IO
{
public:
  IO(Interpreter& interpreter, int argc, char* argv[]);
  void operator()() const;

  void addedRule(std::string rule) const;
  void answer(AI::Answer ans) const;
  void conflictRule(std::string rule, std::list<std::string> stack) const;
  void informationsAbout(std::string sentence, std::list<std::string> info) const;
  void unknownCommand(std::string command) const;
private:
  Interpreter& interpreter;
  bool verbose;
  bool inc;
  static /*const*/ std::map<AI::Answer, std::string> AnsToStr;
};

std::map<AI::Answer, std::string> answerMap();

#endif // IO_H
