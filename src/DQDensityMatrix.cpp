#include "DQDensityMatrix.h"

#include <iostream>

//--------------------------------------------------------------------

namespace DQ {
//--------------------------------------------------------------------

DQDensityMatrix::DQDensityMatrix( int qnum/* = 0*/)
    : m_qnum( qnum )
    , m_dim(0)
    , m_data(0)
{
    if ( qnum < 0 )
        throw Exception( std::string( "Invalid number of qbits. " ), std::string( __FUNCTION__ ) );

    if ( qnum > 0 )
    {
        m_dim = 1LL << qnum;
        m_data = new QMComplex[ m_dim * m_dim ];
    }
}

DQDensityMatrix::~DQDensityMatrix()
{
    clear();
}

//--------------------------------------------------------------------

void DQDensityMatrix::clear()
{
    m_qnum = 0;
    m_dim  = 0;
    delete[] m_data;
    m_data = 0;
}

void DQDensityMatrix::fillWithZeros()
{
    std::memset( m_data, 0, m_dim * m_dim * sizeof( QMComplex ) );
}


//--------------------------------------------------------------------

DQMStateReg* DQDensityMatrix::convertToStateReg( MPI_Comm baseComm/* = MPI_COMM_WORLD */)
{
    if ( m_dim <= 0 )
        throw Exception( std::string( "Attempt to convert empty density matrix. " ), std::string( __FUNCTION__ ) );

    DQMStateReg* reg = new DQMStateReg( m_qnum * 2, baseComm );
    long long startIdx = reg->getLocalRegLen() * reg->getContext().rank();
    for ( long long i = 0; i < reg->getLocalRegLen(); ++i )
        (*reg)[i] = m_data[ startIdx + i ];

    return reg;
}

//--------------------------------------------------------------------

std::ostream& operator<<( std::ostream& oStr, const DQDensityMatrix& mat )
{
    for ( long long i = 0; i < mat.m_dim; ++i )
    {
        for ( long long q = 0; q < mat.m_dim; ++q )
            oStr << mat.m_data[ i * mat.m_dim + q ];
        
        oStr << "\r\n";
    }
    oStr.flush();

    return oStr;
}

std::istream& operator>>( std::istream& iStr, DQDensityMatrix& mat )
{
    int qNum;
    iStr >> qNum;

    if ( qNum <= 0 )
        throw Exception( std::string( "Invalid number of qbits. " ), std::string( __FUNCTION__ ) );

    mat.clear();
    mat.m_qnum = qNum;
    mat.m_dim = 1LL << qNum;
    mat.m_data = new QMComplex[ mat.m_dim * mat.m_dim ];

    for ( long long i = 0; i < mat.m_dim * mat.m_dim; ++i )       
        iStr >> mat.m_data[i];

    return iStr;
}

//--------------------------------------------------------------------
}