#ifndef H_DQUANT
#define H_DQUANT

#include "QMComplex.h"
#include "Exception.h"
#include "DQMachine.h"
#include "dislib.h"

#include <cstdlib>
#include <string>

//--------------------------------------------------------------------

#ifndef SILENT
	#define QOUT( _x_ ) if (DQuant::amIMaster()) std::cout <<  _x_; std::cout.flush()
#else
	#define QOUT( _x_ )
#endif

#define MASTERPRINT( _x_ ) if (DQuant::amIMaster()) std::cout <<  _x_; std::cout.flush()

//--------------------------------------------------------------------

namespace DQuant{
//--------------------------------------------------------------------

enum
{
	DQERROR = -1,
	MASTERID = 0
};

//--------------------------------------------------------------------

class DQuant
{
private:
    DQuant() {}

public:
    inline static int dquantInit( int* argc, char*** argv )
    {
	    int res  = shmem_init( argc, argv );
	    int npes = shmem_n_pes();

	    if ( !( npes != 0 && (npes & (npes - 1) ) == 0 ) ) 
        {
		    std::string errMsg( "Incorrect number of processess. Must be power of two (" );
		    errMsg.append( std::to_string( (long long)npes ) ).append( ")." );
		    throw Exception( errMsg.c_str(), __FUNCTION__ );
	    }

	    m_isInitialized = true;
	    return res;
    }
 
    //NOTE: this code is from Korj >>normal disgen<<
    inline static double drand( void )
    {
	    double s = 0.0;
	    for ( int i = 0; i < 12; ++i )
		    s += double( rand() ) / RAND_MAX;

	    return s - 6.0;
    }

    inline static void dquantFinalize( void )    { shmem_finalize(); m_isInitialized = false; } 
    inline static bool dquantInitialized( void ) { return m_isInitialized; } 
    inline static bool dquantFinilized( void )   { return !m_isInitialized; }
    inline static void dsrand( unsigned long long seed ) { srand( seed ^ ( shmem_my_pe() + 1 ) ); }
    inline static void synchronise( void ) { shmem_barrier_all(); }
    inline static bool amIMaster( void )   { static bool buf = ( my_pe() == MASTERID ); return buf; }
    inline static double time( void ) { return shmem_time(); }

private:
    static bool m_isInitialized;
};

//--------------------------------------------------------------------
}

#endif
