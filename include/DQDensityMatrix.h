#ifndef H_DQDENSITYMTRIX
#define H_DQDENSITYMTRIX

#include "mpi.h"
#include "QMComplex.h"
#include "Exception.h"
#include "DQMStateReg.h"
#include "mpi.h"
#include "defs.h"

#include <string>

//--------------------------------------------------------------------

namespace DQ {
//--------------------------------------------------------------------

class DQDensityMatrix
{
public:
    DQDensityMatrix( int qNum = 0 );
    ~DQDensityMatrix();

    inline QMComplex& at( long long row, long long col ) 
    {
        #ifndef PERFORMANCE
        if ( row >= m_dim )
            throw QDensityMatOutOfBounds( std::string( __FUNCTION__ ), 0, m_dim, row );  
        if ( col >= m_dim )
            throw QDensityMatOutOfBounds( std::string( __FUNCTION__ ), 0, m_dim, col ); 
        #endif

        return m_data[ row * m_dim + col ];
    }

    inline const QMComplex& at( long long row, long long col ) const
    {
        #ifndef PERFORMANCE
        if ( row >= m_dim || row < 0 )
            throw QDensityMatOutOfBounds( std::string( __FUNCTION__ ), 0, m_dim, row );  
        if ( col >= m_dim || col < 0 )
            throw QDensityMatOutOfBounds( std::string( __FUNCTION__ ), 0, m_dim, col ); 
        #endif

        return m_data[ row * m_dim + col ];
    }

    void clear();
    void fillWithZeros();

    DQMStateReg* convertToStateReg( MPI_Comm baseComm = MPI_COMM_WORLD );

    friend std::ostream& operator<<( std::ostream& oStr, const DQDensityMatrix& mat );
    friend std::istream& operator>>( std::istream& iStr, DQDensityMatrix& mat );

private:
    QMComplex* m_data;

    long long m_dim;
    int m_qnum;
};

//--------------------------------------------------------------------
}

//--------------------------------------------------------------------
#endif
