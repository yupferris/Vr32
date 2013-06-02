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
	vip.Reset();
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

	vip.StartFrame();
	nvc->Run(400000);
	vip.EndFrame();
}

void VirtualBoy::SetVideoDriver(IVideoDriver *videoDriver)
{
	vip.SetVideoDriver(videoDriver);
}

void VirtualBoy::SetAudioDriver(IAudioDriver *audioDriver)
{
}

void VirtualBoy::CpuCyclesCallback(int numCycles)
{
	vip.CpuCyclesCallback(numCycles);
}

unsigned char VirtualBoy::ReadByte(unsigned int address)
{
	switch (address & 0x07000000)
	{
	case 0: return vip.ReadByte(address);
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
	if (!(address & 0x01))
	{
		switch (address & 0x07000000)
		{
		case 0: return vip.ReadWord(address);
		case 0x01000000:
			// VSU
			break;

		case 0x02000000:
			// Hardware
			break;

		case 0x04000000:
			// Cart Expansion
			break;

		case 0x05000000: return ((unsigned short *)wram)[(address / 2) & 0xffff];

		case 0x06000000:
			address &= 0xffffff;
			ensureRamSize(address);
			return ((unsigned short *)ram)[address / 2];

		case 0x07000000: return ((unsigned short *)rom)[(address & (romSize - 1)) / 2];
		}
	}

	return 0x00;
}

unsigned int VirtualBoy::ReadDword(unsigned int address)
{
	if (!(address & 0x03))
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

		case 0x05000000: return ((unsigned int *)wram)[(address / 4) & 0xffff];

		case 0x06000000:
			address &= 0xffffff;
			ensureRamSize(address);
			return ((unsigned int *)ram)[address / 4];

		case 0x07000000: return ((unsigned int *)rom)[(address & (romSize - 1)) / 4];
		}
	}

	return 0x00;
}

void VirtualBoy::WriteByte(unsigned int address, unsigned char value)
{
	switch (address & 0x07000000)
	{
	case 0: vip.WriteByte(address, value); break;
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

	case 0x07000000: rom[address & (romSize - 1)] = value;
	}
}

void VirtualBoy::WriteWord(unsigned int address, unsigned short value)
{
	if (!(address & 0x01))
	{
		switch (address & 0x07000000)
		{
		case 0: vip.WriteWord(address, value); break;
		case 0x01000000:
			// VSU
			break;

		case 0x02000000:
			// Hardware
			break;

		case 0x04000000:
			// Cart Expansion
			break;

		case 0x05000000: ((unsigned short *)wram)[(address & 0xffff) / 2] = value; break;

		case 0x06000000:
			address &= 0xffffff;
			ensureRamSize(address);
			((unsigned short *)ram)[address / 2] = value;
			break;

		case 0x07000000: ((unsigned short *)rom)[(address & (romSize - 1)) / 2] = value;
		}
	}
}

void VirtualBoy::WriteDword(unsigned int address, unsigned int value)
{
	if (!(address & 0x03))
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

		case 0x05000000: ((unsigned int *)wram)[(address & 0xffff) / 4] = value; break;

		case 0x06000000:
			address &= 0xffffff;
			ensureRamSize(address);
			((unsigned int *)ram)[address / 4] = value;
			break;

		case 0x07000000: ((unsigned int *)rom)[(address & (romSize - 1)) / 4] = value;
		}
	}
}

void VirtualBoy::ensureRamSize(unsigned int ramAddress)
{
	if ((int)ramAddress > ramSize)
	{
		auto oldRamSize = ramSize;
		if (!ramSize) ramSize = 1;
		while (ramSize < (int)ramAddress) ramSize *= 2;
		Random random(Time::GetCurrent());
		for (int i = oldRamSize; i < ramSize; i++) ram[i] = random.GetNextInt(256);
	}
}

void VirtualBoy::LoadRom(const List<unsigned char>& rom)
{
	if (rom.Count() < 1024) throw FSL_EXCEPTION("ROM image must be at least 1024 bytes in size");
	if (rom.Count() > 0x1000000) throw FSL_EXCEPTION("ROM image must be at most 16mb in size");
	for (int i = 1; i < 0x1000000; i <<= 1)
	{
		if ((rom.Count() & i) == rom.Count())
		{
			Reset();
			romSize = rom.Count();
			for (int i = 0; i < romSize; i++) this->rom[i] = rom[i];
			return;
		}
	}
	throw FSL_EXCEPTION("ROM image size must be a power of 2");
}

void VirtualBoy::LoadRam(const List<unsigned char>& ram)
{
	if (ram.Count() < 1024) throw FSL_EXCEPTION("RAM image must be at least 1024 bytes in size");
	if (ram.Count() > 0x1000000) throw FSL_EXCEPTION("RAM image must be at most 16mb in size");
	for (int i = 1; i < 0x1000000; i <<= 1)
	{
		if ((ram.Count() & i) == ram.Count())
		{
			ramSize = ram.Count();
			for (int i = 0; i < 0x1000000; i++) this->ram[i] = i < ramSize ? ram[i] : 0;
			return;
		}
	}
	throw FSL_EXCEPTION("RAM image size must be a power of 2");
}

int VirtualBoy::GetOutputWidth() const
{
	return vip.GetOutputWidth();
}

int VirtualBoy::GetOutputHeight() const
{
	return vip.GetOutputHeight();
}

unsigned char *VirtualBoy::GetRam() const
{
	return ram;
}

int VirtualBoy::GetRamSize() const
{
	return ramSize;
}
