#ifndef __VIP_H__
#define __VIP_H__

#include "Common.h"

class Vip
{
public:
	Vip();
	~Vip();

	void Update();

	void SetVideoDriver(IVideoDriver *videoDriver);

	void CpuCyclesCallback(int numCycles);

	unsigned char ReadByte(unsigned int address);
	unsigned short ReadWord(unsigned int address);
	void WriteByte(unsigned int address, unsigned char value);
	void WriteWord(unsigned int address, unsigned short value);

private:
	void flush();

	IVideoDriver *videoDriver;
};

#endif
