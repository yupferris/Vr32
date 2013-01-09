#ifndef __NVC_H__
#define __NVC_H__

#include "Common.h"

class Nvc : public Cpu32
{
public:
	Nvc(Emulator32 *emulator);

	virtual void Reset();

private:
	unsigned int r[32];
	unsigned int pc;
	unsigned int psw, eipc, eipsw, fepc, fepsw, ecr, adtre, chcw, tkcw, pir;

	Random random;
};

#endif
