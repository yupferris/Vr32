#include "Vip.h"

const int Vip::frameWidth = 384;
const int Vip::frameHeight = 256;

Vip::Vip()
{
	leftFrameBuffer0 = new unsigned char[0x6000];
	leftFrameBuffer1 = new unsigned char[0x6000];
	rightFrameBuffer0 = new unsigned char[0x6000];
	rightFrameBuffer1 = new unsigned char[0x6000];
	chrRam = new unsigned char[0x8000];

	outputFramebuffer = new unsigned int[GetOutputWidth() * GetOutputHeight()];

	videoDriver = nullptr;

	Reset();
}

Vip::~Vip()
{
	delete [] leftFrameBuffer0;
	delete [] leftFrameBuffer1;
	delete [] rightFrameBuffer0;
	delete [] rightFrameBuffer1;
	delete [] chrRam;

	delete [] outputFramebuffer;
}

void Vip::Reset()
{
	Random random(Time::GetCurrent());
	for (int i = 0; i < 0x6000; i++)
	{
		leftFrameBuffer0[i] = random.GetNextInt(256);
		leftFrameBuffer1[i] = random.GetNextInt(256);
		rightFrameBuffer0[i] = random.GetNextInt(256);
		rightFrameBuffer1[i] = random.GetNextInt(256);
	}
	for (int i = 0; i < 0x8000; i++) chrRam[i] = random.GetNextInt(256);

	currentLeftFrameBuffer = leftFrameBuffer0;
	currentRightFrameBuffer = rightFrameBuffer0;

	cyclesSinceLastFlush = 0;
}

void Vip::StartFrame()
{
}

void Vip::EndFrame()
{
	flush();

	if (videoDriver)
	{
		for (int x = 0; x < GetOutputWidth(); x += 8)
		{
			int framebufferXOffset = (x / 8 * 2 * frameHeight);
			
			for (int y = 0; y < GetOutputHeight(); y++)
			{
				int outputYOffset = y * GetOutputWidth();

				// meh
			}
		}
		//videoDriver->SetOutput(GetOutputWidth(), GetOutputHeight(), outputFramebuffer);
	}
}

void Vip::SetVideoDriver(IVideoDriver *videoDriver)
{
	this->videoDriver = videoDriver;
}

void Vip::CpuCyclesCallback(int numCycles)
{
	cyclesSinceLastFlush += numCycles;
}

unsigned char Vip::ReadByte(unsigned int address)
{
	address &= 0x0007ffff;
	return 0;
}

unsigned short Vip::ReadWord(unsigned int address)
{
	address &= 0x0007ffff;
	if (address < 0x006000)
	{
		// Left Frame Buffer 0
	}
	else if (address < 0x008000)
	{
		// Chr Ram Pattern Table 0
	}
	else if (address < 0x00e000)
	{
		// Left Frame Buffer 1
	}
	else if (address < 0x010000)
	{
		// Chr Ram Pattern Table 1
	}
	else if (address < 0x016000)
	{
		// Right Frame Buffer 0
	}
	else if (address < 0x018000)
	{
		// Chr Ram Pattern Table 2
	}
	else if (address < 0x01e000)
	{
		// Right Frame Buffer 1
	}
	else if (address < 0x020000)
	{
		// Chr Ram Pattern Table 3
	}
	else if (address < 0x03d800)
	{
		// Background Segments and Window Parameter Table
	}
	else if (address < 0x03dc00)
	{
		// Window Attributes
	}
	else if (address < 0x03e000)
	{
		// Column Table
	}
	else if (address < 0x040000)
	{
		// OAM
	}
	else if (address < 0x060000)
	{
		// VIP Control Registers
		if (address < 0x05e000) address = (address & 0x001fff) | 0x05e000; // Mirroring
		switch (address)
		{
		case 0x0005f800: // Interrupt Pending
			break;
		case 0x0005f802: // Interrupt Enable
			break;
		case 0x0005f804: // Interrupt Clear
			break;
		case 0x0005f820: // Display Status
			break;
		case 0x0005f822: // Display Control
			break;
		case 0x0005f824: // LED Brightness 1
			break;
		case 0x0005f826: // LED Brightness 2
			break;
		case 0x0005f828: // LED Brightness 3
			break;
		case 0x0005f82a: // LED Brightness Idle
			break;
		case 0x0005f82e: // Frame Repeat
			break;
		case 0x0005f830: // Column Table Address
			break;
		case 0x0005f840: // Drawing Status
			break;
		case 0x0005f842: // Drawing Control
			break;
		case 0x0005f844: // Version
			break;
		case 0x0005f848: // OBJ Group 0 Pointer
			break;
		case 0x0005f84a: // OBJ Group 1 Pointer
			break;
		case 0x0005f84c: // OBJ Group 2 Pointer
			break;
		case 0x0005f84e: // OBJ Group 3 Pointer
			break;
		case 0x0005f860: // BG Palette 0
			break;
		case 0x0005f862: // BG Palette 1
			break;
		case 0x0005f864: // BG Palette 2
			break;
		case 0x0005f866: // BG Palette 3
			break;
		case 0x0005f868: // OBJ Palette 0
			break;
		case 0x0005f86a: // OBJ Palette 1
			break;
		case 0x0005f86c: // OBJ Palette 2
			break;
		case 0x0005f86e: // OBJ Palette 3
			break;
		case 0x0005f870: // Clear Color
			break;
		}
	}
	else if (address >= 0x078000)
	{
		// Chr Ram Pattern Table Mirrors
	}
	return 0;
}

void Vip::WriteByte(unsigned int address, unsigned char value)
{
	address &= 0x0007ffff;
}

void Vip::WriteWord(unsigned int address, unsigned short value)
{
	address &= 0x0007ffff;
	if (address < 0x006000)
	{
		// Left Frame Buffer 0
	}
	else if (address < 0x008000)
	{
		// Chr Ram Pattern Table 0
	}
	else if (address < 0x00e000)
	{
		// Left Frame Buffer 1
	}
	else if (address < 0x010000)
	{
		// Chr Ram Pattern Table 1
	}
	else if (address < 0x016000)
	{
		// Right Frame Buffer 0
	}
	else if (address < 0x018000)
	{
		// Chr Ram Pattern Table 2
	}
	else if (address < 0x01e000)
	{
		// Right Frame Buffer 1
	}
	else if (address < 0x020000)
	{
		// Chr Ram Pattern Table 3
	}
	else if (address < 0x03d800)
	{
		// Background Segments and Window Parameter Table
	}
	else if (address < 0x03dc00)
	{
		// Window Attributes
	}
	else if (address < 0x03e000)
	{
		// Column Table
	}
	else if (address < 0x040000)
	{
		// OAM
	}
	else if (address < 0x060000)
	{
		// VIP Control Registers
		if (address < 0x05e000) address = (address & 0x001fff) | 0x05e000; // Mirroring
		switch (address)
		{
		case 0x0005f800: // Interrupt Pending
			break;
		case 0x0005f802: // Interrupt Enable
			break;
		case 0x0005f804: // Interrupt Clear
			break;
		case 0x0005f820: // Display Status
			break;
		case 0x0005f822: // Display Control
			break;
		case 0x0005f824: // LED Brightness 1
			break;
		case 0x0005f826: // LED Brightness 2
			break;
		case 0x0005f828: // LED Brightness 3
			break;
		case 0x0005f82a: // LED Brightness Idle
			break;
		case 0x0005f82e: // Frame Repeat
			break;
		case 0x0005f830: // Column Table Address
			break;
		case 0x0005f840: // Drawing Status
			break;
		case 0x0005f842: // Drawing Control
			break;
		case 0x0005f844: // Version
			break;
		case 0x0005f848: // OBJ Group 0 Pointer
			break;
		case 0x0005f84a: // OBJ Group 1 Pointer
			break;
		case 0x0005f84c: // OBJ Group 2 Pointer
			break;
		case 0x0005f84e: // OBJ Group 3 Pointer
			break;
		case 0x0005f860: // BG Palette 0
			break;
		case 0x0005f862: // BG Palette 1
			break;
		case 0x0005f864: // BG Palette 2
			break;
		case 0x0005f866: // BG Palette 3
			break;
		case 0x0005f868: // OBJ Palette 0
			break;
		case 0x0005f86a: // OBJ Palette 1
			break;
		case 0x0005f86c: // OBJ Palette 2
			break;
		case 0x0005f86e: // OBJ Palette 3
			break;
		case 0x0005f870: // Clear Color
			break;
		}
	}
	else if (address >= 0x078000)
	{
		// Chr Ram Pattern Table Mirrors
	}
}

int Vip::GetOutputWidth() const
{
	return 384;
}

int Vip::GetOutputHeight() const
{
	return 224;
}

void Vip::flush()
{


	cyclesSinceLastFlush = 0;
}
