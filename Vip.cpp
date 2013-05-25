#include "Vip.h"

Vip::Vip()
{
	videoDriver = nullptr;
}

Vip::~Vip()
{
}

void Vip::Update()
{
	flush();
}

void Vip::SetVideoDriver(IVideoDriver *videoDriver)
{
	this->videoDriver = videoDriver;
}

void Vip::CpuCyclesCallback(int numCycles)
{
}

unsigned char Vip::ReadByte(unsigned int address)
{
	return 0;
}

unsigned short Vip::ReadWord(unsigned int address)
{
	return 0;
}

void Vip::WriteByte(unsigned int address, unsigned char value)
{
}

void Vip::WriteWord(unsigned int address, unsigned short value)
{
}

void Vip::flush()
{
}
