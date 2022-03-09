#include "MDPLibrary/SharedPointer.h"
#include "MDPLibrary/Optional.h"
#include <iostream>
#include <vector>
#include <algorithm>

class Segment
{
	public:
	Segment(long firstPoint, long secondPoint)
	: Segment(std::min(firstPoint, secondPoint), std::max(firstPoint, secondPoint), true)
	{
	}

	long leftPoint()
	{
		return _leftPoint;
	}

	long rightPoint()
	{
		return _rightPoint;
	}

	private:
	long _leftPoint;
	long _rightPoint;
	
	Segment(long leftPoint, long rightPoint, bool mainConstructorMarker)
	: _leftPoint {leftPoint}, _rightPoint {rightPoint}
	{
	}
};

template <typename Input, typename Output>
class Function
{
	public:
	virtual Output output(Input) = 0;
	virtual Output output(Input, Input) = 0;
};

template <typename Value>
class SegmentTree
{
	using SubSegmentTree = SegmentTree<Value>;

	public:
	virtual Value functionOutput() = 0;
	virtual Value functionOutput(SharedPointer<Segment>) = 0;
	virtual Optional<SharedPointer<SubSegmentTree>> leftSubSegmentTree() = 0;
	virtual Optional<SharedPointer<SubSegmentTree>> rightSubSegmentTree() = 0;
	virtual SharedPointer<Segment> segment() = 0;
	virtual SharedPointer<Function<Value, Value>> function() = 0;
};

template <typename Value>
class Leaf : public SegmentTree<Value>
{
	using SubSegmentTree = SegmentTree<Value>;

	public:
	Leaf(Value functionOutput, SharedPointer<Function<Value, Value>> function, SharedPointer<Segment> segment)
	: _functionOutput {functionOutput}, _segment {segment}, _function {function}
	{
	}
	
	Leaf(SharedPointer<Function<Value, Value>> function, Value functionInput, SharedPointer<Segment> segment)
	: Leaf(function->output(functionInput), function, segment)
	{
	}

	template <typename Element>
	Leaf(SharedPointer<Function<Value, Value>> function, SharedPointer<std::vector<Element>> vector, SharedPointer<Segment> segment)
	: Leaf(function->output(vector->at(0)), function, segment)
	{
		if (vector->size() != 1) {
			throw "Leaf(SharedPointer<Function<Value, Value>> function, SharedPointer<std::vector<Element>> vector, SharedPointer<Segment> segment): \
				Vector has more or less than 1 element.";
		}
	}

	Value functionOutput() override
	{
		return _functionOutput;
	}

	Value functionOutput(SharedPointer<Segment> segment) override
	{
		if (segment->leftPoint() == segment->rightPoint()) {
			return functionOutput();
		}
		throw "Value functionOutput(SharedPointer<Segment> segment) override: \
			Leaf`s leftPoint is not equal to rightPoint.";
	}

	Optional<SharedPointer<SubSegmentTree>> leftSubSegmentTree() override
	{
		return Optional<SharedPointer<SubSegmentTree>>();
	}

	Optional<SharedPointer<SubSegmentTree>> rightSubSegmentTree() override
	{
		return Optional<SharedPointer<SubSegmentTree>>();
	}

	SharedPointer<Segment> segment() override
	{
		return _segment;
	}

	SharedPointer<Function<Value, Value>> function() override
	{
		return _function;
	}

	private:
	Value _functionOutput;
	SharedPointer<Function<Value, Value>> _function;
	SharedPointer<Segment> _segment;
};

template <typename Value>
class Node : public SegmentTree<Value>
{
	using SubSegmentTree = SegmentTree<Value>;

	public:
	Node(SharedPointer<SubSegmentTree> leftSubSegmentTree, SharedPointer<SubSegmentTree> rightSubSegmentTree)
	: Node(
		leftSubSegmentTree,
		rightSubSegmentTree,
		leftSubSegmentTree->function(),
		new Segment(leftSubSegmentTree->segment()->leftPoint(), rightSubSegmentTree->segment()->rightPoint())
	)
	{
		if (leftSubSegmentTree->function() != rightSubSegmentTree->function()) {
			throw "Node(SharedPointer<SubSegmentTree> leftSubSegmentTree, SharedPointer<SubSegmentTree> rightSubSegmentTree): \
				LeftSubSegmentTree`s function is not equal to rightSubSegmentTree`s function.";
		}
	}

	template <typename Element>
	Node(SharedPointer<Function<Value, Value>> function, SharedPointer<std::vector<Element>> vector, SharedPointer<Segment> segment)
	: Node(_leftSubSegmentTreeOfVector(function, vector, segment), _rightSubSegmentTreeOfVector(function, vector, segment), function, segment)
	{
	}

	Value functionOutput() override
	{
		return _functionOutput;
	}

	Value functionOutput(SharedPointer<Segment> segment) override
	{
		if (segment == _segment) {
			return _functionOutput;
		}
		else {
			Optional<Value> leftSubSegmentTreeFunctionOutput(_subSegmentTreeFunctionOutputIfSubSegmentTreeIncludesArgumentSegment(_leftSubSegmentTree, segment));
			Optional<Value> rightSubSegmentTreeFunctionOtput(_subSegmentTreeFunctionOutputIfSubSegmentTreeIncludesArgumentSegment(_rightSubSegmentTree, segment));
			if (!leftSubSegmentTreeFunctionOutput.doesValueExist() && !rightSubSegmentTreeFunctionOtput.doesValueExist()) {
				throw "Value functionOutput(SharedPointer<Segment> segment) override: \
					Node does not have at least one subSegmentTree, that include argument segment.";
			}
			if (!leftSubSegmentTreeFunctionOutput.doesValueExist()) {
				return rightSubSegmentTreeFunctionOtput.value();
			}
			if (!rightSubSegmentTreeFunctionOtput.doesValueExist()) {
				return leftSubSegmentTreeFunctionOutput.value();
			}
			return _function->output(leftSubSegmentTreeFunctionOutput.value(), rightSubSegmentTreeFunctionOtput.value());
		}
	}

	Optional<SharedPointer<SubSegmentTree>> leftSubSegmentTree() override
	{
		return Optional<SharedPointer<SubSegmentTree>>(_leftSubSegmentTree);
	}

	Optional<SharedPointer<SubSegmentTree>> rightSubSegmentTree() override
	{
		return Optional<SharedPointer<SubSegmentTree>>(_rightSubSegmentTree);
	}

	SharedPointer<Segment> segment() override
	{
		return _segment;
	}

	SharedPointer<Function<Value, Value>> function() override
	{
		return _function;
	}

	private:
	SharedPointer<SubSegmentTree> _leftSubSegmentTree;
	SharedPointer<SubSegmentTree> _rightSubSegmentTree;
	SharedPointer<Function<Value, Value>> _function;
	SharedPointer<Segment> _segment;
	Value _functionOutput;
	
	Node(
		SharedPointer<SubSegmentTree> leftSubSegmentTree,
		SharedPointer<SubSegmentTree> rightSubSegmentTree,
		SharedPointer<Function<Value, Value>> function,
		SharedPointer<Segment> segment
	) 
	: _leftSubSegmentTree {leftSubSegmentTree}, _rightSubSegmentTree {rightSubSegmentTree}, _function {function}, _segment {segment}
	{
		_functionOutput = _function->output(_leftSubSegmentTree->functionOutput(), _rightSubSegmentTree->functionOutput());
	}

	template <typename Element>
	SharedPointer<SubSegmentTree> _leftSubSegmentTreeOfVector(
		SharedPointer<Function<Value, Value>> function,
		SharedPointer<std::vector<Element>> vector,
		SharedPointer<Segment> segment
	)
	{
		SharedPointer<std::vector<Element>> leftHalfVector = _leftHalfVector(vector);
		if (leftHalfVector->size() == 1) {
			return new Leaf<Element>(function, leftHalfVector, new Segment(segment->leftPoint(), segment->leftPoint()));
		}
		else {
			return new Node<Element>(function, leftHalfVector, new Segment(segment->leftPoint(), (segment->rightPoint() - segment->leftPoint()) / 2));
		}
	}
	
	template <typename Element>
	SharedPointer<SubSegmentTree> _rightSubSegmentTreeOfVector(
		SharedPointer<Function<Value, Value>> function,
		SharedPointer<std::vector<Element>> vector,
		SharedPointer<Segment> segment
	)
	{
		SharedPointer<std::vector<Element>> rightHalfVector = _rightHalfVector(vector);
		if (rightHalfVector->size() == 1) {
			return new Leaf<Element>(function, rightHalfVector, new Segment(segment->rightPoint(), segment->rightPoint()));
		}
		else {
			return new Node<Element> (function, rightHalfVector, new Segment((segment->rightPoint() - segment->leftPoint()) / 2 + 1, segment->rightPoint()));
		}
	}

	template <typename Element>
	SharedPointer<std::vector<Element>> _leftHalfVector(SharedPointer<std::vector<Element>> vector)
	{
		return new std::vector<Element>(vector->begin(), vector->end() - vector->size() / 2);
	}
	
	template <typename Element>
	SharedPointer<std::vector<Element>> _rightHalfVector(SharedPointer<std::vector<Element>> vector)
	{
		return new std::vector<Element>(vector->end() - vector->size() / 2, vector->end());
	}

	Optional<Value> _subSegmentTreeFunctionOutputIfSubSegmentTreeIncludesArgumentSegment(
		Optional<SharedPointer<SubSegmentTree>> subSegmentTree,
		SharedPointer<Segment> segment
	)
	{
		if (_doesInclude(segment, subSegmentTree.value()->segment())) {
			return Optional<Value>(subSegmentTree.value()->functionOutput(_unionSegment(segment, subSegmentTree.value()->segment())));
		}
		return Optional<Value>();
	}

	bool _doesInclude(SharedPointer<Segment> a, SharedPointer<Segment> b)
	{
		if (a->leftPoint() > b->rightPoint()) {
			return false;
		}
		if (a->rightPoint() < b->leftPoint()) {
			return false;
		}
		return true;
	}

	SharedPointer<Segment> _unionSegment(SharedPointer<Segment> a, SharedPointer<Segment> b)
	{
		return new Segment(std::max(a->leftPoint(), b->leftPoint()), std::min(a->rightPoint(), b->rightPoint()));
	}
};

class Min : public Function<SharedPointer<long>, SharedPointer<long>>
{
	public:
	Min()
	{
	}

	SharedPointer<long> output(SharedPointer<long> a) override
	{
		return a;
	}
	
	SharedPointer<long> output(SharedPointer<long> a, SharedPointer<long> b) override
	{
		return new long (std::min(*a, *b));
	}
};

int main()
{
	try {
		using Element = SharedPointer<long>;

		SharedPointer<Function<Element, Element>> function = new Min;

		SharedPointer<SegmentTree<Element>> firstLeaf = new Leaf<Element>(function, new long (44), new Segment(1, 1));
		SharedPointer<SegmentTree<Element>> secondLeaf = new Leaf<Element>(function, new long (97), new Segment(2, 2));
		SharedPointer<SegmentTree<Element>> thirdLeaf = new Leaf<Element>(function, new long (86), new Segment(3, 3));
		SharedPointer<SegmentTree<Element>> forthLeaf = new Leaf<Element>(function, new long (11), new Segment(4, 4));

		SharedPointer<SegmentTree<Element>> aNode = new Node<Element>(firstLeaf, secondLeaf);
		SharedPointer<SegmentTree<Element>> bNode = new Node<Element>(thirdLeaf, forthLeaf);

		SharedPointer<SegmentTree<Element>> mainNode = new Node<Element>(aNode, bNode);

		std::cout << "mainNode->fucntionOutput() = " << *mainNode->functionOutput() << '\n';
		std::cout << "mainNode->fucntionOutput(new Segment(1, 3)) = " << *mainNode->functionOutput(new Segment(1, 3)) << '\n';
		std::cout << "mainNode->fucntionOutput(new Segment(2, 4)) = " << *mainNode->functionOutput(new Segment(2, 4)) << '\n';
		std::cout << "mainNode->fucntionOutput(new Segment(3, 4)) = " << *mainNode->functionOutput(new Segment(3, 4)) << '\n';
		std::cout << "mainNode->fucntionOutput(new Segment(2, 3)) = " << *mainNode->functionOutput(new Segment(2, 3)) << '\n';

		SharedPointer<std::vector<Element>> vector = new std::vector<Element> {new long(44), new long(97), new long(86), new long(11)};
		SharedPointer<SegmentTree<Element>> vectorSegmentTree = new Node<Element>(function, vector, new Segment(1, vector->size()));
		
		std::cout << "mainNode->fucntionOutput() = " << *vectorSegmentTree->functionOutput() << '\n';
		std::cout << "mainNode->fucntionOutput(new Segment(1, 3)) = " << *vectorSegmentTree->functionOutput(new Segment(1, 3)) << '\n';
		std::cout << "mainNode->fucntionOutput(new Segment(2, 4)) = " << *vectorSegmentTree->functionOutput(new Segment(2, 4)) << '\n';
		std::cout << "mainNode->fucntionOutput(new Segment(3, 4)) = " << *vectorSegmentTree->functionOutput(new Segment(3, 4)) << '\n';
		std::cout << "mainNode->fucntionOutput(new Segment(2, 3)) = " << *vectorSegmentTree->functionOutput(new Segment(2, 3)) << '\n';
	}
	catch (const char* message) {
		std::cout << message << '\n'; 
	}
	return 0;
}