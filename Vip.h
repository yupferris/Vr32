#ifndef __VIP_H__
#define __VIP_H__

#include "Common.h"

class Vip
{
public:
	Vip();
	~Vip();

	void Reset();

	void StartFrame();
	void EndFrame();

	void SetVideoDriver(IVideoDriver *videoDriver);

	void CpuCyclesCallback(int numCycles);

	unsigned char ReadByte(unsigned int address);
	unsigned short ReadWord(unsigned int address);
	void WriteByte(unsigned int address, unsigned char value);
	void WriteWord(unsigned int address, unsigned short value);

	int GetOutputWidth() const;
	int GetOutputHeight() const;

private:
	static const int frameWidth, frameHeight;

	void flush();

	unsigned char *leftFrameBuffer0, *leftFrameBuffer1;
	unsigned char *rightFrameBuffer0, *rightFrameBuffer1;
	unsigned char *chrRam;

	unsigned int *outputFramebuffer;

	IVideoDriver *videoDriver;

	unsigned char *currentLeftFrameBuffer, *currentRightFrameBuffer;

	int cyclesSinceLastFlush;
};

#endif
