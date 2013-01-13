#include "Interpreter.h"
#include "Knowledge.h"
#include "IO.h"
#include <string>

int main(int argc, char* argv[])
{
  std::string inputLine;
  Knowledge bazaWiedzy;
  Interpreter interpreter(bazaWiedzy);
  IO io(interpreter, argc, argv);
  interpreter.setOutput(&io);
  io();
}
