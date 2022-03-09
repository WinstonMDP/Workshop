#include <string>

class NumberSet
{
	public:
	NumberSet (std::string name)
	: _name {name}
	{
	}

	std::string name()
	{
		return _name;
	}

	private:
	std::string _name;
};

bool operator== (NumberSet a, NumberSet b)
{
	return a.name() == b.name();
}

bool operator> (NumberSet a, NumberSet b)
{
	return a.name() == "ComplexNumber";
}

class Number
{
	public:
	virtual NumberSet set() = 0;

	private:
};

class NaturalNumber : public Number
{
	public:
	NaturalNumber()
	{
	}

	NaturalNumber (long value)
	: _value {value}
	{
		checkValidity (value);
	}

	long value()
	{
		return _value;
	}

	NumberSet set() override
	{
		return NumberSet ("NaturalNumber");
	}

	private:
	long _value;

	void checkValidity (long value)
	{
		if (value <= 0) {
			throw "Value must be greater than zero.";
		}
	}
};

class Integer : public Number
{
	public:
	Integer()
	{
	}

	Integer (NaturalNumber naturalValue, char sign)
	: _status {NATURALNUMBERWITHSIGN}, _naturalValue (naturalValue), _sign {sign}
	{
		checkValidity (naturalValue, sign);
	}

	Integer (char zero)
	: _status {ZERO}
	{
		checkValidity (zero);
	}

	NaturalNumber naturalValue()
	{
		if (_status == NATURALNUMBERWITHSIGN) {
			return _naturalValue;
		}
		else {
			throw "It doesn't have a naturalValue.";
		}
	}
	bool isZero()
	{
		if (_status == ZERO) {
			return true;
		}
		else {
			return false;
		}
	}

	char sign()
	{
		if (_status == NATURALNUMBERWITHSIGN) {
			return _sign;
		}
		else {
			throw "It does't have a sign.";
		}
	}

	NumberSet set() override
	{
		return NumberSet ("Integer");
	}

	private:
	enum Status
	{
		ZERO,
		NATURALNUMBERWITHSIGN
	};
	Status _status;

	NaturalNumber _naturalValue;
	char _sign;

	void checkValidity (NaturalNumber naturalValue, char sign)
	{
		if (!(sign == '-' || sign == '+')) {
			throw "Sing must be '-' or '+'.";
		}
	}
	void checkValidity (char zero) 
	{
		if (zero != '0') {
			throw "Zero must be '0'.";
		}
	}
};

class RationalNumber : public Number
{
	public:
	RationalNumber()
	{
	}

	RationalNumber (Integer numerator, NaturalNumber denominator)
	: _numerator {numerator}, _denominator {denominator}
	{
	}

	Integer numerator()
	{
		return _numerator;
	}

	NaturalNumber denominator()
	{
		return _denominator;
	}

	NumberSet set() override
	{
		return NumberSet ("RationalNumber");
	}

	private:
	Integer _numerator;
	NaturalNumber _denominator;
};

class IrrationalNumber : public Number
{
	public:
	IrrationalNumber()
	{
	}

	IrrationalNumber (long double value)
	: _value {value}
	{
	}

	long double value()
	{
		return _value;
	}

	NumberSet set() override
	{
		return NumberSet ("IrrationalNumber");
	}

	private:
	long double _value;
};

class RealNumber : public Number
{
	public:
	RealNumber()
	{
	}

	RealNumber (RationalNumber rationalValue)
	: _status {RATIONAL}, _rationalValue {rationalValue}
	{
	}

	RealNumber (IrrationalNumber irrationalValue)
	: _status {IRRATIONAL}, _irrationalValue {irrationalValue}
	{
	}

	RationalNumber rationalValue()
	{
		if (_status == RATIONAL) {
			return _rationalValue;
		}
		else {
			throw "It doesn't have a rationalValue";
		}
	}

	IrrationalNumber irrationalValue()
	{
		if (_status == IRRATIONAL) {
			return _irrationalValue;
		}
		else {
			throw "It doesn't have a irrationalValue.";
		}
	}

	NumberSet set() override
	{
		return NumberSet ("RealNumber");
	}

	template <class B>
	RealNumber toSet (B convertible)
	{
		return RealNumber();
	}

	private:
	enum Status
	{
		RATIONAL,
		IRRATIONAL
	};
	Status _status;
	RationalNumber _rationalValue;
	IrrationalNumber _irrationalValue;
};

class PurelyImaginaryNumber : public Number
{
	public:
	PurelyImaginaryNumber()
	{
	}

	PurelyImaginaryNumber (RealNumber coefficient)
	: _coefficient {coefficient}
	{
	}

	RealNumber coefficient()
	{
		return _coefficient;
	}

	NumberSet set() override
	{
		return NumberSet ("PurelyImaginaryNumber");
	}

	private:
	RealNumber _coefficient;
};

class ComplexNumber : public Number
{
	public:
	ComplexNumber()
	{
	}

	ComplexNumber (RealNumber realValue, PurelyImaginaryNumber imaginaryValue)
	: _realValue {realValue}, _imaginaryValue {imaginaryValue}
	{
	}

	RealNumber realValue()
	{
		return _realValue;
	}

	PurelyImaginaryNumber imaginaryValue()
	{
		return _imaginaryValue;
	}

	NumberSet set() override
	{
		return NumberSet ("ComplexNumber");
	}

	template <class B>
	ComplexNumber toSet (B convertible)
	{
		if (convertible.set() == NumberSet ("RealNumber")) {
			return ComplexNumber (convertible.rationalValue(), PurelyImaginaryNumber (RealNumber (RationalNumber (Integer (0, '+'), NaturalNumber(1)))));
		}
		else {
			RealNumber realNumber;
			return ComplexNumber (realNumber.toSet (convertible).rationalValue(), PurelyImaginaryNumber (RealNumber (RationalNumber (Integer (0, '+'), NaturalNumber(1)))));
		}
			
	}

	private:
	RealNumber _realValue;
	PurelyImaginaryNumber _imaginaryValue;
};

RealNumber operator+ (RealNumber a, RealNumber b)
{
	return RealNumber (RationalNumber (Integer (1, '+'), NaturalNumber (1)));
}

PurelyImaginaryNumber operator+ (PurelyImaginaryNumber a, PurelyImaginaryNumber b)
{
	return PurelyImaginaryNumber (a.coefficient() + b.coefficient());
}

ComplexNumber operator+ (ComplexNumber a, ComplexNumber b)
{
	return ComplexNumber (a.realValue() + b.realValue(), a.imaginaryValue() + b.imaginaryValue());
}

template <class A, class B, class C>
C operator+ (A a, B b)
{
	if (a.set() == b.set()) {
		return a + b;
	}
	else {
		if (a.set() > b.set()) {
			return a + a.toSet (b);
		}
		else {
			return b + b.toSet (a);
		}
	}
}

int main()
{
	NaturalNumber naturalNumber (5);
	Integer integer (naturalNumber, '+');
	RationalNumber rationalNumber (integer, naturalNumber);
	RealNumber realNumber (rationalNumber);
	PurelyImaginaryNumber purelyImaginaryNumber (realNumber);
	ComplexNumber complexNumber (realNumber, purelyImaginaryNumber);

	RealNumber real (RationalNumber (Integer (NaturalNumber (3), '+'), NaturalNumber (1)));
	ComplexNumber sum = complexNumber + real;
	return 0;
}