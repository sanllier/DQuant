#include "DQContext.h"
#include "Exception.h"
#include "mpicheck.h"
#include "DQuant.h"

#include <string>
#include <cmath>

//--------------------------------------------------------------------

namespace DQ {
//--------------------------------------------------------------------

DQContext::DQContext( int qnum, MPI_Comm baseComm/* = MPI_COMM_WORLD */)
    : m_rank( -1 )
    , m_comm( MPI_COMM_NULL )
    , m_commSize(0)
    , m_active( false )
{
    if ( baseComm == MPI_COMM_NULL )
        throw Exception( std::string( "Invalid baseComm ( MPI_COMM_NULL )" ), std::string( __FUNCTION__ ) ); 

    int isMPIInitialized = 0;
    CHECK( MPI_Initialized( &isMPIInitialized ) );
    if ( !isMPIInitialized )
        throw MPIException( std::string( "MPI is not initialized ( call DQuat::dquantInit )" ), std::string( __FUNCTION__ ) );

    int baseCommId = 0;
    int baseCommSize = 0;
    CHECK( MPI_Comm_rank( baseComm, &baseCommId ) );
    CHECK( MPI_Comm_size( baseComm, &baseCommSize ) );
    if ( baseCommSize <= 0 )
        throw Exception( std::string( "Smth wrong with baseComm ( size <= 0 )" ), std::string( __FUNCTION__ ) ); 

    int targetCommSize = int( pow( 2, floor( log( baseCommSize ) / log(2) ) ) );
    const long long stateVecSize = 1LL << qnum;
    if ( targetCommSize > stateVecSize ) targetCommSize = int( stateVecSize );

    m_active = baseCommId < targetCommSize;

    int* ranks = new int[ targetCommSize ];
    for ( int i = 0; i < targetCommSize; ++i )
        ranks[i] = i;

    MPI_Group baseGroup;
    MPI_Group targetGroup;
    CHECK( MPI_Comm_group( baseComm, &baseGroup ) );
    CHECK( MPI_Group_incl( baseGroup, targetCommSize, ranks, &targetGroup ) );

    delete[] ranks;
    ranks = 0;

    CHECK( MPI_Comm_create( baseComm, targetGroup, &m_comm ) ); 
    CHECK( MPI_Group_free( &baseGroup ) );
    CHECK( MPI_Group_free( &targetGroup ) );

    if ( m_comm != MPI_COMM_NULL )
    {
        CHECK( MPI_Comm_rank( m_comm, &m_rank ) );
        CHECK( MPI_Comm_size( m_comm, &m_commSize ) );        
    }
    else
    {
        m_rank = -1;
        m_commSize = 0;
    }
}

DQContext::~DQContext()
{
    if ( DQuant::dquantInitialized() && m_active )
        CHECK( MPI_Comm_free( &m_comm ) );
}

//--------------------------------------------------------------------
}
