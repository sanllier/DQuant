#ifndef H_DQCONTEXT
#define H_DQCONTEXT

#include "mpi.h"

//--------------------------------------------------------------------

namespace DQ {
//--------------------------------------------------------------------

class DQContext
{
public:
    DQContext( int qnum, MPI_Comm baseComm = MPI_COMM_WORLD );
    ~DQContext();

    inline int rank() const { return m_rank; }
    inline MPI_Comm comm() const { return m_comm; }
    inline int commSize() const { return m_commSize; }
    inline bool isActive() const { return m_active; }

private:
    int m_rank;

    MPI_Comm m_comm;
    int m_commSize;

    bool m_active;
};

//--------------------------------------------------------------------
}

//--------------------------------------------------------------------
#endif
