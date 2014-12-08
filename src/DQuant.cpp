#include "DQuant.h"
#include "mpi.h"
#include "mpicheck.h"

#include <ctime>
#include <cstdlib>

//--------------------------------------------------------------------

namespace DQ {
//--------------------------------------------------------------------

bool DQuant::m_isInitialized = false;

//--------------------------------------------------------------------

void DQuant::dquantInit( int* argc, char*** argv )
{
    int isMPIInitialized = 0;
    CHECK( MPI_Initialized( &isMPIInitialized ) );
    if ( !isMPIInitialized )
        CHECK( MPI_Init( argc, argv ) );

    if ( !m_isInitialized )
        dsrand( (int)time(0) );

	m_isInitialized = true;
}

void DQuant::dquantFinalize()
{
    int isMPIInitialized = 0;
    CHECK( MPI_Initialized( &isMPIInitialized ) );
    if ( isMPIInitialized )
        CHECK( MPI_Finalize() );

    m_isInitialized = false;
}

//--------------------------------------------------------------------

void DQuant::dsrand( int seed )
{
    int rank = 0;
    CHECK( MPI_Comm_rank( MPI_COMM_WORLD, &rank ) );
    srand( seed ^ ( rank + 1 ) );
}

bool DQuant::amIMaster()
{
    static int rank = -1;
    if ( rank == -1 )
        CHECK( MPI_Comm_rank( MPI_COMM_WORLD, &rank ) );

    return rank == MASTERID;
}

//--------------------------------------------------------------------
}
