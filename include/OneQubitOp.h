#ifndef H_ONEQUBITOP
#define H_ONEQUBITOP

#include "DQMOperator.h"

//--------------------------------------------------------------------

namespace DQuant {
//--------------------------------------------------------------------

enum PresetOneQubitOpType
{
	EMPTYONEQ,
	HADAMARD,
	NOT,
	YPAULI,
	ZPAULI
};

class OneQubitOp: public DQMOperator
{
public:
	OneQubitOp( PresetOneQubitOpType type = EMPTYONEQ );
    ~OneQubitOp() {}
    using DQMOperator::operator=;

	void setHadamard( void );
	void setNot( void );	//XPauli
	void setYPauli( void );
	void setZPauli( void );
};

//-------------------------------------------------------------------
}

#endif