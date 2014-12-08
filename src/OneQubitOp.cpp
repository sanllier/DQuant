#include "OneQubitOp.h"
#include "Exception.h"
#include "defs.h"

#include <cmath>
#include <iostream>

//--------------------------------------------------------------------

namespace DQ {
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
        case IDENT:
            setIdent();
            break;
		case EMPTYONEQ:
			break;
		default:
			throw Exception( "Incorrect type of one qubit operator.",
						     "OneQubitOp::OneQubitOp(PresetOpType type = EMPTY):DQMOperator(ONEQUBIT)." );
	}
}

void OneQubitOp::setHadamard()
{
	const BASETYPE temp = BASETYPE( 1.0 / sqrt(2.0) );

	m_operator[0][0] =  temp; 
	m_operator[0][1] =  temp;
	m_operator[1][0] =  temp;
	m_operator[1][1] = -temp;
}

//XPauli
void OneQubitOp::setNot()
{
	m_operator[0][1] = BASETYPE(1);
	m_operator[1][0] = BASETYPE(1);
}

void OneQubitOp::setYPauli()
{
	m_operator[0][1] = QMComplex( BASETYPE(0), -BASETYPE(1) );
	m_operator[1][0] = QMComplex( BASETYPE(0),  BASETYPE(1) );
}

void OneQubitOp::setZPauli()
{
	m_operator[0][0] =  BASETYPE(1);
	m_operator[1][1] = -BASETYPE(1);
}

void OneQubitOp::setIdent()
{
	m_operator[0][0] =  BASETYPE(1);
	m_operator[1][1] =  BASETYPE(1);
}

//--------------------------------------------------------------------
}
