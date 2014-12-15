#include "DQuant.h"
#include "DQMStateReg.h"
#include "mpicheck.h"

#include <cmath>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <algorithm>

namespace DQ {
//--------------------------------------------------------------------

#define ACTIVE_ONLY if ( !m_ctx.isActive() ) return
#define ACTIVE_ONLY_R( _x_ ) if ( !m_ctx.isActive() ) return _x_

//--------------------------------------------------------------------

//enum
//{
//    KRONSTEPHDLR = 6
//};

//--------------------------------------------------------------------

DQMStateReg::DQMStateReg( int qnum, MPI_Comm baseComm/* = MPI_COMM_WORLD */)
    : m_ctx( qnum, baseComm )
    , m_fullRegSize( 1LL << qnum )
    , m_myPartSize(0)
    , m_qnum( qnum )
    , m_qRegister(0)
{
    if ( m_ctx.isActive() )
    {
        m_myPartSize = m_fullRegSize / m_ctx.commSize();
        m_qRegister = new QMComplex[ unsigned( m_myPartSize ) ];
        QOUT( "-- DQMStateReg was successfully created\r\n" );
    }
}

DQMStateReg::~DQMStateReg()
{
    delete[] m_qRegister;
    m_qRegister = 0;
}

//--------------------------------------------------------------------

BASETYPE DQMStateReg::getNorm() const
{
    ACTIVE_ONLY_R( BASETYPE(0) );

    BASETYPE localSum = BASETYPE(0);
    for ( long long i = 0; i < m_myPartSize; ++i ) 
    {
        const BASETYPE temp = BASETYPE( abs( m_qRegister[i] ) );
        localSum += temp * temp;
    }

    BASETYPE totalSum = BASETYPE(0);
    CHECK( MPI_Allreduce( &localSum, &totalSum, 1, MPI_BASETYPE, MPI_SUM, m_ctx.comm() ) );
    return BASETYPE( sqrt( totalSum ) );
}

BASETYPE DQMStateReg::copmFidelity( const DQMStateReg& scndPart ) const
{
    ACTIVE_ONLY_R( BASETYPE(0) );

#ifndef PERFORMANCE
    if ( m_fullRegSize != scndPart.m_fullRegSize) 
        throw Exception( "Incomparable states.", __FUNCTION__ );
#endif

    QMComplex sum = 0;
    for ( long long i = 0; i < m_myPartSize; ++i )
        sum += m_qRegister[i] * scndPart.m_qRegister[i].conj();

    BASETYPE localRe = sum.real();
    BASETYPE localIm = sum.imag();
    BASETYPE totalRe = 0.0;
    BASETYPE totalIm = 0.0;
    CHECK( MPI_Allreduce( &localRe, &totalRe, 1, MPI_BASETYPE, MPI_SUM, m_ctx.comm() ) );
    CHECK( MPI_Allreduce( &localIm, &totalIm, 1, MPI_BASETYPE, MPI_SUM, m_ctx.comm() ) );

    const BASETYPE temp = BASETYPE( abs( QMComplex( totalRe, totalIm ) ) );
    return temp * temp;
}

//--------------------------------------------------------------------

//static DQMStateReg* kronWorkReg = 0;
//static QMComplex* kronRecvPart = 0;
//static QMComplex* kronResult = 0;
//int kronIterWorkPartShift = 0;
//int kronIterResultPartShift = 0;
//
//void kronStepHdlr( int from, void* data, int size )
//{
//#ifndef PERFORMANCE
//    if ( !kronResult || !kronRecvPart || !kronWorkReg )
//        throw Exception( "NULL static variable." , __FUNCTION__ );
//#endif
//
//	QMComplex* recvData = ( QMComplex* )(data);
//    const long long recvElementsNum = size / sizeof( *recvData );
//    const QMComplex* workPart = kronWorkReg->getReg();
//    const long long workRegLen = kronWorkReg->getLocalRegLen();
//
//    for ( long long i = 0; i < recvElementsNum; ++i )
//    {
//        kronResult[ kronIterResultPartShift++ ] = workPart[ kronIterWorkPartShift++ ] * recvData[i];
//        if ( kronIterWorkPartShift >= workRegLen )
//            kronIterWorkPartShift = 0;
//    }
//    kronRecvPart = 0;
//}
//
//void DQMStateReg::kron( DQMStateReg& mulState, DQMStateReg* outputState )
//{
//    if ( outputState == 0 )
//        throw Exception( "NULL pointer is invalid" , __FUNCTION__ );
//
//    shmem_register_handler( kronStepHdlr, KRONSTEPHDLR );
//
//    outputState->m_qnum = m_qnum * mulState.m_fullRegSize;
//    outputState->m_fullRegSize = m_fullRegSize * mulState.m_fullRegSize;
//    outputState->m_myPartSize = m_myPartSize * mulState.m_fullRegSize;
//    
//    kronWorkReg = this;
//    kronResult = new QMComplex[ outputState->m_myPartSize ];
//    kronIterWorkPartShift = 0;
//    kronIterResultPartShift = 0;
//
//    for ( int i = 0; i < m_nodesNum; ++i )
//    {
//        if ( i == m_myID )
//            for ( int q = 0; q < m_nodesNum; ++q )
//                shmem_send( mulState.getReg(), KRONSTEPHDLR, sizeof( *( mulState.getReg() ) ) * mulState.getLocalRegLen(), i );
//
//        DQuant::synchronise();
//    }
//
//    outputState->m_qRegister = kronResult;
//}

//--------------------------------------------------------------------

double DQMStateReg::fillRandom()
{
    ACTIVE_ONLY_R(0.0);

#ifdef TIMER
    QOUT( "-- randoming...\r\n" );
    double startTime = MPI_Wtime();
#endif

    const static BASETYPE DRAND_MAX = BASETYPE(3);
    for ( long long i = 0; i < m_myPartSize; ++i )
        m_qRegister[i] = QMComplex( DQuant::drand() / DRAND_MAX, DQuant::drand() / DRAND_MAX );

    const BASETYPE norm = getNorm();    
    for ( long long i = 0; i < m_myPartSize; ++i )
        m_qRegister[i] /= norm;

#ifdef TIMER
    double randTime = MPI_Wtime() - startTime;
    QOUT( "-- randoming time: " << randTime << "\r\n" );
    return randTime;
#else 
    return 0.0;
#endif
}

void DQMStateReg::fillWithZeros()
{
    ACTIVE_ONLY;

    memset( m_qRegister, 0, sizeof( *m_qRegister ) * unsigned( m_myPartSize ) );
}

void DQMStateReg::fillSpecial()
{
    ACTIVE_ONLY;

    fillWithZeros();
    if ( m_ctx.rank() == 0 )
        m_qRegister[0] = 1.0;
}

//--------------------------------------------------------------------

void DQMStateReg::normalize()
{
    BASETYPE curNorm = getNorm();
    for ( long long i = 0; i < m_myPartSize; ++i )
        m_qRegister[i] /= curNorm;
}

//--------------------------------------------------------------------

void DQMStateReg::swapInternalData( DQMStateReg* scndPart )
{
    ACTIVE_ONLY;

    if ( m_qnum != scndPart->m_qnum )
        throw Exception( std::string( "Can't swap states (sizes are different)." ), std::string( __FUNCTION__ ) );

    std::swap( m_qRegister, scndPart->m_qRegister );
}

//--------------------------------------------------------------------

QMComplex& DQMStateReg::operator[]( long long idx )
{
    static QMComplex dummyElement( 0, 0 );
    ACTIVE_ONLY_R( dummyElement );

    if ( idx / m_myPartSize == m_ctx.rank() )
        return m_qRegister[ idx % m_myPartSize ];

    return dummyElement;
}

std::ostream& operator<<( std::ostream& oStr, const DQMStateReg& qState )
{
    if ( !qState.m_ctx.isActive() )
        return oStr;

    for ( int i = 0; i < qState.m_ctx.commSize(); ++i ) 
    {
        if ( qState.m_ctx.rank() == i ) 
        {
            oStr << "Rank " << qState.m_ctx.rank() << ": ";

            for ( long long q = 0; q < qState.m_myPartSize; ++q ) 
                oStr << qState.m_qRegister[q] << " ";

            oStr.flush();
        }
        CHECK( MPI_Barrier( qState.m_ctx.comm() ) );
    }

    if ( qState.m_ctx.rank() == MASTERID )
    {
        oStr << "\r\n";
        oStr.flush();
    }
    CHECK( MPI_Barrier( qState.m_ctx.comm() ) );
    return oStr;
}

DQMStateReg& DQMStateReg::operator=( const DQMStateReg& src )
{
    ACTIVE_ONLY_R(*this);

    memcpy( m_qRegister, src.m_qRegister, sizeof( *m_qRegister ) * unsigned( m_myPartSize ) );
    return *this;
}

//--------------------------------------------------------------------
}
