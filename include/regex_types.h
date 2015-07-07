#ifndef PULL_STAR_REGEX_TYPES_H_
#define PULL_STAR_REGEX_TYPES_H_

#include <iostream>

namespace pull_star {

enum RegExType {
  Union,
  Concat,
  Repeat,
  Primitive,
  Blank
};

class RegEx {
 public:

  RegEx(RegExType re_type) {
    this->re_type = re_type;
  }

  RegExType getType() {
    return re_type;
  }

 private:
  RegExType re_type;
};

class RegExUnion : public RegEx {
 private:
  RegEx *first;
  RegEx *second;

 public:
  RegExUnion(RegEx *first, RegEx *second)
      : RegEx(RegExType::Union) {
    this->first = first;
    this->second = second;
  }

  RegEx *getFirst() {
    return first;
  }

  RegEx *getSecond() {
    return second;
  }
};

class RegExConcat : public RegEx {
 private:
  RegEx *first;
  RegEx *second;

 public:
  RegExConcat(RegEx *first, RegEx *second)
      : RegEx(RegExType::Concat) {
    this->first = first;
    this->second = second;
  }

  RegEx *getLeft() {
    return first;
  }

  RegEx *getRight() {
    return second;
  }
};

enum RegExRepeatType {
  ZeroOrMore,
  OneOrMore,
  MinToMax
};

class RegExRepeat : public RegEx {
 private:
  RegEx *internal;
  RegExRepeatType r_type;
  int min;
  int max;

 public:
  RegExRepeat(RegEx *internal, RegExRepeatType r_type, int min = -1, int max =
                  -1)
      : RegEx(RegExType::Repeat) {
    this->internal = internal;
    this->r_type = r_type;
    this->min = min;
    this->max = max;
  }

  RegEx *getInternal() {
    return internal;
  }

  RegExRepeatType getRepeatType() {
    return r_type;
  }

  int getMin() {
    return min;
  }

  int getMax() {
    return max;
  }
};

enum RegExPrimitiveType {
  Mgram,
  Dot,
  Range
};

class RegExPrimitive : public RegEx {
 private:
  std::string primitive;
  RegExPrimitiveType p_type;

 public:
  RegExPrimitive(std::string primitive, RegExPrimitiveType p_type =
                     RegExPrimitiveType::Mgram)
      : RegEx(RegExType::Primitive) {
    this->p_type = p_type;
    if (p_type == RegExPrimitiveType::Range) {
      // Expand all ranges
      std::string buf = "";
      for (size_t i = 0; i < primitive.length(); i++) {
        if (primitive[i] == '-')
          for (char c = primitive[i - 1] + 1; c < primitive[i + 1]; c++)
            buf += c;
        else
          buf += primitive[i];
      }
      primitive = buf;
    }
    this->primitive = primitive;
  }

  std::string getPrimitive() {
    return primitive;
  }

  RegExPrimitiveType getPrimitiveType() {
    return p_type;
  }
};

class RegExBlank : public RegEx {
 public:
  RegExBlank()
      : RegEx(RegExType::Blank) {
  }
};

}

#endif // PULL_STAR_REGEX_TYPES_H_
