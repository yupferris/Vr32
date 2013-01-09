#include "Nvc.h"

Nvc::Nvc(Emulator32 *emulator)
	: Cpu32(emulator)
{
}

void Nvc::Reset()
{
	random.SetSeed(Time::GetCurrent());
	r[0] = 0;
	for (int i = 1; i < 32; i++) r[i] = random.GetNextInt();
	pc = 0xfffffff0;
	psw = 0x00008000;
	eipc = random.GetNextInt();
	eipsw = random.GetNextInt();
	fepc = random.GetNextInt();
	fepsw = random.GetNextInt();
	ecr = 0x0000fff0;
}
