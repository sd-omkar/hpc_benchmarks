#ifndef __DOUBLE_ARRAY_H_
#define __DOUBLE_ARRAY_H_

class DoubleArray
{
private:
	int _n;
	double *_data;

public:
	DoubleArray()
	{

	}

	DoubleArray(int n)
	{
		_data = NULL;
		resize(n);
	}

	~DoubleArray()
	{
		delete[] _data;
	}

	void pup(PUP::er &p)
	{
		// don't forget to pup superclass
		p | _n; // pup array length
		if (p.isUnpacking())
			_data = new double[_n];
		PUParray(p, _data, _n); // pup data in the array
	}

	void resize(int n)
	{
		if (_data != NULL) delete[] _data;
		_data = new double[n];
		_n = n;
	}

	double &operator[](int i)
	{
		return _data[i];
	}
};

#endif //__DOUBLE_ARRAY_H_