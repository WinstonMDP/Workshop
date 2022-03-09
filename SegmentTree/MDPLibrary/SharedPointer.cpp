#include "SharedPointer.h"

template <typename Value>
SharedPointer<Value>::SharedPointer(Value* primitivePointer)
: _primitivePointer {primitivePointer}
{
    if (_primitivePointer != nullptr) {
        if (_primitivePointersCounts.count (_primitivePointer) == 0) {
            long long* newPrimitivePointerCount = new long long (1);
            _primitivePointersCounts.insert (std::pair <Value*, long long*> (_primitivePointer, newPrimitivePointerCount));
            _primitivePointerCount = newPrimitivePointerCount;
        }
        else {
            _primitivePointerCount = _primitivePointersCounts.at (_primitivePointer);
            ++(*_primitivePointerCount);
        }
    }
    else {
        _primitivePointerCount = nullptr;
    }
}

template <typename Value>
SharedPointer<Value>::SharedPointer()
: SharedPointer (nullptr)
{
}

template <typename Value>
SharedPointer<Value>::SharedPointer(const SharedPointer<Value>& sharedPointer)
: SharedPointer (sharedPointer.primitivePointer())
{
}

template <typename Value>
SharedPointer<Value>& SharedPointer<Value>::operator=(const SharedPointer<Value>& sharedPointer)
{
    deleteMethod();
    _primitivePointer = sharedPointer.primitivePointer();
    if (_primitivePointer != nullptr) {
        _primitivePointerCount = _primitivePointersCounts.at (_primitivePointer);
        ++(*_primitivePointerCount);
    }
    else {
        _primitivePointerCount = nullptr;
    }
    return *this;
}

template <typename Value>
Value& SharedPointer<Value>::operator*() const
{
    return *_primitivePointer;
}

template <typename Value>
Value* SharedPointer<Value>::operator->()
{
    return _primitivePointer;
}

template <typename FriendValue>
bool operator>(const SharedPointer<FriendValue>& a, const SharedPointer<FriendValue>& b)
{
    return *a > *b;
}

template <typename FriendValue>
bool operator<(const SharedPointer<FriendValue>& a, const SharedPointer<FriendValue>& b)
{
    return *a < *b;
}

template <typename Value>
Value* SharedPointer<Value>::primitivePointer() const
{
    return _primitivePointer;
}

template <typename Value>
SharedPointer<Value>::~SharedPointer()
{
    deleteMethod();
}

template <typename Value>
void SharedPointer<Value>::deleteMethod()
{
    if (_primitivePointer != nullptr) {
        --(*_primitivePointerCount);
        if (*_primitivePointerCount == 0) {
            _primitivePointersCounts.erase (_primitivePointer);
            delete _primitivePointer;
            delete _primitivePointerCount;
        }
    }
}

template <class Value>
std::map<Value*, long long*> SharedPointer <Value>::_primitivePointersCounts;