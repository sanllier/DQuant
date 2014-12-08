#ifndef H_ONEQUBITOP
#define H_ONEQUBITOP

#include "DQMOperator.h"

//--------------------------------------------------------------------

namespace DQ {
//--------------------------------------------------------------------

enum PresetOneQubitOpType
{
	EMPTYONEQ,
	HADAMARD,
	NOT,
	YPAULI,
	ZPAULI,
    IDENT
};

class OneQubitOp: public DQMOperator
{
public:
	OneQubitOp( PresetOneQubitOpType type = EMPTYONEQ );
    ~OneQubitOp() {}
    using DQMOperator::operator=;

	void setHadamard();
	void setNot();	//XPauli
	void setYPauli();
	void setZPauli();
    void setIdent();
};

//-------------------------------------------------------------------
}

#endif