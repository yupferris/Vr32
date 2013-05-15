#include "VirtualBoy.h"

VirtualBoy::VirtualBoy()
{
	nvc = new Nvc(this);
	rom = new unsigned char[0x1000000];
	ram = new unsigned char[0x1000000];
	wram = new unsigned char[0x10000];
	for (int i = 0; i < 0x1000000; i++) rom[i] = ram[i] = 0;
	romSize = ramSize = 0;
	Reset();
}

VirtualBoy::~VirtualBoy()
{
	delete nvc;

	delete [] rom;
	delete [] ram;
	delete [] wram;
}

void VirtualBoy::Reset()
{
	nvc->Reset();
	for (int i = 0; i < 0x10000; i++) wram[i] = 0;
}

void VirtualBoy::Update()
{
	Threading::Sleep(1);
}

void VirtualBoy::SetVideoModule(IVideoModule *videoModule)
{
	// TODO
}

unsigned char VirtualBoy::ReadByte(unsigned int address)
{
	return 0;
}

unsigned short VirtualBoy::ReadWord(unsigned int address)
{
	return 0;
}

unsigned int VirtualBoy::ReadDword(unsigned int address)
{
	return 0;
}

void VirtualBoy::WriteByte(unsigned int address, unsigned char value)
{

}

void VirtualBoy::WriteWord(unsigned int address, unsigned short value)
{

}

void VirtualBoy::WriteDword(unsigned int address, unsigned int value)
{

}

void VirtualBoy::LoadRom(const List<unsigned char>& rom)
{
	if (rom.Count() < 1024) throw FSL_EXCEPTION("ROM image must be at least 1024 bytes in size");
	if (rom.Count() > 0x1000000) throw FSL_EXCEPTION("ROM image must be at most 16mb in size");
	bool found = false;
	for (int i = 1; i < 0x1000000; i <<= 1)
	{
		if ((rom.Count() & i) == rom.Count())
		{
			found = true;
			break;
		}
	}
	if (!found) throw FSL_EXCEPTION("ROM image size must be a power of 2");
	Reset();
	romSize = rom.Count();
	for (int i = 0; i < romSize; i++) this->rom[i] = rom[i];
}

void VirtualBoy::LoadRam(const List<unsigned char>& ram)
{
	if (ram.Count() < 1024) throw FSL_EXCEPTION("ROM image must be at least 1024 bytes in size");
	if (ram.Count() > 0x1000000) throw FSL_EXCEPTION("ROM image must be at most 16mb in size");
	bool found = false;
	for (int i = 1; i < 0x1000000; i <<= 1)
	{
		if ((ram.Count() & i) == ram.Count())
		{
			found = true;
			break;
		}
	}
	if (!found) throw FSL_EXCEPTION("RAM image size must be a power of 2");
	ramSize = ram.Count();
	for (int i = 0; i < 0x1000000; i++) this->ram[i] = i < ramSize ? ram[i] : 0;
}

int VirtualBoy::GetOutputWidth() const
{
	return 384;
}

int VirtualBoy::GetOutputHeight() const
{
	return 224;
}

unsigned char *VirtualBoy::GetRam() const
{
	return ram;
}

int VirtualBoy::GetRamSize() const
{
	return ramSize;
}
