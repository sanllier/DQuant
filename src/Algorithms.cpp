#include "Algorithms.h"
#include "dislib.h"

#include <cmath>
#include <iostream>

//CRAP
#ifndef M_PI
    #define M_PI 3.1415926535;
#endif

//--------------------------------------------------------------------

namespace DQuant { namespace Algorithms {
//--------------------------------------------------------------------

void nHadamardTransform( DQMachine& qMachine, double nF )
{
	OneQubitOp op( HADAMARD );
	if ( nF != 0.0 )
		op.addNoise(nF);

	const int qNum = qMachine.getCurReg()->getQubitsNum();
	for ( int i = 0; i < qNum; ++i )
		qMachine.oneQubitEvolution( op, i );
}

//FIXME
void phaseInverseTest( DQMachine& qMachine )
{
	QMComplex* rawReg = qMachine.getCurReg()->getReg();
	const long long rangeStart = qMachine.getCurReg()->getLocalRegLen() * qMachine.getCurReg()->getId();
	const long long rangeEnd = rangeStart + qMachine.getCurReg()->getLocalRegLen();
	const int shift = qMachine.getCurReg()->getQubitsNum();// / 2;
	const long long end = 1LL << shift;
	long long temp = 0;

	for( long long i = 1; i < end; ++i )
    {
		temp = i << shift;
		if ( temp >= rangeStart && temp < rangeEnd )
			rawReg[ temp - rangeStart ] *= -1;
	}
}

void phaseInverse( DQMachine& qMachine )
{
	OneQubitOp NOTop(NOT);
	const  int numQubits = qMachine.getCurReg()->getQubitsNum() / 2;

	//Inversing all qubits
	for ( int i = 1; i < numQubits; ++i )
		qMachine.oneQubitEvolution( NOTop, i );
	
	//Doing n-qubits controlled-Z (see Nielsen-Chuang fig. 4.10)
	toffoli( qMachine, 1, 2, numQubits + 1 );

	for( int i = 0; i < numQubits - 3; ++i )
		toffoli( qMachine, i + 3, numQubits + i + 1, numQubits + i + 2 );

	TwoQubitOp CZ(CTRLZ);
	qMachine.twoQubitEvolution( CZ,  2 * numQubits - 2, numQubits );

	for( int i = numQubits - 4; i >= 0; --i )
		toffoli( qMachine, i + 3, numQubits + i + 1, numQubits + i + 2 );

	toffoli( qMachine, 1, 2, numQubits + 1 );
	//finished

	//Inversing qubits back
	for ( int i = 1; i <= numQubits; ++i )
		qMachine.oneQubitEvolution( NOTop, i );
}

void toffoli( DQMachine& qMachine, int i, int j, int k, double nF )
{
	TwoQubitOp CNOTop(CNOT);
	TwoQubitOp SQCNOTop(SQCN);
	TwoQubitOp SQCNOTCop(SQCNC);

	if ( nF != 0.0 )
    {
		CNOTop.addNoise(nF);
		SQCNOTop.addNoise(nF);
		SQCNOTCop.addNoise(nF);
	}

	qMachine.twoQubitEvolution( SQCNOTop,  j, k );
	qMachine.twoQubitEvolution( CNOTop,    i, j );
	qMachine.twoQubitEvolution( SQCNOTCop, j, k );
	qMachine.twoQubitEvolution( CNOTop,    i, j );
	qMachine.twoQubitEvolution( SQCNOTop,  i, k );
}

void grover( DQMachine& qMachine, void (*oracle)(DQMStateReg* reg), double nF )
{
#ifdef TIMER
    double startTimeTotal = DQuant::time();
    double startTimeCycle;
#endif

	nHadamardTransform( qMachine, nF );

	const int stepsNum = (int)ceil( M_PI * sqrt( double( 1LL << ( qMachine.getCurReg()->getQubitsNum() / 2 ) ) ) / 4.0 );

	//QOUT("-- total GA iterations: " << stepsNum  << "\r\n");

	for (int i = 1; i <= stepsNum; ++i) 
    {
		//QOUT("-- " << i << " | ");
	#ifdef TIMER
		startTimeCycle = DQuant::time();
	#endif

		oracle( qMachine.getCurReg() );
		nHadamardTransform( qMachine, nF );
		phaseInverse( qMachine );
		nHadamardTransform( qMachine, nF );

	#ifdef TIMER
		//QOUT("iteration time: " << DQuant::time() - startTimeCycle << "\r\n");
	#endif
	}

#ifdef TIMER
	//QOUT("-- TOTAL GROVER TIME: " << DQuant::time() - startTimeTotal << "\r\n");
#endif
}

//--------------------------------------------------------------------

inline void makePsiAdd( DQMStateReg& state )
{
    const int shift = state.getLocalRegLen() % 2;
    for ( long long i = 0; i < state.getLocalRegLen(); ++i )
        state.fastAccess(i) = i % 2 + shift;
}

DQMStateReg* shoreCode( DQMachine& qMachine, int q, double nF )
{
    const int qNum = qMachine.getCurReg()->getQubitsNum();
    if ( q < 0 || q >= qNum * qNum )
        throw QRegisterOutOfBounds( __FUNCTION__, 0, qNum * qNum, q );

    DQMStateReg psiAdd(8);
    makePsiAdd( psiAdd );
    DQMStateReg* resState = new DQMStateReg( qMachine.getCurReg()->getQubitsNum() * psiAdd.getFullRegLen() );

    if ( qMachine.getCurReg()->getQubitsNum() == 1 )
        qMachine.getCurReg()->kron( psiAdd, resState );
    //else  //WTF
        //qMachine.getCurReg()->kron(  );

	TwoQubitOp CNOTop(CNOT);
    OneQubitOp HOp(HADAMARD);

	if ( nF != 0.0 )
    {
		CNOTop.addNoise(nF);
	}

	qMachine.twoQubitEvolution( CNOTop, q, qNum + 3 );
    qMachine.twoQubitEvolution( CNOTop, q, qNum + 6 );
    qMachine.oneQubitEvolution( HOp, q );
    qMachine.oneQubitEvolution( HOp, q + 3 );
    qMachine.oneQubitEvolution( HOp, q + 6 );
    qMachine.twoQubitEvolution( CNOTop, q, qNum + 1 );
    qMachine.twoQubitEvolution( CNOTop, q, qNum + 2 );	
    qMachine.twoQubitEvolution( CNOTop, q + 3, qNum + 4 );
    qMachine.twoQubitEvolution( CNOTop, q + 3, qNum + 5 );	
    qMachine.twoQubitEvolution( CNOTop, q + 6, qNum + 7 );
    qMachine.twoQubitEvolution( CNOTop, q + 6, qNum + 8 );

    return resState;
}

//--------------------------------------------------------------------
} }
