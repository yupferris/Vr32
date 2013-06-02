#ifndef __NVC_H__
#define __NVC_H__

#include "Common.h"

class Nvc : public Cpu32
{
public:
	Nvc(IEmulator32 *emulator);

	virtual void Reset();

	void Run(int targetCycleCount);

private:
	void setReg(int index, unsigned int value);

	void setPsw(unsigned int value);

	static bool isSigned(unsigned int value);

	void cycles(int numCycles);

	void invalidOpcode();

	unsigned int r[32];
	unsigned int pc;
	bool pswZ, pswS, pswOv, pswCy, pswFpr, pswFud, pswFov, pswFzd, pswFiv, pswFro, pswId, pswAe, pswEp, pswNp;
	unsigned int pswInt;
	unsigned int eipc, eipsw, fepc, fepsw, ecr, adtre, chcw, tkcw, pir;

	int cycleCount;
};

#endif
