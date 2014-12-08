#include "DQMOperator.h"
#include "Exception.h"
#include "DQuant.h"
#include "mpicheck.h"
#include "mpi.h"
#include "defs.h"

#include <cstring>
#include <cmath>

//--------------------------------------------------------------------

namespace DQ {
//--------------------------------------------------------------------

DQMOperator::DQMOperator( OperatorSize size )
    : m_size( size )
{
#ifndef PERFORMANCE
	if ( m_size != ONEQUBIT && m_size != TWOQUBIT ) 
		throw OperatorException( "Incorrect size of DQMOperator.", __FUNCTION__ );
#endif

	m_operator = new QMComplex* [ m_size ];
	for ( int i = 0; i < m_size; ++i ) 
    {
		m_operator[i] = new QMComplex[ m_size ];
        memset( m_operator[i], 0, sizeof( **m_operator ) * m_size );
	}
}

DQMOperator::~DQMOperator()
{
	for ( int i = 0; i < m_size; ++i )
		delete[] m_operator[i];

	delete[] m_operator;
	m_operator = 0;
}

//--------------------------------------------------------------------

void DQMOperator::clear( void )
{
	for ( int i = 0; i < m_size; ++i )
        memset( m_operator[i], 0, sizeof( **m_operator ) * m_size );
}

void DQMOperator::addNoise( double noiseFactor )
{
	BASETYPE theta = 0.0;

	if ( DQuant::amIMaster() )
		theta = DQuant::drand() * noiseFactor;

    CHECK( MPI_Bcast( &theta, 1, MPI_BASETYPE, MASTERID, MPI_COMM_WORLD ) );

	DQMOperator noise( ONEQUBIT );
	noise[0][0] =  noise[1][1] = cos( theta );
	noise[0][1] =  sin( theta );
	noise[1][0] = -sin (theta );

	DQMOperator temp( ONEQUBIT );
	int shift = m_size == ONEQUBIT ? 0 : 2;

	temp[0][0] = m_operator[0 + shift][0 + shift];
	temp[0][1] = m_operator[0 + shift][1 + shift];
	temp[1][0] = m_operator[1 + shift][0 + shift];
	temp[1][1] = m_operator[1 + shift][1 + shift];

	temp = temp * noise;

	m_operator[0 + shift][0 + shift] = temp[0][0];
	m_operator[0 + shift][1 + shift] = temp[0][1];
	m_operator[1 + shift][0 + shift] = temp[1][0];
	m_operator[1 + shift][1 + shift] = temp[1][1];
}

//--------------------------------------------------------------------

QMComplex* DQMOperator::operator[]( int idx )
{
#ifndef PERFORMANCE
	if ( idx >= m_size )
		throw OperatorException( "DQMOperator incorrect index.", __FUNCTION__ );
#endif
	return m_operator[ idx ];
}

DQMOperator& DQMOperator::operator=( const DQMOperator& scndOp )
{
#ifndef PERFORMANCE
	if ( m_size != scndOp.m_size )
		throw OperatorException( "Incorrect assignment (different sizes).", __FUNCTION__ );
#endif
	for ( int i = 0; i < m_size; ++i )
		memcpy( m_operator[i], scndOp.m_operator[i], sizeof(**m_operator) * m_size );

	return *this;
}

DQMOperator DQMOperator::operator*( const DQMOperator& rOp) const
{
#ifndef PERFORMANCE
	if ( m_size != rOp.m_size) {
		throw OperatorException( "Incorrect multiplication (different sizes).", __FUNCTION__ );
	}
#endif

	DQMOperator temp( m_size );
	for (int i = 0; i < m_size; ++i)
		for (int q = 0; q < m_size; ++q)
			for (int k = 0; k < m_size; ++k)
				temp[i][q] += m_operator[i][k] * rOp.m_operator[k][q];

	return temp;
}

std::ostream& operator<<( std::ostream& oStr, const DQMOperator& op )
{
	for ( int i = 0; i < op.m_size; ++i ) 
    {
		for ( int q = 0; q < op.m_size; ++q ) 
			oStr << op.m_operator[i][q] << " ";

		oStr << "\r\n";
	}
	return oStr;
}

//--------------------------------------------------------------------
}
