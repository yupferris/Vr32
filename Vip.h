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
	static const int frameBufferSize;

	void flush();

	unsigned short paletteToRegister(unsigned int *palette);
	void registerToPalette(unsigned short reg, unsigned int *palette);

	unsigned char *leftFrameBuffer0, *leftFrameBuffer1;
	unsigned char *rightFrameBuffer0, *rightFrameBuffer1;
	unsigned char *chrRam;

	unsigned int *outputFramebuffer;

	IVideoDriver *videoDriver;

	unsigned short interruptPendingReg;
	unsigned short interruptEnableReg;
	unsigned short interruptClearReg;
	unsigned short displayStatusReg;
	unsigned short displayControlReg;
	unsigned short ledBrightness1Reg, ledBrightness2Reg, ledBrightmess3Reg, ledBrightnessIdleReg;
	unsigned short frameRepeatReg;
	unsigned short columnTableAddressReg;
	unsigned short drawingStatusReg;
	unsigned short drawingControlReg;
	unsigned short objGroup0PointerReg, objGroup1PointerReg, objGroup2PointerReg, objGroup3PointerReg;

	unsigned int bgPalettes[4][4], objPalettes[4][4];
	unsigned int clearColorReg;

	unsigned char *currentLeftFrameBuffer, *currentRightFrameBuffer;

	int cyclesSinceLastFlush;
};

#endif
