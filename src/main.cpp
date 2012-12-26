#include <iostream>
#include <string>

#include "Interpreter.h"
#include "Knowledge.h"

int main()
{
  std::string inputLine;
  Knowledge bazaWiedzy;
  Interpreter interpreter(bazaWiedzy);

  getline(std::cin, inputLine);
  do {
    interpreter.parseLine(inputLine);
    getline(std::cin, inputLine);
  } while (inputLine != "");
}
