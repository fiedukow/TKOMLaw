#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/spirit/include/qi_and_predicate.hpp>
#include <boost/spirit/include/qi_hold.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/qi_repeat.hpp>
#include <boost/fusion/tuple.hpp>

#include <iostream>
#include <string>
#include <cstring>
#include <vector>

using namespace boost::spirit;

void show_string(const std::vector<char>& s)
{
  std::string thisstring(s.begin(), s.end());
  std::cout << "Zdanie: " << thisstring << std::endl;
}

void show_int(int i)
{
  std::cout << "Int: " << i << std::endl;
}

int main()
{
  using qi::parse;
  using ascii::char_;

  std::cout << "Krzysiu, powiedz mi jak mam zyc..." << std::endl;

  std::string inputLine;

  getline(std::cin, inputLine);  
  do {
    const char* begin = inputLine.c_str();
    bool result = parse(begin, begin + std::strlen(begin), "Jesli " >> lexeme['"' >> +(char_ - '"') >> '"'][&show_string] >> " to " >> (lexeme['"' >> +(char_ - '"') >> '"'])[&show_string] >> '.');    
    std::cout << (result ? "OK" : "BAD") << std::endl;
    getline(std::cin, inputLine);  
  } while (inputLine != "");
}
