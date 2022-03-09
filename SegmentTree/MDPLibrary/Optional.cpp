#include "Optional.h"

template <typename Value>
Optional<Value>::Optional(Value value)
: _value {value}
{
}

template <typename Value>
Optional<Value>::Optional()
: Optional(nullptr)
{
}

template <typename Value>
bool Optional<Value>::doesValueExist()
{
	return _value != nullptr;
}

template <typename Value>
Value Optional<Value>::value()
{
	if (_value == nullptr) {
		throw "Optional contains nullptr.";
	}
	return _value;
}