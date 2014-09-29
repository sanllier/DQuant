#ifndef H_DQMOPERATOR
#define H_DQMOPERATOR

#include "QMComplex.h"

#include <iostream>

//--------------------------------------------------------------------

namespace DQuant {
//--------------------------------------------------------------------

enum OperatorSize
{
	ONEQUBIT = 2,
	TWOQUBIT = 4
};

class DQMOperator
{
public:
	DQMOperator( OperatorSize size );
	~DQMOperator();

	void clear( void );
	//NOTE: You must call DQuant::dsrand( seed ) before use it
	void addNoise( double noiseFactor );

	QMComplex* operator[]( int idx );
	DQMOperator& operator=( const DQMOperator& scndOp );	//deep-copy
	DQMOperator operator*( const DQMOperator& rOp ) const;
	friend std::ostream& operator<<( std::ostream& oStr, const DQMOperator& op );

protected:
	QMComplex** m_operator;
	OperatorSize m_size;
};

//--------------------------------------------------------------------
}

#endif
