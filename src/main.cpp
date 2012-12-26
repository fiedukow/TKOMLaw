#include <iostream>
#include <string>

#include "Interpreter.h"

int main()
{
  std::string inputLine;
  Interpreter& interpreter = Interpreter::getInstance();
  getline(std::cin, inputLine);
  do {
    interpreter.parseLine(inputLine);
    getline(std::cin, inputLine);
  } while (inputLine != "");
}
