#ifndef OPTIONAL
#define OPTIONAL

template <typename Value>
class Optional
{
	public:
	Optional(Value);
	Optional();
	bool doesValueExist();
	Value value();

	private:
	Value _value;
};

#include "Optional.cpp"

#endif