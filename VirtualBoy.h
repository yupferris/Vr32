#ifndef __VIRTUALBOY_H__
#define __VIRTUALBOY_H__

#include "Common.h"
#include "Nvc.h"
#include "Vip.h"

class VirtualBoy : public IEmulator32
{
public:
	VirtualBoy();
	~VirtualBoy();

	virtual void Reset();
	virtual void Update();

	virtual void SetVideoDriver(IVideoDriver *videoDriver);
	virtual void SetAudioDriver(IAudioDriver *audioDriver);

	virtual void CpuCyclesCallback(int numCycles);

	virtual unsigned char ReadByte(unsigned int address);
	virtual unsigned short ReadWord(unsigned int address);
	virtual unsigned int ReadDword(unsigned int address);
	virtual void WriteByte(unsigned int address, unsigned char value);
	virtual void WriteWord(unsigned int address, unsigned short value);
	virtual void WriteDword(unsigned int address, unsigned int value);

	void ensureRamSize(unsigned int ramAddress);

	void LoadRom(const List<unsigned char>& rom);
	void LoadRam(const List<unsigned char>& ram);

	virtual int GetOutputWidth() const;
	virtual int GetOutputHeight() const;

	unsigned char *GetRam() const;
	int GetRamSize() const;

private:
	Nvc *nvc;
	Vip vip;

	unsigned char *rom;
	unsigned char *ram;
	unsigned char *wram;
	int romSize, ramSize;

	unsigned int frameTimer;
};

#endif
