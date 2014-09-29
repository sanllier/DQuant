#ifndef H_DQMSTATEREG
#define H_DQMSTATEREG

#include "QMComplex.h"

#include <iostream>

//--------------------------------------------------------------------

namespace DQuant {
//--------------------------------------------------------------------
class DQMachine;

class DQMStateReg
{
public:
	friend DQMachine;
	friend void exchngElemHdlr( int from, void* data, int size );

	DQMStateReg( int qnum );	//qnum is total number of simulating qubits
	~DQMStateReg();

	//NOTE: it is сollective operation
	double getNorm( void ) const;

	//NOTE: it is сollective operation
	//Returns -1.0 if vectors are incomparable 
	double copmFidelity( const DQMStateReg& scndPart ) const;

	//NOTE: it is сollective operation
	//Also you must call DQuant::dsrand( seed ) before use it
	double fillRandom( void );	//returns working time if TIMER was defined
	void fillWithZeros( void );
	void fillSpecial( void ); 	//{1.0, 0.0, ..., 0.0} - vector useful for debug

	//private data getters
    inline long long getLocalRegLen( void ) const { return m_myPartSize; }
    inline int getQubitsNum( void ) const { return m_qnum; }
    inline long long getFullRegLen (void ) const {return m_fullRegSize; }
    inline int getPartsNum( void ) const { return m_nodesNum; }
    inline int getId( void ) const { return m_myID; }
    inline QMComplex* getReg( void ) { return m_qRegister; }
    inline const QMComplex* getReg( void ) const { return m_qRegister; }

	QMComplex& operator[]( long long idx );	//deep-copy
	friend std::ostream& operator<<( std::ostream& oStr, const DQMStateReg& qState );
	DQMStateReg& operator=( const DQMStateReg& src );

private:
	int m_myID;	      //rank of current process
	int m_nodesNum;	  //total number of processes
	int m_qnum;		  //total number of simulating qubits
	long long m_fullRegSize;	//total state vector size (global vector)
	long long m_myPartSize;     //size of state vector local part
	QMComplex* m_qRegister;	    //local part of state vector
};

//--------------------------------------------------------------------
}

#endif
