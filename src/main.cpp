#include "Interpreter.h"
#include "Knowledge.h"
#include "IO.h"

int main(int argc, char* argv[])
{
  std::string inputLine;
  Knowledge bazaWiedzy;
  Interpreter interpreter(bazaWiedzy);
  IO io(interpreter, ((argc > 1) ? std::string(argv[1]) + std::string(" ")
                                 : ""));
  interpreter.setOutput(&io);
  io();
}
