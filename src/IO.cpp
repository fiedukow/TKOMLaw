#include "IO.h"
#include <iostream>
#include <string>

#include "Interpreter.h"

/*const*/ std::map<AI::Answer, std::string> IO::AnsToStr = answerMap();

std::map<AI::Answer, std::string> answerMap()
{
  std::map<AI::Answer, std::string> result;
  result[AI::Answer::YES] = "PRAWDA";
  result[AI::Answer::NO] = "NIE PRAWDA";
  result[AI::Answer::DK] = "NIE WIADOMO";
  return result;
}

IO::IO(Interpreter& interpreter)
  : interpreter(interpreter)
{
}

void IO::operator()() const
{
  std::string inputLine;
  getline(std::cin, inputLine);
  do {
    interpreter.parseLine(inputLine);
    getline(std::cin, inputLine);
  } while (inputLine != "");
}

void IO::addedRule(std::string rule) const
{
  std::cout << "Dodano regule: " << rule << std::endl;
}

void IO::answer(AI::Answer ans) const
{
  std::cout << AnsToStr[ans] << std::endl;
}

void IO::conflictRule(std::string rule, std::list<std::string> stack) const
{
  int count = 0;
  std::cout << "Nie moge dodac reguły " << rule << std::endl;
  std::cout << "Konflikt: " << std::endl;
  for(auto& i : stack)
    std::cout << "#" << ++count << " " << i << std::endl;
}

void IO::unknownCommand(std::string command) const
{
  std::cout << "Komenda nie zgodna ze skladnia: " << command << std::endl;
}

void IO::informationsAbout(std::string sentence,
                           std::list<std::string> info) const
{
  int count = 0;
  std::cout << "Na temat: "
            << sentence
            << ", wiem nastepujace rzeczy:"
            << std::endl;
  for(auto& i : info)
    std::cout << "#" << ++count << " " << i << std::endl;
}
