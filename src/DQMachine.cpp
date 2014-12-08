#include "DQuant.h"
#include "DQMachine.h"
#include "BitIndex.h"
#include "defs.h"
#include "mpicheck.h"

#include <iostream>

//--------------------------------------------------------------------

namespace DQ {
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
		m_curCopy->setCurReg( curReg );
	}

	return m_curCopy;
}

DQMachine::DQMachine( DQMStateReg* curReg/* = 0 */)
    : m_work(0)
    , m_buf(0)
    , m_bufWin( MPI_WIN_NULL )
{
    m_work = curReg;

	if ( m_work ) 
    {
		m_buf = new DQMStateReg( m_work->getQubitsNum() );
		QOUT( "-- DQMachine: buf state was alloced\r\n" );
	} 
}

DQMachine::~DQMachine()
{
	delete m_buf;
	m_buf = 0;

	QOUT( "-- DQMachine was destroyed\r\n" );
}

DQMStateReg* DQMachine::setCurReg( DQMStateReg* curReg )
{
    if ( !curReg )
        throw Exception( std::string( "Trying to set invalid (NULL) state." ), std::string( __FUNCTION__ )  );

    DQMStateReg* prevState = m_work;
    m_work = curReg;

    if ( !m_buf || curReg->getQubitsNum() != m_buf->getQubitsNum() )
    {
        delete m_buf;
        QOUT( "-- DQMachine: old buf state was deleted\r\n" );
        m_buf = new DQMStateReg( curReg->getQubitsNum() );
        QOUT( "-- DQMachine: buf state was alloced\r\n" );
    }

    QOUT( "-- DQMachine: new state was setted\r\n" );
    return prevState;
}

DQMStateReg* DQMachine::getCurReg( void ) const
{
	return m_work;
}

//--------------------------------------------------------------------

void DQMachine::oneQubitEvolution( OneQubitOp& op, int target )
{
    if ( !m_work->getContext().isActive() )
        return;

	m_buf->fillWithZeros();
    createBufWindow();

	const int qubitsNumPerProc = m_work->getQubitsNum() - int( log2( m_work->getContext().commSize() ) );
	const bool isUnlocal = ( ( m_work->getQubitsNum() - 1 ) - target ) >= qubitsNumPerProc;
	const long long rangeStart = m_work->getLocalRegLen() * m_work->getContext().rank();
	const int flipBit = ( m_work->getQubitsNum() - 1 ) - target;
	const int targetBits[1] = { flipBit };
	BitIndex< unsigned long long > locIdx, oppositeIdx;

    QMComplex* bufReg  = m_buf->getRaw();
    QMComplex* workReg = m_work->getRaw();
    QMComplex remoteElement;

    CHECK( MPI_Barrier( m_work->getContext().comm() ) );

	for ( long long i = 0; i < m_work->getLocalRegLen(); ++i ) 
    {
		locIdx = i + rangeStart;

		oppositeIdx = locIdx;
		oppositeIdx.flipBitAtPos( flipBit );

		const int arrayPos = (int)locIdx.select( targetBits, 1 );

		if ( isUnlocal )
        {
            int targetProc = int( oppositeIdx >> qubitsNumPerProc );

            MPI_Win_fence( 0, m_bufWin ); 
            CHECK( MPI_Get( &remoteElement, 1, MPI_COMPLEXTYPE, targetProc, MPI_Aint(i), 1, MPI_COMPLEXTYPE, m_bufWin ) );
            MPI_Win_fence( 0, m_bufWin ); 

            bufReg[i] += op[arrayPos][arrayPos] * workReg[i] +
                         op[arrayPos][ oppositeIdx.select( targetBits, 1 ) ] * remoteElement;
		} 
        else
        {  
   			bufReg[i] += op[arrayPos][arrayPos]                              * workReg[i] +
                         op[arrayPos][ oppositeIdx.select( targetBits, 1 ) ] * workReg[ oppositeIdx.getIdx() - rangeStart ];
		}
	}

    CHECK( MPI_Barrier( m_work->getContext().comm() ) );
    m_work->swapInternalData( m_buf );
    deleteBufWindow();
    CHECK( MPI_Barrier( m_work->getContext().comm() ) );
}

void DQMachine::twoQubitEvolution( TwoQubitOp& op, int targetA, int targetB )
{
    if ( !m_work->getContext().isActive() )
        return;

	m_buf->fillWithZeros();
    createBufWindow();

	const int qubitsNumPerProc = m_work->getQubitsNum() - int( log2( m_work->getContext().commSize() ) );
    const bool isAUnlocal = ( ( m_work->getQubitsNum() - 1 ) - targetA ) >= qubitsNumPerProc;
	const bool isBUnlocal = ( ( m_work->getQubitsNum() - 1 ) - targetB ) >= qubitsNumPerProc;
	const long long rangeStart = m_work->getLocalRegLen() * m_work->getContext().rank();
    const int flipBitA  = ( m_work->getQubitsNum() - 1 ) - targetA;
	const int flipBitB  = ( m_work->getQubitsNum() - 1 ) - targetB;
    const int targetBits[2] = { flipBitA, flipBitB };
	BitIndex< unsigned long long > locIdx, oppositeIdxA, oppositeIdxB, oppositeIdxAB;

    QMComplex* bufReg  = m_buf->getRaw();
    QMComplex* workReg = m_work->getRaw();
    QMComplex remoteElement;

	CHECK( MPI_Barrier( m_work->getContext().comm() ) );

	for ( long long i = 0; i < m_work->getLocalRegLen(); ++i )
    {
		locIdx = i + rangeStart;

		oppositeIdxA = oppositeIdxB = locIdx;
		oppositeIdxA.flipBitAtPos( flipBitA );
		oppositeIdxB.flipBitAtPos( flipBitB );

		oppositeIdxAB = oppositeIdxA;
       	oppositeIdxAB.flipBitAtPos ( flipBitB );

	    const int arrayPos = (int)locIdx.select( targetBits, 2 );

		if ( isAUnlocal && isBUnlocal ) 
        {	
            bufReg[i] += op[arrayPos][arrayPos] * workReg[i];

            int targetProc = int( oppositeIdxA >> qubitsNumPerProc );
            CHECK( MPI_Get( &remoteElement, 1, MPI_COMPLEXTYPE, targetProc, MPI_Aint(i), 1, MPI_COMPLEXTYPE, m_bufWin ) );
            bufReg[i] += op[arrayPos][ oppositeIdxA.select( targetBits, 2 ) ] * workReg[ oppositeIdxA.getIdx() - rangeStart ];

            targetProc = int( oppositeIdxB >> qubitsNumPerProc );
            CHECK( MPI_Get( &remoteElement, 1, MPI_COMPLEXTYPE, targetProc, MPI_Aint(i), 1, MPI_COMPLEXTYPE, m_bufWin ) );
            bufReg[i] += op[arrayPos][ oppositeIdxB.select( targetBits, 2 ) ] * workReg[ oppositeIdxB.getIdx() - rangeStart ]; 
	 					
            targetProc = int( oppositeIdxAB >> qubitsNumPerProc );
            CHECK( MPI_Get( &remoteElement, 1, MPI_COMPLEXTYPE, targetProc, MPI_Aint(i), 1, MPI_COMPLEXTYPE, m_bufWin ) );
            bufReg[i] += op[arrayPos][ oppositeIdxAB.select( targetBits, 2 ) ] * workReg[ oppositeIdxAB.getIdx() - rangeStart ];
		} 
        else if ( isAUnlocal ) 
        {
            bufReg[i] += op[arrayPos][arrayPos]                               * workReg[i] +
					     op[arrayPos][ oppositeIdxB.select( targetBits, 2 ) ] * workReg[ oppositeIdxB.getIdx() - rangeStart ];

            int targetProc = int( oppositeIdxA >> qubitsNumPerProc );
            CHECK( MPI_Get( &remoteElement, 1, MPI_COMPLEXTYPE, targetProc, MPI_Aint(i), 1, MPI_COMPLEXTYPE, m_bufWin ) );
            bufReg[i] += op[arrayPos][ oppositeIdxA.select( targetBits, 2 ) ] * workReg[ oppositeIdxA.getIdx() - rangeStart ];

            targetProc = int( oppositeIdxAB >> qubitsNumPerProc );
            CHECK( MPI_Get( &remoteElement, 1, MPI_COMPLEXTYPE, targetProc, MPI_Aint(i), 1, MPI_COMPLEXTYPE, m_bufWin ) );
            bufReg[i] += op[arrayPos][ oppositeIdxAB.select( targetBits, 2 ) ] * workReg[ oppositeIdxAB.getIdx() - rangeStart ];			
		} 
        else if ( isBUnlocal ) 
        {
            bufReg[i] += op[arrayPos][arrayPos]                             * workReg[i] +
    		             op[arrayPos][ oppositeIdxA.select(targetBits, 2) ] * workReg[ oppositeIdxA.getIdx() - rangeStart ];

            int targetProc = int( oppositeIdxB >> qubitsNumPerProc );
            CHECK( MPI_Get( &remoteElement, 1, MPI_COMPLEXTYPE, targetProc, MPI_Aint(i), 1, MPI_COMPLEXTYPE, m_bufWin ) );
            bufReg[i] += op[arrayPos][ oppositeIdxB.select( targetBits, 2 ) ] * workReg[ oppositeIdxB.getIdx() - rangeStart ];

            targetProc = int( oppositeIdxAB >> qubitsNumPerProc );
            CHECK( MPI_Get( &remoteElement, 1, MPI_COMPLEXTYPE, targetProc, MPI_Aint(i), 1, MPI_COMPLEXTYPE, m_bufWin ) );
            bufReg[i] += op[arrayPos][ oppositeIdxAB.select( targetBits, 2 ) ] * workReg[ oppositeIdxAB.getIdx() - rangeStart ];
		} 
        else 
        {
	        bufReg[i] += op[arrayPos][arrayPos]                                * workReg[i] + 
						 op[arrayPos][ oppositeIdxA.select( targetBits, 2 ) ]  * workReg[ oppositeIdxA.getIdx() - rangeStart ] + 
	      			     op[arrayPos][ oppositeIdxB.select( targetBits, 2 ) ]  * workReg[ oppositeIdxB.getIdx() - rangeStart ] + 
	 					 op[arrayPos][ oppositeIdxAB.select( targetBits, 2 ) ] * workReg[ oppositeIdxAB.getIdx() - rangeStart ];
		}  
	}

    CHECK( MPI_Barrier( m_work->getContext().comm() ) );
    m_work->swapInternalData( m_buf );
    deleteBufWindow();
    CHECK( MPI_Barrier( m_work->getContext().comm() ) );
}

//--------------------------------------------------------------------

void DQMachine::createBufWindow()
{
    if ( m_bufWin != MPI_WIN_NULL )
        throw Exception( std::string( "Trying to create window, but previous was not closed." ), std::string( __FUNCTION__ ) );

    MPI_Aint size = unsigned( m_work->getLocalRegLen() * sizeof( QMComplex ) );
    MPI_Comm comm = m_work->getContext().comm();

    CHECK( MPI_Win_create( m_work->getRaw(), size, sizeof( QMComplex ), MPI_INFO_NULL, comm, &m_bufWin ) );
}

void DQMachine::deleteBufWindow()
{
    if ( m_bufWin != MPI_WIN_NULL )
        CHECK( MPI_Win_free( &m_bufWin ) );
}

//--------------------------------------------------------------------
}
