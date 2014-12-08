#include "TwoQubitOp.h"
#include "Exception.h"

//--------------------------------------------------------------------

namespace DQ {
//--------------------------------------------------------------------

TwoQubitOp::TwoQubitOp( PresetTwoQubitOpType type ):DQMOperator( TWOQUBIT )
{
	switch ( type ) {
		case CNOT:
			setCNot();
			break;
        case CTRLZ:
            setCtrlZ();
            break;
        case SQCN:
            setSQCNot();
            break;
        case SQCNC:
            setSQCNotC();
            break;
		case EMPTYTWOQ:
			break;
		default:
			throw Exception( "Incorrect type of two qubit operator.",
						     "TwoQubitOp::TwoQubitOp(PresetTwoQubitOpType type):DQMOperator(TWOQUBIT)." );
	}
}

void TwoQubitOp::setCNot()
{
	m_operator[0][0] = BASETYPE(1);
	m_operator[1][1] = BASETYPE(1);
	m_operator[2][3] = BASETYPE(1);
	m_operator[3][2] = BASETYPE(1);
}

void TwoQubitOp::setCtrlZ()
{
	m_operator[0][0] =  BASETYPE(1);
	m_operator[1][1] =  BASETYPE(1);
	m_operator[2][2] =  BASETYPE(1);
	m_operator[3][3] = -BASETYPE(1);
}

void TwoQubitOp::setSQCNot()
{
	m_operator[0][0] = BASETYPE(1);
	m_operator[1][1] = BASETYPE(1);
	m_operator[2][2] = QMComplex( BASETYPE(0.5),  BASETYPE(0.5) );
	m_operator[3][3] = QMComplex( BASETYPE(0.5),  BASETYPE(0.5) );
	m_operator[2][3] = QMComplex( BASETYPE(0.5), -BASETYPE(0.5) );
	m_operator[3][2] = QMComplex( BASETYPE(0.5), -BASETYPE(0.5) );
}

void TwoQubitOp::setSQCNotC()
{
	m_operator[0][0] = BASETYPE(1);
	m_operator[1][1] = BASETYPE(1);
	m_operator[2][2] = QMComplex( BASETYPE(0.5), -BASETYPE(0.5) );
	m_operator[3][3] = QMComplex( BASETYPE(0.5), -BASETYPE(0.5) );
	m_operator[2][3] = QMComplex( BASETYPE(0.5),  BASETYPE(0.5) );
	m_operator[3][2] = QMComplex( BASETYPE(0.5),  BASETYPE(0.5) );
}

//--------------------------------------------------------------------
}
