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
	Random random(Time::GetCurrent());
	for (int i = 0; i < 0x10000; i++) wram[i] = random.GetNextInt(256);

	frameTimer = 0;
}

void VirtualBoy::Update()
{
	auto currentTime = Time::GetCurrent();
	int t = 20 - (currentTime - frameTimer);
	Threading::Sleep(t > 1 ? t : 1);
	frameTimer = currentTime;

	nvc->Run(400000);
}

void VirtualBoy::SetVideoDriver(IVideoDriver *videoDriver)
{
}

void VirtualBoy::SetAudioDriver(IAudioDriver *audioDriver)
{
}

void VirtualBoy::CpuCyclesCallback(int numCycles)
{
}

unsigned char VirtualBoy::ReadByte(unsigned int address)
{
	switch (address & 0x07000000)
	{
	case 0:
		// VIP
		break;

	case 0x01000000:
		// VSU
		break;

	case 0x02000000:
		// Hardware
		break;

	case 0x04000000:
		// Cart Expansion
		break;

	case 0x05000000: return wram[address & 0xffff];

	case 0x06000000:
		address &= 0xffffff;
		ensureRamSize(address);
		return ram[address];

	case 0x07000000: return rom[address & (romSize - 1)];
	}

	return 0x00;
}

unsigned short VirtualBoy::ReadWord(unsigned int address)
{
	address &= 0xfffffffe;
	auto low = ReadByte(address);
	auto high = ReadByte(address + 1);
	return (high << 8) | low;
}

unsigned int VirtualBoy::ReadDword(unsigned int address)
{
	address &= 0xfffffffc;
	auto b1 = ReadByte(address);
	auto b2 = ReadByte(address + 1);
	auto b3 = ReadByte(address + 2);
	auto b4 = ReadByte(address + 3);
	return (b4 << 24) | (b3 << 16) | (b2 << 8) | b1;
}

void VirtualBoy::WriteByte(unsigned int address, unsigned char value)
{
	switch (address & 0x07000000)
	{
	case 0:
		// VIP
		break;

	case 0x01000000:
		// VSU
		break;

	case 0x02000000:
		// Hardware
		break;

	case 0x04000000:
		// Cart Expansion
		break;

	case 0x05000000: wram[address & 0xffff] = value; break;

	case 0x06000000:
		address &= 0xffffff;
		ensureRamSize(address);
		ram[address] = value;
		break;

	case 0x07000000: rom[address & (romSize - 1)] = value; break;
	}
}

void VirtualBoy::WriteWord(unsigned int address, unsigned short value)
{
	address &= 0xfffffffe;
	auto low = value & 0xff;
	auto high = (value >> 8) & 0xff;
	WriteByte(address, low);
	WriteByte(address + 1, high);
}

void VirtualBoy::WriteDword(unsigned int address, unsigned int value)
{
	address &= 0xfffffffc;
	auto b1 = value & 0xff;
	auto b2 = (value >> 8) & 0xff;
	auto b3 = (value >> 16) & 0xff;
	auto b4 = (value >> 24) & 0xff;
	WriteByte(address, b1);
	WriteByte(address + 1, b2);
	WriteByte(address + 2, b3);
	WriteByte(address + 3, b4);
}

void VirtualBoy::ensureRamSize(unsigned int ramAddress)
{
	if ((int)ramAddress > ramSize)
	{
		auto oldRamSize = ramSize;
		while (ramSize < (int)ramAddress) ramSize *= 2;
		Random random(Time::GetCurrent());
		for (int i = oldRamSize; i < ramSize; i++) ram[i] = random.GetNextInt(256);
	}
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
	if (ram.Count() < 1024) throw FSL_EXCEPTION("RAM image must be at least 1024 bytes in size");
	if (ram.Count() > 0x1000000) throw FSL_EXCEPTION("RAM image must be at most 16mb in size");
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
