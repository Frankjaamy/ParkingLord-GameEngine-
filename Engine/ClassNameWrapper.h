#pragma once
#include "HashString.h"

class ClassNameWrapper {
public:
	ClassNameWrapper(const char * name):class_name_(name) {
	}
	inline size_t GetThisClassName() const { return class_name_.GetHashedValue(); }
private:
	HashString class_name_;
};