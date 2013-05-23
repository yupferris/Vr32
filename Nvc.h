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

	void setPswZ(bool set);
	bool getPswZ() const;
	void setPswS(bool set);
	bool getPswS() const;
	void setPswOv(bool set);
	bool getPswOv() const;
	void setPswCy(bool set);
	void setPswId(bool set);

	static bool isSigned(unsigned int value);

	void cycles(int numCycles);

	void invalidOpcode();

	unsigned int r[32];
	unsigned int pc;
	unsigned int psw, eipc, eipsw, fepc, fepsw, ecr, adtre, chcw, tkcw, pir;

	int cycleCount;
};

#endif
