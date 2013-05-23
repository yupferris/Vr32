#include "Nvc.h"

Nvc::Nvc(IEmulator32 *emulator)
	: Cpu32(emulator)
{
}

void Nvc::Reset()
{
	Random random(Time::GetCurrent());
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

void Nvc::Run(int targetCycleCount)
{
	for (cycleCount = 0; cycleCount < targetCycleCount; )
	{
		unsigned int instruction = emulator->ReadWord(pc);
		if ((instruction & 0xe000) == 0x8000)
		{
			// Format III
			unsigned int cond = (instruction >> 9) & 0x0f;
			unsigned int disp9 = instruction & 0x01ff;
			if (disp9 & 0x0100) disp9 |= 0xfffffe00;
			bool branchTaken = false;
			switch (cond)
			{
			case 0x02: branchTaken = getPswZ(); break; // BZ/BE

			case 0x05: branchTaken = true; break; // BR

			case 0x0a: branchTaken = !getPswZ(); break; // BNZ/BNE

			case 0x0e: branchTaken = getPswS() == getPswOv(); break; // BGE

			default:
				invalidOpcode();
			}
			if (branchTaken)
			{
				cycles(3);
				pc = (pc + disp9) & 0xfffffffe;
			}
			else
			{
				cycles(1);
				pc += 2;
			}
		}
		else
		{
			unsigned char opcode = (instruction >> 10) & 0x3f;
			unsigned int reg1 = instruction & 0x1f;
			unsigned int reg2 = (instruction >> 5) & 0x1f;
			if (opcode < 0x10)
			{
				// Format I
				bool incrementPc = true;
				switch (opcode)
				{
				case 0x00: setReg(reg2, r[reg1]); cycles(1); break; // MOV (Register)
				case 0x01: // ADD (Register)
					{
						unsigned int r1 = r[reg1];
						unsigned int r2 = r[reg2];
						unsigned int res = r2 + r1;
						setReg(reg2, res);
						setPswCy(res < r2);
						setPswOv(isSigned(r2) != isSigned(res));
						setPswS(isSigned(res));
						setPswZ(!res);
						cycles(1);
					}
					break;

				case 0x02: // SUB
					{
						unsigned int r1 = r[reg1];
						unsigned int r2 = r[reg2];
						unsigned int res = r2 - r1;
						setReg(reg2, res);
						setPswCy(r1 > r2);
						setPswOv(isSigned(r2) != isSigned(res));
						setPswS(isSigned(res));
						setPswZ(!res);
						cycles(1);
					}
					break;

				case 0x06: pc = r[reg1] & 0xfffffffe; incrementPc = false; cycles(3); break; // JMP

				default:
					invalidOpcode();
				}
				if (incrementPc) pc += 2;
			}
			else if (opcode < 0x20)
			{
				// Format II
				unsigned int imm5 = reg1;
				unsigned int seImm5 = imm5;
				if (seImm5 & 0x10) seImm5 |= 0xffffffe0;
				switch (opcode)
				{
				case 0x10: setReg(reg2, seImm5); cycles(1); break; // MOV (Immediate)
				case 0x11: // Add (Immediate)
					{
						unsigned int r2 = r[reg2];
						unsigned int res = r2 + seImm5;
						setReg(reg2, res);
						setPswCy(res < r2);
						setPswOv(isSigned(r2) != isSigned(res));
						setPswS(isSigned(res));
						setPswZ(!res);
						cycles(1);
					}
					break;

				case 0x13: // CMP (Immediate)
					{
						unsigned int r2 = r[reg2];
						unsigned int res = r2 - seImm5;
						setPswCy(seImm5 > r2);
						setPswOv(isSigned(r2) != isSigned(res));
						setPswS(isSigned(res));
						setPswZ(!res);
						cycles(1);
					}
					break;

				case 0x16: setPswId(false); cycles(1); break; // CLI

				case 0x1c: // LDSR
					switch (imm5)
					{
					case 0: eipc = r[reg2]; break;
					case 1: eipsw = r[reg2]; break;
					case 2: fepc = r[reg2]; break;
					case 3: fepsw = r[reg2]; break;
					case 5: psw = r[reg2]; break;
					case 24: chcw = r[reg2]; break;
					case 25: adtre = r[reg2]; break;
					}
					cycles(1);
					break;

				case 0x1e: setPswId(true); cycles(1); break; // SEI

				default:
					invalidOpcode();
				}
				pc += 2;
			}
			else
			{
				// Formats IV-VII
				unsigned int imm16 = emulator->ReadWord(pc + 2);
				unsigned int seImm16 = imm16;
				if (seImm16 & 0x8000) seImm16 |= 0xffff0000;
				instruction = (instruction << 16) | imm16;
				unsigned int disp26 = instruction & 0x03ffffff;
				if (disp26 & 0x02000000) disp26 |= 0xfc000000;
				unsigned int disp16 = seImm16;
				bool incrementPc = true;
				switch (opcode)
				{
				case 0x28: setReg(reg2, r[reg1] + seImm16); cycles(1); break; // MOVEA
				case 0x29: // ADDI
					{
						unsigned int r1 = r[reg1];
						unsigned int res = r1 + seImm16;
						setReg(reg2, res);
						setPswCy(res < r1);
						setPswOv(isSigned(r1) != isSigned(res));
						setPswS(isSigned(res));
						setPswZ(!res);
						cycles(1);
					}
					break;
				case 0x2a: pc = (pc + disp26) & 0xfffffffe; incrementPc = false; cycles(3); break; // JR
				case 0x2b: setReg(31, pc + 4); pc = (pc + disp26) & 0xfffffffe; incrementPc = false; cycles(3); break; // JAL

				case 0x2d: // ANDI
					{
						unsigned int res = r[reg1] & imm16;
						setReg(reg2, res);
						setPswOv(false);
						setPswS(false);
						setPswZ(!res);
						cycles(1);
					}
					break;

				case 0x2f: setReg(reg2, r[reg1] + (imm16 << 16)); cycles(1); break; // MOVHI
				case 0x30: // LD.B
					{
						unsigned int res = emulator->ReadByte(r[reg1] + disp16);
						if (res & 0x80) res |= 0xffffff00;
						setReg(reg2, res);
						cycles(5);
					}
					break;

				case 0x33: setReg(reg2, emulator->ReadDword(r[reg1] + disp16)); cycles(5); break; // LD.W
				case 0x34: emulator->WriteByte(r[reg1] + disp16, r[reg2] & 0xff); cycles(4); break; // ST.B
				case 0x35: emulator->WriteWord(r[reg1] + disp16, r[reg2] & 0xffff); cycles(4); break; // ST.H

				case 0x37: emulator->WriteDword(r[reg1] + disp16, r[reg2]); cycles(4); break; // ST.W

				default:
					invalidOpcode();
				}
				if (incrementPc) pc += 4;
			}
		}
	}
}

void Nvc::setReg(int index, unsigned int value)
{
	if (index) r[index] = value;
}

void Nvc::setPswZ(bool set)
{
	if (set)
	{
		psw |= 0x00000001;
	}
	else
	{
		psw &= 0xfffffffe;
	}
}

bool Nvc::getPswZ() const
{
	return (psw & 0x00000001) != 0;
}

void Nvc::setPswS(bool set)
{
	if (set)
	{
		psw |= 0x00000002;
	}
	else
	{
		psw &= 0xfffffffd;
	}
}

bool Nvc::getPswS() const
{
	return (psw & 0x00000002) != 0;
}

void Nvc::setPswOv(bool set)
{
	if (set)
	{
		psw |= 0x00000004;
	}
	else
	{
		psw &= 0xfffffffb;
	}
}

bool Nvc::getPswOv() const
{
	return (psw & 0x00000004) != 0;
}

void Nvc::setPswCy(bool set)
{
	if (set)
	{
		psw |= 0x00000008;
	}
	else
	{
		psw &= 0xfffffff7;
	}
}

void Nvc::setPswId(bool set)
{
	if (set)
	{
		psw |= 0x00001000;
	}
	else
	{
		psw &= 0xffffefff;
	}
}

bool Nvc::isSigned(unsigned int value)
{
	return (value & 0x80000000) != 0;
}

void Nvc::cycles(int numCycles)
{
	emulator->CpuCyclesCallback(numCycles);
	cycleCount++;
}

void Nvc::invalidOpcode()
{
	throw FSL_EXCEPTION("Invalid opcode");
}
