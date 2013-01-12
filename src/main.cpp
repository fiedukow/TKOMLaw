#include "Interpreter.h"
#include "Knowledge.h"
#include "IO.h"

int main()
{
  std::string inputLine;
  Knowledge bazaWiedzy;
  Interpreter interpreter(bazaWiedzy);
  IO io(interpreter);
  interpreter.setOutput(&io);
  io();
}
