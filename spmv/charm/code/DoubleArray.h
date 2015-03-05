#ifndef __DOUBLE_ARRAY_H_
#define __DOUBLE_ARRAY_H_

/*
 * This class is needed to be able to provide the operand vector
 * as a readonly global variable to all chares.
 * Pointers are not allowed to be global-readonly (think of distributed memory topologies)
 * and thus we have to provide a PUPable object which can be serialized into a message and
 * is sent to all SMP nodes once.
 */

class DoubleArray
{
private:
	int _n;
	double *_data;

public:
	DoubleArray() : _data(NULL)
	{

	}

	DoubleArray(int n) : _data(NULL)
	{
		resize(n);
	}

	~DoubleArray()
	{
		if(_data!=NULL)
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

	inline int size()
	{
		return _n;
	}

	double &operator[](int i)
	{
		return _data[i];
	}
};

#endif //__DOUBLE_ARRAY_H_