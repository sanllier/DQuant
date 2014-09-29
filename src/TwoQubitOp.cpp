#include "TwoQubitOp.h"
#include "Exception.h"

//--------------------------------------------------------------------

namespace DQuant {
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

void TwoQubitOp::setCNot( void )
{
	m_operator[0][0] = 1.0;
	m_operator[1][1] = 1.0;
	m_operator[2][3] = 1.0;
	m_operator[3][2] = 1.0;
}

void TwoQubitOp::setCtrlZ( void )
{
	m_operator[0][0] =  1.0;
	m_operator[1][1] =  1.0;
	m_operator[2][2] =  1.0;
	m_operator[3][3] = -1.0;
}

void TwoQubitOp::setSQCNot( void )
{
	m_operator[0][0] = 1.0;
	m_operator[1][1] = 1.0;
	m_operator[2][2] = QMComplex( 0.5,  0.5 );
	m_operator[3][3] = QMComplex( 0.5,  0.5 );
	m_operator[2][3] = QMComplex( 0.5, -0.5 );
	m_operator[3][2] = QMComplex( 0.5, -0.5 );
}

void TwoQubitOp::setSQCNotC( void )
{
	m_operator[0][0] = 1.0;
	m_operator[1][1] = 1.0;
	m_operator[2][2] = QMComplex( 0.5, -0.5 );
	m_operator[3][3] = QMComplex( 0.5, -0.5 );
	m_operator[2][3] = QMComplex( 0.5,  0.5 );
	m_operator[3][2] = QMComplex( 0.5,  0.5 );
}

//--------------------------------------------------------------------
}
