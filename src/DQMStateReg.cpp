#include "DQuant.h"
#include "DQMStateReg.h"
#include "dislib.h"

#include <cmath>
#include <cstring>
#include <cstdlib>
#include <iostream>

namespace DQuant {
//--------------------------------------------------------------------

DQMStateReg::DQMStateReg( int qnum )
    : m_nodesNum(0)
	, m_myID(0)
	, m_fullRegSize(0)
	, m_myPartSize(0)
	, m_qnum(0)
	, m_qRegister(0)
{
	QOUT( "-- making state register (" << qnum << "q)...\r\n" );
	if ( !DQuant::dquantInitialized() )
		throw DislibInactive( __FUNCTION__ );

	m_myID = my_pe();
	m_nodesNum = num_pes();
	m_fullRegSize = 1LL << qnum;
	m_qnum = qnum;

#ifndef PERFORMANCE
	if ( m_fullRegSize < m_nodesNum )
		throw Exception( "Size of qregister is smaller than number of processess.",
			             __FUNCTION__ );
#endif
	
	m_myPartSize = (int)( m_fullRegSize / m_nodesNum );
	m_qRegister = new QMComplex[ m_myPartSize ];
	QOUT( "-- successfully\r\n" );
}

DQMStateReg::~DQMStateReg()
{
	delete[] m_qRegister;
	m_qRegister = 0;
}

//--------------------------------------------------------------------

double DQMStateReg::getNorm( void ) const
{
	double sum = 0.0;
    double temp = 0.0;

	for ( long long i = 0; i < m_myPartSize; ++i ) 
    {
		temp = abs( m_qRegister[i] );
		sum += temp * temp;
	}	

	shmem_double_allsum( &sum );
	return sqrt(sum);
}

double DQMStateReg::copmFidelity( const DQMStateReg& scndPart ) const
{
#ifndef PERFORMANCE
	if ( m_fullRegSize != scndPart.m_fullRegSize) 
	    throw Exception( "Incomparable states.", __FUNCTION__ );
#endif

	QMComplex sum = 0;
	for ( long long i = 0; i < m_myPartSize; ++i )
		sum += m_qRegister[i] * scndPart.m_qRegister[i].conj();

	double tempRe = sum.real();
	double tempIm = sum.imag();
	shmem_double_allsum( &tempRe );
	shmem_double_allsum( &tempIm );

	double temp = abs( QMComplex( tempRe, tempIm ) );
	return temp * temp;
}

//--------------------------------------------------------------------

double DQMStateReg::fillRandom( void )
{
#ifdef TIMER
    QOUT( "-- randoming...\r\n" );
    double startTime = shmem_time();
#endif

    const static double DRAND_MAX = 3.0;
	for ( long long i = 0; i < m_myPartSize; ++i )
		m_qRegister[i] = QMComplex( DQuant::drand() / DRAND_MAX, DQuant::drand() / DRAND_MAX );

	double norm = getNorm();    
	for ( long long i = 0; i < m_myPartSize; ++i )
		m_qRegister[i] /= norm;

#ifdef TIMER
    double randTime = DQuant::time() - startTime;
    QOUT( "-- randoming time: " << randTime << "\r\n" );
    return randTime;
#else 
    return 0.0;
#endif
}

void DQMStateReg::fillWithZeros( void )
{
	memset( m_qRegister, 0, sizeof( *m_qRegister ) * m_myPartSize );
}

void DQMStateReg::fillSpecial( void )
{
	fillWithZeros();
	if ( m_myID == 0 )
		m_qRegister[0] = 1.0;
}

//--------------------------------------------------------------------

QMComplex& DQMStateReg::operator[]( long long idx )
{
	if ( idx / m_myPartSize == m_myID )
		return m_qRegister[ idx % m_myPartSize ];

	static QMComplex dummyElement( 0, 0 );
	return dummyElement;
}

std::ostream& operator<<( std::ostream& oStr, const DQMStateReg& qState )
{
	for ( int i = 0; i < qState.m_nodesNum; ++i ) 
    {
		if ( qState.m_myID == i ) 
        {
			for ( long long q = 0; q < qState.m_myPartSize; ++q ) 
				oStr << qState.m_qRegister[q] << " ";

			oStr.flush();
		}
		DQuant::synchronise();
	}

	if ( qState.m_myID == MASTERID )
    {
		oStr << "\r\n";
		oStr.flush();
	}
	DQuant::synchronise();
	return oStr;
}

DQMStateReg& DQMStateReg::operator=( const DQMStateReg& src )
{
	memcpy( m_qRegister, src.m_qRegister, sizeof( *m_qRegister ) * m_myPartSize );
	return *this;
}

//--------------------------------------------------------------------
}
