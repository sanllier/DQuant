#ifndef H_DQMACHINE
#define H_DQMACHINE

#include "OneQubitOp.h"
#include "TwoQubitOp.h"
#include "DQMStateReg.h"
#include "mpi.h"

//--------------------------------------------------------------------

namespace DQ {
//--------------------------------------------------------------------

class DQMachine
{
private:
	//only one instance of DQMachine exists at one time
	DQMachine( DQMStateReg* curReg = 0 );

public:
    ~DQMachine();

	static DQMachine* getMachine( DQMStateReg* curReg = 0 );	

	DQMStateReg* setCurReg( DQMStateReg* curReg );
	DQMStateReg* getCurReg( void ) const;

	void oneQubitEvolution( OneQubitOp& op, int target );	
	void twoQubitEvolution( TwoQubitOp& op, int targetA, int targetB );	

private:
    void createBufWindow();
    void deleteBufWindow();

private:
    static DQMachine* m_curCopy;

    DQMStateReg* m_work;
    DQMStateReg* m_buf;

    MPI_Win m_bufWin;
};

//--------------------------------------------------------------------
}

#endif
