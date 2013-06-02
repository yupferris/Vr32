#include "Vip.h"

const int Vip::frameWidth = 384;
const int Vip::frameHeight = 256;
const int Vip::frameBufferSize = Vip::frameWidth * Vip::frameHeight;

Vip::Vip()
{
	leftFrameBuffer0 = new unsigned char[frameBufferSize];
	leftFrameBuffer1 = new unsigned char[frameBufferSize];
	rightFrameBuffer0 = new unsigned char[frameBufferSize];
	rightFrameBuffer1 = new unsigned char[frameBufferSize];
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
	for (int i = 0; i < frameBufferSize; i++)
	{
		leftFrameBuffer0[i] = random.GetNextInt(256);
		leftFrameBuffer1[i] = random.GetNextInt(256);
		rightFrameBuffer0[i] = random.GetNextInt(256);
		rightFrameBuffer1[i] = random.GetNextInt(256);
	}
	for (int i = 0; i < 0x8000; i++) chrRam[i] = random.GetNextInt(256);

	isColumnTableAddressLocked = false;
	areDisplaySyncSignalsEnabled = false;
	isVipMemoryRefreshing = false;
	isDisplayEnabled = false;
	displayProcedureStatus = DisplayProcedureStatus::None;

	interruptClearReg = random.GetNextInt(0x10000);
	displayControlReg = random.GetNextInt(0x10000);
	ledBrightness1Reg = random.GetNextInt(0x10000);
	ledBrightness2Reg = random.GetNextInt(0x10000);
	ledBrightmess3Reg = random.GetNextInt(0x10000);
	frameRepeatReg = random.GetNextInt(0x10000);
	drawingControlReg = random.GetNextInt(0x10000);
	objGroup0PointerReg = random.GetNextInt(0x10000);
	objGroup1PointerReg = random.GetNextInt(0x10000);
	objGroup2PointerReg = random.GetNextInt(0x10000);
	objGroup3PointerReg = random.GetNextInt(0x10000);

	for (int i = 0; i < 4; i++)
	{
		registerToPalette(random.GetNextInt(0x10000), bgPalettes[i]);
		registerToPalette(random.GetNextInt(0x10000), objPalettes[i]);
	}
	clearColorReg = random.GetNextInt(4);

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
		auto outputWidth = GetOutputWidth();
		auto outputHeight = GetOutputHeight();
		for (int y = 0; y < outputHeight; y++)
		{
			auto yIndex = y * outputWidth;
			for (int x = 0; x < outputWidth; x++)
			{
				auto pixelIndex = yIndex + x;
				auto left = currentLeftFrameBuffer[pixelIndex] * 85;
				auto right = currentRightFrameBuffer[pixelIndex] * 85;
				outputFramebuffer[pixelIndex] = (left << 16) | (right << 8) | right;
			}
		}
		videoDriver->SetOutput(GetOutputWidth(), GetOutputHeight(), outputFramebuffer);
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
		return readPixelByte(leftFrameBuffer0, address);
	}
	else if (address < 0x008000)
	{
		// Chr Ram Pattern Table 0
	}
	else if (address < 0x00e000)
	{
		// Left Frame Buffer 1
		return readPixelByte(leftFrameBuffer1, address - 0x008000);
	}
	else if (address < 0x010000)
	{
		// Chr Ram Pattern Table 1
	}
	else if (address < 0x016000)
	{
		// Right Frame Buffer 0
		return readPixelByte(rightFrameBuffer0, address - 0x010000);
	}
	else if (address < 0x018000)
	{
		// Chr Ram Pattern Table 2
	}
	else if (address < 0x01e000)
	{
		// Right Frame Buffer 1
		return readPixelByte(rightFrameBuffer1, address - 0x018000);
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
		return readPixelWord(leftFrameBuffer0, address);
	}
	else if (address < 0x008000)
	{
		// Chr Ram Pattern Table 0
	}
	else if (address < 0x00e000)
	{
		// Left Frame Buffer 1
		return readPixelWord(leftFrameBuffer1, address - 0x008000);
	}
	else if (address < 0x010000)
	{
		// Chr Ram Pattern Table 1
	}
	else if (address < 0x016000)
	{
		// Right Frame Buffer 0
		return readPixelWord(rightFrameBuffer0, address - 0x010000);
	}
	else if (address < 0x018000)
	{
		// Chr Ram Pattern Table 2
	}
	else if (address < 0x01e000)
	{
		// Right Frame Buffer 1
		return readPixelWord(rightFrameBuffer1, address - 0x018000);
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
		case 0x0005f800: flush(); break; // Interrupt Pending
		case 0x0005f802: break; // Interrupt Enable
		case 0x0005f820: // Display Status
			flush();
			return
				((isColumnTableAddressLocked ? 1 : 0) << 10) |
				((areDisplaySyncSignalsEnabled ? 1 : 0) << 9) |
				((isVipMemoryRefreshing ? 1 : 0) << 8) |
				((displayProcedureStatus == DisplayProcedureStatus::Beginning ? 1 : 0) << 7) |
				((displayProcedureStatus == DisplayProcedureStatus::Ready ? 1 : 0) << 6) |
				((displayProcedureStatus == DisplayProcedureStatus::RightFb1BeingDisplayed ? 1 : 0) << 5) |
				((displayProcedureStatus == DisplayProcedureStatus::LeftFb1BeingDisplayed ? 1 : 0) << 4) |
				((displayProcedureStatus == DisplayProcedureStatus::RightFb0BeingDisplayed ? 1 : 0) << 3) |
				((displayProcedureStatus == DisplayProcedureStatus::LeftFb0BeingDisplayed ? 1 : 0) << 2) |
				((isDisplayEnabled ? 1 : 0) << 1);
		case 0x0005f830: flush(); break; // Column Table Address
		case 0x0005f840: flush(); break; // Drawing Status
		case 0x0005f844: return 2; // Version
		case 0x0005f848: return objGroup0PointerReg; // OBJ Group 0 Pointer
		case 0x0005f84a: return objGroup1PointerReg; // OBJ Group 1 Pointer
		case 0x0005f84c: return objGroup2PointerReg; // OBJ Group 2 Pointer
		case 0x0005f84e: return objGroup3PointerReg; // OBJ Group 3 Pointer
		case 0x0005f860: return paletteToRegister(bgPalettes[0]); // BG Palette 0
		case 0x0005f862: return paletteToRegister(bgPalettes[1]); // BG Palette 1
		case 0x0005f864: return paletteToRegister(bgPalettes[2]); // BG Palette 2
		case 0x0005f866: return paletteToRegister(bgPalettes[3]); // BG Palette 3
		case 0x0005f868: return paletteToRegister(objPalettes[0]); // OBJ Palette 0
		case 0x0005f86a: return paletteToRegister(objPalettes[1]); // OBJ Palette 1
		case 0x0005f86c: return paletteToRegister(objPalettes[2]); // OBJ Palette 2
		case 0x0005f86e: return paletteToRegister(objPalettes[3]); // OBJ Palette 3
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
		writePixelByte(leftFrameBuffer0, address, value);
	}
	else if (address < 0x008000)
	{
		// Chr Ram Pattern Table 0
	}
	else if (address < 0x00e000)
	{
		// Left Frame Buffer 1
		writePixelByte(leftFrameBuffer1, address - 0x008000, value);
	}
	else if (address < 0x010000)
	{
		// Chr Ram Pattern Table 1
	}
	else if (address < 0x016000)
	{
		// Right Frame Buffer 0
		writePixelByte(rightFrameBuffer0, address - 0x010000, value);
	}
	else if (address < 0x018000)
	{
		// Chr Ram Pattern Table 2
	}
	else if (address < 0x01e000)
	{
		// Right Frame Buffer 1
		writePixelByte(rightFrameBuffer1, address - 0x018000, value);
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
		writePixelWord(leftFrameBuffer0, address, value);
	}
	else if (address < 0x008000)
	{
		// Chr Ram Pattern Table 0
	}
	else if (address < 0x00e000)
	{
		// Left Frame Buffer 1
		writePixelWord(leftFrameBuffer1, address - 0x008000, value);
	}
	else if (address < 0x010000)
	{
		// Chr Ram Pattern Table 1
	}
	else if (address < 0x016000)
	{
		// Right Frame Buffer 0
		writePixelWord(rightFrameBuffer0, address - 0x010000, value);
	}
	else if (address < 0x018000)
	{
		// Chr Ram Pattern Table 2
	}
	else if (address < 0x01e000)
	{
		// Right Frame Buffer 1
		writePixelWord(rightFrameBuffer1, address - 0x018000, value);
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
		case 0x0005f802: break; // Interrupt Enable
		case 0x0005f804: break; // Interrupt Clear
		case 0x0005f822: // Display Control
			flush();
			isColumnTableAddressLocked = ((value >> 10) & 1) == 1;
			areDisplaySyncSignalsEnabled = ((value >> 9) & 1) == 1;
			if ((value >> 8) & 1) startVipRefresh();
			isDisplayEnabled = ((value >> 1) & 1) == 1;
			break;
		case 0x0005f824: break; // LED Brightness 1
		case 0x0005f826: break; // LED Brightness 2
		case 0x0005f828: break; // LED Brightness 3
		case 0x0005f82e: flush(); frameRepeatReg = value; // Frame Repeat
		case 0x0005f842: break; // Drawing Control
		case 0x0005f848: break; // OBJ Group 0 Pointer
		case 0x0005f84a: break; // OBJ Group 1 Pointer
		case 0x0005f84c: break; // OBJ Group 2 Pointer
		case 0x0005f84e: break; // OBJ Group 3 Pointer
		case 0x0005f860: flush(); registerToPalette(value, bgPalettes[0]); break; // BG Palette 0
		case 0x0005f862: flush(); registerToPalette(value, bgPalettes[1]); break; // BG Palette 1
		case 0x0005f864: flush(); registerToPalette(value, bgPalettes[2]); break; // BG Palette 2
		case 0x0005f866: flush(); registerToPalette(value, bgPalettes[3]); break; // BG Palette 3
		case 0x0005f868: flush(); registerToPalette(value, objPalettes[0]); break; // OBJ Palette 0
		case 0x0005f86a: flush(); registerToPalette(value, objPalettes[1]); break; // OBJ Palette 1
		case 0x0005f86c: flush(); registerToPalette(value, objPalettes[2]); break; // OBJ Palette 2
		case 0x0005f86e: flush(); registerToPalette(value, objPalettes[3]); break; // OBJ Palette 3
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

void Vip::startVipRefresh()
{
	// TODO: I have no fucking clue what else this entails.
	isVipMemoryRefreshing = true;
}

unsigned char Vip::readPixelByte(const unsigned char *frameBuffer, unsigned int address)
{
	throw FSL_EXCEPTION("Pixel read");
}

unsigned short Vip::readPixelWord(const unsigned char *frameBuffer, unsigned int address)
{
	return (readPixelByte(frameBuffer, address + 1) << 16) | readPixelByte(frameBuffer, address);
}

void Vip::writePixelByte(unsigned char *frameBuffer, unsigned int address, unsigned char value)
{
	throw FSL_EXCEPTION("Pixel write");
}

void Vip::writePixelWord(unsigned char *frameBuffer, unsigned int address, unsigned short value)
{
	writePixelByte(frameBuffer, address, value & 0xff);
	writePixelByte(frameBuffer, address + 1, (value >> 8) & 0xff);
}

unsigned short Vip::paletteToRegister(unsigned int *palette)
{
	return (palette[3] << 6) | (palette[2] << 4) | (palette[1] << 2);
}

void Vip::registerToPalette(unsigned short reg, unsigned int *palette)
{
	palette[0] = 0;
	for (int i = 1; i < 4; i++) palette[i] = (reg >> (i * 2)) & 0x03;
}
