#include <iostream>
#include <vector>

template <class I, class O>
class Function
{
	protected:
	Function()
	{
	}

	public:
	virtual O output (I) = 0;
	virtual O output (I, I) = 0;
};

template <class I, class O>
class TreePart
{
	protected:
	TreePart()
	{
	}

	public:
	virtual O value() = 0;
	virtual long leftIndex() = 0;
	virtual long rightIndex() = 0;
	virtual O query (long leftIndex, long rightIndex) = 0;
};

template <class I, class O>
class Leaf : public TreePart <I, O>
{
	public:
	Leaf (O value, Function <I, O>* function)
	: _function {function}, _value {value}
	{
	}

	Leaf (std::vector <I>* vector, Function <I, O>* function)
	{
		Leaf (vector->front(), function);
	}

	O value() override
	{
		return _value;
	}

	long leftIndex() override
	{
		throw "Leaf doesn't have a leftIndex.";
	}

	long rightIndex() override
	{
		throw "Leaf doesn't have a rightIndex.";
	}

	long index()
	{
		return _index;
	}

	O query (long leftIndex, long rightIndex) override
	{
		if (leftIndex == rightIndex && leftIndex == index()) {
			return value();
		}
		else {
			throw "Invalid query for Leaf.";
		}
	}

	private:
	long _index;
	Function <I, O>* _function;
	O _value;
};

template <class I, class O>
class Node : public TreePart <I, O>
{
	public:
	Node (TreePart <I, O>* leftPart, TreePart <I, O>* rightPart, Function <I, O>* function)
	: _leftPart {leftPart}, _rightPart {rightPart}, _function {function}, _value {function.output (_leftPart->value(), _rightPart->value())}
	{
	}

	Node (std::vector <I>* vector, Function <I, O>* function)
	{
		TreePart <I, O>* leftPart;
		TreePart <I, O>* rightPart;
		if (vector->size() >= 4) {
			std::vector <I>* leftPartVector (vector->begin(), vector->end() - vector->size() / 2);
			std::vector <I>* rightPartVector (std::vector <I> (vector->begin() + vector->size() / 2, vector->end()));
			leftPart = new Node <I, O> (leftPartVector, function);
			rightPart = new Node <I, O> (rightPartVector, function);
		}
		else if (vector->size() == 3) {
			std::vector <I>* leftPartVector (std::vector <I> (vector->begin(), vector->end() - 1));
			std::vector <I>* rightPartVector (std::vector <I> (vector->end()));
			leftPart = new Node <I, O> (leftPartVector, function);
			rightPart = new Leaf <I, O> (rightPartVector, function);
		}
		else if (vector->size() == 2) {
			std::vector <I>* leftPartVector (std::vector <I> (vector->begin()));
			std::vector <I>* rightPartVector (std::vector <I> (vector->end()));
			leftPart = new Leaf <I, O> (leftPartVector, function);
			rightPart = new Leaf <I, O> (rightPartVector, function);
		}
		Node (leftPart, rightPart, function);
	}

	O value() override
	{
		return _value;
	}

	long leftIndex() override
	{
		return _leftIndex;
	}

	long rightIndex() override
	{
		return _rightIndex;
	}

	O query (long leftIndex, long rightIndex) override
	{
		if (leftIndex <= this->leftIndex() && rightIndex >= this->rightIndex()) {
			return value();
		}
		else {
			bool isValidLeftPartForQueryVar = isValidForQuery (_leftPart, leftIndex, rightIndex);
			bool isValidRightPartForQueryVar = isValidForQuery (_rightPart, leftIndex, rightIndex);
			if (isValidLeftPartForQueryVar && isValidRightPartForQueryVar) {
				O leftPartQueryValue = _leftPart->query (std::max (_leftPart->leftIndex(), leftIndex), std::min (_leftPart->rightIndex(), rightIndex));
				O rightPartQueryValue = _rightPart->query (std::max (_rightPart->leftIndex(), leftIndex), std::min (_rightPart->rightIndex(), rightIndex));
				return _function->output (leftPartQueryValue, rightPartQueryValue);
			}
			else if (isValidLeftPartForQueryVar) {
				return _leftPart->query (std::max (_leftPart->leftIndex(), leftIndex), std::min (_leftPart->rightIndex(), rightIndex));
			}
			else if (isValidRightPartForQueryVar){
				return _rightPart->query (std::max (_rightPart->leftIndex(), leftIndex), std::min (_rightPart->rightIndex(), rightIndex));
			}
			else {
				throw "It doesn't have valid part for query.";
			}
		}
	}

	private:
	TreePart <I, O>* _leftPart;
	TreePart <I, O>* _rightPart;
	long _leftIndex;
	long _rightIndex;
	Function <I, O>* _function;
	O _value;

	bool isValidForQuery (TreePart <I, O>* part, long leftIndex, long rightIndex)
	{
		return part->leftIndex() < rightIndex && part->rightIndex() > leftIndex;
	}
};

class MaxFunction : public Function <long, long>
{
	public:
	long output (long x) override
	{
		return x;
	}

	long output (long a, long b) override
	{
		if (a > b) {
			return a;
		}
		else {
			return b;
		}
	}
};

int main()
{
	long nElements;
	std::cin >> nElements;

	std::vector <long>* mass;
	for (long i = 0; i < nElements; ++i) {
		long number;
		std::cin >> number;
		mass->push_back (number);
	}

	MaxFunction* function;
	Node <long, long> tree (mass, function);


	return 0;
}