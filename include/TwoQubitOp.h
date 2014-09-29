#ifndef H_TWOQUBITOP
#define H_TWOQUBITOP

#include "DQMOperator.h"

//--------------------------------------------------------------------

namespace DQuant {
//--------------------------------------------------------------------

enum PresetTwoQubitOpType
{
	EMPTYTWOQ,
	CNOT,
    CTRLZ,
    SQCN,
    SQCNC
};

class TwoQubitOp: public DQMOperator
{
public:
	TwoQubitOp( PresetTwoQubitOpType type = EMPTYTWOQ );
    ~TwoQubitOp() {}
    using DQMOperator::operator=;

	void setCNot( void );	
	void setCtrlZ( void );
	void setSQCNot( void );
	void setSQCNotC( void );
};

//--------------------------------------------------------------------
}

#endif
