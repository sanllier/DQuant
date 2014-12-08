#ifndef H_DQDENSITYMTRIX
#define H_DQDENSITYMTRIX

#include "mpi.h"
#include "defs.h"

//--------------------------------------------------------------------

namespace DQ {
//--------------------------------------------------------------------

class DQDensityMatrix
{
public:
    DQDensityMatrix();
    ~DQDensityMatrix();

private:
    BASETYPE* m_data;

    int m_dim;
    long long m_localColsNum;
};

//--------------------------------------------------------------------
}

//--------------------------------------------------------------------
#endif
