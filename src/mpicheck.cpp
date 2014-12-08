#include "mpicheck.h"
#include "Exception.h"
#include "mpi.h"

#include <string>

//------------------------------------------------------------

void checkMPIRes( int errCode, const char* location )
{
    if ( errCode != MPI_SUCCESS )
    {
        char errText[ MPI_MAX_ERROR_STRING ];
        int len;
        MPI_Error_string( errCode, errText, &len );
        throw DQ::MPIException( std::string( errText ), std::string( location ) );
    }
}

//------------------------------------------------------------
