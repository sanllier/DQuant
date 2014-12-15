#ifndef H_DQUANT
#define H_DQUANT

#include "QMComplex.h"
#include "Exception.h"
#include "DQMachine.h"
#include "DQDensityMatrix.h"
#include "defs.h"

#include <cstdlib>

//--------------------------------------------------------------------

#define MASTERPRINT( _x_ ) if (DQuant::amIMaster()) std::cout <<  _x_; std::cout.flush()

#ifndef SILENT
	#define QOUT( _x_ ) MASTERPRINT( _x_ )
#else
	#define QOUT( _x_ )
#endif

//--------------------------------------------------------------------

namespace DQ {
//--------------------------------------------------------------------

class DQuant
{
private:
    DQuant() {}

public:
    static void dquantInit( int* argc, char*** argv );
    static void dquantFinalize( void ); 

    inline static bool dquantInitialized() { return m_isInitialized; } 
    inline static bool dquantFinilized()   { return !m_isInitialized; }

    //NOTE: this code is by Korj >>normal disgen<<
    inline static BASETYPE drand( void )
    {
	    BASETYPE s = 0.0;
	    for ( int i = 0; i < 12; ++i )
		    s += BASETYPE( rand() ) / RAND_MAX;

	    return s - 6.0;
    }
  
    static void dsrand( int seed );
    static bool amIMaster( void );

private:
    static bool m_isInitialized;

};

//--------------------------------------------------------------------
}

#endif
