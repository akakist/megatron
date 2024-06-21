#pragma once
#include <string>
#include <sys/types.h>
#include <stdint.h>
class Integer
{

public:
    int64_t value;
public:
    Integer():value(0) {}
    Integer(int64_t v):value(v) {}
    int64_t get() const
    {
        return value;
    }
    void set(int64_t v)
    {
        value=v;
    }
    int operator==(const Integer& a) const;
    int operator<=(const Integer& a) const;
    int operator<(const Integer& a) const;
    std::string dump() const;
    Integer& operator+=(const int64_t & a);
    Integer& operator/=(const int64_t & a);
    Integer& operator*=(const int64_t & a);
    Integer& operator%=(const int64_t & a);
};
Integer operator -(const Integer& a, const Integer& b);
Integer operator +(const Integer& a, const Integer& b);

