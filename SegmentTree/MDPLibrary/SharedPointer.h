#ifndef SHARED_POINTER
#define SHARED_POINTER

#include <map>

template <typename Value>
class SharedPointer
{
	public:
	SharedPointer(Value*);
	SharedPointer();
	SharedPointer(const SharedPointer&);
	SharedPointer& operator=(const SharedPointer&);
	Value& operator*() const;
	Value* operator->();
	Value* primitivePointer() const;
	~SharedPointer();

	friend bool operator==(const SharedPointer& a, const SharedPointer& b)
	{
		return a.primitivePointer() == b.primitivePointer();
	}
	friend bool operator!=(const SharedPointer& a, const SharedPointer& b)
	{
		return a.primitivePointer() != b.primitivePointer();
	}
	template <typename FriendValue>
	friend bool operator<(const SharedPointer<FriendValue>& a, const SharedPointer<FriendValue>& b);
	template <typename FriendValue>
	friend bool operator>(const SharedPointer<FriendValue>& a, const SharedPointer<FriendValue>& b);

	private:
	Value* _primitivePointer;
	static std::map<Value*, long long*> _primitivePointersCounts;
	long long* _primitivePointerCount;

    void deleteMethod();
};

#include "SharedPointer.cpp"

#endif