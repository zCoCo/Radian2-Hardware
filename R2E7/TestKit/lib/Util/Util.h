#ifndef _UTIL_H
#define _UTIL_H

  // // Named Type Prototype for Strong Typing (see Units.h for use)
  // template <typename T, typename UniqParam>
  // class NamedType{
  // public:
  //   explicit NamedType(T const& value) : value_(value) {}
  //   explicit NamedType(T&& value) : value_(std::move(value)) {}
  //   T& get() { return value_; }
  //   T const& get() const {return value_; }
  // private:
  //   T value_;
  // };

  typedef union{
    float number;
    int8_t bytes[4];
  } FloatUnion_t;

#endif //_UTIL_H
