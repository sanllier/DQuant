#ifndef H_DQMSTATEREG
#define H_DQMSTATEREG

#include "QMComplex.h"
#include "DQContext.h"
#include "mpi.h"
#include "defs.h"

#include <iostream>

//--------------------------------------------------------------------

namespace DQ {
//--------------------------------------------------------------------
class DQMachine;

class DQMStateReg
{
public:
	DQMStateReg( int qnum, MPI_Comm baseComm = MPI_COMM_WORLD );	//qnum is total number of simulating qubits
	~DQMStateReg();

	//NOTE: it is сollective operation
	BASETYPE getNorm() const;

    //NOTE: it is collective operation
    //void kron( DQMStateReg& mulState, DQMStateReg* outputState );

	//NOTE: it is сollective operation
	//Returns -1.0 if vectors are incomparable 
	BASETYPE copmFidelity( const DQMStateReg& scndPart ) const;

	//NOTE: it is сollective operation
	//Also you must call DQuant::dsrand( seed ) before use it
	double fillRandom();	//returns working time if TIMER was defined
	void fillWithZeros();
	void fillSpecial(); 	//{1.0, 0.0, ..., 0.0} - vector useful for debug

    //NOTE: it is collective operation
    void normalize();

	//private data getters
    inline const DQContext& getContext() const { return m_ctx; }
    inline long long getLocalRegLen() const { return m_myPartSize; }
    inline long long getFullRegLen () const {return m_fullRegSize; }
    inline int getQubitsNum() const { return m_qnum; }
    inline QMComplex* getRaw() { return m_qRegister; }
    inline const QMComplex* getRaw() const { return m_qRegister; }

    void swapInternalData( DQMStateReg* scndPart );

	QMComplex& operator[]( long long idx );
    //NOTE: unsafe
    inline QMComplex& fastAccess( long long idx ) { return m_qRegister[ idx ]; }

	friend std::ostream& operator<<( std::ostream& oStr, const DQMStateReg& qState );
	DQMStateReg& operator=( const DQMStateReg& src );

private:
    DQContext m_ctx;

	int m_qnum;		            //total number of simulating qubits
	long long m_fullRegSize;	//total state vector size (global vector)
	long long  m_myPartSize;    //size of state vector local part
	QMComplex* m_qRegister;	    //local part of state vector
};

//--------------------------------------------------------------------
}

//--------------------------------------------------------------------
#endif
