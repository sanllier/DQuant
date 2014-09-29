#include "OneQubitOp.h"
#include "Exception.h"

#include <cmath>
#include <iostream>

//--------------------------------------------------------------------

namespace DQuant {
//--------------------------------------------------------------------

OneQubitOp::OneQubitOp( PresetOneQubitOpType type ):DQMOperator( ONEQUBIT )
{
	switch ( type ) {
		case HADAMARD:
			setHadamard();
			break;
		case NOT:
			setNot();
			break;
		case YPAULI:
			setYPauli();
			break;
		case ZPAULI:
			setZPauli();
			break;
		case EMPTYONEQ:
			break;
		default:
			throw Exception( "Incorrect type of one qubit operator.",
						     "OneQubitOp::OneQubitOp(PresetOpType type = EMPTY):DQMOperator(ONEQUBIT)." );
	}
}

void OneQubitOp::setHadamard( void )
{
	double temp = 1.0 / sqrt(2.0);

	m_operator[0][0] =  temp; 
	m_operator[0][1] =  temp;
	m_operator[1][0] =  temp;
	m_operator[1][1] = -temp;
}

//XPauli
void OneQubitOp::setNot( void )
{
	m_operator[0][1] = 1.0;
	m_operator[1][0] = 1.0;
}

void OneQubitOp::setYPauli( void )
{
	m_operator[0][1] = QMComplex(0.0, -1.0);
	m_operator[1][0] = QMComplex(0.0,  1.0);
}

void OneQubitOp::setZPauli( void )
{
	m_operator[0][0] =  1.0;
	m_operator[1][1] = -1.0;
}

//--------------------------------------------------------------------
}
