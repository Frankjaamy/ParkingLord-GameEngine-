#pragma once
#include <string.h>
#include <functional>
using namespace std;

typedef size_t hashed_string_value;
class HashString {
public:
	HashString(const char * s):m_string_(s) {
		m_hashed_value = hash<string>{}(m_string_);
	}
	HashString(const HashString & other):m_string_(other.m_string_), m_hashed_value(other.m_hashed_value) {
	}
	const char * GetContent() const { return m_string_.c_str(); }
	const size_t GetHashedValue() const { return m_hashed_value; }
public:
	inline HashString operator = (const HashString & other) {
		return HashString(other);
	}

private:
	string m_string_;
	size_t m_hashed_value;
};	
inline bool operator == (const HashString & h1, const HashString & h2) {
	return h1.GetHashedValue() == h2.GetHashedValue();
}

inline bool operator != (const HashString & h1, const HashString & h2) {
	return h1.GetHashedValue() != h2.GetHashedValue();
}