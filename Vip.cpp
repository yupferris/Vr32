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

	interruptPendingReg = random.GetNextInt(0xffff);
	interruptEnableReg = random.GetNextInt(0xffff);
	interruptClearReg = random.GetNextInt(0xffff);
	displayStatusReg = random.GetNextInt(0xffff);
	displayControlReg = random.GetNextInt(0xffff);
	ledBrightness1Reg = random.GetNextInt(0xffff);
	ledBrightness2Reg = random.GetNextInt(0xffff);
	ledBrightmess3Reg = random.GetNextInt(0xffff);
	ledBrightnessIdleReg = random.GetNextInt(0xffff);
	frameRepeatReg = random.GetNextInt(0xffff);
	columnTableAddressReg = random.GetNextInt(0xffff);
	drawingStatusReg = random.GetNextInt(0xffff);
	drawingControlReg = random.GetNextInt(0xffff);
	objGroup0PointerReg = random.GetNextInt(0xffff);
	objGroup1PointerReg = random.GetNextInt(0xffff);
	objGroup2PointerReg = random.GetNextInt(0xffff);
	objGroup3PointerReg = random.GetNextInt(0xffff);
	bgPalette0Reg = random.GetNextInt(0xffff);
	bgPalette1Reg = random.GetNextInt(0xffff);
	bgPalette2Reg = random.GetNextInt(0xffff);
	bgPalette3Reg = random.GetNextInt(0xffff);
	objPalette0Reg = random.GetNextInt(0xffff);
	objPalette1Reg = random.GetNextInt(0xffff);
	objPalette2Reg = random.GetNextInt(0xffff);
	objPalette3Reg = random.GetNextInt(0xffff);
	clearColorReg = random.GetNextInt(0xffff);

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
	if (address < 0x006000)
	{
		// Left Frame Buffer 0
		return leftFrameBuffer0[address];
	}
	else if (address < 0x008000)
	{
		// Chr Ram Pattern Table 0
	}
	else if (address < 0x00e000)
	{
		// Left Frame Buffer 1
		return leftFrameBuffer1[address - 0x008000];
	}
	else if (address < 0x010000)
	{
		// Chr Ram Pattern Table 1
	}
	else if (address < 0x016000)
	{
		// Right Frame Buffer 0
		return rightFrameBuffer0[address - 0x010000];
	}
	else if (address < 0x018000)
	{
		// Chr Ram Pattern Table 2
	}
	else if (address < 0x01e000)
	{
		// Right Frame Buffer 1
		return rightFrameBuffer1[address - 0x018000];
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
	else if (address >= 0x078000)
	{
		// Chr Ram Pattern Table Mirrors
		return chrRam[address & 0x7fff];
	}
	return 0;
}

unsigned short Vip::ReadWord(unsigned int address)
{
	address &= 0x0007ffff;
	if (address < 0x006000)
	{
		// Left Frame Buffer 0
		return ((unsigned short *)leftFrameBuffer0)[address / 2];
	}
	else if (address < 0x008000)
	{
		// Chr Ram Pattern Table 0
	}
	else if (address < 0x00e000)
	{
		// Left Frame Buffer 1
		return ((unsigned short *)leftFrameBuffer1)[(address - 0x008000) / 2];
	}
	else if (address < 0x010000)
	{
		// Chr Ram Pattern Table 1
	}
	else if (address < 0x016000)
	{
		// Right Frame Buffer 0
		return ((unsigned short *)rightFrameBuffer0)[(address - 0x010000) / 2];
	}
	else if (address < 0x018000)
	{
		// Chr Ram Pattern Table 2
	}
	else if (address < 0x01e000)
	{
		// Right Frame Buffer 1
		return ((unsigned short *)rightFrameBuffer1)[(address - 0x018000) / 2];
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
		case 0x0005f800: flush(); return interruptPendingReg; // Interrupt Pending
		case 0x0005f802: return interruptEnableReg; // Interrupt Enable
		case 0x0005f804: return interruptClearReg; // Interrupt Clear
		case 0x0005f820: flush(); return displayStatusReg; // Display Status
		case 0x0005f822: return displayControlReg; // Display Control
		case 0x0005f824: return ledBrightness1Reg; // LED Brightness 1
		case 0x0005f826: return ledBrightness2Reg; // LED Brightness 2
		case 0x0005f828: return ledBrightmess3Reg; // LED Brightness 3
		case 0x0005f82a: return ledBrightnessIdleReg; // LED Brightness Idle
		case 0x0005f82e: return frameRepeatReg; // Frame Repeat
		case 0x0005f830: flush(); return columnTableAddressReg; // Column Table Address
		case 0x0005f840: flush(); return drawingStatusReg; // Drawing Status
		case 0x0005f842: return drawingControlReg; // Drawing Control
		case 0x0005f844: return 2; // Version
		case 0x0005f848: return objGroup0PointerReg; // OBJ Group 0 Pointer
		case 0x0005f84a: return objGroup1PointerReg; // OBJ Group 1 Pointer
		case 0x0005f84c: return objGroup2PointerReg; // OBJ Group 2 Pointer
		case 0x0005f84e: return objGroup3PointerReg; // OBJ Group 3 Pointer
		case 0x0005f860: return bgPalette0Reg; // BG Palette 0
		case 0x0005f862: return bgPalette1Reg; // BG Palette 1
		case 0x0005f864: return bgPalette2Reg; // BG Palette 2
		case 0x0005f866: return bgPalette3Reg; // BG Palette 3
		case 0x0005f868: return objPalette0Reg; // OBJ Palette 0
		case 0x0005f86a: return objPalette1Reg; // OBJ Palette 1
		case 0x0005f86c: return objPalette2Reg; // OBJ Palette 2
		case 0x0005f86e: return objPalette3Reg; // OBJ Palette 3
		case 0x0005f870: return clearColorReg; // Clear Color
		}
	}
	else if (address >= 0x078000)
	{
		// Chr Ram Pattern Table Mirrors
		return ((unsigned short *)chrRam)[(address & 0x7fff) / 2];
	}
	return 0;
}

void Vip::WriteByte(unsigned int address, unsigned char value)
{
	address &= 0x0007ffff;
	if (address < 0x006000)
	{
		// Left Frame Buffer 0
		leftFrameBuffer0[address] = value;
	}
	else if (address < 0x008000)
	{
		// Chr Ram Pattern Table 0
	}
	else if (address < 0x00e000)
	{
		// Left Frame Buffer 1
		leftFrameBuffer1[address - 0x008000] = value;
	}
	else if (address < 0x010000)
	{
		// Chr Ram Pattern Table 1
	}
	else if (address < 0x016000)
	{
		// Right Frame Buffer 0
		rightFrameBuffer0[address - 0x010000] = value;
	}
	else if (address < 0x018000)
	{
		// Chr Ram Pattern Table 2
	}
	else if (address < 0x01e000)
	{
		// Right Frame Buffer 1
		rightFrameBuffer1[address - 0x018000] = value;
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
	else if (address >= 0x078000)
	{
		// Chr Ram Pattern Table Mirrors
		chrRam[address & 0x7fff] = value;
	}
}

void Vip::WriteWord(unsigned int address, unsigned short value)
{
	address &= 0x0007ffff;
	if (address < 0x006000)
	{
		// Left Frame Buffer 0
		((unsigned short *)leftFrameBuffer0)[address / 2] = value;
	}
	else if (address < 0x008000)
	{
		// Chr Ram Pattern Table 0
	}
	else if (address < 0x00e000)
	{
		// Left Frame Buffer 1
		((unsigned short *)leftFrameBuffer1)[(address - 0x008000) / 2] = value;
	}
	else if (address < 0x010000)
	{
		// Chr Ram Pattern Table 1
	}
	else if (address < 0x016000)
	{
		// Right Frame Buffer 0
		((unsigned short *)rightFrameBuffer0)[(address - 0x010000) / 2] = value;
	}
	else if (address < 0x018000)
	{
		// Chr Ram Pattern Table 2
	}
	else if (address < 0x01e000)
	{
		// Right Frame Buffer 1
		((unsigned short *)rightFrameBuffer1)[(address - 0x018000) / 2] = value;
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
		case 0x0005f800: break; // Interrupt Pending
		case 0x0005f802: break; // Interrupt Enable
		case 0x0005f804: break; // Interrupt Clear
		case 0x0005f820: break; // Display Status
		case 0x0005f822: break; // Display Control
		case 0x0005f824: break; // LED Brightness 1
		case 0x0005f826: break; // LED Brightness 2
		case 0x0005f828: break; // LED Brightness 3
		case 0x0005f82a: break; // LED Brightness Idle
		case 0x0005f82e: break; // Frame Repeat
		case 0x0005f830: break; // Column Table Address
		case 0x0005f840: break; // Drawing Status
		case 0x0005f842: break; // Drawing Control
		case 0x0005f848: break; // OBJ Group 0 Pointer
		case 0x0005f84a: break; // OBJ Group 1 Pointer
		case 0x0005f84c: break; // OBJ Group 2 Pointer
		case 0x0005f84e: break; // OBJ Group 3 Pointer
		case 0x0005f860: flush(); bgPalette0Reg = value & 0x00ff; break; // BG Palette 0
		case 0x0005f862: flush(); bgPalette1Reg = value & 0x00ff; break; // BG Palette 1
		case 0x0005f864: flush(); bgPalette2Reg = value & 0x00ff; break; // BG Palette 2
		case 0x0005f866: flush(); bgPalette3Reg = value & 0x00ff; break; // BG Palette 3
		case 0x0005f868: flush(); objPalette0Reg = value & 0x00ff; break; // OBJ Palette 0
		case 0x0005f86a: flush(); objPalette1Reg = value & 0x00ff; break; // OBJ Palette 1
		case 0x0005f86c: flush(); objPalette2Reg = value & 0x00ff; break; // OBJ Palette 2
		case 0x0005f86e: flush(); objPalette3Reg = value & 0x00ff; break; // OBJ Palette 3
		case 0x0005f870: flush(); clearColorReg = value & 0x0003; // Clear Color
		}
	}
	else if (address >= 0x078000)
	{
		// Chr Ram Pattern Table Mirrors
		((unsigned short *)chrRam)[(address & 0x7fff) / 2] = value;
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
