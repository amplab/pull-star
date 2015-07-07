#include "regex_parser.h"

// Parse exception
const char* pull_star::ParseException::what() const throw () {
  return std::string("Error parsing regular expression.").c_str();
}

pull_star::RegExParser::RegExParser(char *exp) {
  this->expression_ = exp;
  this->blank_regex_ = new RegExBlank();
}

pull_star::RegExParser::~RegExParser() {
  delete blank_regex_;
}

pull_star::RegEx *pull_star::RegExParser::parse() {
  return regex();
}

char pull_star::RegExParser::peek() {
  return expression_[0];
}

void pull_star::RegExParser::eat(char c) {
  if (peek() == c) {
    expression_ = expression_ + 1;
  } else {
    throw new ParseException;
  }
}

char pull_star::RegExParser::next() {
  char c = peek();
  eat(c);
  return c;
}

bool pull_star::RegExParser::more() {
  return (strlen(expression_) > 0);
}

pull_star::RegEx* pull_star::RegExParser::regex() {
  RegEx *t = term();
  if (more() && peek() == '|') {
    eat('|');
    RegEx *r = regex();
    return new RegExUnion(t, r);
  }
  return t;
}

pull_star::RegEx* pull_star::RegExParser::concat(RegEx *a, RegEx *b) {
  if (a->getType() == RegExType::Blank) {
    return b;
  } else if (b->getType() == RegExType::Blank) {
    return a;
  } else if (a->getType() == RegExType::Primitive
      && b->getType() == RegExType::Primitive) {
    if (((RegExPrimitive *) a)->getPrimitiveType() == RegExPrimitiveType::Mgram
        && ((RegExPrimitive *) b)->getPrimitiveType()
            == RegExPrimitiveType::Mgram) {
      std::string a_str = ((RegExPrimitive *) a)->getPrimitive();
      std::string b_str = ((RegExPrimitive *) b)->getPrimitive();
      RegEx *ret = new RegExPrimitive(a_str + b_str);
      delete a;
      delete b;
      return ret;
    }
  }
  return new RegExConcat(a, b);
}

pull_star::RegEx* pull_star::RegExParser::term() {
  RegEx *f = blank_regex_;
  while (more() && peek() != ')' && peek() != '|') {
    RegEx *next_f = factor();
    f = concat(f, next_f);
  }
  return f;
}

pull_star::RegEx* pull_star::RegExParser::factor() {
  RegEx *b = base();

  if (more() && peek() == '*') {
    eat('*');
    b = new RegExRepeat(b, RegExRepeatType::ZeroOrMore);
  } else if (more() && peek() == '+') {
    eat('+');
    b = new RegExRepeat(b, RegExRepeatType::OneOrMore);
  } else if (more() && peek() == '{') {
    eat('{');
    int min = nextInt();
    eat(',');
    int max = nextInt();
    eat('}');
    b = new RegExRepeat(b, RegExRepeatType::MinToMax, min, max);
  }

  return b;
}

pull_star::RegEx* pull_star::RegExParser::base() {
  if (peek() == '(') {
    eat('(');
    RegEx *r = regex();
    eat(')');
    return r;
  }
  return primitive();
}

char pull_star::RegExParser::nextChar() {
  if (peek() == '\\') {
    eat('\\');
  }
  return next();
}

int pull_star::RegExParser::nextInt() {
  int num = 0;
  while (peek() >= 48 && peek() <= 57) {
    num = num * 10 + (next() - 48);
  }
  return num;
}

pull_star::RegEx* pull_star::RegExParser::primitive() {
  std::string p = "";
  while (more() && peek() != '|' && peek() != '(' && peek() != ')'
      && peek() != '*' && peek() != '+' && peek() != '{' && peek() != '}') {
    p += nextChar();
  }
  return parsePrimitives(p);
}

pull_star::RegEx* pull_star::RegExParser::parsePrimitives(std::string p_str) {
  RegEx* p = nextPrimitive(p_str);
  if (p_str.length() != 0) {
    return new RegExConcat(p, parsePrimitives(p_str));
  }
  return p;
}

pull_star::RegEx* pull_star::RegExParser::nextPrimitive(std::string &p_str) {
  if (p_str[0] == '.') {
    p_str = p_str.substr(1);
    return new RegExPrimitive(".", RegExPrimitiveType::Dot);
  } else if (p_str[0] == '[') {
    size_t p_pos = 1;
    while (p_pos < p_str.length() && p_str[p_pos] != ']') {
      p_pos++;
    }
    if (p_pos == p_str.length())
      throw new ParseException;
    std::string str = p_str.substr(1, p_pos - 1);
    p_str = p_str.substr(p_pos + 1);
    return new RegExPrimitive(str, RegExPrimitiveType::Range);
  }
  size_t p_pos = 0;
  while (p_pos < p_str.length() && p_str[p_pos] != '.' && p_str[p_pos] != '['
      && p_str[p_pos] != ']') {
    p_pos++;
  }
  std::string str = p_str.substr(0, p_pos);
  p_str = p_str.substr(p_pos);
  return new RegExPrimitive(str, RegExPrimitiveType::Mgram);
}

pull_star::RegExPrimitiveType pull_star::RegExParser::getPrimitiveType(
    std::string mgram) {
  if (mgram == ".") {
    return RegExPrimitiveType::Dot;
  } else if (mgram[0] == '[') {
    if (mgram[mgram.length() - 1] != ']')
      throw new ParseException;
    return RegExPrimitiveType::Range;
  }
  return RegExPrimitiveType::Mgram;
}
