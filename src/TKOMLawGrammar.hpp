#pragma once
#ifndef _TKOM_LAW_GRAMMAR_HPP_
#define _TKOM_LAW_GRAMMAR_HPP_

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

#include "InputStruct.h"

using namespace boost::spirit;
using namespace boost;

template <typename Iterator>
struct TKOMLawGrammar : public qi::grammar<Iterator, InputStruct()>
{
  TKOMLawGrammar() : TKOMLawGrammar::base_type(zdanie)
  {
    using ascii::char_;
    using qi::_1;
    using qi::_val;
    using phoenix::at_c;
    using phoenix::push_back;

    zdanie =
      (
        (zdanie_twierdzace)[_val=_1, at_c<0>(_val) = SentenceType::CLAIM]
        |
        (regula)[_val=_1, at_c<0>(_val) = SentenceType::RULE]
        |
        (pytanie)[_val=_1, at_c<0>(_val) = SentenceType::QUESTION]
        |
        (wyszukiwanie)[_val=_1, at_c<0>(_val) = SentenceType::SEARCH]
      );

    regula =
      (
        qi::string("Jesli ") >>
        suma_logiczna [push_back(at_c<3>(_val), _1)] >>
        qi::string(" to ") [at_c<2>(_val) = LogicOperator::IMPL] >>
        zdanie_proste [push_back(at_c<3>(_val), _1)] >>
        qi::string(".")
      );

    zdanie_twierdzace =
      (
        suma_logiczna[_val=_1] >>
        qi::string(".")
      );

    pytanie =
      (
        zdanie_proste[_val=_1] >>
        qi::string("?")
      );

    wyszukiwanie =
      (
        qi::string("Co wiesz o ") >>
        zdanie_jezykowe[_val=_1] >>
        qi::string("?")
      );

    suma_logiczna =
      (
        (
          iloczyn_logiczny[push_back(at_c<3>(_val), _1)] >>
          qi::string(" lub ")[at_c<2>(_val) = LogicOperator::OR] >>
          suma_logiczna[push_back(at_c<3>(_val), _1)]
        )
        |
        (
          iloczyn_logiczny[_val=_1]
        )
      );

    iloczyn_logiczny =
      (
        (
          zdanie_proste [push_back(at_c<3>(_val), _1)] >>
          qi::string(" i ") [at_c<2>(_val) = LogicOperator::AND] >>
          iloczyn_logiczny [push_back(at_c<3>(_val), _1)]
        )
        |
        (
          zdanie_proste[_val=_1]
        )
      );

    zdanie_proste =
      (
        (
          qi::string("nie ") [at_c<2>(_val) = LogicOperator::NOT] >>
          zdanie_proste [push_back(at_c<3>(_val), _1)]
        )
        |
        (
          zdanie_jezykowe[_val=_1]
        )
      );

    zdanie_jezykowe =
      (
        lexeme['"' >> +(char_ - '"') >> '"'][_val=_1]
      );
  }

  qi::rule<Iterator, InputStruct()> zdanie;
  qi::rule<Iterator, InputStruct()> regula;
  qi::rule<Iterator, InputStruct()> zdanie_twierdzace;
  qi::rule<Iterator, InputStruct()> pytanie;
  qi::rule<Iterator, InputStruct()> wyszukiwanie;
  qi::rule<Iterator, InputStruct()> suma_logiczna;
  qi::rule<Iterator, InputStruct()> iloczyn_logiczny;
  qi::rule<Iterator, InputStruct()> zdanie_proste;
  qi::rule<Iterator, InputStruct()> zdanie_jezykowe;
};

typedef TKOMLawGrammar<std::string::iterator> TKOMLawGrammarString;


#endif
