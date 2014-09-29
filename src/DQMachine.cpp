#include "DQuant.h"
#include "DQMachine.h"
#include "BitIndex.h"

#include <iostream>
#include <algorithm>

//--------------------------------------------------------------------

namespace DQuant {
//--------------------------------------------------------------------

struct SendData
{
	QMComplex data;
	long long idx;
	char opRow;
	char opCol;
};

enum
{
	EXCHNGHDLR = 5
};

//workReg is static because of HANDLERS must have access to it
static DQMStateReg* workReg = 0;
static DQMStateReg* buf     = 0;
static DQMOperator* workOp  = 0;

//------------------------------HANDLERS------------------------------

void
exchngElemHdlr( int from, void* data, int size )
{
	SendData* recvData = ( SendData * )(data);
	buf->m_qRegister[ recvData->idx ] += (*workOp)[ recvData->opRow ][ recvData->opCol ] * recvData->data;
}

//--------------------------------------------------------------------

#ifdef _MSC_VER
inline double log2( double n )
{
    return log(n) / log(2);
}
#endif

//--------------------------------------------------------------------

DQMachine* DQMachine::m_curCopy = 0;

DQMachine* DQMachine::getMachine( DQMStateReg *curReg )
{
	if ( !m_curCopy ) 
    {
		QOUT( "-- making new DQMachine\r\n" );
		m_curCopy = new DQMachine( curReg );
		QOUT( "-- DQMachine was created\r\n" );
	} 
    else 
    {
		QOUT( "-- DQMachine also was created. Instance pointer was returned\r\n" );
		QOUT( "-- WARNING: for setting new current state register use DQMachine::setCurReg\r\n" );
	}

	return m_curCopy;
}

DQMachine::DQMachine( DQMStateReg* curReg )
{
    shmem_register_handler( exchngElemHdlr, EXCHNGHDLR );
	workReg = curReg;

	if ( workReg ) 
    {
		buf = new DQMStateReg( workReg->m_qnum );
		QOUT( "-- DQMachine: buf state was alloced\r\n" );
	} 
    else 
    {
		buf = 0;
	}
}

DQMachine::~DQMachine()
{
	delete buf;
	buf = 0;

    delete m_curCopy;
	m_curCopy = 0;

	QOUT( "-- DQMachine was destroyed\r\n" );
}

DQMStateReg* DQMachine::setCurReg( DQMStateReg* curReg )
{
	DQMStateReg* temp = workReg;
	workReg = curReg;

	if ( workReg && temp && temp->m_qnum != workReg->m_qnum ) 
    {
		if ( buf ) {
			delete buf;
			buf = 0;
			QOUT( "-- DQMachine: old buf state was deleted\r\n" );
		} 
	}

	if ( !buf && workReg ) 
    {
		buf = new DQMStateReg( workReg->m_qnum );	
		QOUT( "-- DQMachine: buf state was alloced\r\n" );
	}

	QOUT( "-- DQMachine: new state was setted\r\n" );

	return temp;
}

DQMStateReg* DQMachine::getCurReg( void ) const
{
	return workReg;
}

//--------------------------------------------------------------------

inline void sendElement( QMComplex& elem, long long locIdx, int opR, int opC, int target )
{
    SendData sendingElement = { elem, locIdx, opR, opC };
	shmem_send( &sendingElement, EXCHNGHDLR, sizeof( sendingElement ), target );
}

void DQMachine::oneQubitEvolution( OneQubitOp& op, int target )
{
	workOp = &op;
	buf->fillWithZeros();

	const int qubitsNumPerProc = workReg->m_qnum - int( log2( workReg->m_nodesNum ) );
	const bool isUnlocal = ( ( workReg->m_qnum - 1 ) - target ) >= qubitsNumPerProc;
	const long long rangeStart = workReg->m_myPartSize * workReg->m_myID;
	const int flipBit = ( workReg->m_qnum - 1 ) - target;
	unsigned long long arrayPos = 0;
	const int targetBits[1] = { flipBit };
	BitIndex< unsigned long long > locIdx, oppositeIdx;

	DQuant::synchronise();

	for ( long long int i = 0; i < workReg->m_myPartSize; ++i ) 
    {
		locIdx = i + rangeStart;
		oppositeIdx = locIdx;
		oppositeIdx.flipBitAtPos( flipBit );

		arrayPos = locIdx.select( targetBits, 1 );

		if ( isUnlocal )
        {
			sendElement( workReg->m_qRegister[i], i, arrayPos, oppositeIdx.select( targetBits, 1 ), 
				        oppositeIdx >> qubitsNumPerProc );
			buf->m_qRegister[i] += op[arrayPos][arrayPos] * workReg->m_qRegister[i];
		} 
        else
        {
			buf->m_qRegister[i] += op[arrayPos][arrayPos] * workReg->m_qRegister[i] +
                                   op[arrayPos][ oppositeIdx.select( targetBits, 1 ) ] * 
                                   workReg->m_qRegister[ oppositeIdx.getIdx() - rangeStart ];
		}
	}

	DQuant::synchronise();
    std::swap( workReg->m_qRegister, buf->m_qRegister );
    DQuant::synchronise();
}

void DQMachine::twoQubitEvolution( TwoQubitOp& op, int targetA, int targetB )
{
	workOp = &op;
	buf->fillWithZeros();

	const int qubitsNumPerProc = workReg->m_qnum - int( log2( workReg->m_nodesNum ) );
	const bool isAUnlocal = ( ( workReg->m_qnum - 1 ) - targetA ) >= qubitsNumPerProc;
	const bool isBUnlocal = ( ( workReg->m_qnum - 1 ) - targetB ) >= qubitsNumPerProc;
	const long long rangeStart = workReg->m_myPartSize * workReg->m_myID;
	const int flipBitA  = ( workReg->m_qnum - 1 ) - targetA;
	const int flipBitB  = ( workReg->m_qnum - 1 ) - targetB;
	unsigned long long arrayPos = 0;
	const int targetBits[2] = { flipBitA, flipBitB };
	BitIndex< unsigned long long > locIdx, oppositeIdxA, oppositeIdxB, oppositeIdxAB;

	DQuant::synchronise();

	for ( long long i = 0; i < workReg->m_myPartSize; ++i )
    {
		locIdx = i + rangeStart;
		oppositeIdxA = oppositeIdxB = locIdx;
		oppositeIdxA.flipBitAtPos( flipBitA );
		oppositeIdxB.flipBitAtPos( flipBitB );

		oppositeIdxAB = oppositeIdxA;
		oppositeIdxAB.flipBitAtPos (flipBitB );

		arrayPos = locIdx.select( targetBits, 2 );

		if ( isAUnlocal && isBUnlocal ) 
        {	
			sendElement(workReg->m_qRegister[i], i, arrayPos, oppositeIdxA.select(targetBits, 2), 
				        oppositeIdxA >> qubitsNumPerProc);
			sendElement(workReg->m_qRegister[i], i, arrayPos, oppositeIdxB.select(targetBits, 2), 
				        oppositeIdxB >> qubitsNumPerProc);
			sendElement(workReg->m_qRegister[i], i, arrayPos, oppositeIdxAB.select(targetBits, 2), 
				        oppositeIdxAB >> qubitsNumPerProc);
			buf->m_qRegister[i] += op[arrayPos][arrayPos] * workReg->m_qRegister[i];
		} 
        else if ( isAUnlocal ) 
        {
			sendElement(workReg->m_qRegister[i], i, arrayPos, oppositeIdxA.select(targetBits, 2), 
				        oppositeIdxA >> qubitsNumPerProc);
			sendElement(workReg->m_qRegister[oppositeIdxB.getIdx() - rangeStart], i, 
				        arrayPos, oppositeIdxAB.select(targetBits, 2), 
				        oppositeIdxA >> qubitsNumPerProc);	
			buf->m_qRegister[i] += op[arrayPos][arrayPos] * workReg->m_qRegister[i] +
								  op[arrayPos][oppositeIdxB.select(targetBits, 2)] * 
								  workReg->m_qRegister[oppositeIdxB.getIdx() - rangeStart];
		} 
        else if ( isBUnlocal ) 
        {
			sendElement(workReg->m_qRegister[i], i, arrayPos, oppositeIdxB.select(targetBits, 2), 
				        oppositeIdxB >> qubitsNumPerProc);
			sendElement(workReg->m_qRegister[oppositeIdxA.getIdx() - rangeStart], i, 
				        arrayPos, oppositeIdxAB.select(targetBits, 2), 
				        oppositeIdxAB >> qubitsNumPerProc);	
			buf->m_qRegister[i] += op[arrayPos][arrayPos] * workReg->m_qRegister[i] +
								  op[arrayPos][oppositeIdxA.select(targetBits, 2)] * 
								  workReg->m_qRegister[oppositeIdxA.getIdx() - rangeStart];
		} 
        else 
        {
			buf->m_qRegister[i] += op[arrayPos][arrayPos] * workReg->m_qRegister[i] + 
						          op[arrayPos][ oppositeIdxA.select(targetBits, 2) ]  * workReg->m_qRegister[ oppositeIdxA.getIdx() - rangeStart ] + 
							      op[arrayPos][ oppositeIdxB.select(targetBits, 2) ]  * workReg->m_qRegister[ oppositeIdxB.getIdx() - rangeStart ] + 
							      op[arrayPos][ oppositeIdxAB.select(targetBits, 2) ] * workReg->m_qRegister[ oppositeIdxAB.getIdx() - rangeStart ];
		}  
	}

	DQuant::synchronise();
    std::swap( workReg->m_qRegister, buf->m_qRegister ); 
    DQuant::synchronise();
}

//--------------------------------------------------------------------
}
