#include "stdafx.h"
#include "CrystalTile2.h"

#include "Asm.h"

const char Condition[16][3] = {
	"eq",
		"ne",
		"cs",
		"cc",
		"mi",
		"pl",
		"vs",
		"vc",
		"hi",
		"ls",
		"ge",
		"lt",
		"gt",
		"le",
		"",
		""
};

const char Registre[16][4] = {
	"r0",
		"r1",
		"r2",
		"r3",
		"r4",
		"r5",
		"r6",
		"r7",
		"r8",
		"r9",
		"r10",
		"r11",
		"r12",
		"sp",
		"lr",
		"pc",
};

const char MSR_FIELD[16][5] = {
	"",
		"c",
		"x",
		"xc",
		"s",
		"sc",
		"sx",
		"sxc",
		"f",
		"fc",
		"fx",
		"fxc",
		"fs",
		"fsc",
		"fsx",
		"fsxc"
};

const char *swi9[0x20] = {
	// ARM9				
	"SoftReset",			// 0
	"-/-",					// 1
	"-/-",					// 2
	"WaitByLoop",			// 3
	"IntrWait",				// 4
	"VBlankIntrWait",		// 5
	"Halt",					// 6
	"-/-",					// 7
	"-/-",					// 8
	"Div",					// 9
	"-/-",					// A
	"CpuSet",				// B
	"CpuFastSet",			// C
	"Sqrt",					// D
	"GetCRC16",				// E
	"IsDebugger",			// F
	"BitUnPack",			// 10
	"LZ77UnCompWram",		// 11
	"LZ77UnCompVram",		// 12
	"HuffUnComp",			// 13
	"RLUnCompWram",			// 14
	"RLUnCompVram",			// 15
	"Diff8bitUnFilterWram",	// 16
	"-/-",					// 17
	"Diff16bitUnFilter",	// 18
	"-/-",					// 19
	"-/-",					// 1A
	"-/-",					// 1B
	"-/-",					// 1C
	"-/-",					// 1D
	"-/-",					// 1E
	"CustomPost",			// 1F
};

const char * swi7[0x20] = {
	// ARM7
	"SoftReset",		// 0
	"-/-",				// 1
	"-/-",				// 2
	"WaitByLoop",		// 3
	"IntrWait",			// 4
	"VBlankIntrWait",	// 5
	"Halt",				// 6
	"Stop/Sleep",		// 7
	"SoundBias",		// 8
	"Div",				// 9
	"-/-",				// A
	"CpuSet",			// B
	"CpuFastSet",		// C
	"Sqrt",				// D
	"GetCRC16",			// E
	"IsDebugger",		// F
	"BitUnPack",		// 10
	"LZ77UnCompWram",	// 11
	"LZ77UnCompVram",	// 12
	"HuffUnComp",		// 13
	"RLUnCompWram",		// 14
	"RLUnCompVram",		// 15
	"-/-",				// 16
	"-/-",				// 17
	"-/-",				// 18
	"SoundBias",		// 19
	"GetSineTable",		// 1A
	"GetPitchTable",	// 1B
	"GetVolumeTable",	// 1C
	"GetBootProcs",		// 1D
	"-/-",				// 1E
	"-/-"				// 1F
};

#define	CONDITION(i)  (i)>>28
#define	REG_POS(i,n)		 (((i)>>n)&0xF)
#define	ROR(i, j)	((((UINT)(i))>>(j)) | (((UINT)(i))<<(32-(j))))
#define	BIT_N(i,n)	(((i)>>(n))&1)
#define	BIT0(i)		((i)&1)
#define	BIT1(i)		BIT_N(i,1)
#define	BIT2(i)		BIT_N(i,2)
#define	BIT3(i)		BIT_N(i,3)
#define	BIT4(i)		BIT_N(i,4)
#define	BIT5(i)		BIT_N(i,5)
#define	BIT6(i)		BIT_N(i,6)
#define	BIT7(i)		BIT_N(i,7)
#define	BIT8(i)		BIT_N(i,8)
#define	BIT9(i)		BIT_N(i,9)
#define	BIT10(i)	 BIT_N(i,10)
#define	BIT11(i)	 BIT_N(i,11)
#define	BIT12(i)	 BIT_N(i,12)
#define	BIT13(i)	 BIT_N(i,13)
#define	BIT14(i)	 BIT_N(i,14)
#define	BIT15(i)	 BIT_N(i,15)
#define	BIT16(i)	 BIT_N(i,16)
#define	BIT17(i)	 BIT_N(i,17)
#define	BIT18(i)	 BIT_N(i,18)
#define	BIT19(i)	 BIT_N(i,19)
#define	BIT20(i)	 BIT_N(i,20)
#define	BIT21(i)	 BIT_N(i,21)
#define	BIT22(i)	 BIT_N(i,22)
#define	BIT23(i)	 BIT_N(i,23)
#define	BIT24(i)	 BIT_N(i,24)
#define	BIT25(i)	 BIT_N(i,25)
#define	BIT26(i)	 BIT_N(i,26)
#define	BIT27(i)	 BIT_N(i,27)
#define	BIT28(i)	 BIT_N(i,28)
#define	BIT29(i)	 BIT_N(i,29)
#define	BIT30(i)	 BIT_N(i,30)
#define	BIT31(i)	((i)>>31)

#define DATAPROC_LSL_IMM(nom, s) char tmp[10] = "";\
	if(((nOpCode>>7)&0x1F)!=0)\
	sprintf(tmp, ", lsl %Xh", ((nOpCode>>7)&0x1F));\
	sprintf(txt, "%s%s%s %s, %s, %s%s",\
#nom,\
	Condition[CONDITION(nOpCode)],\
	s,\
	Registre[REG_POS(nOpCode,12)],\
	Registre[REG_POS(nOpCode,16)],\
	Registre[REG_POS(nOpCode,0)],\
	tmp);
#define DATAPROC_ROR_IMM(nom, s) char tmp[10] = "";\
	if(((nOpCode>>7)&0x1F)!=0)\
	sprintf(tmp, ", rrx");\
	sprintf(txt, "%s%s%s %s, %s, %s%s",\
#nom,\
	Condition[CONDITION(nOpCode)],\
	s,\
	Registre[REG_POS(nOpCode,12)],\
	Registre[REG_POS(nOpCode,16)],\
	Registre[REG_POS(nOpCode,0)],\
	tmp\
	);

#define DATAPROC_REG_SHIFT(nom, shift,s) sprintf(txt, "%s%s%s %s, %s, %s, %s %s",\
#nom,\
	Condition[CONDITION(nOpCode)],\
	s,\
	Registre[REG_POS(nOpCode,12)],\
	Registre[REG_POS(nOpCode,16)],\
	Registre[REG_POS(nOpCode,0)],\
#shift,\
	Registre[REG_POS(nOpCode,8)]\
	);

#define DATAPROC_IMM_SHIFT(nom, shift, s) sprintf(txt, "%s%s%s %s, %s, %s, %s %Xh",\
#nom,\
	Condition[CONDITION(nOpCode)],\
	s,\
	Registre[REG_POS(nOpCode,12)],\
	Registre[REG_POS(nOpCode,16)],\
	Registre[REG_POS(nOpCode,0)],\
#shift,\
	((nOpCode>>7)&0x1F)\
	);

#define DATAPROC_IMM_VALUE(nom,s) UINT shift_op = ROR((nOpCode&0xFF), (nOpCode>>7)&0x1E);\
	sprintf(txt, "%s%s%s %s, %s, %Xh",\
#nom,\
	Condition[CONDITION(nOpCode)],\
	s,\
	Registre[REG_POS(nOpCode,12)],\
	Registre[REG_POS(nOpCode,16)],\
	shift_op\
	);

#define DATAPROC_ONE_OP_LSL_IMM(nom, s, v) char tmp[10] = "";\
	if(((nOpCode>>7)&0x1F)!=0)\
	sprintf(tmp, ", lsl %Xh", ((nOpCode>>7)&0x1F));\
	sprintf(txt, "%s%s%s %s, %s%s",\
#nom,\
	Condition[CONDITION(nOpCode)],\
	s,\
	Registre[REG_POS(nOpCode,v)],\
	Registre[REG_POS(nOpCode,0)],\
	tmp); 

#define DATAPROC_ONE_OP_ROR_IMM(nom, s, v) char tmp[10] = "";\
	if(((nOpCode>>7)&0x1F)==0)\
	sprintf(tmp, ", rrx");\
 else\
 sprintf(tmp, ", ror %d", (nOpCode>>7)&0x1F);\
 sprintf(txt, "%s%s%s %s, %s%s",\
#nom,\
	Condition[CONDITION(nOpCode)],\
	s,\
	Registre[REG_POS(nOpCode,v)],\
	Registre[REG_POS(nOpCode,0)],\
	tmp\
	);

#define DATAPROC_ONE_OP_REG_SHIFT(nom, shift,s, v) sprintf(txt, "%s%s%s %s, %s, %s %s",\
#nom,\
	Condition[CONDITION(nOpCode)],\
	s,\
	Registre[REG_POS(nOpCode,v)],\
	Registre[REG_POS(nOpCode,0)],\
#shift,\
	Registre[REG_POS(nOpCode,8)]\
	);

#define DATAPROC_ONE_OP_IMM_SHIFT(nom, shift, s, v) sprintf(txt, "%s%s%s %s, %s, %s %Xh",\
#nom,\
	Condition[CONDITION(nOpCode)],\
	s,\
	Registre[REG_POS(nOpCode,v)],\
	Registre[REG_POS(nOpCode,0)],\
#shift,\
	((nOpCode>>7)&0x1F)\
	);

#define DATAPROC_ONE_OP_IMM_VALUE(nom, s, v)\
	UINT shift_op = ROR((nOpCode&0xFF), (nOpCode>>7)&0x1E);\
	sprintf(txt, "%s%s%s %s, %Xh",\
#nom,\
	Condition[CONDITION(nOpCode)],\
	s,\
	Registre[REG_POS(nOpCode,v)],\
	shift_op\
	);

//#define SIGNEXTEND_24(nOpCode) (((nOpCode)&0xFFFFFF)|(0xFF000000*BIT23(nOpCode)))

#define LDRSTR_LSL_IMM(nom, op, op2, op3) char tmp[10] = "";\
	if(((nOpCode>>7)&0x1F)!=0)\
	sprintf(tmp, ", lsl %Xh", ((nOpCode>>7)&0x1F));\
	sprintf(txt, "%s%s %s, [%s%s, %s%s%s%s",\
#nom,\
	Condition[CONDITION(nOpCode)],\
	Registre[REG_POS(nOpCode,12)],\
	Registre[REG_POS(nOpCode,16)],\
	op2,\
	op,\
	Registre[REG_POS(nOpCode,0)],\
	tmp,\
	op3);
#define LDRSTR_ROR_IMM(nom, op, op2, op3) char tmp[10] = "";\
	if(((nOpCode>>7)&0x1F)!=0)\
	sprintf(tmp, ", rrx");\
	sprintf(txt, "%s%s %s, [%s%s, %s%s%s%s",\
#nom,\
	Condition[CONDITION(nOpCode)],\
	Registre[REG_POS(nOpCode,12)],\
	Registre[REG_POS(nOpCode,16)],\
	op2,\
	op,\
	Registre[REG_POS(nOpCode,0)],\
	tmp,\
	op3\
	);

#define LDRSTR_IMM_SHIFT(nom, shift, op, op2, op3)\
	sprintf(txt, "%s%s %s, [%s%s, %s%s, %s %Xh%s",\
#nom,\
	Condition[CONDITION(nOpCode)],\
	Registre[REG_POS(nOpCode,12)],\
	Registre[REG_POS(nOpCode,16)],\
	op2,\
	op,\
	Registre[REG_POS(nOpCode,0)],\
#shift,\
	((nOpCode>>7)&0x1F),\
	op3\
	);

#define RegList(nb) g_szBufferA[0] = 0;\
	int prec = 0;\
	for(int j = 0; j < nb; j++)\
{\
	if(prec)\
{\
	if((!BIT_N(nOpCode, j+1))||(j==nb-1))\
{\
	sprintf(g_szBufferA, "%s%s,", g_szBufferA, Registre[j]);\
	prec = 0;\
}\
}\
 else\
{\
	if(BIT_N(nOpCode, j))\
{\
	if((BIT_N(nOpCode, j+1))&&(j!=nb-1))\
{\
	sprintf(g_szBufferA, "%s%s-", g_szBufferA, Registre[j]);\
	prec = 1;\
}\
 else\
 sprintf(g_szBufferA, "%s%s,", g_szBufferA, Registre[j]);\
}\
}\
}\
	g_szBufferA[strlen(g_szBufferA)-1]='\0';

int DES_UND(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "????");
	return 4;
}

//-----------------------and------------------------------------
int DES_AND_LSL_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_LSL_IMM(and, "");
	return 4;
}

int DES_AND_LSL_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(and, lsl, "");
	return 4;
}

int DES_AND_LSR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_SHIFT(and, lsr, "");
	return 4;
}

int DES_AND_LSR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(and, lsr, "");
	return 4;
}

int DES_AND_ASR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_SHIFT(and, asr, "");
	return 4;
}

int DES_AND_ASR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(and, asr, "");
	return 4;
}

int DES_AND_ROR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ROR_IMM(and, "");
	return 4;
}

int DES_AND_ROR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(and, ror, "");
	return 4;
}

int DES_AND_IMM_VAL(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_VALUE(and, "");
	return 4;
}

int DES_AND_S_LSL_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_LSL_IMM(and, "s");
	return 4;
}

int DES_AND_S_LSL_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(and, lsl, "s");
	return 4;
}

int DES_AND_S_LSR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_SHIFT(and, lsr, "s");
	return 4;
}

int DES_AND_S_LSR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(and, lsr, "s");
	return 4;
}

int DES_AND_S_ASR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_SHIFT(and, asr, "s");
	return 4;
}

int DES_AND_S_ASR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(and, asr, "s");
	return 4;
}

int DES_AND_S_ROR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ROR_IMM(and, "s");
	return 4;
}

int DES_AND_S_ROR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(and, ror, "s");
	return 4;
}

int DES_AND_S_IMM_VAL(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_VALUE(and, "s");
	return 4;
}

//--------------eor------------------------------
int DES_EOR_LSL_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_LSL_IMM(eor, "");
	return 4;
}

int DES_EOR_LSL_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(eor, lsl, "");
	return 4;
}

int DES_EOR_LSR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_SHIFT(eor, lsr, "");
	return 4;
}

int DES_EOR_LSR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(eor, lsr, "");
	return 4;
}

int DES_EOR_ASR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_SHIFT(eor, asr, "");
	return 4;
}

int DES_EOR_ASR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(eor, asr, "");
	return 4;
}

int DES_EOR_ROR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ROR_IMM(eor, "");
	return 4;
}

int DES_EOR_ROR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(eor, ror, "");
	return 4;
}

int DES_EOR_IMM_VAL(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_VALUE(eor, "");
	return 4;
}

int DES_EOR_S_LSL_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_LSL_IMM(eor, "s");
	return 4;
}

int DES_EOR_S_LSL_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(eor, lsl, "s");
	return 4;
}

int DES_EOR_S_LSR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_SHIFT(eor, lsr, "s");
	return 4;
}

int DES_EOR_S_LSR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(eor, lsr, "s");
	return 4;
}

int DES_EOR_S_ASR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_SHIFT(eor, asr, "s");
	return 4;
}

int DES_EOR_S_ASR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(eor, asr, "s");
	return 4;
}

int DES_EOR_S_ROR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ROR_IMM(eor, "s");
	return 4;
}

int DES_EOR_S_ROR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(eor, ror, "s");
	return 4;
}

int DES_EOR_S_IMM_VAL(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_VALUE(eor, "s");
	return 4;
}

//-------------sub-------------------------------------

int DES_SUB_LSL_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_LSL_IMM(sub, "");
	return 4;
}

int DES_SUB_LSL_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(sub, lsl, "");
	return 4;
}

int DES_SUB_LSR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_SHIFT(sub, lsr, "");
	return 4;
}

int DES_SUB_LSR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(sub, lsr, "");
	return 4;
}

int DES_SUB_ASR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_SHIFT(sub, asr, "");
	return 4;
}

int DES_SUB_ASR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(sub, asr, "");
	return 4;
}

int DES_SUB_ROR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ROR_IMM(sub, "");
	return 4;
}

int DES_SUB_ROR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(sub, ror, "");
	return 4;
}

int DES_SUB_IMM_VAL(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_VALUE(sub, "");
	return 4;
}

int DES_SUB_S_LSL_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_LSL_IMM(sub, "s");
	return 4;
}

int DES_SUB_S_LSL_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(sub, lsl, "s");
	return 4;
}

int DES_SUB_S_LSR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_SHIFT(sub, lsr, "s");
	return 4;
}

int DES_SUB_S_LSR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(sub, lsr, "s");
	return 4;
}

int DES_SUB_S_ASR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_SHIFT(sub, asr, "s");
	return 4;
}

int DES_SUB_S_ASR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(sub, asr, "s");
	return 4;
}

int DES_SUB_S_ROR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ROR_IMM(sub, "s");
	return 4;
}

int DES_SUB_S_ROR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(sub, ror, "s");
	return 4;
}

int DES_SUB_S_IMM_VAL(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_VALUE(sub, "s");
	return 4;
}

//------------------rsb------------------------

int DES_RSB_LSL_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_LSL_IMM(rsb, "");
	return 4;
}

int DES_RSB_LSL_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(rsb, lsl, "");
	return 4;
}

int DES_RSB_LSR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_SHIFT(rsb, lsr, "");
	return 4;
}

int DES_RSB_LSR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(rsb, lsr, "");
	return 4;
}

int DES_RSB_ASR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_SHIFT(rsb, asr, "");
	return 4;
}

int DES_RSB_ASR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(rsb, asr, "");
	return 4;
}

int DES_RSB_ROR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ROR_IMM(rsb, "");
	return 4;
}

int DES_RSB_ROR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(rsb, ror, "");
	return 4;
}

int DES_RSB_IMM_VAL(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_VALUE(rsb, "");
	return 4;
}

int DES_RSB_S_LSL_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_LSL_IMM(rsb, "s");
	return 4;
}

int DES_RSB_S_LSL_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(rsb, lsl, "s");
	return 4;
}

int DES_RSB_S_LSR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_SHIFT(rsb, lsr, "s");
	return 4;
}

int DES_RSB_S_LSR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(rsb, lsr, "s");
	return 4;
}

int DES_RSB_S_ASR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_SHIFT(rsb, asr, "s");
	return 4;
}

int DES_RSB_S_ASR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(rsb, asr, "s");
	return 4;
}

int DES_RSB_S_ROR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ROR_IMM(rsb, "s");
	return 4;
}

int DES_RSB_S_ROR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(rsb, ror, "s");
	return 4;
}

int DES_RSB_S_IMM_VAL(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_VALUE(rsb, "s");
	return 4;
}

//------------------add-----------------------------------

int DES_ADD_LSL_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_LSL_IMM(add, "");
	return 4;
}

int DES_ADD_LSL_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(add, lsl, "");
	return 4;
}

int DES_ADD_LSR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_SHIFT(add, lsr, "");
	return 4;
}

int DES_ADD_LSR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(add, lsr, "");
	return 4;
}

int DES_ADD_ASR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_SHIFT(add, asr, "");
	return 4;
}

int DES_ADD_ASR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(add, asr, "");
	return 4;
}

int DES_ADD_ROR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ROR_IMM(add, "");
	return 4;
}

int DES_ADD_ROR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(add, ror, "");
	return 4;
}

int DES_ADD_IMM_VAL(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_VALUE(add, "");
	return 4;
}

int DES_ADD_S_LSL_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_LSL_IMM(add, "s");
	return 4;
}

int DES_ADD_S_LSL_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(add, lsl, "s");
	return 4;
}

int DES_ADD_S_LSR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_SHIFT(add, lsr, "s");
	return 4;
}

int DES_ADD_S_LSR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(add, lsr, "s");
	return 4;
}

int DES_ADD_S_ASR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_SHIFT(add, asr, "s");
	return 4;
}

int DES_ADD_S_ASR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(add, asr, "s");
	return 4;
}

int DES_ADD_S_ROR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ROR_IMM(add, "s");
	return 4;
}

int DES_ADD_S_ROR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(add, ror, "s");
	return 4;
}

int DES_ADD_S_IMM_VAL(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_VALUE(add, "s");
	return 4;
}

//------------------adc-----------------------------------

int DES_ADC_LSL_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_LSL_IMM(adc, "");
	return 4;
}

int DES_ADC_LSL_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(adc, lsl, "");
	return 4;
}

int DES_ADC_LSR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_SHIFT(adc, lsr, "");
	return 4;
}

int DES_ADC_LSR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(adc, lsr, "");
	return 4;
}

int DES_ADC_ASR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_SHIFT(adc, asr, "");
	return 4;
}

int DES_ADC_ASR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(adc, asr, "");
	return 4;
}

int DES_ADC_ROR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ROR_IMM(adc, "");
	return 4;
}

int DES_ADC_ROR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(adc, ror, "");
	return 4;
}

int DES_ADC_IMM_VAL(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_VALUE(adc, "");
	return 4;
}

int DES_ADC_S_LSL_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_LSL_IMM(adc, "s");
	return 4;
}

int DES_ADC_S_LSL_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(adc, lsl, "s");
	return 4;
}

int DES_ADC_S_LSR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_SHIFT(adc, lsr, "s");
	return 4;
}

int DES_ADC_S_LSR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(adc, lsr, "s");
	return 4;
}

int DES_ADC_S_ASR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_SHIFT(adc, asr, "s");
	return 4;
}

int DES_ADC_S_ASR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(adc, asr, "s");
	return 4;
}

int DES_ADC_S_ROR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ROR_IMM(adc, "s");
	return 4;
}

int DES_ADC_S_ROR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(adc, ror, "s");
	return 4;
}

int DES_ADC_S_IMM_VAL(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_VALUE(adc, "s");
	return 4;
}

//-------------sbc-------------------------------------

int DES_SBC_LSL_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_LSL_IMM(sbc, "");
	return 4;
}

int DES_SBC_LSL_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(sbc, lsl, "");
	return 4;
}

int DES_SBC_LSR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_SHIFT(sbc, lsr, "");
	return 4;
}

int DES_SBC_LSR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(sbc, lsr, "");
	return 4;
}

int DES_SBC_ASR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_SHIFT(sbc, asr, "");
	return 4;
}

int DES_SBC_ASR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(sbc, asr, "");
	return 4;
}

int DES_SBC_ROR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ROR_IMM(sbc, "");
	return 4;
}

int DES_SBC_ROR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(sbc, ror, "");
	return 4;
}

int DES_SBC_IMM_VAL(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_VALUE(sbc, "");
	return 4;
}

int DES_SBC_S_LSL_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_LSL_IMM(sbc, "s");
	return 4;
}

int DES_SBC_S_LSL_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(sbc, lsl, "s");
	return 4;
}

int DES_SBC_S_LSR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_SHIFT(sbc, lsr, "s");
	return 4;
}

int DES_SBC_S_LSR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(sbc, lsr, "s");
	return 4;
}

int DES_SBC_S_ASR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_SHIFT(sbc, asr, "s");
	return 4;
}

int DES_SBC_S_ASR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(sbc, asr, "s");
	return 4;
}

int DES_SBC_S_ROR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ROR_IMM(sbc, "s");
	return 4;
}

int DES_SBC_S_ROR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(sbc, ror, "s");
	return 4;
}

int DES_SBC_S_IMM_VAL(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_VALUE(sbc, "s");
	return 4;
}

//---------------rsc----------------------------------

int DES_RSC_LSL_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_LSL_IMM(rsc, "");
	return 4;
}

int DES_RSC_LSL_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(rsc, lsl, "");
	return 4;
}

int DES_RSC_LSR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_SHIFT(rsc, lsr, "");
	return 4;
}

int DES_RSC_LSR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(rsc, lsr, "");
	return 4;
}

int DES_RSC_ASR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_SHIFT(rsc, asr, "");
	return 4;
}

int DES_RSC_ASR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(rsc, asr, "");
	return 4;
}

int DES_RSC_ROR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ROR_IMM(rsc, "");
	return 4;
}

int DES_RSC_ROR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(rsc, ror, "");
	return 4;
}

int DES_RSC_IMM_VAL(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_VALUE(rsc, "");
	return 4;
}

int DES_RSC_S_LSL_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_LSL_IMM(rsc, "s");
	return 4;
}

int DES_RSC_S_LSL_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(rsc, lsl, "s");
	return 4;
}

int DES_RSC_S_LSR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_SHIFT(rsc, lsr, "s");
	return 4;
}

int DES_RSC_S_LSR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(rsc, lsr, "s");
	return 4;
}

int DES_RSC_S_ASR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_SHIFT(rsc, asr, "s");
	return 4;
}

int DES_RSC_S_ASR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(rsc, asr, "s");
	return 4;
}

int DES_RSC_S_ROR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ROR_IMM(rsc, "s");
	return 4;
}

int DES_RSC_S_ROR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(rsc, ror, "s");
	return 4;
}

int DES_RSC_S_IMM_VAL(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_VALUE(rsc, "s");
	return 4;
}

//-------------------tst----------------------------

int DES_TST_LSL_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_LSL_IMM(tst, "", 16);
	return 4;
}

int DES_TST_LSL_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_REG_SHIFT(tst, lsl, "", 16);
	return 4;
}

int DES_TST_LSR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_IMM_SHIFT(tst, lsr, "", 16);
	return 4;
}

int DES_TST_LSR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_REG_SHIFT(tst, lsr, "", 16);
	return 4;
}

int DES_TST_ASR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_IMM_SHIFT(tst, asr, "", 16);
	return 4;
}

int DES_TST_ASR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_REG_SHIFT(tst, asr, "", 16);
	return 4;
}

int DES_TST_ROR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_ROR_IMM(tst, "", 16);
	return 4;
}

int DES_TST_ROR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_REG_SHIFT(tst, ror, "", 16);
	return 4;
}

int DES_TST_IMM_VAL(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_IMM_VALUE(tst, "", 16);
	return 4;
}

//-------------------teq----------------------------

int DES_TEQ_LSL_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_LSL_IMM(teq, "", 16);
	return 4;
}

int DES_TEQ_LSL_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_REG_SHIFT(teq, lsl, "", 16);
	return 4;
}

int DES_TEQ_LSR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_IMM_SHIFT(teq, lsr, "", 16);
	return 4;
}

int DES_TEQ_LSR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_REG_SHIFT(teq, lsr, "", 16);
	return 4;
}

int DES_TEQ_ASR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_IMM_SHIFT(teq, asr, "", 16);
	return 4;
}

int DES_TEQ_ASR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_REG_SHIFT(teq, asr, "", 16);
	return 4;
}

int DES_TEQ_ROR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_ROR_IMM(teq, "", 16);
	return 4;
}

int DES_TEQ_ROR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_REG_SHIFT(teq, ror, "", 16);
	return 4;
}

int DES_TEQ_IMM_VAL(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_IMM_VALUE(teq, "", 16);
	return 4;
}

//-------------cmp-------------------------------------

int DES_CMP_LSL_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_LSL_IMM(cmp, "", 16);
	return 4;
}

int DES_CMP_LSL_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_REG_SHIFT(cmp, lsl, "", 16);
	return 4;
}

int DES_CMP_LSR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_IMM_SHIFT(cmp, lsr, "", 16);
	return 4;
}

int DES_CMP_LSR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_REG_SHIFT(cmp, lsr, "", 16);
	return 4;
}

int DES_CMP_ASR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_IMM_SHIFT(cmp, asr, "", 16);
	return 4;
}

int DES_CMP_ASR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_REG_SHIFT(cmp, asr, "", 16);
	return 4;
}

int DES_CMP_ROR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_ROR_IMM(cmp, "", 16);
	return 4;
}

int DES_CMP_ROR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_REG_SHIFT(cmp, ror, "", 16);
	return 4;
}

int DES_CMP_IMM_VAL(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_IMM_VALUE(cmp, "", 16);
	return 4;
}

//---------------cmn---------------------------

int DES_CMN_LSL_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_LSL_IMM(cmn, "", 16);
	return 4;
}

int DES_CMN_LSL_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_REG_SHIFT(cmn, lsl, "", 16);
	return 4;
}

int DES_CMN_LSR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_IMM_SHIFT(cmp, asr, "", 16);
	return 4;
}

int DES_CMN_LSR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_REG_SHIFT(cmn, lsr, "", 16);
	return 4;
}

int DES_CMN_ASR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_IMM_SHIFT(cmn, asr, "", 16);
	return 4;
}

int DES_CMN_ASR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_REG_SHIFT(cmn, asr, "", 16);
	return 4;
}

int DES_CMN_ROR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_ROR_IMM(cmn, "", 16);
	return 4;
}

int DES_CMN_ROR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_REG_SHIFT(cmn, ror, "", 16);
	return 4;
}

int DES_CMN_IMM_VAL(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_IMM_VALUE(cmn, "", 16);
	return 4;
}

//------------------orr-------------------

int DES_ORR_LSL_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_LSL_IMM(orr, "");
	return 4;
}

int DES_ORR_LSL_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(orr, lsl, "");
	return 4;
}

int DES_ORR_LSR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_SHIFT(orr, lsr, "");
	return 4;
}

int DES_ORR_LSR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(orr, lsr, "");
	return 4;
}

int DES_ORR_ASR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_SHIFT(orr, asr, "");
	return 4;
}

int DES_ORR_ASR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(orr, asr, "");
	return 4;
}

int DES_ORR_ROR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ROR_IMM(orr, "");
	return 4;
}

int DES_ORR_ROR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(orr, ror, "");
	return 4;
}

int DES_ORR_IMM_VAL(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_VALUE(orr, "");
	return 4;
}

int DES_ORR_S_LSL_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_LSL_IMM(orr, "s");
	return 4;
}

int DES_ORR_S_LSL_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(orr, lsl, "s");
	return 4;
}

int DES_ORR_S_LSR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_SHIFT(orr, lsr, "s");
	return 4;
}

int DES_ORR_S_LSR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(orr, lsr, "s");
	return 4;
}

int DES_ORR_S_ASR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_SHIFT(orr, asr, "s");
	return 4;
}

int DES_ORR_S_ASR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(orr, asr, "s");
	return 4;
}

int DES_ORR_S_ROR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ROR_IMM(orr, "s");
	return 4;
}

int DES_ORR_S_ROR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(orr, ror, "s");
	return 4;
}

int DES_ORR_S_IMM_VAL(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_VALUE(orr, "s");
	return 4;
}

//------------------mov-------------------

int DES_MOV_LSL_IMM(UINT adr, UINT nOpCode, char * txt)
{
	if(nOpCode==0xE1A00000)
		strcpy(txt, "nop");
	else
	{
		DATAPROC_ONE_OP_LSL_IMM(mov, "", 12);
	}
	return 4;
}

int DES_MOV_LSL_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_REG_SHIFT(mov, lsl, "", 12);
	return 4;
}

int DES_MOV_LSR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_IMM_SHIFT(mov, lsr, "", 12);
	return 4;
}

int DES_MOV_LSR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_REG_SHIFT(mov, lsr, "", 12);
	return 4;
}

int DES_MOV_ASR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_IMM_SHIFT(mov, asr, "", 12);
	return 4;
}

int DES_MOV_ASR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_REG_SHIFT(mov, asr, "", 12);
	return 4;
}

int DES_MOV_ROR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_ROR_IMM(mov, "", 12);
	return 4;
}

int DES_MOV_ROR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_REG_SHIFT(mov, ror, "", 12);
	return 4;
}

int DES_MOV_IMM_VAL(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_IMM_VALUE(mov, "", 12);
	return 4;
}

int DES_MOV_S_LSL_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_LSL_IMM(mov, "s", 12);
	return 4;
}

int DES_MOV_S_LSL_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_REG_SHIFT(mov, lsl, "s", 12);
	return 4;
}

int DES_MOV_S_LSR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_IMM_SHIFT(mov, lsr, "s", 12);
	return 4;
}

int DES_MOV_S_LSR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_REG_SHIFT(mov, lsr, "s", 12);
	return 4;
}

int DES_MOV_S_ASR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_IMM_SHIFT(mov, asr, "s", 12);
	return 4;
}

int DES_MOV_S_ASR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_REG_SHIFT(mov, asr, "s", 12);
	return 4;
}

int DES_MOV_S_ROR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_ROR_IMM(mov, "s", 12);
	return 4;
}

int DES_MOV_S_ROR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_REG_SHIFT(mov, ror, "s", 12);
	return 4;
}

int DES_MOV_S_IMM_VAL(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_IMM_VALUE(mov, "s", 12);
	return 4;
}

//------------------bic-------------------

int DES_BIC_LSL_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_LSL_IMM(bic, "");
	return 4;
}

int DES_BIC_LSL_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(bic, lsl, "");
	return 4;
}

int DES_BIC_LSR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_SHIFT(bic, lsr, "");
	return 4;
}

int DES_BIC_LSR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(bic, lsr, "");
	return 4;
}

int DES_BIC_ASR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_SHIFT(bic, asr, "");
	return 4;
}

int DES_BIC_ASR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(bic, asr, "");
	return 4;
}

int DES_BIC_ROR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ROR_IMM(bic, "");
	return 4;
}

int DES_BIC_ROR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(bic, ror, "");
	return 4;
}

int DES_BIC_IMM_VAL(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_VALUE(bic, "");
	return 4;
}

int DES_BIC_S_LSL_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_LSL_IMM(bic, "s");
	return 4;
}

int DES_BIC_S_LSL_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(bic, lsl, "s");
	return 4;
}

int DES_BIC_S_LSR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_SHIFT(bic, lsr, "s");
	return 4;
}

int DES_BIC_S_LSR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(bic, lsr, "s");
	return 4;
}

int DES_BIC_S_ASR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_SHIFT(bic, asr, "s");
	return 4;
}

int DES_BIC_S_ASR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(bic, asr, "s");
	return 4;
}

int DES_BIC_S_ROR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ROR_IMM(bic, "s");
	return 4;
}

int DES_BIC_S_ROR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_REG_SHIFT(bic, ror, "s");
	return 4;
}

int DES_BIC_S_IMM_VAL(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_IMM_VALUE(bic, "s");
	return 4;
}

//------------------mvn-------------------

int DES_MVN_LSL_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_LSL_IMM(mvn, "", 12);
	return 4;
}

int DES_MVN_LSL_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_REG_SHIFT(mvn, lsl, "", 12);
	return 4;
}

int DES_MVN_LSR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_IMM_SHIFT(mvn, lsr, "", 12);
	return 4;
}

int DES_MVN_LSR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_REG_SHIFT(mvn, lsr, "", 12);
	return 4;
}

int DES_MVN_ASR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_IMM_SHIFT(mvn, asr, "", 12);
	return 4;
}

int DES_MVN_ASR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_REG_SHIFT(mvn, asr, "", 12);
	return 4;
}

int DES_MVN_ROR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_ROR_IMM(mvn, "", 12);
	return 4;
}

int DES_MVN_ROR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_REG_SHIFT(mvn, ror, "", 12);
	return 4;
}

int DES_MVN_IMM_VAL(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_IMM_VALUE(mvn, "", 12);
	return 4;
}

int DES_MVN_S_LSL_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_LSL_IMM(mvn, "s", 12);
	return 4;
}

int DES_MVN_S_LSL_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_REG_SHIFT(mvn, lsl, "s", 12);
	return 4;
}

int DES_MVN_S_LSR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_IMM_SHIFT(mvn, lsr, "s", 12);
	return 4;
}

int DES_MVN_S_LSR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_REG_SHIFT(mvn, lsr, "s", 12);
	return 4;
}

int DES_MVN_S_ASR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_IMM_SHIFT(mov, asr, "s", 12);
	return 4;
}

int DES_MVN_S_ASR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_REG_SHIFT(mvn, asr, "s", 12);
	return 4;
}

int DES_MVN_S_ROR_IMM(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_ROR_IMM(mvn, "s", 12);
	return 4;
}

int DES_MVN_S_ROR_REG(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_REG_SHIFT(mvn, ror, "s", 12);
	return 4;
}

int DES_MVN_S_IMM_VAL(UINT adr, UINT nOpCode, char * txt)
{
	DATAPROC_ONE_OP_IMM_VALUE(mvn, "s", 12);
	return 4;
}


//-------------MUL------------------------

int DES_MUL(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "mul%s %s, %s, %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)], Registre[REG_POS(nOpCode,8)]);
	return 4;
}

int DES_MLA(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "mla%s %s, %s, %s, %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)], Registre[REG_POS(nOpCode,8)], Registre[REG_POS(nOpCode,12)]);
	return 4;
}

int DES_MUL_S(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "mul%sS %s, %s, %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)], Registre[REG_POS(nOpCode,8)]);
	return 4;
}

int DES_MLA_S(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "mla%sS %s, %s, %s, %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)], Registre[REG_POS(nOpCode,8)], Registre[REG_POS(nOpCode,12)]);
	return 4;
}


//----------UMUL--------------------------

int DES_UMULL(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "umull%s %s, %s, %s, %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)], Registre[REG_POS(nOpCode,8)]);
	return 4;
}

int DES_UMLAL(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "umlal%s %s, %s, %s, %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)], Registre[REG_POS(nOpCode,8)]);
	return 4;
}

int DES_UMULL_S(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "umull%ss %s, %s, %s, %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)], Registre[REG_POS(nOpCode,8)]);
	return 4;
}

int DES_UMLAL_S(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "umlal%ss %s, %s, %s, %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)], Registre[REG_POS(nOpCode,8)]);
	return 4;
}

//----------SMUL--------------------------

int DES_SMULL(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "smull%s %s, %s, %s, %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)], Registre[REG_POS(nOpCode,8)]);
	return 4;
}

int DES_SMLAL(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "smlal%s %s, %s, %s, %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)], Registre[REG_POS(nOpCode,8)]);
	return 4;
}

int DES_SMULL_S(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "smull%sS %s, %s, %s, %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)], Registre[REG_POS(nOpCode,8)]);
	return 4;
}

int DES_SMLAL_S(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "smlal%sS %s, %s, %s, %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)], Registre[REG_POS(nOpCode,8)]);
	return 4;
}

//---------------SWP------------------------------

int DES_SWP(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "swp%s %s, %s, [%s]", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,0)], Registre[REG_POS(nOpCode,16)]);
	return 4;
}

int DES_SWPB(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "swpb%s %s, %s, [%s]", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,0)], Registre[REG_POS(nOpCode,16)]);
	return 4;
}

//------------LDRH-----------------------------

int DES_LDRH_P_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldrh%s %s, [%s, %Xh]", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], ((nOpCode>>4)&0xF0)|(nOpCode&0xF));
	return 4;
}

int DES_LDRH_M_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldrh%s %s, [%s, -%Xh]", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], ((nOpCode>>4)&0xF0)|(nOpCode&0xF));
	return 4;
}

int DES_LDRH_P_REG_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldrh%s %s, [%s, %s]", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)]);
	return 4;
}

int DES_LDRH_M_REG_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldrh%s %s, [%s, -%s]", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)]);
	return 4;
}

int DES_LDRH_PRE_INDE_P_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldrh%s %s, [%s, %Xh]!", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], ((nOpCode>>4)&0xF0)|(nOpCode&0xF));
	return 4;
}

int DES_LDRH_PRE_INDE_M_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldrh%s %s, [%s, -%Xh]!", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], ((nOpCode>>4)&0xF0)|(nOpCode&0xF));
	return 4;
}

int DES_LDRH_PRE_INDE_P_REG_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldrh%s %s, [%s, %s]!", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)]);
	return 4;
}

int DES_LDRH_PRE_INDE_M_REG_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldrh%s %s, [%s, -%s]!", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)]);
	return 4;
}

int DES_LDRH_POS_INDE_P_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldrh%s %s, [%s], %Xh", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], ((nOpCode>>4)&0xF0)|(nOpCode&0xF));
	return 4;
}

int DES_LDRH_POS_INDE_M_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldrh%s %s, [%s], -%Xh", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], ((nOpCode>>4)&0xF0)|(nOpCode&0xF));
	return 4;
}

int DES_LDRH_POS_INDE_P_REG_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldrh%s %s, [%s], %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)]);
	return 4;
}

int DES_LDRH_POS_INDE_M_REG_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldrh%s %s, [%s], -%s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)]);
	return 4;
}

//------------STRH-----------------------------

int DES_STRH_P_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "strh%s %s, [%s, %Xh]", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], ((nOpCode>>4)&0xF0)|(nOpCode&0xF));
	return 4;
}

int DES_STRH_M_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "strh%s %s, [%s, -%Xh]", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], ((nOpCode>>4)&0xF0)|(nOpCode&0xF));
	return 4;
}

int DES_STRH_P_REG_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "strh%s %s, [%s, %s]", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)]);
	return 4;
}

int DES_STRH_M_REG_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "strh%s %s, [%s, -%s]", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)]);
	return 4;
}

int DES_STRH_PRE_INDE_P_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "strh%s %s, [%s, %Xh]!", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], ((nOpCode>>4)&0xF0)|(nOpCode&0xF));
	return 4;
}

int DES_STRH_PRE_INDE_M_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "strh%s %s, [%s, -%Xh]!", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], ((nOpCode>>4)&0xF0)|(nOpCode&0xF));
	return 4;
}

int DES_STRH_PRE_INDE_P_REG_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "strh%s %s, [%s, %s]!", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)]);
	return 4;
}

int DES_STRH_PRE_INDE_M_REG_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "strh%s %s, [%s, -%s]!", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)]);
	return 4;
}

int DES_STRH_POS_INDE_P_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "strh%s %s, [%s], %Xh", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], ((nOpCode>>4)&0xF0)|(nOpCode&0xF));
	return 4;
}

int DES_STRH_POS_INDE_M_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "strh%s %s, [%s], -%Xh", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], ((nOpCode>>4)&0xF0)|(nOpCode&0xF));
	return 4;
}

int DES_STRH_POS_INDE_P_REG_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "strh%s %s, [%s], %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)]);
	return 4;
}

int DES_STRH_POS_INDE_M_REG_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "strh%s %s, [%s], -%s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)]);
	return 4;
}

//----------------LDRSH--------------------------

int DES_LDRSH_P_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldrsh%s %s, [%s, %Xh]", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], ((nOpCode>>4)&0xF0)|(nOpCode&0xF));
	return 4;
}

int DES_LDRSH_M_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldrsh%s %s, [%s, -%Xh]", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], ((nOpCode>>4)&0xF0)|(nOpCode&0xF));
	return 4;
}

int DES_LDRSH_P_REG_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldrsh%s %s, [%s, %s]", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)]);
	return 4;
}

int DES_LDRSH_M_REG_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldrsh%s %s, [%s, -%s]", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)]);
	return 4;
}

int DES_LDRSH_PRE_INDE_P_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldrsh%s %s, [%s, %Xh]!", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], ((nOpCode>>4)&0xF0)|(nOpCode&0xF));
	return 4;
}

int DES_LDRSH_PRE_INDE_M_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldrsh%s %s, [%s, -%Xh]!", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], ((nOpCode>>4)&0xF0)|(nOpCode&0xF));
	return 4;
}

int DES_LDRSH_PRE_INDE_P_REG_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldrsh%s %s, [%s, %s]!", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)]);
	return 4;
}

int DES_LDRSH_PRE_INDE_M_REG_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldrsh%s %s, [%s, -%s]!", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)]);
	return 4;
}

int DES_LDRSH_POS_INDE_P_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldrsh%s %s, [%s], %Xh", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], ((nOpCode>>4)&0xF0)|(nOpCode&0xF));
	return 4;
}

int DES_LDRSH_POS_INDE_M_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldrsh%s %s, [%s], -%Xh", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], ((nOpCode>>4)&0xF0)|(nOpCode&0xF));
	return 4;
}

int DES_LDRSH_POS_INDE_P_REG_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldrsh%s %s, [%s], %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)]);
	return 4;
}

int DES_LDRSH_POS_INDE_M_REG_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldrsh%s %s, [%s], -%s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)]);
	return 4;
}

//----------------------LDRSB----------------------

int DES_LDRSB_P_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldrsb%s %s, [%s, %Xh]", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], ((nOpCode>>4)&0xF0)|(nOpCode&0xF));
	return 4;
}

int DES_LDRSB_M_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldrsb%s %s, [%s, -%Xh]", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], ((nOpCode>>4)&0xF0)|(nOpCode&0xF));
	return 4;
}

int DES_LDRSB_P_REG_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldrsb%s %s, [%s, %s]", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)]);
	return 4;
}

int DES_LDRSB_M_REG_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldrsb%s %s, [%s, -%s]", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)]);
	return 4;
}

int DES_LDRSB_PRE_INDE_P_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldrsb%s %s, [%s, %Xh]!", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], ((nOpCode>>4)&0xF0)|(nOpCode&0xF));
	return 4;
}

int DES_LDRSB_PRE_INDE_M_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldrsb%s %s, [%s, -%Xh]!", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], ((nOpCode>>4)&0xF0)|(nOpCode&0xF));
	return 4;
}

int DES_LDRSB_PRE_INDE_P_REG_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldrsb%s %s, [%s, %s]!", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)]);
	return 4;
}

int DES_LDRSB_PRE_INDE_M_REG_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldrsb%s %s, [%s, -%s]!", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)]);
	return 4;
}

int DES_LDRSB_POS_INDE_P_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldrsb%s %s, [%s], %Xh", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], ((nOpCode>>4)&0xF0)|(nOpCode&0xF));
	return 4;
}

int DES_LDRSB_POS_INDE_M_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldrsb%s %s, [%s], -%Xh", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], ((nOpCode>>4)&0xF0)|(nOpCode&0xF));
	return 4;
}

int DES_LDRSB_POS_INDE_P_REG_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldrsb%s %s, [%s], %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)]);
	return 4;
}

int DES_LDRSB_POS_INDE_M_REG_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldrsb%s %s, [%s], -%s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)]);
	return 4;
}

//--------------MRS--------------------------------

int DES_MRS_CPSR(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "mrs%s %s, CPSR", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)]);
	return 4;
}

int DES_MRS_SPSR(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "mrs%s %s, SPSR", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)]);
	return 4;
}

//--------------MSR--------------------------------

int DES_MSR_CPSR(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "msr%s CPSR_%s, %s", Condition[CONDITION(nOpCode)], MSR_FIELD[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)]);
	return 4;
}

int DES_MSR_SPSR(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "msr%s SPSR_%s, %s", Condition[CONDITION(nOpCode)], MSR_FIELD[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)]);
	return 4;
}

int DES_MSR_CPSR_IMM_VAL(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "msr%s CPSR_%s, %Xh", Condition[CONDITION(nOpCode)], MSR_FIELD[REG_POS(nOpCode,16)], ROR((nOpCode&0xFF), ((nOpCode>>7)&0x1E)));
	return 4;
}

int DES_MSR_SPSR_IMM_VAL(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "msr%s SPSR_%s, %Xh", Condition[CONDITION(nOpCode)], MSR_FIELD[REG_POS(nOpCode,16)], ROR((nOpCode&0xFF), (nOpCode>>7)&0x1E));
	return 4;
}

//-----------------BRANCH--------------------------

int DES_BX(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "bx%s %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,0)]);
	return 4;
}

int DES_BLX_REG(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "blx%s %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,0)]);
	return 4;
}

#define SIGNEXTEND_24(nOpCode) (((int)((nOpCode)<<8))>>8)

int DES_B(UINT adr, UINT nOpCode, char * txt)
{
	if(CONDITION(nOpCode)==0xF)
	{
		sprintf(txt, "blx%s %08Xh", Condition[CONDITION(nOpCode)], adr+(SIGNEXTEND_24(nOpCode)<<2)+8);
		return 4;
}
	sprintf(txt, "b%s %08Xh", Condition[CONDITION(nOpCode)], adr+(SIGNEXTEND_24(nOpCode)<<2)+8);
	return 4;
}

int DES_BL(UINT adr, UINT nOpCode, char * txt)
{
	if(CONDITION(nOpCode)==0xF)
	{
		sprintf(txt, "blx%s %08Xh", Condition[CONDITION(nOpCode)], adr+(SIGNEXTEND_24(nOpCode)<<2)+10);
		return 4;
}
	sprintf(txt, "bl%s %08Xh", Condition[CONDITION(nOpCode)], adr+(SIGNEXTEND_24(nOpCode)<<2)+8);
	return 4;
}

//----------------CLZ-------------------------------

int DES_CLZ(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "clz%s %s, %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,0)]);
	return 4;
}


//--------------------QADD--QSUB------------------------------

int DES_QADD(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "qadd%s %s, %s, %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,0)], Registre[REG_POS(nOpCode,16)]);
	return 4;
}

int DES_QSUB(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "qsub%s %s, %s, %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,0)], Registre[REG_POS(nOpCode,16)]);
	return 4;
}

int DES_QDADD(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "qdadd%s %s, %s, %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,0)], Registre[REG_POS(nOpCode,16)]);
	return 4;
}

int DES_QDSUB(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "qdsub%s %s, %s, %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,0)], Registre[REG_POS(nOpCode,16)]);
	return 4;
}

//-----------------SMUL-------------------------------

int DES_SMUL_B_B(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "smulbb%s %s, %s, %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)], Registre[REG_POS(nOpCode,8)]);
	return 4;
}

int DES_SMUL_B_T(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "smulbt%s %s, %s, %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)], Registre[REG_POS(nOpCode,8)]);
	return 4;
}

int DES_SMUL_T_B(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "smultb%s %s, %s, %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)], Registre[REG_POS(nOpCode,8)]);
	return 4;
}

int DES_SMUL_T_T(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "smultt%s %s, %s, %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)], Registre[REG_POS(nOpCode,8)]);
	return 4;
}

//-----------SMLA----------------------------

int DES_SMLA_B_B(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "smlabb%s %s, %s, %s, %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)], Registre[REG_POS(nOpCode,8)], Registre[REG_POS(nOpCode,12)]);
	return 4;
}

int DES_SMLA_B_T(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "smlabt%s %s, %s, %s, %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)], Registre[REG_POS(nOpCode,8)], Registre[REG_POS(nOpCode,12)]);
	return 4;
}

int DES_SMLA_T_B(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "smlatb%s %s, %s, %s, %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)], Registre[REG_POS(nOpCode,8)], Registre[REG_POS(nOpCode,12)]);
	return 4;
}

int DES_SMLA_T_T(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "smlatt%s %s, %s, %s, %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)], Registre[REG_POS(nOpCode,8)], Registre[REG_POS(nOpCode,12)]);
	return 4;
}

//--------------SMLAL---------------------------------------

int DES_SMLAL_B_B(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "smlabb%s %s, %s, %s, %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)], Registre[REG_POS(nOpCode,8)]);
	return 4;
}

int DES_SMLAL_B_T(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "smlabt%s %s, %s, %s, %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)], Registre[REG_POS(nOpCode,8)]);
	return 4;
}

int DES_SMLAL_T_B(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "smlatb%s %s, %s, %s, %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)], Registre[REG_POS(nOpCode,8)]);
	return 4;
}

int DES_SMLAL_T_T(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "smlatt%s %s, %s, %s, %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)], Registre[REG_POS(nOpCode,8)]);
	return 4;
}

//--------------SMULW--------------------

int DES_SMULW_B(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "smulwb%s %s, %s, %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)], Registre[REG_POS(nOpCode,8)]);
	return 4;
}

int DES_SMULW_T(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "smulwt%s %s, %s, %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)], Registre[REG_POS(nOpCode,8)]);
	return 4;
}

//--------------SMLAW-------------------
int DES_SMLAW_B(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "smlawb%s %s, %s, %s, %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)], Registre[REG_POS(nOpCode,8)], Registre[REG_POS(nOpCode,12)]);
	return 4;
}

int DES_SMLAW_T(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "smlawt%s %s, %s, %s, %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,16)], Registre[REG_POS(nOpCode,0)], Registre[REG_POS(nOpCode,8)], Registre[REG_POS(nOpCode,12)]);
	return 4;
}

//------------ldr---------------------------

int DES_LDR_P_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldr%s %s, [%s, %Xh]", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], nOpCode&0xFFF);
	if(REG_POS(nOpCode,16)==15)
		return 4 | (((nOpCode&0xFFF)+4)<<3);
	return 4;
}

int DES_LDR_M_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldr%s %s, [%s, -%Xh]", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], nOpCode&0xFFF);
	return 4;
}

int DES_LDR_P_LSL_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_LSL_IMM(ldr, "", "", "]");
	return 4;
}

int DES_LDR_M_LSL_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_LSL_IMM(ldr, "-", "", "]");
	return 4;
}

int DES_LDR_P_LSR_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(ldr, lsr, "", "", "]");
	return 4;
}

int DES_LDR_M_LSR_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(ldr, lsr, "m", "", "]");
	return 4;
}

int DES_LDR_P_ASR_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(ldr, asr, "", "", "]");
	return 4;
}

int DES_LDR_M_ASR_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(ldr, asr, "-", "", "]");
	return 4;
}

int DES_LDR_P_ROR_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_ROR_IMM(ldr, "", "", "]");
	return 4;
}

int DES_LDR_M_ROR_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_ROR_IMM(ldr, "-", "", "]");
	return 4;
}

int DES_LDR_P_IMM_OFF_PREIND(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldr%s %s, [%s, %Xh]!", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], nOpCode&0xFFF);
	return 4;
}

int DES_LDR_M_IMM_OFF_PREIND(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldr%s %s, [%s, -%Xh]!", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], nOpCode&0xFFF);
	return 4;
}

int DES_LDR_P_LSL_IMM_OFF_PREIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_LSL_IMM(ldr, "", "", "]!");
	return 4;
}

int DES_LDR_M_LSL_IMM_OFF_PREIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_LSL_IMM(ldr, "-", "", "]!");
	return 4;
}

int DES_LDR_P_LSR_IMM_OFF_PREIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(ldr, lsr, "", "", "]!");
	return 4;
}

int DES_LDR_M_LSR_IMM_OFF_PREIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(ldr, lsr, "-", "", "]!");
	return 4;
}

int DES_LDR_P_ASR_IMM_OFF_PREIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(ldr, asr, "", "", "]!");
	return 4;
}

int DES_LDR_M_ASR_IMM_OFF_PREIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(ldr, asr, "-", "", "]!");
	return 4;
}

int DES_LDR_P_ROR_IMM_OFF_PREIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_ROR_IMM(ldr, "", "", "]!");
	return 4;
}

int DES_LDR_M_ROR_IMM_OFF_PREIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_ROR_IMM(ldr, "-", "", "]!");
	return 4;
}

int DES_LDR_P_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldr%s %s, [%s], %Xh!", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], nOpCode&0xFFF);
	return 4;
}

int DES_LDR_P_IMM_OFF_POSTIND2(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldr%s %s, [%s], %Xh!", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], nOpCode&0xFFF);
	return 4;
}

int DES_LDR_M_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldr%s %s, [%s], -%Xh!", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], nOpCode&0xFFF);
	return 4;
}

int DES_LDR_P_LSL_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_LSL_IMM(ldr, "", "]", "");
	return 4;
}

int DES_LDR_M_LSL_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_LSL_IMM(ldr, "-", "]", "");
	return 4;
}

int DES_LDR_P_LSR_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(ldr, lsr, "", "]", "");
	return 4;
}

int DES_LDR_M_LSR_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(ldr, lsr, "-", "]", "");
	return 4;
}

int DES_LDR_P_ASR_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(ldr, asr, "", "]", "");
	return 4;
}

int DES_LDR_M_ASR_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(ldr, asr, "-", "]", "");
	return 4;
}

int DES_LDR_P_ROR_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_ROR_IMM(ldr, "", "]", "");
	return 4;
}

int DES_LDR_M_ROR_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_ROR_IMM(ldr, "-", "]", "");
	return 4;
}

//-----------------LDRB-------------------------------------------

int DES_LDRB_P_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldrb%s %s, [%s, %Xh]", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], nOpCode&0xFFF);
	return 4;
}

int DES_LDRB_M_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldrb%s %s, [%s, -%Xh]", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], nOpCode&0xFFF);
	return 4;
}

int DES_LDRB_P_LSL_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_LSL_IMM(ldrb, "", "", "]");
	return 4;
}

int DES_LDRB_M_LSL_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_LSL_IMM(ldrb, "-", "", "]");
	return 4;
}

int DES_LDRB_P_LSR_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(ldrb, lsr, "", "", "]");
	return 4;
}

int DES_LDRB_M_LSR_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(ldrb, lsr, "m", "", "]");
	return 4;
}

int DES_LDRB_P_ASR_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(ldrb, asr, "", "", "]");
	return 4;
}

int DES_LDRB_M_ASR_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(ldrb, asr, "-", "", "]");
	return 4;
}

int DES_LDRB_P_ROR_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_ROR_IMM(ldrb, "", "", "]");
	return 4;
}

int DES_LDRB_M_ROR_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_ROR_IMM(ldrb, "-", "", "]");
	return 4;
}

int DES_LDRB_P_IMM_OFF_PREIND(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldrb%s %s, [%s, %Xh]!", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], nOpCode&0xFFF);
	return 4;
}

int DES_LDRB_M_IMM_OFF_PREIND(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldrb%s %s, [%s, -%Xh]!", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], nOpCode&0xFFF);
	return 4;
}

int DES_LDRB_P_LSL_IMM_OFF_PREIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_LSL_IMM(ldrb, "", "", "]!");
	return 4;
}

int DES_LDRB_M_LSL_IMM_OFF_PREIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_LSL_IMM(ldrb, "-", "", "]!");
	return 4;
}

int DES_LDRB_P_LSR_IMM_OFF_PREIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(ldrb, lsr, "", "", "]!");
	return 4;
}

int DES_LDRB_M_LSR_IMM_OFF_PREIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(ldrb, lsr, "-", "", "]!");
	return 4;
}

int DES_LDRB_P_ASR_IMM_OFF_PREIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(ldrb, asr, "", "", "]!");
	return 4;
}

int DES_LDRB_M_ASR_IMM_OFF_PREIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(ldrb, asr, "-", "", "]!");
	return 4;
}

int DES_LDRB_P_ROR_IMM_OFF_PREIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_ROR_IMM(ldrb, "", "", "]!");
	return 4;
}

int DES_LDRB_M_ROR_IMM_OFF_PREIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_ROR_IMM(ldrb, "-", "", "]!");
	return 4;
}

int DES_LDRB_P_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldrb%s %s, [%s], %Xh!", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], nOpCode&0xFFF);
	return 4;
}

int DES_LDRB_M_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldrb%s %s, [%s], -%Xh!", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], nOpCode&0xFFF);
	return 4;
}

int DES_LDRB_P_LSL_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_LSL_IMM(ldrb, "", "]", "");
	return 4;
}

int DES_LDRB_M_LSL_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_LSL_IMM(ldrb, "-", "]", "");
	return 4;
}

int DES_LDRB_P_LSR_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(ldrb, lsr, "", "]", "");
	return 4;
}

int DES_LDRB_M_LSR_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(ldrb, lsr, "-", "]", "");
	return 4;
}

int DES_LDRB_P_ASR_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(ldrb, asr, "", "]", "");
	return 4;
}

int DES_LDRB_M_ASR_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(ldrb, asr, "-", "]", "");
	return 4;
}

int DES_LDRB_P_ROR_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_ROR_IMM(ldrb, "", "]", "");
	return 4;
}

int DES_LDRB_M_ROR_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_ROR_IMM(ldrb, "-", "]", "");
	return 4;
}

//----------------------str--------------------------------

int DES_STR_P_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "str%s %s, [%s, %Xh]", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], nOpCode&0xFFF);
	return 4;
}

int DES_STR_M_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "str%s %s, [%s, -%Xh]", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], nOpCode&0xFFF);
	return 4;
}

int DES_STR_P_LSL_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_LSL_IMM(str, "", "", "]");
	return 4;
}

int DES_STR_M_LSL_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_LSL_IMM(str, "-", "", "]");
	return 4;
}

int DES_STR_P_LSR_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(str, lsr, "", "", "]");
	return 4;
}

int DES_STR_M_LSR_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(str, lsr, "m", "", "]");
	return 4;
}

int DES_STR_P_ASR_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(str, asr, "", "", "]");
	return 4;
}

int DES_STR_M_ASR_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(str, asr, "-", "", "]");
	return 4;
}

int DES_STR_P_ROR_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_ROR_IMM(str, "", "", "]");
	return 4;
}

int DES_STR_M_ROR_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_ROR_IMM(str, "-", "", "]");
	return 4;
}

int DES_STR_P_IMM_OFF_PREIND(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "str%s %s, [%s, %Xh]!", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], nOpCode&0xFFF);
	return 4;
}

int DES_STR_M_IMM_OFF_PREIND(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "str%s %s, [%s, -%Xh]!", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], nOpCode&0xFFF);
	return 4;
}

int DES_STR_P_LSL_IMM_OFF_PREIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_LSL_IMM(str, "", "", "]!");
	return 4;
}

int DES_STR_M_LSL_IMM_OFF_PREIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_LSL_IMM(str, "-", "", "]!");
	return 4;
}

int DES_STR_P_LSR_IMM_OFF_PREIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(str, lsr, "", "", "]!");
	return 4;
}

int DES_STR_M_LSR_IMM_OFF_PREIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(str, lsr, "-", "", "]!");
	return 4;
}

int DES_STR_P_ASR_IMM_OFF_PREIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(str, asr, "", "", "]!");
	return 4;
}

int DES_STR_M_ASR_IMM_OFF_PREIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(str, asr, "-", "", "]!");
	return 4;
}

int DES_STR_P_ROR_IMM_OFF_PREIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_ROR_IMM(str, "", "", "]!");
	return 4;
}

int DES_STR_M_ROR_IMM_OFF_PREIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_ROR_IMM(str, "-", "", "]!");
	return 4;
}

int DES_STR_P_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "str%s %s, [%s], %Xh!", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], nOpCode&0xFFF);
	return 4;
}

int DES_STR_M_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "str%s %s, [%s], -%Xh!", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], nOpCode&0xFFF);
	return 4;
}

int DES_STR_P_LSL_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_LSL_IMM(str, "", "]", "");
	return 4;
}

int DES_STR_M_LSL_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_LSL_IMM(str, "-", "]", "");
	return 4;
}

int DES_STR_P_LSR_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(str, lsr, "", "]", "");
	return 4;
}

int DES_STR_M_LSR_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(str, lsr, "-", "]", "");
	return 4;
}

int DES_STR_P_ASR_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(str, asr, "", "]", "");
	return 4;
}

int DES_STR_M_ASR_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(str, asr, "-", "]", "");
	return 4;
}

int DES_STR_P_ROR_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_ROR_IMM(str, "", "]", "");
	return 4;
}

int DES_STR_M_ROR_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_ROR_IMM(str, "-", "]", "");
	return 4;
}

//-----------------------strb-------------------------------------

int DES_STRB_P_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "strb%s %s, [%s, %Xh]", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], nOpCode&0xFFF);
	return 4;
}

int DES_STRB_M_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "strb%s %s, [%s, -%Xh]", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], nOpCode&0xFFF);
	return 4;
}

int DES_STRB_P_LSL_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_LSL_IMM(strb, "", "", "]");
	return 4;
}

int DES_STRB_M_LSL_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_LSL_IMM(strb, "-", "", "]");
	return 4;
}

int DES_STRB_P_LSR_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(strb, lsr, "", "", "]");
	return 4;
}

int DES_STRB_M_LSR_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(strb, lsr, "m", "", "]");
	return 4;
}

int DES_STRB_P_ASR_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(strb, asr, "", "", "]");
	return 4;
}

int DES_STRB_M_ASR_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(strb, asr, "-", "", "]");
	return 4;
}

int DES_STRB_P_ROR_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_ROR_IMM(strb, "", "", "]");
	return 4;
}

int DES_STRB_M_ROR_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_ROR_IMM(strb, "-", "", "]");
	return 4;
}

int DES_STRB_P_IMM_OFF_PREIND(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "strb%s %s, [%s, %Xh]!", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], nOpCode&0xFFF);
	return 4;
}

int DES_STRB_M_IMM_OFF_PREIND(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "strb%s %s, [%s, -%Xh]!", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], nOpCode&0xFFF);
	return 4;
}

int DES_STRB_P_LSL_IMM_OFF_PREIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_LSL_IMM(strb, "", "", "]!");
	return 4;
}

int DES_STRB_M_LSL_IMM_OFF_PREIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_LSL_IMM(strb, "-", "", "]!");
	return 4;
}

int DES_STRB_P_LSR_IMM_OFF_PREIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(strb, lsr, "", "", "]!");
	return 4;
}

int DES_STRB_M_LSR_IMM_OFF_PREIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(strb, lsr, "-", "", "]!");
	return 4;
}

int DES_STRB_P_ASR_IMM_OFF_PREIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(strb, asr, "", "", "]!");
	return 4;
}

int DES_STRB_M_ASR_IMM_OFF_PREIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(strb, asr, "-", "", "]!");
	return 4;
}

int DES_STRB_P_ROR_IMM_OFF_PREIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_ROR_IMM(strb, "", "", "]!");
	return 4;
}

int DES_STRB_M_ROR_IMM_OFF_PREIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_ROR_IMM(strb, "-", "", "]!");
	return 4;
}

int DES_STRB_P_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "strb%s %s, [%s], %Xh!", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], nOpCode&0xFFF);
	return 4;
}

int DES_STRB_M_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "strb%s %s, [%s], -%Xh!", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], nOpCode&0xFFF);
	return 4;
}

int DES_STRB_P_LSL_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_LSL_IMM(strb, "", "]", "");
	return 4;
}

int DES_STRB_M_LSL_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_LSL_IMM(strb, "-", "]", "");
	return 4;
}

int DES_STRB_P_LSR_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(strb, lsr, "", "]", "");
	return 4;
}

int DES_STRB_M_LSR_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(strb, lsr, "-", "]", "");
	return 4;
}

int DES_STRB_P_ASR_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(strb, asr, "", "]", "");
	return 4;
}

int DES_STRB_M_ASR_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(strb, asr, "-", "]", "");
	return 4;
}

int DES_STRB_P_ROR_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_ROR_IMM(strb, "", "]", "");
	return 4;
}

int DES_STRB_M_ROR_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_ROR_IMM(strb, "-", "]", "");
	return 4;
}

//-----------------------ldrbt-------------------------------------

int DES_LDRBT_P_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldrbt%s %s, [%s], %Xh!", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], nOpCode&0xFFF);
	return 4;
}

int DES_LDRBT_M_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldrbt%s %s, [%s], -%Xh!", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], nOpCode&0xFFF);
	return 4;
}

int DES_LDRBT_P_LSL_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_LSL_IMM(ldrbt, "", "]", "");
	return 4;
}

int DES_LDRBT_M_LSL_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_LSL_IMM(ldrbt, "-", "]", "");
	return 4;
}

int DES_LDRBT_P_LSR_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(ldrbt, lsr, "", "]", "");
	return 4;
}

int DES_LDRBT_M_LSR_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(ldrbt, lsr, "-", "]", "");
	return 4;
}

int DES_LDRBT_P_ASR_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(ldrbt, asr, "", "]", "");
	return 4;
}

int DES_LDRBT_M_ASR_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(ldrbt, asr, "-", "]", "");
	return 4;
}

int DES_LDRBT_P_ROR_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_ROR_IMM(ldrbt, "", "]", "");
	return 4;
}

int DES_LDRBT_M_ROR_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_ROR_IMM(ldrbt, "-", "]", "");
	return 4;
}

//----------------------STRBT----------------------------

int DES_STRBT_P_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "strbt%s %s, [%s], %Xh!", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], nOpCode&0xFFF);
	return 4;
}

int DES_STRBT_M_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "strbt%s %s, [%s], -%Xh!", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,12)], Registre[REG_POS(nOpCode,16)], nOpCode&0xFFF);
	return 4;
}

int DES_STRBT_P_LSL_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_LSL_IMM(STRBT, "", "]", "");
	return 4;
}

int DES_STRBT_M_LSL_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_LSL_IMM(STRBT, "-", "]", "");
	return 4;
}

int DES_STRBT_P_LSR_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(STRBT, lsr, "", "]", "");
	return 4;
}

int DES_STRBT_M_LSR_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(STRBT, lsr, "-", "]", "");
	return 4;
}

int DES_STRBT_P_ASR_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(STRBT, asr, "", "]", "");
	return 4;
}

int DES_STRBT_M_ASR_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_IMM_SHIFT(STRBT, asr, "-", "]", "");
	return 4;
}

int DES_STRBT_P_ROR_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_ROR_IMM(STRBT, "", "]", "");
	return 4;
}

int DES_STRBT_M_ROR_IMM_OFF_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	LDRSTR_ROR_IMM(STRBT, "-", "]", "");
	return 4;
}

//---------------------LDM-----------------------------

int DES_LDMIA(UINT adr, UINT nOpCode, char * txt)
{
	RegList(16);
	sprintf(txt, "ldmia%s %s, %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,16)], g_szBufferA);
	return 4;
}

int DES_LDMIB(UINT adr, UINT nOpCode, char * txt)
{
	RegList(16);
	sprintf(txt, "ldmib%s %s, %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,16)], g_szBufferA);
	return 4;
}

int DES_LDMDA(UINT adr, UINT nOpCode, char * txt)
{
	RegList(16);
	sprintf(txt, "ldmda%s %s, %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,16)], g_szBufferA);
	return 4;
}

int DES_LDMDB(UINT adr, UINT nOpCode, char * txt)
{
	RegList(16);
	sprintf(txt, "ldmdb%s %s, %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,16)], g_szBufferA);
	return 4;
}

int DES_LDMIA_W(UINT adr, UINT nOpCode, char * txt)
{
	RegList(16);
	sprintf(txt, "ldmia%s [%s]!, %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,16)], g_szBufferA);
	return 4;
}

int DES_LDMIB_W(UINT adr, UINT nOpCode, char * txt)
{
	RegList(16);
	sprintf(txt, "ldmib%s [%s]!, %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,16)], g_szBufferA);
	return 4;
}

int DES_LDMDA_W(UINT adr, UINT nOpCode, char * txt)
{
	RegList(16);
	sprintf(txt, "ldmda%s [%s]!, %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,16)], g_szBufferA);
	return 4;
}

int DES_LDMDB_W(UINT adr, UINT nOpCode, char * txt)
{
	RegList(16);
	sprintf(txt, "ldmdb%s [%s]!, %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,16)], g_szBufferA);
	return 4;
}

int DES_LDMIA2(UINT adr, UINT nOpCode, char * txt)
{
	RegList(16);
	sprintf(txt, "ldmia%s %s, %s^", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,16)], g_szBufferA);
	return 4;
}

int DES_LDMIB2(UINT adr, UINT nOpCode, char * txt)
{
	RegList(16);
	sprintf(txt, "ldmib%s %s, %s^", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,16)], g_szBufferA);
	return 4;
}

int DES_LDMDA2(UINT adr, UINT nOpCode, char * txt)
{
	RegList(16);
	sprintf(txt, "ldmda%s %s, %s^", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,16)], g_szBufferA);
	return 4;
}

int DES_LDMDB2(UINT adr, UINT nOpCode, char * txt)
{
	RegList(16);
	sprintf(txt, "ldmdb%s %s, %s^", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,16)], g_szBufferA);
	return 4;
}

int DES_LDMIA2_W(UINT adr, UINT nOpCode, char * txt)
{
	RegList(16);
	sprintf(txt, "ldmia%s [%s]!, %s^", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,16)], g_szBufferA);
	if(BIT15(nOpCode)==0) sprintf(txt, "%s ?????", txt);
	return 4;
}

int DES_LDMIB2_W(UINT adr, UINT nOpCode, char * txt)
{
	RegList(16);
	sprintf(txt, "ldmib%s [%s]!, %s^", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,16)], g_szBufferA);
	if(BIT15(nOpCode)==0) sprintf(txt, "%s ?????", txt);
	return 4;
}

int DES_LDMDA2_W(UINT adr, UINT nOpCode, char * txt)
{
	RegList(16);
	sprintf(txt, "ldmda%s [%s]!, %s^", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,16)], g_szBufferA);
	if(BIT15(nOpCode)==0) sprintf(txt, "%s ?????", txt);
	return 4;
}

int DES_LDMDB2_W(UINT adr, UINT nOpCode, char * txt)
{
	RegList(16);
	sprintf(txt, "ldmdb%s [%s]!, %s^", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,16)], g_szBufferA);
	if(BIT15(nOpCode)==0) sprintf(txt, "%s ?????", txt);
	return 4;
}

//------------------------------STM----------------------------------

int DES_STMIA(UINT adr, UINT nOpCode, char * txt)
{
	RegList(16);
	sprintf(txt, "stmia%s [%s], %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,16)], g_szBufferA);
	return 4;
}

int DES_STMIB(UINT adr, UINT nOpCode, char * txt)
{
	RegList(16);
	sprintf(txt, "stmib%s [%s], %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,16)], g_szBufferA);
	return 4;
}

int DES_STMDA(UINT adr, UINT nOpCode, char * txt)
{
	RegList(16);
	sprintf(txt, "stmda%s [%s], %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,16)], g_szBufferA);
	return 4;
}

int DES_STMDB(UINT adr, UINT nOpCode, char * txt)
{
	RegList(16);
	sprintf(txt, "stmdb%s [%s], %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,16)], g_szBufferA);
	return 4;
}

int DES_STMIA_W(UINT adr, UINT nOpCode, char * txt)
{
	RegList(16);
	sprintf(txt, "stmia%s [%s]!, %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,16)], g_szBufferA);
	return 4;
}

int DES_STMIB_W(UINT adr, UINT nOpCode, char * txt)
{
	RegList(16);
	sprintf(txt, "stmib%s [%s]!, %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,16)], g_szBufferA);
	return 4;
}

int DES_STMDA_W(UINT adr, UINT nOpCode, char * txt)
{
	RegList(16);
	sprintf(txt, "stmda%s [%s]!, %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,16)], g_szBufferA);
	return 4;
}

int DES_STMDB_W(UINT adr, UINT nOpCode, char * txt)
{
	RegList(16);
	sprintf(txt, "stmdb%s [%s]!, %s", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,16)], g_szBufferA);
	return 4;
}

int DES_STMIA2(UINT adr, UINT nOpCode, char * txt)
{
	RegList(16);
	sprintf(txt, "stmia%s [%s], %s^", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,16)], g_szBufferA);
	return 4;
}

int DES_STMIB2(UINT adr, UINT nOpCode, char * txt)
{
	RegList(16);
	sprintf(txt, "stmib%s [%s], %s^", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,16)], g_szBufferA);
	return 4;
}

int DES_STMDA2(UINT adr, UINT nOpCode, char * txt)
{
	RegList(16);
	sprintf(txt, "stmda%s [%s], %s^", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,16)], g_szBufferA);
	return 4;
}

int DES_STMDB2(UINT adr, UINT nOpCode, char * txt)
{
	RegList(16);
	sprintf(txt, "stmdb%s [%s], %s^", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,16)], g_szBufferA);
	return 4;
}

int DES_STMIA2_W(UINT adr, UINT nOpCode, char * txt)
{
	RegList(16);
	sprintf(txt, "stmia%s [%s]!, %s^ ?????", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,16)], g_szBufferA);
	return 4;
}

int DES_STMIB2_W(UINT adr, UINT nOpCode, char * txt)
{
	RegList(16);
	sprintf(txt, "stmib%s [%s]!, %s^ ?????", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,16)], g_szBufferA);
	return 4;
}

int DES_STMDA2_W(UINT adr, UINT nOpCode, char * txt)
{
	RegList(16);
	sprintf(txt, "stmda%s [%s]!, %s^ ?????", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,16)], g_szBufferA);
	return 4;
}

int DES_STMDB2_W(UINT adr, UINT nOpCode, char * txt)
{
	RegList(16);
	sprintf(txt, "stmdb%s [%s]!, %s^ ?????", Condition[CONDITION(nOpCode)], Registre[REG_POS(nOpCode,16)], g_szBufferA);
	return 4;
}

//---------------------STC----------------------------------

int DES_STC_P_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	if(CONDITION(nOpCode)==0xF)
	{
		sprintf(txt, "stc2 cp%X, cr%X, [%s, %Xh]", REG_POS(nOpCode, 8), REG_POS(nOpCode, 12), Registre[REG_POS(nOpCode, 16)], (nOpCode&0xFF)<<2);
		return 4;
	}
	sprintf(txt, "stc%s %X, cp%X, [%s, %Xh]",Condition[CONDITION(nOpCode)], REG_POS(nOpCode, 8), REG_POS(nOpCode, 12), Registre[REG_POS(nOpCode, 16)], (nOpCode&0xFF)<<2);
	return 4;
}

int DES_STC_M_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	if(CONDITION(nOpCode)==0xF)
	{
		sprintf(txt, "stc2 cp%X, cr%X, [%s, -%Xh]", REG_POS(nOpCode, 8), REG_POS(nOpCode, 12), Registre[REG_POS(nOpCode, 16)], (nOpCode&0xFF)<<2);
		return 4;
	}
	sprintf(txt, "stc%s cp%X, cr%X, [%s, -%Xh]",Condition[CONDITION(nOpCode)], REG_POS(nOpCode, 8), REG_POS(nOpCode, 12), Registre[REG_POS(nOpCode, 16)], (nOpCode&0xFF)<<2);
	return 4;
}

int DES_STC_P_PREIND(UINT adr, UINT nOpCode, char * txt)
{
	if(CONDITION(nOpCode)==0xF)
	{
		sprintf(txt, "stc2 cp%X, cr%X, [%s, %Xh]!", REG_POS(nOpCode, 8), REG_POS(nOpCode, 12), Registre[REG_POS(nOpCode, 16)], (nOpCode&0xFF)<<2);
		return 4;
	}
	sprintf(txt, "stc%s cp%X, cr%X, [%s, %Xh]!",Condition[CONDITION(nOpCode)], REG_POS(nOpCode, 8), REG_POS(nOpCode, 12), Registre[REG_POS(nOpCode, 16)], (nOpCode&0xFF)<<2);
	return 4;
}

int DES_STC_M_PREIND(UINT adr, UINT nOpCode, char * txt)
{
	if(CONDITION(nOpCode)==0xF)
	{
		sprintf(txt, "stc2 cp%X, cr%X, [%s, -%Xh]!", REG_POS(nOpCode, 8), REG_POS(nOpCode, 12), Registre[REG_POS(nOpCode, 16)], (nOpCode&0xFF)<<2);
		return 4;
	}
	sprintf(txt, "stc%s cp%X, cr%X, [%s, -%Xh]!",Condition[CONDITION(nOpCode)], REG_POS(nOpCode, 8), REG_POS(nOpCode, 12), Registre[REG_POS(nOpCode, 16)], (nOpCode&0xFF)<<2);
	return 4;
}

int DES_STC_P_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	if(CONDITION(nOpCode)==0xF)
	{
		sprintf(txt, "stc2 cp%X, cr%X, [%s], %Xh", REG_POS(nOpCode, 8), REG_POS(nOpCode, 12), Registre[REG_POS(nOpCode, 16)], (nOpCode&0xFF)<<2);
		return 4;
	}
	sprintf(txt, "stc%s cp%X, cr%X, [%s], %Xh",Condition[CONDITION(nOpCode)], REG_POS(nOpCode, 8), REG_POS(nOpCode, 12), Registre[REG_POS(nOpCode, 16)], (nOpCode&0xFF)<<2);
	return 4;
}

int DES_STC_M_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	if(CONDITION(nOpCode)==0xF)
	{
		sprintf(txt, "stc2 cp%X, cr%X, [%s], -%Xh", REG_POS(nOpCode, 8), REG_POS(nOpCode, 12), Registre[REG_POS(nOpCode, 16)], (nOpCode&0xFF)<<2);
		return 4;
	}
	sprintf(txt, "stc%s cp%X, cr%X, [%s], -%Xh",Condition[CONDITION(nOpCode)], REG_POS(nOpCode, 8), REG_POS(nOpCode, 12), Registre[REG_POS(nOpCode, 16)], (nOpCode&0xFF)<<2);
	return 4;
}

int DES_STC_OPTION(UINT adr, UINT nOpCode, char * txt)
{
	if(CONDITION(nOpCode)==0xF)
	{
		sprintf(txt, "stc2 cp%X, cr%X, [%s], %X", REG_POS(nOpCode, 8), REG_POS(nOpCode, 12), Registre[REG_POS(nOpCode, 16)], nOpCode&0xFF);
		return 4;
	}
	sprintf(txt, "stc%s cp%X, cr%X, [%s], %X",Condition[CONDITION(nOpCode)], REG_POS(nOpCode, 8), REG_POS(nOpCode, 12), Registre[REG_POS(nOpCode, 16)], nOpCode&0xFF);
	return 4;
}

//---------------------LDC----------------------------------

int DES_LDC_P_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	if(CONDITION(nOpCode)==0xF)
	{
		sprintf(txt, "ldc2 cp%X, cr%X, [%s, %Xh]", REG_POS(nOpCode, 8), REG_POS(nOpCode, 12), Registre[REG_POS(nOpCode, 16)], (nOpCode&0xFF)<<2);
		return 4;
	}
	sprintf(txt, "ldc%s cp%X, cr%X, [%s, %Xh]",Condition[CONDITION(nOpCode)], REG_POS(nOpCode, 8), REG_POS(nOpCode, 12), Registre[REG_POS(nOpCode, 16)], (nOpCode&0xFF)<<2);
	return 4;
}

int DES_LDC_M_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	if(CONDITION(nOpCode)==0xF)
	{
		sprintf(txt, "ldc2 cp%X, cr%X, [%s, -%Xh]", REG_POS(nOpCode, 8), REG_POS(nOpCode, 12), Registre[REG_POS(nOpCode, 16)], (nOpCode&0xFF)<<2);
		return 4;
	}
	sprintf(txt, "ldc%s cp%X, cr%X, [%s, -%Xh]",Condition[CONDITION(nOpCode)], REG_POS(nOpCode, 8), REG_POS(nOpCode, 12), Registre[REG_POS(nOpCode, 16)], (nOpCode&0xFF)<<2);
	return 4;
}

int DES_LDC_P_PREIND(UINT adr, UINT nOpCode, char * txt)
{
	if(CONDITION(nOpCode)==0xF)
	{
		sprintf(txt, "ldc2 cp%X, cr%X, [%s, %Xh]!", REG_POS(nOpCode, 8), REG_POS(nOpCode, 12), Registre[REG_POS(nOpCode, 16)], (nOpCode&0xFF)<<2);
		return 4;
	}
	sprintf(txt, "ldc%s cp%X, cr%X, [%s, %Xh]!",Condition[CONDITION(nOpCode)], REG_POS(nOpCode, 8), REG_POS(nOpCode, 12), Registre[REG_POS(nOpCode, 16)], (nOpCode&0xFF)<<2);
	return 4;
}

int DES_LDC_M_PREIND(UINT adr, UINT nOpCode, char * txt)
{
	if(CONDITION(nOpCode)==0xF)
	{
		sprintf(txt, "ldc2 cp%X, cr%X, [%s, -%Xh]!", REG_POS(nOpCode, 8), REG_POS(nOpCode, 12), Registre[REG_POS(nOpCode, 16)], (nOpCode&0xFF)<<2);
		return 4;
	}
	sprintf(txt, "ldc%s cp%X, cr%X, [%s, -%Xh]!",Condition[CONDITION(nOpCode)], REG_POS(nOpCode, 8), REG_POS(nOpCode, 12), Registre[REG_POS(nOpCode, 16)], (nOpCode&0xFF)<<2);
	return 4;
}

int DES_LDC_P_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	if(CONDITION(nOpCode)==0xF)
	{
		sprintf(txt, "ldc2 cp%X, cr%X, [%s], %Xh", REG_POS(nOpCode, 8), REG_POS(nOpCode, 12), Registre[REG_POS(nOpCode, 16)], (nOpCode&0xFF)<<2);
		return 4;
	}
	sprintf(txt, "ldc%s cp%X, cr%X, [%s], %Xh",Condition[CONDITION(nOpCode)], REG_POS(nOpCode, 8), REG_POS(nOpCode, 12), Registre[REG_POS(nOpCode, 16)], (nOpCode&0xFF)<<2);
	return 4;
}

int DES_LDC_M_POSTIND(UINT adr, UINT nOpCode, char * txt)
{
	if(CONDITION(nOpCode)==0xF)
	{
		sprintf(txt, "ldc2 cp%X, cr%X, [%s], -%Xh", REG_POS(nOpCode, 8), REG_POS(nOpCode, 12), Registre[REG_POS(nOpCode, 16)], (nOpCode&0xFF)<<2);
		return 4;
	}
	sprintf(txt, "ldc%s cp%X, cr%X, [%s], -%Xh",Condition[CONDITION(nOpCode)], REG_POS(nOpCode, 8), REG_POS(nOpCode, 12), Registre[REG_POS(nOpCode, 16)], (nOpCode&0xFF)<<2);
	return 4;
}

int DES_LDC_OPTION(UINT adr, UINT nOpCode, char * txt)
{
	if(CONDITION(nOpCode)==0xF)
	{
		sprintf(txt, "ldc2 cp%X, cr%X, [%s], %X", REG_POS(nOpCode, 8), REG_POS(nOpCode, 12), Registre[REG_POS(nOpCode, 16)], nOpCode&0xFF);
		return 4;
	}
	sprintf(txt, "ldc%s cp%X, cr%X, [%s], %X",Condition[CONDITION(nOpCode)], REG_POS(nOpCode, 8), REG_POS(nOpCode, 12), Registre[REG_POS(nOpCode, 16)], nOpCode&0xFF);
	return 4;
}

//----------------MCR-----------------------

int DES_MCR(UINT adr, UINT nOpCode, char * txt)
{
	if(CONDITION(nOpCode)==0xF)
	{
		sprintf(txt, "mcr2 cp%d, %X, %s, cr%d, cr%d, %X",REG_POS(nOpCode, 8), (nOpCode>>21)&7, Registre[REG_POS(nOpCode, 12)], REG_POS(nOpCode, 16), REG_POS(nOpCode, 0), (nOpCode>>5)&0x7);
		return 4;
	}
	sprintf(txt, "mcr%s cp%d, %X, %s, cr%d, cr%d, %X",Condition[CONDITION(nOpCode)], REG_POS(nOpCode, 8), (nOpCode>>21)&7, Registre[REG_POS(nOpCode, 12)], REG_POS(nOpCode, 16), REG_POS(nOpCode, 0), (nOpCode>>5)&0x7);
	return 4;
}

//----------------MRC-----------------------

int DES_MRC(UINT adr, UINT nOpCode, char * txt)
{
	if(CONDITION(nOpCode)==0xF)
	{
		sprintf(txt, "mrc2 cP%d, %X, %s, cr%d, cr%d, %X",REG_POS(nOpCode, 8), (nOpCode>>21)&7, Registre[REG_POS(nOpCode, 12)], REG_POS(nOpCode, 16), REG_POS(nOpCode, 0), (nOpCode>>5)&0x7);
		return 4;
	}
	sprintf(txt, "mrc%s cp%d, %X, %s, cr%d, cr%d, %X",Condition[CONDITION(nOpCode)], REG_POS(nOpCode, 8), (nOpCode>>21)&7, Registre[REG_POS(nOpCode, 12)], REG_POS(nOpCode, 16), REG_POS(nOpCode, 0), (nOpCode>>5)&0x7);
	return 4;
}

//--------------SWI--------------------------

int DES_SWI(UINT adr, UINT nOpCode, char * txt)
{
	const char **swi= *txt==0?swi9:swi7;
	BYTE no = (nOpCode>>16)&0xFF;
	sprintf(txt, "swi%s %02X  ;%s",Condition[CONDITION(nOpCode)], no, no>0x1F?"-/-":swi[no]);
	return 4;
}

//----------------BKPT-------------------------
int DES_BKPT(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "bkpt %Xh",((nOpCode>>4)&0xFFF)|(nOpCode&0xF));
	return 4;
}

//----------------CDP-----------------------

int DES_CDP(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "cdp-------------------------------");
	return 4;
}

//------------------------------------------------------------
// THUMB
//------------------------------------------------------------
#define REG_NUM(nOpCode, n) (((nOpCode)>>n)&0x7)

int DES_UND_THUMB(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "????");
	return 2;
}

int DES_LSL_0(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "lsl %s, %s, 0", Registre[REG_NUM(nOpCode, 0)], Registre[REG_NUM(nOpCode, 3)]);
	return 2;
}

int DES_LSL(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "lsl %s, %s, %Xh", Registre[REG_NUM(nOpCode, 0)], Registre[REG_NUM(nOpCode, 3)], (nOpCode>>6) & 0x1F);
	return 2;
}

int DES_LSR_0(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "lsr %s, %s, 0", Registre[REG_NUM(nOpCode, 0)], Registre[REG_NUM(nOpCode, 3)]);
	return 2;
}

int DES_LSR(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "lsr %s, %s, %Xh", Registre[REG_NUM(nOpCode, 0)], Registre[REG_NUM(nOpCode, 3)], (nOpCode>>6) & 0x1F);
	return 2;
}

int DES_ASR_0(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "asr %s, %s, 0", Registre[REG_NUM(nOpCode, 0)], Registre[REG_NUM(nOpCode, 3)]);
	return 2;
}

int DES_ASR(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "asr %s, %s, %Xh", Registre[REG_NUM(nOpCode, 0)], Registre[REG_NUM(nOpCode, 3)], (nOpCode>>6) & 0x1F);
	return 2;
}

int DES_ADD_REG(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "add %s, %s, %s", Registre[REG_NUM(nOpCode, 0)], Registre[REG_NUM(nOpCode, 3)], Registre[REG_NUM(nOpCode, 6)]);
	return 2;
}

int DES_SUB_REG(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "sub %s, %s, %s", Registre[REG_NUM(nOpCode, 0)], Registre[REG_NUM(nOpCode, 3)], Registre[REG_NUM(nOpCode, 6)]);
	return 2;
}

int DES_ADD_IMM3(UINT adr, UINT nOpCode, char * txt)
{
	UINT imm = REG_NUM(nOpCode, 6);
	if(imm)
		sprintf(txt, "add %s, %s, %Xh", Registre[REG_NUM(nOpCode, 0)], Registre[REG_NUM(nOpCode, 3)], imm);
	else
		sprintf(txt, "mov %s, %s", Registre[REG_NUM(nOpCode, 0)], Registre[REG_NUM(nOpCode, 3)]);
	return 2;
}

int DES_SUB_IMM3(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "sub %s, %s, %Xh", Registre[REG_NUM(nOpCode, 0)], Registre[REG_NUM(nOpCode, 3)], REG_NUM(nOpCode, 6));
	return 2;
}

int DES_MOV_IMM8(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "mov %s, %Xh", Registre[REG_NUM(nOpCode, 8)], nOpCode&0xFF);
	return 2;
}

int DES_CMP_IMM8(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "cmp %s, %Xh", Registre[REG_NUM(nOpCode, 8)], nOpCode&0xFF);
	return 2;
}

int DES_ADD_IMM8(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "add %s, %Xh", Registre[REG_NUM(nOpCode, 8)], nOpCode&0xFF);
	return 2;
}

int DES_SUB_IMM8(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "sub %s, %Xh", Registre[REG_NUM(nOpCode, 8)], nOpCode&0xFF);
	return 2;
}

int DES_AND(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "and %s, %s", Registre[REG_NUM(nOpCode, 0)], Registre[REG_NUM(nOpCode, 3)]);
	return 2;
}

int DES_EOR(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "eor %s, %s", Registre[REG_NUM(nOpCode, 0)], Registre[REG_NUM(nOpCode, 3)]);
	return 2;
}

int DES_LSL_REG(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "lsl %s, %s", Registre[REG_NUM(nOpCode, 0)], Registre[REG_NUM(nOpCode, 3)]);
	return 2;
}

int DES_LSR_REG(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "lsr %s, %s", Registre[REG_NUM(nOpCode, 0)], Registre[REG_NUM(nOpCode, 3)]);
	return 2;
}

int DES_ASR_REG(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "asr %s, %s", Registre[REG_NUM(nOpCode, 0)], Registre[REG_NUM(nOpCode, 3)]);
	return 2;
}

int DES_ADC_REG(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "adc %s, %s", Registre[REG_NUM(nOpCode, 0)], Registre[REG_NUM(nOpCode, 3)]);
	return 2;
}

int DES_SBC_REG(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "sbc %s, %s", Registre[REG_NUM(nOpCode, 0)], Registre[REG_NUM(nOpCode, 3)]);
	return 2;
}

int DES_ROR_REG(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ror %s, %s", Registre[REG_NUM(nOpCode, 0)], Registre[REG_NUM(nOpCode, 3)]);
	return 2;
}

int DES_TST(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "tst %s, %s", Registre[REG_NUM(nOpCode, 0)], Registre[REG_NUM(nOpCode, 3)]);
	return 2;
}

int DES_NEG(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "neg %s, %s", Registre[REG_NUM(nOpCode, 0)], Registre[REG_NUM(nOpCode, 3)]);
	return 2;
}

int DES_CMP(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "cmp %s, %s", Registre[REG_NUM(nOpCode, 0)], Registre[REG_NUM(nOpCode, 3)]);
	return 2;
}

int DES_CMN(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "cmn %s, %s", Registre[REG_NUM(nOpCode, 0)], Registre[REG_NUM(nOpCode, 3)]);
	return 2;
}

int DES_ORR(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "orr %s, %s", Registre[REG_NUM(nOpCode, 0)], Registre[REG_NUM(nOpCode, 3)]);
	return 2;
}

int DES_MUL_REG(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "mul %s, %s", Registre[REG_NUM(nOpCode, 0)], Registre[REG_NUM(nOpCode, 3)]);
	return 2;
}

int DES_BIC(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "bic %s, %s", Registre[REG_NUM(nOpCode, 0)], Registre[REG_NUM(nOpCode, 3)]);
	return 2;
}

int DES_MVN(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "mvn %s, %s", Registre[REG_NUM(nOpCode, 0)], Registre[REG_NUM(nOpCode, 3)]);
	return 2;
}

int DES_ADD_SPE(UINT adr, UINT nOpCode, char * txt)
{
	BYTE Rd = (nOpCode&7) | ((nOpCode>>4)&8);
	sprintf(txt, "add %s, %s", Registre[Rd], Registre[REG_POS(nOpCode, 3)]);
	return 2;
}

int DES_CMP_SPE(UINT adr, UINT nOpCode, char * txt)
{
	BYTE Rd = (nOpCode&7) | ((nOpCode>>4)&8);
	sprintf(txt, "cmp %s, %s", Registre[Rd], Registre[REG_POS(nOpCode, 3)]);
	return 2;
}

int DES_MOV_SPE(UINT adr, UINT nOpCode, char * txt)
{
	BYTE Rd = (nOpCode&7) | ((nOpCode>>4)&8);
	if( Rd == 8 && Rd == REG_POS(nOpCode, 3) )
		strcpy(txt, "nop");
	else
		sprintf(txt, "mov %s, %s", Registre[Rd], Registre[REG_POS(nOpCode, 3)]);
	return 2;
}

int DES_BX_THUMB(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "bx %s", Registre[REG_POS(nOpCode, 3)]);
	return 2;
}

int DES_BLX_THUMB(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "blx %s", Registre[REG_POS(nOpCode, 3)]);
	return 2;
}

int DES_LDR_PCREL(UINT adr, UINT nOpCode, char * txt)
{
	//sprintf(txt, "ldr %s, [pc, %Xh]", Registre[REG_NUM(nOpCode, 8)], (nOpCode&0xFF)<<2);
	UINT nVal = (nOpCode&0xFF)<<2;
	sprintf(txt, "ldr %s, [pc, %Xh]", Registre[REG_NUM(nOpCode, 8)], nVal);
	if(!(adr&3)) nVal+=2;
	//nVal+=2;
	return 2 | nVal<<3 | 0x80000000;
}

int DES_STR_REG_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "str %s, [%s, %s]", Registre[REG_NUM(nOpCode, 0)], Registre[REG_NUM(nOpCode, 3)], Registre[REG_NUM(nOpCode, 6)]);
	return 2;
}

int DES_STRH_REG_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "strh %s, [%s, %s]", Registre[REG_NUM(nOpCode, 0)], Registre[REG_NUM(nOpCode, 3)], Registre[REG_NUM(nOpCode, 6)]);
	return 2;
}

int DES_STRB_REG_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "strb %s, [%s, %s]", Registre[REG_NUM(nOpCode, 0)], Registre[REG_NUM(nOpCode, 3)], Registre[REG_NUM(nOpCode, 6)]);
	return 2;
}

int DES_LDRSB_REG_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldrsb %s, [%s, %s]", Registre[REG_NUM(nOpCode, 0)], Registre[REG_NUM(nOpCode, 3)], Registre[REG_NUM(nOpCode, 6)]);
	return 2;
}

int DES_LDR_REG_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldr %s, [%s, %s]", Registre[REG_NUM(nOpCode, 0)], Registre[REG_NUM(nOpCode, 3)], Registre[REG_NUM(nOpCode, 6)]);
	return 2;
}

int DES_LDRH_REG_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldrh %s, [%s, %s]", Registre[REG_NUM(nOpCode, 0)], Registre[REG_NUM(nOpCode, 3)], Registre[REG_NUM(nOpCode, 6)]);
	return 2;
}

int DES_LDRB_REG_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldrb %s, [%s, %s]", Registre[REG_NUM(nOpCode, 0)], Registre[REG_NUM(nOpCode, 3)], Registre[REG_NUM(nOpCode, 6)]);
	return 2;
}

int DES_LDRSH_REG_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldrsh %s, [%s, %s]", Registre[REG_NUM(nOpCode, 0)], Registre[REG_NUM(nOpCode, 3)], Registre[REG_NUM(nOpCode, 6)]);
	return 2;
}

int DES_STR_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "str %s, [%s, %Xh]", Registre[REG_NUM(nOpCode, 0)], Registre[REG_NUM(nOpCode, 3)], ((nOpCode>>4)&0x7C));
	return 2;
}

int DES_LDR_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldr %s, [%s, %Xh]", Registre[REG_NUM(nOpCode, 0)], Registre[REG_NUM(nOpCode, 3)], ((nOpCode>>4)&0x7C));
	return 2;
}

int DES_STRB_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "strb %s, [%s, %Xh]", Registre[REG_NUM(nOpCode, 0)], Registre[REG_NUM(nOpCode, 3)], ((nOpCode>>6)&0x1F));
	return 2;
}

int DES_LDRB_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldrb %s, [%s, %Xh]", Registre[REG_NUM(nOpCode, 0)], Registre[REG_NUM(nOpCode, 3)], ((nOpCode>>6)&0x1F));
	return 2;
}

int DES_STRH_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "strh %s, [%s, %Xh]", Registre[REG_NUM(nOpCode, 0)], Registre[REG_NUM(nOpCode, 3)], ((nOpCode>>5)&0x3E));
	return 2;
}

int DES_LDRH_IMM_OFF(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldrh %s, [%s, %Xh]", Registre[REG_NUM(nOpCode, 0)], Registre[REG_NUM(nOpCode, 3)], ((nOpCode>>5)&0x3E));
	return 2;
}

int DES_STR_SPREL(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "str %s, [sp, %Xh]", Registre[REG_NUM(nOpCode, 8)], (nOpCode&0xFF)<<2);
	return 2;
}

int DES_LDR_SPREL(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "ldr %s, [sp, %Xh]", Registre[REG_NUM(nOpCode, 8)], (nOpCode&0xFF)<<2);
	return 2;
}

int DES_ADD_2PC(UINT adr, UINT nOpCode, char * txt)
{
	//sprintf(txt, "add %s=%08Xh", Registre[REG_NUM(nOpCode, 8)], adr+2+((nOpCode&0xFF)<<2));
	UINT nVal = ((nOpCode&0xFF)<<2);
	if(!(adr&3))
		adr += 2;
	sprintf(txt, "add %s, [pc, %Xh]    (=%08Xh)", Registre[REG_NUM(nOpCode, 8)], nVal, adr+2+nVal);

	return 2;
}

int DES_ADD_2SP(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "add %s, sp, %Xh", Registre[REG_NUM(nOpCode, 8)], (nOpCode&0xFF)<<2);
	return 2;
}

int DES_ADJUST_P_SP(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "add sp, %Xh", (nOpCode&0x7F)<<2);
	return 2;
}

int DES_ADJUST_M_SP(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "sub sp, %Xh", (nOpCode&0x7F)<<2);
	return 2;
}

int DES_PUSH(UINT adr, UINT nOpCode, char * txt)
{
	RegList(8);
	sprintf(txt, "push %s", g_szBufferA);
	return 2;
}

int DES_PUSH_LR(UINT adr, UINT nOpCode, char * txt)
{
	RegList(8);
	sprintf(txt, "push %s, lr", g_szBufferA);
	return 2;
}

int DES_POP(UINT adr, UINT nOpCode, char * txt)
{
	RegList(8);
	sprintf(txt, "pop %s", g_szBufferA);
	return 2;
}

int DES_POP_PC(UINT adr, UINT nOpCode, char * txt)
{
	RegList(8);
	sprintf(txt, "pop %s, pc", g_szBufferA);
	return 2;
}

int DES_BKPT_THUMB(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "bkpt");
	return 2;
}

int DES_STMIA_THUMB(UINT adr, UINT nOpCode, char * txt)
{
	RegList(8);
	sprintf(txt, "stmia [%s]!, %s", Registre[REG_NUM(nOpCode, 8)], g_szBufferA);
	return 2;
}

int DES_LDMIA_THUMB(UINT adr, UINT nOpCode, char * txt)
{
	RegList(8);
	sprintf(txt, "ldmia [%s]!, %s", Registre[REG_NUM(nOpCode, 8)], g_szBufferA);
	return 2;
}

int DES_B_COND(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "b%s %Xh", Condition[(nOpCode>>8)&0xF], adr+(((int)((signed char)(nOpCode&0xFF)))<<1)+4);
	return 2;
}

int DES_SWI_THUMB(UINT adr, UINT nOpCode, char * txt)
{
	const char **swi= *txt==0?swi9:swi7;
	BYTE no = nOpCode&0xFF;
	sprintf(txt, "swi %02X  ;%s", no, no>0x1F?"-/-":swi[no]);
	//sprintf(txt, "swi %Xh", nOpCode & 0xFF);
	return 2;
}

#define SIGNEEXT_IMM11(nOpCode) (((nOpCode)&0x7FF) | (BIT10(nOpCode) * 0xFFFFF800))

int DES_B_UNCOND(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "b %Xh", adr+(SIGNEEXT_IMM11(nOpCode)<<1)+4);
	return 2;
}

UINT part = 0;

int DES_BLX(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "blx %Xh", (part + ((nOpCode&0x7FF)<<1))&0xFFFFFFFC);
	return 4;
}

int DES_BL_10(UINT adr, UINT nOpCode, char * txt)
{
	part = adr+4 + (SIGNEEXT_IMM11(nOpCode)<<12);
	sprintf(txt, "CALCUL LA PARTIE HAUTE DE L'ADRESSE");
	return 4;

}

int DES_BL_THUMB(UINT adr, UINT nOpCode, char * txt)
{
	sprintf(txt, "bl %Xh", (part + ((nOpCode&0x7FF)<<1))&0xFFFFFFFE);
	return 4;
} 

#define TYPE_RETOUR int 
#define PARAMETRES UINT adr, UINT nOpCode, char * txt
#define CALLTYPE 
#define NOM_TAB des_arm_instructions_set
#define NOM_THUMB_TAB des_thumb_instructions_set

TYPE_RETOUR	(CALLTYPE *NOM_TAB[4096])(PARAMETRES)={
	DES_AND_LSL_IMM,	   // 000 0000 0 0000
		DES_AND_LSL_REG,	   // 000 0000 0 0001
		DES_AND_LSR_IMM,	   // 000 0000 0 0010
		DES_AND_LSR_REG,	   // 000 0000 0 0011
		DES_AND_ASR_IMM,	   // 000 0000 0 0100
		DES_AND_ASR_REG,	   // 000 0000 0 0101
		DES_AND_ROR_IMM,	   // 000 0000 0 0110
		DES_AND_ROR_REG,	   // 000 0000 0 0111

		DES_AND_LSL_IMM,	   // 000 0000 0 1000
		DES_MUL,			   // 000 0000 0 1001
		DES_AND_LSR_IMM,	   // OOO OOOO O 1010
		DES_STRH_POS_INDE_M_REG_OFF,	// 000 0000	0 1011
		DES_AND_ASR_IMM,	   // 000 0000 0 1100
		DES_UND,			   // 000 0000 0 1101
		DES_AND_ROR_IMM,	   // 000 0000 0 1110
		DES_UND,			   // 000 0000 0 1111

		DES_AND_S_LSL_IMM,  // 000 0000 1 0000
		DES_AND_S_LSL_REG,  // 000 0000 1 0001
		DES_AND_S_LSR_IMM,  // 000 0000 1 0010
		DES_AND_S_LSR_REG,  // 000 0000 1 0011
		DES_AND_S_ASR_IMM,  // 000 0000 1 0100
		DES_AND_S_ASR_REG,  // 000 0000 1 0101
		DES_AND_S_ROR_IMM,  // 000 0000 1 0110
		DES_AND_S_ROR_REG,  // 000 0000 1 0111

		DES_AND_S_LSL_IMM,  // 000 0000 1 1000
		DES_MUL_S,		   // 000 0000 1 1001
		DES_AND_S_LSR_IMM,  // 000 0000 1 1010
		DES_LDRH_POS_INDE_M_REG_OFF,	// 000 0000	1 1011
		DES_AND_S_ASR_IMM,  // 000 0000 1 1100
		DES_LDRSB_POS_INDE_M_REG_OFF,// 000 0000	1 1101
		DES_AND_S_ROR_IMM,  // 000 0000 1 1110
		DES_LDRSH_POS_INDE_M_REG_OFF,// 000 0000	1 1111

		DES_EOR_LSL_IMM,	   // 000 0001 0 0000
		DES_EOR_LSL_REG,	   // 000 0001 0 0001
		DES_EOR_LSR_IMM,	   // 000 0001 0 0010
		DES_EOR_LSR_REG,	   // 000 0001 0 0011
		DES_EOR_ASR_IMM,	   // 000 0001 0 0100
		DES_EOR_ASR_REG,	   // 000 0001 0 0101
		DES_EOR_ROR_IMM,	   // 000 0001 0 0110
		DES_EOR_ROR_REG,	   // 000 0001 0 0111

		DES_EOR_LSL_IMM,	   // 000 0001 0 1000
		DES_MLA,			   // 000 0001 0 1001
		DES_EOR_LSR_IMM,	   // OOO OOO1 O 1010
		DES_UND,			   // 000 0001 0 1011
		DES_EOR_ASR_IMM,	   // 000 0001 0 1100
		DES_UND,			   // 000 0001 0 1101
		DES_EOR_ROR_IMM,	   // 000 0001 0 1110
		DES_UND,			   // 000 0001 0 1111

		DES_EOR_S_LSL_IMM,  // 000 0001 1 0000
		DES_EOR_S_LSL_REG,  // 000 0001 1 0001
		DES_EOR_S_LSR_IMM,  // 000 0001 1 0010
		DES_EOR_S_LSR_REG,  // 000 0001 1 0011
		DES_EOR_S_ASR_IMM,  // 000 0001 1 0100
		DES_EOR_S_ASR_REG,  // 000 0001 1 0101
		DES_EOR_S_ROR_IMM,  // 000 0001 1 0110
		DES_EOR_S_ROR_REG,  // 000 0001 1 0111

		DES_EOR_S_LSL_IMM,  // 000 0001 1 1000
		DES_MLA_S,		   // 000 0001 1 1001
		DES_EOR_S_LSR_IMM,  // 000 0001 1 1010
		DES_UND,			   // 000 0001 1 1011
		DES_EOR_S_ASR_IMM,  // 000 0001 1 1100
		DES_UND,			   // 000 0001 1 1101
		DES_EOR_S_ROR_IMM,  // 000 0001 1 1110
		DES_UND,			   // 000 0001 1 1111

		DES_SUB_LSL_IMM,	   // 000 0010 0 0000
		DES_SUB_LSL_REG,	   // 000 0010 0 0001
		DES_SUB_LSR_IMM,	   // 000 0010 0 0010
		DES_SUB_LSR_REG,	   // 000 0010 0 0011
		DES_SUB_ASR_IMM,	   // 000 0010 0 0100
		DES_SUB_ASR_REG,	   // 000 0010 0 0101
		DES_SUB_ROR_IMM,	   // 000 0010 0 0110
		DES_SUB_ROR_REG,	   // 000 0010 0 0111

		DES_SUB_LSL_IMM,	   // 000 0010 0 1000
		DES_UND,			   // 000 0010 0 1001
		DES_SUB_LSR_IMM,	   // OOO OO1O O 1010
		DES_STRH_POS_INDE_M_IMM_OFF,	// 000 0010	0 1011
		DES_SUB_ASR_IMM,	   // 000 0010 0 1100
		DES_UND,			   // 000 0010 0 1101
		DES_SUB_ROR_IMM,	   // 000 0010 0 1110
		DES_UND,			   // 000 0010 0 1111

		DES_SUB_S_LSL_IMM,  // 000 0010 1 0000
		DES_SUB_S_LSL_REG,  // 000 0010 1 0001
		DES_SUB_S_LSR_IMM,  // 000 0010 1 0010
		DES_SUB_S_LSR_REG,  // 000 0010 1 0011
		DES_SUB_S_ASR_IMM,  // 000 0010 1 0100
		DES_SUB_S_ASR_REG,  // 000 0010 1 0101
		DES_SUB_S_ROR_IMM,  // 000 0010 1 0110
		DES_SUB_S_ROR_REG,  // 000 0010 1 0111

		DES_SUB_S_LSL_IMM,  // 000 0010 1 1000
		DES_UND,			   // 000 0010 1 1001
		DES_SUB_S_LSR_IMM,  // 000 0010 1 1010
		DES_LDRH_POS_INDE_M_IMM_OFF,	// 000 0010	1 1011
		DES_SUB_S_ASR_IMM,  // 000 0010 1 1100
		DES_LDRSB_POS_INDE_M_IMM_OFF,// 000 0010	1 1101
		DES_SUB_S_ROR_IMM,  // 000 0010 1 1110
		DES_LDRSH_POS_INDE_M_IMM_OFF,// 000 0010	1 1111
		//--------------------	   
		DES_RSB_LSL_IMM,	   // 000 0011 0 0000
		DES_RSB_LSL_REG,	   // 000 0011 0 0001
		DES_RSB_LSR_IMM,	   // 000 0011 0 0010
		DES_RSB_LSR_REG,	   // 000 0011 0 0011
		DES_RSB_ASR_IMM,	   // 000 0011 0 0100
		DES_RSB_ASR_REG,	   // 000 0011 0 0101
		DES_RSB_ROR_IMM,	   // 000 0011 0 0110
		DES_RSB_ROR_REG,	   // 000 0011 0 0111

		DES_RSB_LSL_IMM,	   // 000 0011 0 1000
		DES_UND,			   // 000 0011 0 1001
		DES_RSB_LSR_IMM,	   // OOO OO11 O 1010
		DES_UND,			   // 000 0011 0 1011
		DES_RSB_ASR_IMM,	   // 000 0011 0 1100
		DES_UND,			   // 000 0011 0 1101
		DES_RSB_ROR_IMM,	   // 000 0011 0 1110
		DES_UND,			   // 000 0011 0 1111

		DES_RSB_S_LSL_IMM,  // 000 0011 1 0000
		DES_RSB_S_LSL_REG,  // 000 0011 1 0001
		DES_RSB_S_LSR_IMM,  // 000 0011 1 0010
		DES_RSB_S_LSR_REG,  // 000 0011 1 0011
		DES_RSB_S_ASR_IMM,  // 000 0011 1 0100
		DES_RSB_S_ASR_REG,  // 000 0011 1 0101
		DES_RSB_S_ROR_IMM,  // 000 0011 1 0110
		DES_RSB_S_ROR_REG,  // 000 0011 1 0111

		DES_RSB_S_LSL_IMM,  // 000 0011 1 1000
		DES_UND,			   // 000 0011 1 1001
		DES_RSB_S_LSR_IMM,  // 000 0011 1 1010
		DES_UND,			   // 000 0011 1 1011
		DES_RSB_S_ASR_IMM,  // 000 0011 1 1100
		DES_UND,			   // 000 0011 1 1101
		DES_RSB_S_ROR_IMM,  // 000 0011 1 1110
		DES_UND,			   // 000 0011 1 1111
		//--------------------------
		DES_ADD_LSL_IMM,	   // 000 0100 0 0000
		DES_ADD_LSL_REG,	   // 000 0100 0 0001
		DES_ADD_LSR_IMM,	   // 000 0100 0 0010
		DES_ADD_LSR_REG,	   // 000 0100 0 0011
		DES_ADD_ASR_IMM,	   // 000 0100 0 0100
		DES_ADD_ASR_REG,	   // 000 0100 0 0101
		DES_ADD_ROR_IMM,	   // 000 0100 0 0110
		DES_ADD_ROR_REG,	   // 000 0100 0 0111

		DES_ADD_LSL_IMM,	   // 000 0100 0 1000
		DES_UMULL,		   // 000 0100 0 1001
		DES_ADD_LSR_IMM,	   // OOO O10O O 1010
		DES_STRH_POS_INDE_P_REG_OFF,	// 000 0100	0 1011
		DES_ADD_ASR_IMM,	   // 000 0100 0 1100
		DES_UND,			   // 000 0100 0 1101
		DES_ADD_ROR_IMM,	   // 000 0100 0 1110
		DES_UND,			   // 000 0100 0 1111

		DES_ADD_S_LSL_IMM,  // 000 0100 1 0000
		DES_ADD_S_LSL_REG,  // 000 0100 1 0001
		DES_ADD_S_LSR_IMM,  // 000 0100 1 0010
		DES_ADD_S_LSR_REG,  // 000 0100 1 0011
		DES_ADD_S_ASR_IMM,  // 000 0100 1 0100
		DES_ADD_S_ASR_REG,  // 000 0100 1 0101
		DES_ADD_S_ROR_IMM,  // 000 0100 1 0110
		DES_ADD_S_ROR_REG,  // 000 0100 1 0111

		DES_ADD_S_LSL_IMM,  // 000 0100 1 1000
		DES_UMULL_S,		   // 000 0100 1 1001
		DES_ADD_S_LSR_IMM,  // 000 0100 1 1010
		DES_LDRH_POS_INDE_P_REG_OFF,	// 000 0100	1 1011
		DES_ADD_S_ASR_IMM,  // 000 0100 1 1100
		DES_LDRSB_POS_INDE_P_REG_OFF,// 000 0100	1 1101
		DES_ADD_S_ROR_IMM,  // 000 0100 1 1110
		DES_LDRSH_POS_INDE_P_REG_OFF,// 000 0100	1 1111
		//-----------------------------------------
		DES_ADC_LSL_IMM,	   // 000 0101 0 0000
		DES_ADC_LSL_REG,	   // 000 0101 0 0001
		DES_ADC_LSR_IMM,	   // 000 0101 0 0010
		DES_ADC_LSR_REG,	   // 000 0101 0 0011
		DES_ADC_ASR_IMM,	   // 000 0101 0 0100
		DES_ADC_ASR_REG,	   // 000 0101 0 0101
		DES_ADC_ROR_IMM,	   // 000 0101 0 0110
		DES_ADC_ROR_REG,	   // 000 0101 0 0111

		DES_ADC_LSL_IMM,	   // 000 0101 0 1000
		DES_UMLAL,		   // 000 0101 0 1001
		DES_ADC_LSR_IMM,	   // OOO O101 O 1010
		DES_UND,			   // 000 0101 0 1011
		DES_ADC_ASR_IMM,	   // 000 0101 0 1100
		DES_UND,			   // 000 0101 0 1101
		DES_ADC_ROR_IMM,	   // 000 0101 0 1110
		DES_UND,			   // 000 0101 0 1111

		DES_ADC_S_LSL_IMM,  // 000 0101 1 0000
		DES_ADC_S_LSL_REG,  // 000 0101 1 0001
		DES_ADC_S_LSR_IMM,  // 000 0101 1 0010
		DES_ADC_S_LSR_REG,  // 000 0101 1 0011
		DES_ADC_S_ASR_IMM,  // 000 0101 1 0100
		DES_ADC_S_ASR_REG,  // 000 0101 1 0101
		DES_ADC_S_ROR_IMM,  // 000 0101 1 0110
		DES_ADC_S_ROR_REG,  // 000 0101 1 0111

		DES_ADC_S_LSL_IMM,  // 000 0101 1 1000
		DES_UMLAL_S,		   // 000 0101 1 1001
		DES_ADC_S_LSR_IMM,  // 000 0101 1 1010
		DES_UND,			   // 000 0101 1 1011
		DES_ADC_S_ASR_IMM,  // 000 0101 1 1100
		DES_UND,			   // 000 0101 1 1101
		DES_ADC_S_ROR_IMM,  // 000 0101 1 1110
		DES_UND,			   // 000 0101 1 1111
		//------------------------------------------
		DES_SBC_LSL_IMM,	   // 000 0110 0 0000
		DES_SBC_LSL_REG,	   // 000 0110 0 0001
		DES_SBC_LSR_IMM,	   // 000 0110 0 0010
		DES_SBC_LSR_REG,	   // 000 0110 0 0011
		DES_SBC_ASR_IMM,	   // 000 0110 0 0100
		DES_SBC_ASR_REG,	   // 000 0110 0 0101
		DES_SBC_ROR_IMM,	   // 000 0110 0 0110
		DES_SBC_ROR_REG,	   // 000 0110 0 0111

		DES_SBC_LSL_IMM,	   // 000 0110 0 1000
		DES_SMULL,		   // 000 0110 0 1001
		DES_SBC_LSR_IMM,	   // OOO O11O O 1010
		DES_STRH_POS_INDE_P_IMM_OFF,	// 000 0110	0 1011
		DES_SBC_ASR_IMM,	   // 000 0110 0 1100
		DES_UND,			   // 000 0110 0 1101
		DES_SBC_ROR_IMM,	   // 000 0110 0 1110
		DES_UND,			   // 000 0110 0 1111

		DES_SBC_S_LSL_IMM,  // 000 0110 1 0000
		DES_SBC_S_LSL_REG,  // 000 0110 1 0001
		DES_SBC_S_LSR_IMM,  // 000 0110 1 0010
		DES_SBC_S_LSR_REG,  // 000 0110 1 0011
		DES_SBC_S_ASR_IMM,  // 000 0110 1 0100
		DES_SBC_S_ASR_REG,  // 000 0110 1 0101
		DES_SBC_S_ROR_IMM,  // 000 0110 1 0110
		DES_SBC_S_ROR_REG,  // 000 0110 1 0111

		DES_SBC_S_LSL_IMM,  // 000 0110 1 1000
		DES_SMULL_S,		   // 000 0110 1 1001
		DES_SBC_S_LSR_IMM,  // 000 0110 1 1010
		DES_LDRH_POS_INDE_P_IMM_OFF,	// 000 0110	1 1011
		DES_SBC_S_ASR_IMM,  // 000 0110 1 1100
		DES_LDRSB_POS_INDE_P_IMM_OFF,// 000 0110	1 1101
		DES_SBC_S_ROR_IMM,  // 000 0110 1 1110
		DES_LDRSH_POS_INDE_P_IMM_OFF,// 000 0110	1 1111
		//------------------------------------------
		DES_RSC_LSL_IMM,	   // 000 0111 0 0000
		DES_RSC_LSL_REG,	   // 000 0111 0 0001
		DES_RSC_LSR_IMM,	   // 000 0111 0 0010
		DES_RSC_LSR_REG,	   // 000 0111 0 0011
		DES_RSC_ASR_IMM,	   // 000 0111 0 0100
		DES_RSC_ASR_REG,	   // 000 0111 0 0101
		DES_RSC_ROR_IMM,	   // 000 0111 0 0110
		DES_RSC_ROR_REG,	   // 000 0111 0 0111

		DES_RSC_LSL_IMM,	   // 000 0111 0 1000
		DES_SMLAL,		   // 000 0111 0 1001
		DES_RSC_LSR_IMM,	   // OOO O111 O 1010
		DES_UND,			   // 000 0111 0 1011
		DES_RSC_ASR_IMM,	   // 000 0111 0 1100
		DES_UND,			   // 000 0111 0 1101
		DES_RSC_ROR_IMM,	   // 000 0111 0 1110
		DES_UND,			   // 000 0111 0 1111

		DES_RSC_S_LSL_IMM,  // 000 0111 1 0000
		DES_RSC_S_LSL_REG,  // 000 0111 1 0001
		DES_RSC_S_LSR_IMM,  // 000 0111 1 0010
		DES_RSC_S_LSR_REG,  // 000 0111 1 0011
		DES_RSC_S_ASR_IMM,  // 000 0111 1 0100
		DES_RSC_S_ASR_REG,  // 000 0111 1 0101
		DES_RSC_S_ROR_IMM,  // 000 0111 1 0110
		DES_RSC_S_ROR_REG,  // 000 0111 1 0111

		DES_RSC_S_LSL_IMM,  // 000 0111 1 1000
		DES_SMLAL_S,		   // 000 0111 1 1001
		DES_RSC_S_LSR_IMM,  // 000 0111 1 1010
		DES_UND,			   // 000 0111 1 1011
		DES_RSC_S_ASR_IMM,  // 000 0111 1 1100
		DES_UND,			   // 000 0111 1 1101
		DES_RSC_S_ROR_IMM,  // 000 0111 1 1110
		DES_UND,			   // 000 0111 1 1111
		//------------------------------------------
		DES_MRS_CPSR,	   // 000 1000 0 0000
		DES_UND,			   // 000 1000 0 0001
		DES_UND,			   // 000 1000 0 0010
		DES_UND,			   // 000 1000 0 0011
		DES_UND,			   // 000 1000 0 0100
		DES_QADD,		   // 000 1000 0 0101
		DES_UND,			   // 000 1000 0 0110
		DES_UND,			   // 000 1000 0 0111		  

		DES_SMLA_B_B,	   // 000 1000 0 1000
		DES_SWP,			   // 000 1000 0 1001
		DES_SMLA_T_B,	   // 000 1000 0 1010
		DES_STRH_M_REG_OFF, // 000 1000 0 1011
		DES_SMLA_B_T,	   // 000 1000 0 1100
		DES_UND,			   // 000 1000 0 1101
		DES_SMLA_T_T,	   // 000 1000 0 1110
		DES_UND,			   // 000 1000 0 1111

		DES_TST_LSL_IMM,	   // 000 1000 1 0000
		DES_TST_LSL_REG,	   // 000 1000 1 0001
		DES_TST_LSR_IMM,	   // 000 1000 1 0010
		DES_TST_LSR_REG,	   // 000 1000 1 0011
		DES_TST_ASR_IMM,	   // 000 1000 1 0100
		DES_TST_ASR_REG,	   // 000 1000 1 0101
		DES_TST_ROR_IMM,	   // 000 1000 1 0110
		DES_TST_ROR_REG,	   // 000 1000 1 0111

		DES_TST_LSL_IMM,	   // 000 1000 1 1000
		DES_UND,			   // 000 1000 1 1001
		DES_TST_LSR_IMM,	   // OOO 100O 1 1010
		DES_LDRH_M_REG_OFF, // 000 1000 1 1011
		DES_TST_ASR_IMM,	   // 000 1000 1 1100
		DES_LDRSB_M_REG_OFF,// 000 1000 1 1101
		DES_TST_ROR_IMM,	   // 000 1000 1 1110
		DES_LDRSH_M_REG_OFF,// 000 1000 1 1111
		//------------------------------------------
		DES_MSR_CPSR,	   // 000 1001 0 0000
		DES_BX,			   // 000 1001 0 0001
		DES_UND,			   // 000 1001 0 0010
		DES_BLX_REG,		   // 000 1001 0 0011
		DES_UND,			   // 000 1001 0 0100
		DES_QSUB,		   // 000 1001 0 0101
		DES_UND,			   // 000 1001 0 0110
		DES_BKPT,			// 000 1001	0 0111	

		DES_SMLAW_B,		 //	000	1001 0 1000
		DES_UND,			 //	000	1001 0 1001
		DES_SMULW_B,			 //	000	1001 0 1010
		DES_STRH_PRE_INDE_M_REG_OFF,	 //	000	1001 0 1011
		DES_SMLAW_T,		 //	000	1001 0 1100
		DES_UND,	 //	000	1001 0 1101
		DES_SMULW_T,	 //	000	1001 0 1110
		DES_UND,	 //	000	1001 0 1111

		DES_TEQ_LSL_IMM,	   // 000 1001 1 0000
		DES_TEQ_LSL_REG,	   // 000 1001 1 0001
		DES_TEQ_LSR_IMM,	   // 000 1001 1 0010
		DES_TEQ_LSR_REG,	   // 000 1001 1 0011
		DES_TEQ_ASR_IMM,	   // 000 1001 1 0100
		DES_TEQ_ASR_REG,	   // 000 1001 1 0101
		DES_TEQ_ROR_IMM,	   // 000 1001 1 0110
		DES_TEQ_ROR_REG,	   // 000 1001 1 0111

		DES_TEQ_LSL_IMM,	   // 000 1001 1 1000
		DES_UND,			   // 000 1001 1 1001
		DES_TEQ_LSR_IMM,	   // OOO 1001 1 1010
		DES_LDRH_PRE_INDE_M_REG_OFF,	 //	000	1001 1 1011
		DES_TEQ_ASR_IMM,	   // 000 1001 1 1100
		DES_LDRSB_PRE_INDE_M_REG_OFF, //	000	1001 1 1101
		DES_TEQ_ROR_IMM,	   // 000 1001 1 1110
		DES_LDRSH_PRE_INDE_M_REG_OFF, //	000	1001 1 1111		
		//------------------------------------------
		DES_MRS_SPSR,	   // 000 1010 0 0000
		DES_UND,			   // 000 1010 0 0001
		DES_UND,			   // 000 1010 0 0010
		DES_UND,			   // 000 1010 0 0011
		DES_UND,			   // 000 1010 0 0100
		DES_QDADD,		   // 000 1010 0 0101
		DES_UND,			   // 000 1010 0 0110
		DES_UND,			   // 000 1010 0 0111

		DES_SMLAL_B_B,	   // 000 1010 0 1000
		DES_SWPB,		   // 000 1010 0 1001
		DES_SMLAL_T_B,	   // 000 1010 0 1010
		DES_STRH_M_IMM_OFF, // 000 1010 0 1011
		DES_SMLAL_B_T,	   // 000 1010 0 1100
		DES_UND,			   // 000 1010 0 1101
		DES_SMLAL_T_T,	   // 000 1010 0 1110
		DES_UND,			   // 000 1010 0 1111

		DES_CMP_LSL_IMM,	   // 000 1010 1 0000
		DES_CMP_LSL_REG,	   // 000 1010 1 0001
		DES_CMP_LSR_IMM,	   // 000 1010 1 0010
		DES_CMP_LSR_REG,	   // 000 1010 1 0011
		DES_CMP_ASR_IMM,	   // 000 1010 1 0100
		DES_CMP_ASR_REG,	   // 000 1010 1 0101
		DES_CMP_ROR_IMM,	   // 000 1010 1 0110
		DES_CMP_ROR_REG,	   // 000 1010 1 0111

		DES_CMP_LSL_IMM,	   // 000 1010 1 1000
		DES_UND,			   // 000 1010 1 1001
		DES_CMP_LSR_IMM,	   // OOO 1O1O 1 1010
		DES_LDRH_M_IMM_OFF, // 000 1010 1 1011
		DES_CMP_ASR_IMM,	   // 000 1010 1 1100
		DES_LDRSB_M_IMM_OFF,// 000 1010 1 1101
		DES_CMP_ROR_IMM,	   // 000 1010 1 1110
		DES_LDRSH_M_IMM_OFF,// 000 1010 1 1111
		//------------------------------------------
		DES_MSR_SPSR,	   // 000 1011 0 0000
		DES_CLZ,			   // 000 1011 0 0001
		DES_UND,			   // 000 1011 0 0010
		DES_UND,			   // 000 1011 0 0011
		DES_UND,			   // 000 1011 0 0100
		DES_QDSUB,		   // 000 1011 0 0101
		DES_UND,			   // 000 1011 0 0110
		DES_UND,			   // 000 1011 0 0111

		DES_SMUL_B_B,	   // 000 1011 0 1000
		DES_UND,			   // 000 1011 0 1001
		DES_SMUL_T_B,	   // 000 1011 0 1010
		DES_STRH_PRE_INDE_M_IMM_OFF,	 //	000	1011 0 1011
		DES_SMUL_B_T,	   // 000 1011 0 1100
		DES_UND,			   // 000 1011 0 1101
		DES_SMUL_T_T,	   // 000 1011 0 1110
		DES_UND,			   // 000 1011 0 1111

		DES_CMN_LSL_IMM,	   // 000 1011 1 0000
		DES_CMN_LSL_REG,	   // 000 1011 1 0001
		DES_CMN_LSR_IMM,	   // 000 1011 1 0010
		DES_CMN_LSR_REG,	   // 000 1011 1 0011
		DES_CMN_ASR_IMM,	   // 000 1011 1 0100
		DES_CMN_ASR_REG,	   // 000 1011 1 0101
		DES_CMN_ROR_IMM,	   // 000 1011 1 0110
		DES_CMN_ROR_REG,	   // 000 1011 1 0111

		DES_CMN_LSL_IMM,	   // 000 1011 1 1000
		DES_UND,			   // 000 1011 1 1001
		DES_CMN_LSR_IMM,	   // OOO 1O11 1 1010
		DES_LDRH_PRE_INDE_M_IMM_OFF,	 //	000	1011 1 1011
		DES_CMN_ASR_IMM,	   // 000 1011 1 1100
		DES_LDRSB_PRE_INDE_M_IMM_OFF, //	000	1011 1 1101
		DES_CMN_ROR_IMM,	   // 000 1011 1 1110
		DES_LDRSH_PRE_INDE_M_IMM_OFF, //	000	1011 1 1111
		//------------------------------------------
		DES_ORR_LSL_IMM,	   // 000 1100 0 0000
		DES_ORR_LSL_REG,	   // 000 1100 0 0001
		DES_ORR_LSR_IMM,	   // 000 1100 0 0010
		DES_ORR_LSR_REG,	   // 000 1100 0 0011
		DES_ORR_ASR_IMM,	   // 000 1100 0 0100
		DES_ORR_ASR_REG,	   // 000 1100 0 0101
		DES_ORR_ROR_IMM,	   // 000 1100 0 0110
		DES_ORR_ROR_REG,	   // 000 1100 0 0111

		DES_ORR_LSL_IMM,	   // 000 1100 0 1000
		DES_UND,			   // 000 1100 0 1001
		DES_ORR_LSR_IMM,	   // OOO 110O O 1010
		DES_STRH_P_REG_OFF, // 000 1100 0 1011
		DES_ORR_ASR_IMM,	   // 000 1100 0 1100
		DES_UND,			   // 000 1100 0 1101
		DES_ORR_ROR_IMM,	   // 000 1100 0 1110
		DES_UND,			   // 000 1100 0 1111

		DES_ORR_S_LSL_IMM,  // 000 1100 1 0000
		DES_ORR_S_LSL_REG,  // 000 1100 1 0001
		DES_ORR_S_LSR_IMM,  // 000 1100 1 0010
		DES_ORR_S_LSR_REG,  // 000 1100 1 0011
		DES_ORR_S_ASR_IMM,  // 000 1100 1 0100
		DES_ORR_S_ASR_REG,  // 000 1100 1 0101
		DES_ORR_S_ROR_IMM,  // 000 1100 1 0110
		DES_ORR_S_ROR_REG,  // 000 1100 1 0111

		DES_ORR_S_LSL_IMM,  // 000 1100 1 1000
		DES_UND,			   // 000 1100 1 1001
		DES_ORR_S_LSR_IMM,  // 000 1100 1 1010
		DES_LDRH_P_REG_OFF, // 000 1100 1 1011
		DES_ORR_S_ASR_IMM,  // 000 1100 1 1100
		DES_LDRSB_P_REG_OFF,// 000 1100 1 1101
		DES_ORR_S_ROR_IMM,  // 000 1100 1 1110
		DES_LDRSH_P_REG_OFF,// 000 1100 1 1111
		//------------------------------------------
		DES_MOV_LSL_IMM,	   // 000 1101 0 0000
		DES_MOV_LSL_REG,	   // 000 1101 0 0001
		DES_MOV_LSR_IMM,	   // 000 1101 0 0010
		DES_MOV_LSR_REG,	   // 000 1101 0 0011
		DES_MOV_ASR_IMM,	   // 000 1101 0 0100
		DES_MOV_ASR_REG,	   // 000 1101 0 0101
		DES_MOV_ROR_IMM,	   // 000 1101 0 0110
		DES_MOV_ROR_REG,	   // 000 1101 0 0111

		DES_MOV_LSL_IMM,	   // 000 1101 0 1000
		DES_UND,			   // 000 1101 0 1001
		DES_MOV_LSR_IMM,	   // OOO 1101 O 1010
		DES_STRH_PRE_INDE_P_REG_OFF,	// 000 1101	0 1011
		DES_MOV_ASR_IMM,	   // 000 1101 0 1100
		DES_UND,			   // 000 1101 0 1101
		DES_MOV_ROR_IMM,	   // 000 1101 0 1110
		DES_UND,			   // 000 1101 0 1111

		DES_MOV_S_LSL_IMM,  // 000 1101 1 0000
		DES_MOV_S_LSL_REG,  // 000 1101 1 0001
		DES_MOV_S_LSR_IMM,  // 000 1101 1 0010
		DES_MOV_S_LSR_REG,  // 000 1101 1 0011
		DES_MOV_S_ASR_IMM,  // 000 1101 1 0100
		DES_MOV_S_ASR_REG,  // 000 1101 1 0101
		DES_MOV_S_ROR_IMM,  // 000 1101 1 0110
		DES_MOV_S_ROR_REG,  // 000 1101 1 0111

		DES_MOV_S_LSL_IMM,  // 000 1101 1 1000
		DES_UND,			   // 000 1101 1 1001
		DES_MOV_S_LSR_IMM,  // 000 1101 1 1010
		DES_LDRH_PRE_INDE_P_REG_OFF,	// 000 1101	1 1011
		DES_MOV_S_ASR_IMM,  // 000 1101 1 1100
		DES_LDRSB_PRE_INDE_P_REG_OFF,// 000 1101	1 1101
		DES_MOV_S_ROR_IMM,  // 000 1101 1 1110
		DES_LDRSH_PRE_INDE_P_REG_OFF,// 000 1101	1 1111
		//------------------------------------------
		DES_BIC_LSL_IMM,	   // 000 1110 0 0000
		DES_BIC_LSL_REG,	   // 000 1110 0 0001
		DES_BIC_LSR_IMM,	   // 000 1110 0 0010
		DES_BIC_LSR_REG,	   // 000 1110 0 0011
		DES_BIC_ASR_IMM,	   // 000 1110 0 0100
		DES_BIC_ASR_REG,	   // 000 1110 0 0101
		DES_BIC_ROR_IMM,	   // 000 1110 0 0110
		DES_BIC_ROR_REG,	   // 000 1110 0 0111

		DES_BIC_LSL_IMM,	   // 000 1110 0 1000
		DES_UND,			   // 000 1110 0 1001
		DES_BIC_LSR_IMM,	   // OOO 111O O 1010
		DES_STRH_P_IMM_OFF, // 000 1110 0 1011
		DES_BIC_ASR_IMM,	   // 000 1110 0 1100
		DES_UND,			   // 000 1110 0 1101
		DES_BIC_ROR_IMM,	   // 000 1110 0 1110
		DES_UND,			   // 000 1110 0 1111

		DES_BIC_S_LSL_IMM,  // 000 1110 1 0000
		DES_BIC_S_LSL_REG,  // 000 1110 1 0001
		DES_BIC_S_LSR_IMM,  // 000 1110 1 0010
		DES_BIC_S_LSR_REG,  // 000 1110 1 0011
		DES_BIC_S_ASR_IMM,  // 000 1110 1 0100
		DES_BIC_S_ASR_REG,  // 000 1110 1 0101
		DES_BIC_S_ROR_IMM,  // 000 1110 1 0110
		DES_BIC_S_ROR_REG,  // 000 1110 1 0111

		DES_BIC_S_LSL_IMM,  // 000 1110 1 1000
		DES_UND,			   // 000 1110 1 1001
		DES_BIC_S_LSR_IMM,  // 000 1110 1 1010
		DES_LDRH_P_IMM_OFF, // 000 1110 1 1011
		DES_BIC_S_ASR_IMM,  // 000 1110 1 1100
		DES_LDRSB_P_IMM_OFF,// 000 1110 1 1101
		DES_BIC_S_ROR_IMM,  // 000 1110 1 1110
		DES_LDRSH_P_IMM_OFF,// 000 1110 1 1111
		//-------------------------------------------
		DES_MVN_LSL_IMM,	   // 000 1111 0 0000
		DES_MVN_LSL_REG,	   // 000 1111 0 0001
		DES_MVN_LSR_IMM,	   // 000 1111 0 0010
		DES_MVN_LSR_REG,	   // 000 1111 0 0011
		DES_MVN_ASR_IMM,	   // 000 1111 0 0100
		DES_MVN_ASR_REG,	   // 000 1111 0 0101
		DES_MVN_ROR_IMM,	   // 000 1111 0 0110
		DES_MVN_ROR_REG,	   // 000 1111 0 0111

		DES_MVN_LSL_IMM,	   // 000 1111 0 1000
		DES_UND,			   // 000 1111 0 1001
		DES_MVN_LSR_IMM,	   // OOO 1111 O 1010
		DES_STRH_PRE_INDE_P_IMM_OFF,	// 000 1111	0 1011
		DES_MVN_ASR_IMM,	   // 000 1111 0 1100
		DES_UND,			   // 000 1111 0 1101
		DES_MVN_ROR_IMM,	   // 000 1111 0 1110
		DES_UND,			   // 000 1111 0 1111

		DES_MVN_S_LSL_IMM,  // 000 1111 1 0000
		DES_MVN_S_LSL_REG,  // 000 1111 1 0001
		DES_MVN_S_LSR_IMM,  // 000 1111 1 0010
		DES_MVN_S_LSR_REG,  // 000 1111 1 0011
		DES_MVN_S_ASR_IMM,  // 000 1111 1 0100
		DES_MVN_S_ASR_REG,  // 000 1111 1 0101
		DES_MVN_S_ROR_IMM,  // 000 1111 1 0110
		DES_MVN_S_ROR_REG,  // 000 1111 1 0111

		DES_MVN_S_LSL_IMM,  // 000 1111 1 1000
		DES_UND,			   // 000 1111 1 1001
		DES_MVN_S_LSR_IMM,  // 000 1111 1 1010
		DES_LDRH_PRE_INDE_P_IMM_OFF,	// 000 1111	1 1011
		DES_MVN_S_ASR_IMM,  // 000 1111 1 1100
		DES_LDRSB_PRE_INDE_P_IMM_OFF,// 000 1111	1 1101
		DES_MVN_S_ROR_IMM,  // 000 1111 1 1110
		DES_LDRSH_PRE_INDE_P_IMM_OFF,// 000 1111	1 1111
		//-------------------------------------------
		DES_AND_IMM_VAL,	   // 001 0000 0 0000
		DES_AND_IMM_VAL,	   // 001 0000 0 0001
		DES_AND_IMM_VAL,	   // 001 0000 0 0010
		DES_AND_IMM_VAL,	   // 001 0000 0 0011
		DES_AND_IMM_VAL,	   // 001 0000 0 0100
		DES_AND_IMM_VAL,	   // 001 0000 0 0101
		DES_AND_IMM_VAL,	   // 001 0000 0 0110
		DES_AND_IMM_VAL,	   // 001 0000 0 0111
		DES_AND_IMM_VAL,	   // 001 0000 0 1000
		DES_AND_IMM_VAL,	   // 001 0000 0 1001
		DES_AND_IMM_VAL,	   // 001 0000 0 1010
		DES_AND_IMM_VAL,	   // 001 0000 0 1011
		DES_AND_IMM_VAL,	   // 001 0000 0 1100
		DES_AND_IMM_VAL,	   // 001 0000 0 1101
		DES_AND_IMM_VAL,	   // 001 0000 0 1110
		DES_AND_IMM_VAL,	   // 001 0000 0 1111

		DES_AND_S_IMM_VAL,  // 001 0000 1 0000
		DES_AND_S_IMM_VAL,  // 001 0000 1 0001
		DES_AND_S_IMM_VAL,  // 001 0000 1 0010
		DES_AND_S_IMM_VAL,  // 001 0000 1 0011
		DES_AND_S_IMM_VAL,  // 001 0000 1 0100
		DES_AND_S_IMM_VAL,  // 001 0000 1 0101
		DES_AND_S_IMM_VAL,  // 001 0000 1 0110
		DES_AND_S_IMM_VAL,  // 001 0000 1 0111
		DES_AND_S_IMM_VAL,  // 001 0000 1 1000
		DES_AND_S_IMM_VAL,  // 001 0000 1 1001
		DES_AND_S_IMM_VAL,  // 001 0000 1 1010
		DES_AND_S_IMM_VAL,  // 001 0000 1 1011
		DES_AND_S_IMM_VAL,  // 001 0000 1 1100
		DES_AND_S_IMM_VAL,  // 001 0000 1 1101
		DES_AND_S_IMM_VAL,  // 001 0000 1 1110
		DES_AND_S_IMM_VAL,  // 001 0000 1 1111
		//------------------------------------------
		DES_EOR_IMM_VAL,	   // 001 0001 0 0000
		DES_EOR_IMM_VAL,	   // 001 0001 0 0001
		DES_EOR_IMM_VAL,	   // 001 0001 0 0010
		DES_EOR_IMM_VAL,	   // 001 0001 0 0011
		DES_EOR_IMM_VAL,	   // 001 0001 0 0100
		DES_EOR_IMM_VAL,	   // 001 0001 0 0101
		DES_EOR_IMM_VAL,	   // 001 0001 0 0110
		DES_EOR_IMM_VAL,	   // 001 0001 0 0111
		DES_EOR_IMM_VAL,	   // 001 0001 0 1000
		DES_EOR_IMM_VAL,	   // 001 0001 0 1001
		DES_EOR_IMM_VAL,	   // 001 0001 0 1010
		DES_EOR_IMM_VAL,	   // 001 0001 0 1011
		DES_EOR_IMM_VAL,	   // 001 0001 0 1100
		DES_EOR_IMM_VAL,	   // 001 0001 0 1101
		DES_EOR_IMM_VAL,	   // 001 0001 0 1110
		DES_EOR_IMM_VAL,	   // 001 0001 0 1111

		DES_EOR_S_IMM_VAL,  // 001 0001 1 0000
		DES_EOR_S_IMM_VAL,  // 001 0001 1 0001
		DES_EOR_S_IMM_VAL,  // 001 0001 1 0010
		DES_EOR_S_IMM_VAL,  // 001 0001 1 0011
		DES_EOR_S_IMM_VAL,  // 001 0001 1 0100
		DES_EOR_S_IMM_VAL,  // 001 0001 1 0101
		DES_EOR_S_IMM_VAL,  // 001 0001 1 0110
		DES_EOR_S_IMM_VAL,  // 001 0001 1 0111
		DES_EOR_S_IMM_VAL,  // 001 0001 1 1000
		DES_EOR_S_IMM_VAL,  // 001 0001 1 1001
		DES_EOR_S_IMM_VAL,  // 001 0001 1 1010
		DES_EOR_S_IMM_VAL,  // 001 0001 1 1011
		DES_EOR_S_IMM_VAL,  // 001 0001 1 1100
		DES_EOR_S_IMM_VAL,  // 001 0001 1 1101
		DES_EOR_S_IMM_VAL,  // 001 0001 1 1110
		DES_EOR_S_IMM_VAL,  // 001 0001 1 1111
		//------------------------------------------
		DES_SUB_IMM_VAL,	   // 001 0010 0 0000
		DES_SUB_IMM_VAL,	   // 001 0010 0 0001
		DES_SUB_IMM_VAL,	   // 001 0010 0 0010
		DES_SUB_IMM_VAL,	   // 001 0010 0 0011
		DES_SUB_IMM_VAL,	   // 001 0010 0 0100
		DES_SUB_IMM_VAL,	   // 001 0010 0 0101
		DES_SUB_IMM_VAL,	   // 001 0010 0 0110
		DES_SUB_IMM_VAL,	   // 001 0010 0 0111
		DES_SUB_IMM_VAL,	   // 001 0010 0 1000
		DES_SUB_IMM_VAL,	   // 001 0010 0 1001
		DES_SUB_IMM_VAL,	   // 001 0010 0 1010
		DES_SUB_IMM_VAL,	   // 001 0010 0 1011
		DES_SUB_IMM_VAL,	   // 001 0010 0 1100
		DES_SUB_IMM_VAL,	   // 001 0010 0 1101
		DES_SUB_IMM_VAL,	   // 001 0010 0 1110
		DES_SUB_IMM_VAL,	   // 001 0010 0 1111

		DES_SUB_S_IMM_VAL,  // 001 0010 1 0000
		DES_SUB_S_IMM_VAL,  // 001 0010 1 0001
		DES_SUB_S_IMM_VAL,  // 001 0010 1 0010
		DES_SUB_S_IMM_VAL,  // 001 0010 1 0011
		DES_SUB_S_IMM_VAL,  // 001 0010 1 0100
		DES_SUB_S_IMM_VAL,  // 001 0010 1 0101
		DES_SUB_S_IMM_VAL,  // 001 0010 1 0110
		DES_SUB_S_IMM_VAL,  // 001 0010 1 0111
		DES_SUB_S_IMM_VAL,  // 001 0010 1 1000
		DES_SUB_S_IMM_VAL,  // 001 0010 1 1001
		DES_SUB_S_IMM_VAL,  // 001 0010 1 1010
		DES_SUB_S_IMM_VAL,  // 001 0010 1 1011
		DES_SUB_S_IMM_VAL,  // 001 0010 1 1100
		DES_SUB_S_IMM_VAL,  // 001 0010 1 1101
		DES_SUB_S_IMM_VAL,  // 001 0010 1 1110
		DES_SUB_S_IMM_VAL,  // 001 0010 1 1111
		//------------------------------------------
		DES_RSB_IMM_VAL,	   // 001 0011 0 0000
		DES_RSB_IMM_VAL,	   // 001 0011 0 0001
		DES_RSB_IMM_VAL,	   // 001 0011 0 0010
		DES_RSB_IMM_VAL,	   // 001 0011 0 0011
		DES_RSB_IMM_VAL,	   // 001 0011 0 0100
		DES_RSB_IMM_VAL,	   // 001 0011 0 0101
		DES_RSB_IMM_VAL,	   // 001 0011 0 0110
		DES_RSB_IMM_VAL,	   // 001 0011 0 0111
		DES_RSB_IMM_VAL,	   // 001 0011 0 1000
		DES_RSB_IMM_VAL,	   // 001 0011 0 1001
		DES_RSB_IMM_VAL,	   // 001 0011 0 1010
		DES_RSB_IMM_VAL,	   // 001 0011 0 1011
		DES_RSB_IMM_VAL,	   // 001 0011 0 1100
		DES_RSB_IMM_VAL,	   // 001 0011 0 1101
		DES_RSB_IMM_VAL,	   // 001 0011 0 1110
		DES_RSB_IMM_VAL,	   // 001 0011 0 1111

		DES_RSB_S_IMM_VAL,  // 001 0011 1 0000
		DES_RSB_S_IMM_VAL,  // 001 0011 1 0001
		DES_RSB_S_IMM_VAL,  // 001 0011 1 0010
		DES_RSB_S_IMM_VAL,  // 001 0011 1 0011
		DES_RSB_S_IMM_VAL,  // 001 0011 1 0100
		DES_RSB_S_IMM_VAL,  // 001 0011 1 0101
		DES_RSB_S_IMM_VAL,  // 001 0011 1 0110
		DES_RSB_S_IMM_VAL,  // 001 0011 1 0111
		DES_RSB_S_IMM_VAL,  // 001 0011 1 1000
		DES_RSB_S_IMM_VAL,  // 001 0011 1 1001
		DES_RSB_S_IMM_VAL,  // 001 0011 1 1010
		DES_RSB_S_IMM_VAL,  // 001 0011 1 1011
		DES_RSB_S_IMM_VAL,  // 001 0011 1 1100
		DES_RSB_S_IMM_VAL,  // 001 0011 1 1101
		DES_RSB_S_IMM_VAL,  // 001 0011 1 1110
		DES_RSB_S_IMM_VAL,  // 001 0011 1 1111
		//------------------------------------------
		DES_ADD_IMM_VAL,	   // 001 0100 0 0000
		DES_ADD_IMM_VAL,	   // 001 0100 0 0001
		DES_ADD_IMM_VAL,	   // 001 0100 0 0010
		DES_ADD_IMM_VAL,	   // 001 0100 0 0011
		DES_ADD_IMM_VAL,	   // 001 0100 0 0100
		DES_ADD_IMM_VAL,	   // 001 0100 0 0101
		DES_ADD_IMM_VAL,	   // 001 0100 0 0110
		DES_ADD_IMM_VAL,	   // 001 0100 0 0111
		DES_ADD_IMM_VAL,	   // 001 0100 0 1000
		DES_ADD_IMM_VAL,	   // 001 0100 0 1001
		DES_ADD_IMM_VAL,	   // 001 0100 0 1010
		DES_ADD_IMM_VAL,	   // 001 0100 0 1011
		DES_ADD_IMM_VAL,	   // 001 0100 0 1100
		DES_ADD_IMM_VAL,	   // 001 0100 0 1101
		DES_ADD_IMM_VAL,	   // 001 0100 0 1110
		DES_ADD_IMM_VAL,	   // 001 0100 0 1111

		DES_ADD_S_IMM_VAL,  // 001 0100 1 0000
		DES_ADD_S_IMM_VAL,  // 001 0100 1 0001
		DES_ADD_S_IMM_VAL,  // 001 0100 1 0010
		DES_ADD_S_IMM_VAL,  // 001 0100 1 0011
		DES_ADD_S_IMM_VAL,  // 001 0100 1 0100
		DES_ADD_S_IMM_VAL,  // 001 0100 1 0101
		DES_ADD_S_IMM_VAL,  // 001 0100 1 0110
		DES_ADD_S_IMM_VAL,  // 001 0100 1 0111
		DES_ADD_S_IMM_VAL,  // 001 0100 1 1000
		DES_ADD_S_IMM_VAL,  // 001 0100 1 1001
		DES_ADD_S_IMM_VAL,  // 001 0100 1 1010
		DES_ADD_S_IMM_VAL,  // 001 0100 1 1011
		DES_ADD_S_IMM_VAL,  // 001 0100 1 1100
		DES_ADD_S_IMM_VAL,  // 001 0100 1 1101
		DES_ADD_S_IMM_VAL,  // 001 0100 1 1110
		DES_ADD_S_IMM_VAL,  // 001 0100 1 1111
		//------------------------------------------
		DES_ADC_IMM_VAL,	   // 001 0101 0 0000
		DES_ADC_IMM_VAL,	   // 001 0101 0 0001
		DES_ADC_IMM_VAL,	   // 001 0101 0 0010
		DES_ADC_IMM_VAL,	   // 001 0101 0 0011
		DES_ADC_IMM_VAL,	   // 001 0101 0 0100
		DES_ADC_IMM_VAL,	   // 001 0101 0 0101
		DES_ADC_IMM_VAL,	   // 001 0101 0 0110
		DES_ADC_IMM_VAL,	   // 001 0101 0 0111
		DES_ADC_IMM_VAL,	   // 001 0101 0 1000
		DES_ADC_IMM_VAL,	   // 001 0101 0 1001
		DES_ADC_IMM_VAL,	   // 001 0101 0 1010
		DES_ADC_IMM_VAL,	   // 001 0101 0 1011
		DES_ADC_IMM_VAL,	   // 001 0101 0 1100
		DES_ADC_IMM_VAL,	   // 001 0101 0 1101
		DES_ADC_IMM_VAL,	   // 001 0101 0 1110
		DES_ADC_IMM_VAL,	   // 001 0101 0 1111

		DES_ADC_S_IMM_VAL,  // 001 0101 1 0000
		DES_ADC_S_IMM_VAL,  // 001 0101 1 0001
		DES_ADC_S_IMM_VAL,  // 001 0101 1 0010
		DES_ADC_S_IMM_VAL,  // 001 0101 1 0011
		DES_ADC_S_IMM_VAL,  // 001 0101 1 0100
		DES_ADC_S_IMM_VAL,  // 001 0101 1 0101
		DES_ADC_S_IMM_VAL,  // 001 0101 1 0110
		DES_ADC_S_IMM_VAL,  // 001 0101 1 0111
		DES_ADC_S_IMM_VAL,  // 001 0101 1 1000
		DES_ADC_S_IMM_VAL,  // 001 0101 1 1001
		DES_ADC_S_IMM_VAL,  // 001 0101 1 1010
		DES_ADC_S_IMM_VAL,  // 001 0101 1 1011
		DES_ADC_S_IMM_VAL,  // 001 0101 1 1100
		DES_ADC_S_IMM_VAL,  // 001 0101 1 1101
		DES_ADC_S_IMM_VAL,  // 001 0101 1 1110
		DES_ADC_S_IMM_VAL,  // 001 0101 1 1111
		//------------------------------------------
		DES_SBC_IMM_VAL,	   // 001 0110 0 0000
		DES_SBC_IMM_VAL,	   // 001 0110 0 0001
		DES_SBC_IMM_VAL,	   // 001 0110 0 0010
		DES_SBC_IMM_VAL,	   // 001 0110 0 0011
		DES_SBC_IMM_VAL,	   // 001 0110 0 0100
		DES_SBC_IMM_VAL,	   // 001 0110 0 0101
		DES_SBC_IMM_VAL,	   // 001 0110 0 0110
		DES_SBC_IMM_VAL,	   // 001 0110 0 0111
		DES_SBC_IMM_VAL,	   // 001 0110 0 1000
		DES_SBC_IMM_VAL,	   // 001 0110 0 1001
		DES_SBC_IMM_VAL,	   // 001 0110 0 1010
		DES_SBC_IMM_VAL,	   // 001 0110 0 1011
		DES_SBC_IMM_VAL,	   // 001 0110 0 1100
		DES_SBC_IMM_VAL,	   // 001 0110 0 1101
		DES_SBC_IMM_VAL,	   // 001 0110 0 1110
		DES_SBC_IMM_VAL,	   // 001 0110 0 1111

		DES_SBC_S_IMM_VAL,  // 001 0110 1 0000
		DES_SBC_S_IMM_VAL,  // 001 0110 1 0001
		DES_SBC_S_IMM_VAL,  // 001 0110 1 0010
		DES_SBC_S_IMM_VAL,  // 001 0110 1 0011
		DES_SBC_S_IMM_VAL,  // 001 0110 1 0100
		DES_SBC_S_IMM_VAL,  // 001 0110 1 0101
		DES_SBC_S_IMM_VAL,  // 001 0110 1 0110
		DES_SBC_S_IMM_VAL,  // 001 0110 1 0111
		DES_SBC_S_IMM_VAL,  // 001 0110 1 1000
		DES_SBC_S_IMM_VAL,  // 001 0110 1 1001
		DES_SBC_S_IMM_VAL,  // 001 0110 1 1010
		DES_SBC_S_IMM_VAL,  // 001 0110 1 1011
		DES_SBC_S_IMM_VAL,  // 001 0110 1 1100
		DES_SBC_S_IMM_VAL,  // 001 0110 1 1101
		DES_SBC_S_IMM_VAL,  // 001 0110 1 1110
		DES_SBC_S_IMM_VAL,  // 001 0110 1 1111
		//------------------------------------------
		DES_RSC_IMM_VAL,	   // 001 0111 0 0000
		DES_RSC_IMM_VAL,	   // 001 0111 0 0001
		DES_RSC_IMM_VAL,	   // 001 0111 0 0010
		DES_RSC_IMM_VAL,	   // 001 0111 0 0011
		DES_RSC_IMM_VAL,	   // 001 0111 0 0100
		DES_RSC_IMM_VAL,	   // 001 0111 0 0101
		DES_RSC_IMM_VAL,	   // 001 0111 0 0110
		DES_RSC_IMM_VAL,	   // 001 0111 0 0111
		DES_RSC_IMM_VAL,	   // 001 0111 0 1000
		DES_RSC_IMM_VAL,	   // 001 0111 0 1001
		DES_RSC_IMM_VAL,	   // 001 0111 0 1010
		DES_RSC_IMM_VAL,	   // 001 0111 0 1011
		DES_RSC_IMM_VAL,	   // 001 0111 0 1100
		DES_RSC_IMM_VAL,	   // 001 0111 0 1101
		DES_RSC_IMM_VAL,	   // 001 0111 0 1110
		DES_RSC_IMM_VAL,	   // 001 0111 0 1111

		DES_RSC_S_IMM_VAL,  // 001 0111 1 0000
		DES_RSC_S_IMM_VAL,  // 001 0111 1 0001
		DES_RSC_S_IMM_VAL,  // 001 0111 1 0010
		DES_RSC_S_IMM_VAL,  // 001 0111 1 0011
		DES_RSC_S_IMM_VAL,  // 001 0111 1 0100
		DES_RSC_S_IMM_VAL,  // 001 0111 1 0101
		DES_RSC_S_IMM_VAL,  // 001 0111 1 0110
		DES_RSC_S_IMM_VAL,  // 001 0111 1 0111
		DES_RSC_S_IMM_VAL,  // 001 0111 1 1000
		DES_RSC_S_IMM_VAL,  // 001 0111 1 1001
		DES_RSC_S_IMM_VAL,  // 001 0111 1 1010
		DES_RSC_S_IMM_VAL,  // 001 0111 1 1011
		DES_RSC_S_IMM_VAL,  // 001 0111 1 1100
		DES_RSC_S_IMM_VAL,  // 001 0111 1 1101
		DES_RSC_S_IMM_VAL,  // 001 0111 1 1110
		DES_RSC_S_IMM_VAL,  // 001 0111 1 1111
		//------------------------------------------
		DES_UND,	   // 001 1000 0 0000
		DES_UND,	   // 001 1000 0 0001
		DES_UND,	   // 001 1000 0 0010
		DES_UND,	   // 001 1000 0 0011
		DES_UND,	   // 001 1000 0 0100
		DES_UND,	   // 001 1000 0 0101
		DES_UND,	   // 001 1000 0 0110
		DES_UND,	   // 001 1000 0 0111
		DES_UND,	   // 001 1000 0 1000
		DES_UND,	   // 001 1000 0 1001
		DES_UND,	   // 001 1000 0 1010
		DES_UND,	   // 001 1000 0 1011
		DES_UND,	   // 001 1000 0 1100
		DES_UND,	   // 001 1000 0 1101
		DES_UND,	   // 001 1000 0 1110
		DES_UND,	   // 001 1000 0 1111

		DES_TST_IMM_VAL,	 //	001	1000 1 0000
		DES_TST_IMM_VAL,	 //	001	1000 1 0001
		DES_TST_IMM_VAL,	 //	001	1000 1 0010
		DES_TST_IMM_VAL,	 //	001	1000 1 0011
		DES_TST_IMM_VAL,	 //	001	1000 1 0100
		DES_TST_IMM_VAL,	 //	001	1000 1 0101
		DES_TST_IMM_VAL,	 //	001	1000 1 0110
		DES_TST_IMM_VAL,	 //	001	1000 1 0111
		DES_TST_IMM_VAL,	 //	001	1000 1 1000
		DES_TST_IMM_VAL,	 //	001	1000 1 1001
		DES_TST_IMM_VAL,	 //	001	1000 1 1010
		DES_TST_IMM_VAL,	 //	001	1000 1 1011
		DES_TST_IMM_VAL,	 //	001	1000 1 1100
		DES_TST_IMM_VAL,	 //	001	1000 1 1101
		DES_TST_IMM_VAL,	 //	001	1000 1 1110
		DES_TST_IMM_VAL,	 //	001	1000 1 1111
		//------------------------------------------
		DES_MSR_CPSR_IMM_VAL,	// 001 1001	0 0000
		DES_MSR_CPSR_IMM_VAL,	// 001 1001	0 0001
		DES_MSR_CPSR_IMM_VAL,	// 001 1001	0 0010
		DES_MSR_CPSR_IMM_VAL,	// 001 1001	0 0011
		DES_MSR_CPSR_IMM_VAL,	// 001 1001	0 0100
		DES_MSR_CPSR_IMM_VAL,	// 001 1001	0 0101
		DES_MSR_CPSR_IMM_VAL,	// 001 1001	0 0110
		DES_MSR_CPSR_IMM_VAL,	// 001 1001	0 0111
		DES_MSR_CPSR_IMM_VAL,	// 001 1001	0 1000
		DES_MSR_CPSR_IMM_VAL,	// 001 1001	0 1001
		DES_MSR_CPSR_IMM_VAL,	// 001 1001	0 1010
		DES_MSR_CPSR_IMM_VAL,	// 001 1001	0 1011
		DES_MSR_CPSR_IMM_VAL,	// 001 1001	0 1100
		DES_MSR_CPSR_IMM_VAL,	// 001 1001	0 1101
		DES_MSR_CPSR_IMM_VAL,	// 001 1001	0 1110
		DES_MSR_CPSR_IMM_VAL,	// 001 1001	0 1111

		DES_TEQ_IMM_VAL,	 //	001	1001 1 0000
		DES_TEQ_IMM_VAL,	 //	001	1001 1 0001
		DES_TEQ_IMM_VAL,	 //	001	1001 1 0010
		DES_TEQ_IMM_VAL,	 //	001	1001 1 0011
		DES_TEQ_IMM_VAL,	 //	001	1001 1 0100
		DES_TEQ_IMM_VAL,	 //	001	1001 1 0101
		DES_TEQ_IMM_VAL,	 //	001	1001 1 0110
		DES_TEQ_IMM_VAL,	 //	001	1001 1 0111
		DES_TEQ_IMM_VAL,	 //	001	1001 1 1000
		DES_TEQ_IMM_VAL,	 //	001	1001 1 1001
		DES_TEQ_IMM_VAL,	 //	001	1001 1 1010
		DES_TEQ_IMM_VAL,	 //	001	1001 1 1011
		DES_TEQ_IMM_VAL,	 //	001	1001 1 1100
		DES_TEQ_IMM_VAL,	 //	001	1001 1 1101
		DES_TEQ_IMM_VAL,	 //	001	1001 1 1110
		DES_TEQ_IMM_VAL,	 //	001	1001 1 1111
		//------------------------------------------
		DES_UND,	   // 001 1010 0 0000
		DES_UND,	   // 001 1010 0 0001
		DES_UND,	   // 001 1010 0 0010
		DES_UND,	   // 001 1010 0 0011
		DES_UND,	   // 001 1010 0 0100
		DES_UND,	   // 001 1010 0 0101
		DES_UND,	   // 001 1010 0 0110
		DES_UND,	   // 001 1010 0 0111
		DES_UND,	   // 001 1010 0 1000
		DES_UND,	   // 001 1010 0 1001
		DES_UND,	   // 001 1010 0 1010
		DES_UND,	   // 001 1010 0 1011
		DES_UND,	   // 001 1010 0 1100
		DES_UND,	   // 001 1010 0 1101
		DES_UND,	   // 001 1010 0 1110
		DES_UND,	   // 001 1010 0 1111

		DES_CMP_IMM_VAL,	 //	001	1010 1 0000
		DES_CMP_IMM_VAL,	 //	001	1010 1 0001
		DES_CMP_IMM_VAL,	 //	001	1010 1 0010
		DES_CMP_IMM_VAL,	 //	001	1010 1 0011
		DES_CMP_IMM_VAL,	 //	001	1010 1 0100
		DES_CMP_IMM_VAL,	 //	001	1010 1 0101
		DES_CMP_IMM_VAL,	 //	001	1010 1 0110
		DES_CMP_IMM_VAL,	 //	001	1010 1 0111
		DES_CMP_IMM_VAL,	 //	001	1010 1 1000
		DES_CMP_IMM_VAL,	 //	001	1010 1 1001
		DES_CMP_IMM_VAL,	 //	001	1010 1 1010
		DES_CMP_IMM_VAL,	 //	001	1010 1 1011
		DES_CMP_IMM_VAL,	 //	001	1010 1 1100
		DES_CMP_IMM_VAL,	 //	001	1010 1 1101
		DES_CMP_IMM_VAL,	 //	001	1010 1 1110
		DES_CMP_IMM_VAL,	 //	001	1010 1 1111
		//------------------------------------------
		DES_MSR_SPSR_IMM_VAL,	// 001 1011	0 0000
		DES_MSR_SPSR_IMM_VAL,	// 001 1011	0 0001
		DES_MSR_SPSR_IMM_VAL,	// 001 1011	0 0010
		DES_MSR_SPSR_IMM_VAL,	// 001 1011	0 0011
		DES_MSR_SPSR_IMM_VAL,	// 001 1011	0 0100
		DES_MSR_SPSR_IMM_VAL,	// 001 1011	0 0101
		DES_MSR_SPSR_IMM_VAL,	// 001 1011	0 0110
		DES_MSR_SPSR_IMM_VAL,	// 001 1011	0 0111
		DES_MSR_SPSR_IMM_VAL,	// 001 1011	0 1000
		DES_MSR_SPSR_IMM_VAL,	// 001 1011	0 1001
		DES_MSR_SPSR_IMM_VAL,	// 001 1011	0 1010
		DES_MSR_SPSR_IMM_VAL,	// 001 1011	0 1011
		DES_MSR_SPSR_IMM_VAL,	// 001 1011	0 1100
		DES_MSR_SPSR_IMM_VAL,	// 001 1011	0 1101
		DES_MSR_SPSR_IMM_VAL,	// 001 1011	0 1110
		DES_MSR_SPSR_IMM_VAL,	// 001 1011	0 1111

		DES_CMN_IMM_VAL,	 //	001	1011 1 0000
		DES_CMN_IMM_VAL,	 //	001	1011 1 0001
		DES_CMN_IMM_VAL,	 //	001	1011 1 0010
		DES_CMN_IMM_VAL,	 //	001	1011 1 0011
		DES_CMN_IMM_VAL,	 //	001	1011 1 0100
		DES_CMN_IMM_VAL,	 //	001	1011 1 0101
		DES_CMN_IMM_VAL,	 //	001	1011 1 0110
		DES_CMN_IMM_VAL,	 //	001	1011 1 0111
		DES_CMN_IMM_VAL,	 //	001	1011 1 1000
		DES_CMN_IMM_VAL,	 //	001	1011 1 1001
		DES_CMN_IMM_VAL,	 //	001	1011 1 1010
		DES_CMN_IMM_VAL,	 //	001	1011 1 1011
		DES_CMN_IMM_VAL,	 //	001	1011 1 1100
		DES_CMN_IMM_VAL,	 //	001	1011 1 1101
		DES_CMN_IMM_VAL,	 //	001	1011 1 1110
		DES_CMN_IMM_VAL,	 //	001	1011 1 1111
		//------------------------------------------
		DES_ORR_IMM_VAL,	   // 001 1100 0 0000
		DES_ORR_IMM_VAL,	   // 001 1100 0 0001
		DES_ORR_IMM_VAL,	   // 001 1100 0 0010
		DES_ORR_IMM_VAL,	   // 001 1100 0 0011
		DES_ORR_IMM_VAL,	   // 001 1100 0 0100
		DES_ORR_IMM_VAL,	   // 001 1100 0 0101
		DES_ORR_IMM_VAL,	   // 001 1100 0 0110
		DES_ORR_IMM_VAL,	   // 001 1100 0 0111
		DES_ORR_IMM_VAL,	   // 001 1100 0 1000
		DES_ORR_IMM_VAL,	   // 001 1100 0 1001
		DES_ORR_IMM_VAL,	   // 001 1100 0 1010
		DES_ORR_IMM_VAL,	   // 001 1100 0 1011
		DES_ORR_IMM_VAL,	   // 001 1100 0 1100
		DES_ORR_IMM_VAL,	   // 001 1100 0 1101
		DES_ORR_IMM_VAL,	   // 001 1100 0 1110
		DES_ORR_IMM_VAL,	   // 001 1100 0 1111

		DES_ORR_S_IMM_VAL,  // 001 1100 1 0000
		DES_ORR_S_IMM_VAL,  // 001 1100 1 0001
		DES_ORR_S_IMM_VAL,  // 001 1100 1 0010
		DES_ORR_S_IMM_VAL,  // 001 1100 1 0011
		DES_ORR_S_IMM_VAL,  // 001 1100 1 0100
		DES_ORR_S_IMM_VAL,  // 001 1100 1 0101
		DES_ORR_S_IMM_VAL,  // 001 1100 1 0110
		DES_ORR_S_IMM_VAL,  // 001 1100 1 0111
		DES_ORR_S_IMM_VAL,  // 001 1100 1 1000
		DES_ORR_S_IMM_VAL,  // 001 1100 1 1001
		DES_ORR_S_IMM_VAL,  // 001 1100 1 1010
		DES_ORR_S_IMM_VAL,  // 001 1100 1 1011
		DES_ORR_S_IMM_VAL,  // 001 1100 1 1100
		DES_ORR_S_IMM_VAL,  // 001 1100 1 1101
		DES_ORR_S_IMM_VAL,  // 001 1100 1 1110
		DES_ORR_S_IMM_VAL,  // 001 1100 1 1111
		//------------------------------------------
		DES_MOV_IMM_VAL,	   // 001 1101 0 0000
		DES_MOV_IMM_VAL,	   // 001 1101 0 0001
		DES_MOV_IMM_VAL,	   // 001 1101 0 0010
		DES_MOV_IMM_VAL,	   // 001 1101 0 0011
		DES_MOV_IMM_VAL,	   // 001 1101 0 0100
		DES_MOV_IMM_VAL,	   // 001 1101 0 0101
		DES_MOV_IMM_VAL,	   // 001 1101 0 0110
		DES_MOV_IMM_VAL,	   // 001 1101 0 0111
		DES_MOV_IMM_VAL,	   // 001 1101 0 1000
		DES_MOV_IMM_VAL,	   // 001 1101 0 1001
		DES_MOV_IMM_VAL,	   // 001 1101 0 1010
		DES_MOV_IMM_VAL,	   // 001 1101 0 1011
		DES_MOV_IMM_VAL,	   // 001 1101 0 1100
		DES_MOV_IMM_VAL,	   // 001 1101 0 1101
		DES_MOV_IMM_VAL,	   // 001 1101 0 1110
		DES_MOV_IMM_VAL,	   // 001 1101 0 1111

		DES_MOV_S_IMM_VAL,  // 001 1101 1 0000
		DES_MOV_S_IMM_VAL,  // 001 1101 1 0001
		DES_MOV_S_IMM_VAL,  // 001 1101 1 0010
		DES_MOV_S_IMM_VAL,  // 001 1101 1 0011
		DES_MOV_S_IMM_VAL,  // 001 1101 1 0100
		DES_MOV_S_IMM_VAL,  // 001 1101 1 0101
		DES_MOV_S_IMM_VAL,  // 001 1101 1 0110
		DES_MOV_S_IMM_VAL,  // 001 1101 1 0111
		DES_MOV_S_IMM_VAL,  // 001 1101 1 1000
		DES_MOV_S_IMM_VAL,  // 001 1101 1 1001
		DES_MOV_S_IMM_VAL,  // 001 1101 1 1010
		DES_MOV_S_IMM_VAL,  // 001 1101 1 1011
		DES_MOV_S_IMM_VAL,  // 001 1101 1 1100
		DES_MOV_S_IMM_VAL,  // 001 1101 1 1101
		DES_MOV_S_IMM_VAL,  // 001 1101 1 1110
		DES_MOV_S_IMM_VAL,  // 001 1101 1 1111
		//------------------------------------------
		DES_BIC_IMM_VAL,	   // 001 1110 0 0000
		DES_BIC_IMM_VAL,	   // 001 1110 0 0001
		DES_BIC_IMM_VAL,	   // 001 1110 0 0010
		DES_BIC_IMM_VAL,	   // 001 1110 0 0011
		DES_BIC_IMM_VAL,	   // 001 1110 0 0100
		DES_BIC_IMM_VAL,	   // 001 1110 0 0101
		DES_BIC_IMM_VAL,	   // 001 1110 0 0110
		DES_BIC_IMM_VAL,	   // 001 1110 0 0111
		DES_BIC_IMM_VAL,	   // 001 1110 0 1000
		DES_BIC_IMM_VAL,	   // 001 1110 0 1001
		DES_BIC_IMM_VAL,	   // 001 1110 0 1010
		DES_BIC_IMM_VAL,	   // 001 1110 0 1011
		DES_BIC_IMM_VAL,	   // 001 1110 0 1100
		DES_BIC_IMM_VAL,	   // 001 1110 0 1101
		DES_BIC_IMM_VAL,	   // 001 1110 0 1110
		DES_BIC_IMM_VAL,	   // 001 1110 0 1111

		DES_BIC_S_IMM_VAL,  // 001 1110 1 0000
		DES_BIC_S_IMM_VAL,  // 001 1110 1 0001
		DES_BIC_S_IMM_VAL,  // 001 1110 1 0010
		DES_BIC_S_IMM_VAL,  // 001 1110 1 0011
		DES_BIC_S_IMM_VAL,  // 001 1110 1 0100
		DES_BIC_S_IMM_VAL,  // 001 1110 1 0101
		DES_BIC_S_IMM_VAL,  // 001 1110 1 0110
		DES_BIC_S_IMM_VAL,  // 001 1110 1 0111
		DES_BIC_S_IMM_VAL,  // 001 1110 1 1000
		DES_BIC_S_IMM_VAL,  // 001 1110 1 1001
		DES_BIC_S_IMM_VAL,  // 001 1110 1 1010
		DES_BIC_S_IMM_VAL,  // 001 1110 1 1011
		DES_BIC_S_IMM_VAL,  // 001 1110 1 1100
		DES_BIC_S_IMM_VAL,  // 001 1110 1 1101
		DES_BIC_S_IMM_VAL,  // 001 1110 1 1110
		DES_BIC_S_IMM_VAL,  // 001 1110 1 1111
		//------------------------------------------
		DES_MVN_IMM_VAL,	   // 001 1111 0 0000
		DES_MVN_IMM_VAL,	   // 001 1111 0 0001
		DES_MVN_IMM_VAL,	   // 001 1111 0 0010
		DES_MVN_IMM_VAL,	   // 001 1111 0 0011
		DES_MVN_IMM_VAL,	   // 001 1111 0 0100
		DES_MVN_IMM_VAL,	   // 001 1111 0 0101
		DES_MVN_IMM_VAL,	   // 001 1111 0 0110
		DES_MVN_IMM_VAL,	   // 001 1111 0 0111
		DES_MVN_IMM_VAL,	   // 001 1111 0 1000
		DES_MVN_IMM_VAL,	   // 001 1111 0 1001
		DES_MVN_IMM_VAL,	   // 001 1111 0 1010
		DES_MVN_IMM_VAL,	   // 001 1111 0 1011
		DES_MVN_IMM_VAL,	   // 001 1111 0 1100
		DES_MVN_IMM_VAL,	   // 001 1111 0 1101
		DES_MVN_IMM_VAL,	   // 001 1111 0 1110
		DES_MVN_IMM_VAL,	   // 001 1111 0 1111

		DES_MVN_S_IMM_VAL,  // 001 1111 1 0000
		DES_MVN_S_IMM_VAL,  // 001 1111 1 0001
		DES_MVN_S_IMM_VAL,  // 001 1111 1 0010
		DES_MVN_S_IMM_VAL,  // 001 1111 1 0011
		DES_MVN_S_IMM_VAL,  // 001 1111 1 0100
		DES_MVN_S_IMM_VAL,  // 001 1111 1 0101
		DES_MVN_S_IMM_VAL,  // 001 1111 1 0110
		DES_MVN_S_IMM_VAL,  // 001 1111 1 0111
		DES_MVN_S_IMM_VAL,  // 001 1111 1 1000
		DES_MVN_S_IMM_VAL,  // 001 1111 1 1001
		DES_MVN_S_IMM_VAL,  // 001 1111 1 1010
		DES_MVN_S_IMM_VAL,  // 001 1111 1 1011
		DES_MVN_S_IMM_VAL,  // 001 1111 1 1100
		DES_MVN_S_IMM_VAL,  // 001 1111 1 1101
		DES_MVN_S_IMM_VAL,  // 001 1111 1 1110
		DES_MVN_S_IMM_VAL,  // 001 1111 1 1111
		//------------------------------------------
		DES_STR_M_IMM_OFF_POSTIND,  //010 0000 0	0000
		DES_STR_M_IMM_OFF_POSTIND,  //010 0000 0	0001
		DES_STR_M_IMM_OFF_POSTIND,  //010 0000 0	0010
		DES_STR_M_IMM_OFF_POSTIND,  //010 0000 0	0011
		DES_STR_M_IMM_OFF_POSTIND,  //010 0000 0	0100
		DES_STR_M_IMM_OFF_POSTIND,  //010 0000 0	0101
		DES_STR_M_IMM_OFF_POSTIND,  //010 0000 0	0110
		DES_STR_M_IMM_OFF_POSTIND,  //010 0000 0	0111
		DES_STR_M_IMM_OFF_POSTIND,  //010 0000 0	1000
		DES_STR_M_IMM_OFF_POSTIND,  //010 0000 0	1001
		DES_STR_M_IMM_OFF_POSTIND,  //010 0000 0	1010
		DES_STR_M_IMM_OFF_POSTIND,  //010 0000 0	1011
		DES_STR_M_IMM_OFF_POSTIND,  //010 0000 0	1100
		DES_STR_M_IMM_OFF_POSTIND,  //010 0000 0	1101
		DES_STR_M_IMM_OFF_POSTIND,  //010 0000 0	1110
		DES_STR_M_IMM_OFF_POSTIND,  //010 0000 0	1111

		DES_LDR_M_IMM_OFF_POSTIND,  //010 0000 1	0000
		DES_LDR_M_IMM_OFF_POSTIND,  //010 0000 1	0001
		DES_LDR_M_IMM_OFF_POSTIND,  //010 0000 1	0010
		DES_LDR_M_IMM_OFF_POSTIND,  //010 0000 1	0011
		DES_LDR_M_IMM_OFF_POSTIND,  //010 0000 1	0100
		DES_LDR_M_IMM_OFF_POSTIND,  //010 0000 1	0101
		DES_LDR_M_IMM_OFF_POSTIND,  //010 0000 1	0110
		DES_LDR_M_IMM_OFF_POSTIND,  //010 0000 1	0111
		DES_LDR_M_IMM_OFF_POSTIND,  //010 0000 1	1000
		DES_LDR_M_IMM_OFF_POSTIND,  //010 0000 1	1001
		DES_LDR_M_IMM_OFF_POSTIND,  //010 0000 1	1010
		DES_LDR_M_IMM_OFF_POSTIND,  //010 0000 1	1011
		DES_LDR_M_IMM_OFF_POSTIND,  //010 0000 1	1100
		DES_LDR_M_IMM_OFF_POSTIND,  //010 0000 1	1101
		DES_LDR_M_IMM_OFF_POSTIND,  //010 0000 1	1110
		DES_LDR_M_IMM_OFF_POSTIND,  //010 0000 1	1111
		//------------------------------------------
		DES_UND,	 //010 0001	0 0000
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		//------------------------------------------
		DES_STRB_M_IMM_OFF_POSTIND,	//010 0010 0 0000
		DES_STRB_M_IMM_OFF_POSTIND,	//010 0010 0 0001
		DES_STRB_M_IMM_OFF_POSTIND,	//010 0010 0 0010
		DES_STRB_M_IMM_OFF_POSTIND,	//010 0010 0 0011
		DES_STRB_M_IMM_OFF_POSTIND,	//010 0010 0 0100
		DES_STRB_M_IMM_OFF_POSTIND,	//010 0010 0 0101
		DES_STRB_M_IMM_OFF_POSTIND,	//010 0010 0 0110
		DES_STRB_M_IMM_OFF_POSTIND,	//010 0010 0 0111
		DES_STRB_M_IMM_OFF_POSTIND,	//010 0010 0 1000
		DES_STRB_M_IMM_OFF_POSTIND,	//010 0010 0 1001
		DES_STRB_M_IMM_OFF_POSTIND,	//010 0010 0 1010
		DES_STRB_M_IMM_OFF_POSTIND,	//010 0010 0 1011
		DES_STRB_M_IMM_OFF_POSTIND,	//010 0010 0 1100
		DES_STRB_M_IMM_OFF_POSTIND,	//010 0010 0 1101
		DES_STRB_M_IMM_OFF_POSTIND,	//010 0010 0 1110
		DES_STRB_M_IMM_OFF_POSTIND,	//010 0010 0 1111

		DES_LDRB_M_IMM_OFF_POSTIND,	//010 0010 1 0000
		DES_LDRB_M_IMM_OFF_POSTIND,	//010 0010 1 0001
		DES_LDRB_M_IMM_OFF_POSTIND,	//010 0010 1 0010
		DES_LDRB_M_IMM_OFF_POSTIND,	//010 0010 1 0011
		DES_LDRB_M_IMM_OFF_POSTIND,	//010 0010 1 0100
		DES_LDRB_M_IMM_OFF_POSTIND,	//010 0010 1 0101
		DES_LDRB_M_IMM_OFF_POSTIND,	//010 0010 1 0110
		DES_LDRB_M_IMM_OFF_POSTIND,	//010 0010 1 0111
		DES_LDRB_M_IMM_OFF_POSTIND,	//010 0010 1 1000
		DES_LDRB_M_IMM_OFF_POSTIND,	//010 0010 1 1001
		DES_LDRB_M_IMM_OFF_POSTIND,	//010 0010 1 1010
		DES_LDRB_M_IMM_OFF_POSTIND,	//010 0010 1 1011
		DES_LDRB_M_IMM_OFF_POSTIND,	//010 0010 1 1100
		DES_LDRB_M_IMM_OFF_POSTIND,	//010 0010 1 1101
		DES_LDRB_M_IMM_OFF_POSTIND,	//010 0010 1 1110
		DES_LDRB_M_IMM_OFF_POSTIND,	//010 0010 1 1111
		//------------------------------------------
		DES_STRBT_M_IMM_OFF_POSTIND,	 //010 0011	0 0000
		DES_STRBT_M_IMM_OFF_POSTIND,	 //010 0011	0 0000
		DES_STRBT_M_IMM_OFF_POSTIND,	 //010 0011	0 0000
		DES_STRBT_M_IMM_OFF_POSTIND,	 //010 0011	0 0000
		DES_STRBT_M_IMM_OFF_POSTIND,	 //010 0011	0 0000
		DES_STRBT_M_IMM_OFF_POSTIND,	 //010 0011	0 0000
		DES_STRBT_M_IMM_OFF_POSTIND,	 //010 0011	0 0000
		DES_STRBT_M_IMM_OFF_POSTIND,	 //010 0011	0 0000
		DES_STRBT_M_IMM_OFF_POSTIND,	 //010 0011	0 0000
		DES_STRBT_M_IMM_OFF_POSTIND,	 //010 0011	0 0000
		DES_STRBT_M_IMM_OFF_POSTIND,	 //010 0011	0 0000
		DES_STRBT_M_IMM_OFF_POSTIND,	 //010 0011	0 0000
		DES_STRBT_M_IMM_OFF_POSTIND,	 //010 0011	0 0000
		DES_STRBT_M_IMM_OFF_POSTIND,	 //010 0011	0 0000
		DES_STRBT_M_IMM_OFF_POSTIND,	 //010 0011	0 0000
		DES_STRBT_M_IMM_OFF_POSTIND,	 //010 0011	0 0000

		DES_LDRBT_M_IMM_OFF_POSTIND,	 //010 0011	0 0000
		DES_LDRBT_M_IMM_OFF_POSTIND,	 //010 0011	0 0000
		DES_LDRBT_M_IMM_OFF_POSTIND,	 //010 0011	0 0000
		DES_LDRBT_M_IMM_OFF_POSTIND,	 //010 0011	0 0000
		DES_LDRBT_M_IMM_OFF_POSTIND,	 //010 0011	0 0000
		DES_LDRBT_M_IMM_OFF_POSTIND,	 //010 0011	0 0000
		DES_LDRBT_M_IMM_OFF_POSTIND,	 //010 0011	0 0000
		DES_LDRBT_M_IMM_OFF_POSTIND,	 //010 0011	0 0000
		DES_LDRBT_M_IMM_OFF_POSTIND,	 //010 0011	0 0000
		DES_LDRBT_M_IMM_OFF_POSTIND,	 //010 0011	0 0000
		DES_LDRBT_M_IMM_OFF_POSTIND,	 //010 0011	0 0000
		DES_LDRBT_M_IMM_OFF_POSTIND,	 //010 0011	0 0000
		DES_LDRBT_M_IMM_OFF_POSTIND,	 //010 0011	0 0000
		DES_LDRBT_M_IMM_OFF_POSTIND,	 //010 0011	0 0000
		DES_LDRBT_M_IMM_OFF_POSTIND,	 //010 0011	0 0000
		DES_LDRBT_M_IMM_OFF_POSTIND,	 //010 0011	0 0000
		//------------------------------------------
		DES_STR_P_IMM_OFF_POSTIND,  //010 0100 0	0000
		DES_STR_P_IMM_OFF_POSTIND,  //010 0100 0	0000
		DES_STR_P_IMM_OFF_POSTIND,  //010 0100 0	0000
		DES_STR_P_IMM_OFF_POSTIND,  //010 0100 0	0000
		DES_STR_P_IMM_OFF_POSTIND,  //010 0100 0	0000
		DES_STR_P_IMM_OFF_POSTIND,  //010 0100 0	0000
		DES_STR_P_IMM_OFF_POSTIND,  //010 0100 0	0000
		DES_STR_P_IMM_OFF_POSTIND,  //010 0100 0	0000
		DES_STR_P_IMM_OFF_POSTIND,  //010 0100 0	0000
		DES_STR_P_IMM_OFF_POSTIND,  //010 0100 0	0000
		DES_STR_P_IMM_OFF_POSTIND,  //010 0100 0	0000
		DES_STR_P_IMM_OFF_POSTIND,  //010 0100 0	0000
		DES_STR_P_IMM_OFF_POSTIND,  //010 0100 0	0000
		DES_STR_P_IMM_OFF_POSTIND,  //010 0100 0	0000
		DES_STR_P_IMM_OFF_POSTIND,  //010 0100 0	0000
		DES_STR_P_IMM_OFF_POSTIND,  //010 0100 0	0000

		DES_LDR_P_IMM_OFF_POSTIND,  //010 0100 0	0000
		DES_LDR_P_IMM_OFF_POSTIND,  //010 0100 0	0000
		DES_LDR_P_IMM_OFF_POSTIND,  //010 0100 0	0000
		DES_LDR_P_IMM_OFF_POSTIND,  //010 0100 0	0000
		DES_LDR_P_IMM_OFF_POSTIND,  //010 0100 0	0000
		DES_LDR_P_IMM_OFF_POSTIND,  //010 0100 0	0000
		DES_LDR_P_IMM_OFF_POSTIND,  //010 0100 0	0000
		DES_LDR_P_IMM_OFF_POSTIND,  //010 0100 0	0000
		DES_LDR_P_IMM_OFF_POSTIND,  //010 0100 0	0000
		DES_LDR_P_IMM_OFF_POSTIND,  //010 0100 0	0000
		DES_LDR_P_IMM_OFF_POSTIND,  //010 0100 0	0000
		DES_LDR_P_IMM_OFF_POSTIND,  //010 0100 0	0000
		DES_LDR_P_IMM_OFF_POSTIND,  //010 0100 0	0000
		DES_LDR_P_IMM_OFF_POSTIND,  //010 0100 0	0000
		DES_LDR_P_IMM_OFF_POSTIND,  //010 0100 0	0000
		DES_LDR_P_IMM_OFF_POSTIND,  //010 0100 0	0000
		//------------------------------------------
		DES_STR_P_IMM_OFF_POSTIND,  //010 0100 0	0000
		DES_STR_P_IMM_OFF_POSTIND,  //010 0100 0	0000
		DES_STR_P_IMM_OFF_POSTIND,  //010 0100 0	0000
		DES_STR_P_IMM_OFF_POSTIND,  //010 0100 0	0000
		DES_STR_P_IMM_OFF_POSTIND,  //010 0100 0	0000
		DES_STR_P_IMM_OFF_POSTIND,  //010 0100 0	0000
		DES_STR_P_IMM_OFF_POSTIND,  //010 0100 0	0000
		DES_STR_P_IMM_OFF_POSTIND,  //010 0100 0	0000
		DES_STR_P_IMM_OFF_POSTIND,  //010 0100 0	0000
		DES_STR_P_IMM_OFF_POSTIND,  //010 0100 0	0000
		DES_STR_P_IMM_OFF_POSTIND,  //010 0100 0	0000
		DES_STR_P_IMM_OFF_POSTIND,  //010 0100 0	0000
		DES_STR_P_IMM_OFF_POSTIND,  //010 0100 0	0000
		DES_STR_P_IMM_OFF_POSTIND,  //010 0100 0	0000
		DES_STR_P_IMM_OFF_POSTIND,  //010 0100 0	0000
		DES_STR_P_IMM_OFF_POSTIND,  //010 0100 0	0000

		DES_LDR_P_IMM_OFF_POSTIND2,	//010 0100 0 0000
		DES_LDR_P_IMM_OFF_POSTIND2,	//010 0100 0 0000
		DES_LDR_P_IMM_OFF_POSTIND2,	//010 0100 0 0000
		DES_LDR_P_IMM_OFF_POSTIND2,	//010 0100 0 0000
		DES_LDR_P_IMM_OFF_POSTIND2,	//010 0100 0 0000
		DES_LDR_P_IMM_OFF_POSTIND2,	//010 0100 0 0000
		DES_LDR_P_IMM_OFF_POSTIND2,	//010 0100 0 0000
		DES_LDR_P_IMM_OFF_POSTIND2,	//010 0100 0 0000
		DES_LDR_P_IMM_OFF_POSTIND2,	//010 0100 0 0000
		DES_LDR_P_IMM_OFF_POSTIND2,	//010 0100 0 0000
		DES_LDR_P_IMM_OFF_POSTIND2,	//010 0100 0 0000
		DES_LDR_P_IMM_OFF_POSTIND2,	//010 0100 0 0000
		DES_LDR_P_IMM_OFF_POSTIND2,	//010 0100 0 0000
		DES_LDR_P_IMM_OFF_POSTIND2,	//010 0100 0 0000
		DES_LDR_P_IMM_OFF_POSTIND2,	//010 0100 0 0000
		DES_LDR_P_IMM_OFF_POSTIND2,	//010 0100 0 0000
		//------------------------------------------
		DES_STRB_P_IMM_OFF_POSTIND,	//010 0110 0 0000
		DES_STRB_P_IMM_OFF_POSTIND,	//010 0100 0 0000
		DES_STRB_P_IMM_OFF_POSTIND,	//010 0100 0 0000
		DES_STRB_P_IMM_OFF_POSTIND,	//010 0100 0 0000
		DES_STRB_P_IMM_OFF_POSTIND,	//010 0100 0 0000
		DES_STRB_P_IMM_OFF_POSTIND,	//010 0100 0 0000
		DES_STRB_P_IMM_OFF_POSTIND,	//010 0100 0 0000
		DES_STRB_P_IMM_OFF_POSTIND,	//010 0100 0 0000
		DES_STRB_P_IMM_OFF_POSTIND,	//010 0100 0 0000
		DES_STRB_P_IMM_OFF_POSTIND,	//010 0100 0 0000
		DES_STRB_P_IMM_OFF_POSTIND,	//010 0100 0 0000
		DES_STRB_P_IMM_OFF_POSTIND,	//010 0100 0 0000
		DES_STRB_P_IMM_OFF_POSTIND,	//010 0100 0 0000
		DES_STRB_P_IMM_OFF_POSTIND,	//010 0100 0 0000
		DES_STRB_P_IMM_OFF_POSTIND,	//010 0100 0 0000
		DES_STRB_P_IMM_OFF_POSTIND,	//010 0100 0 0000

		DES_LDRB_P_IMM_OFF_POSTIND,	//010 0100 0 0000
		DES_LDRB_P_IMM_OFF_POSTIND,	//010 0100 0 0000
		DES_LDRB_P_IMM_OFF_POSTIND,	//010 0100 0 0000
		DES_LDRB_P_IMM_OFF_POSTIND,	//010 0100 0 0000
		DES_LDRB_P_IMM_OFF_POSTIND,	//010 0100 0 0000
		DES_LDRB_P_IMM_OFF_POSTIND,	//010 0100 0 0000
		DES_LDRB_P_IMM_OFF_POSTIND,	//010 0100 0 0000
		DES_LDRB_P_IMM_OFF_POSTIND,	//010 0100 0 0000
		DES_LDRB_P_IMM_OFF_POSTIND,	//010 0100 0 0000
		DES_LDRB_P_IMM_OFF_POSTIND,	//010 0100 0 0000
		DES_LDRB_P_IMM_OFF_POSTIND,	//010 0100 0 0000
		DES_LDRB_P_IMM_OFF_POSTIND,	//010 0100 0 0000
		DES_LDRB_P_IMM_OFF_POSTIND,	//010 0100 0 0000
		DES_LDRB_P_IMM_OFF_POSTIND,	//010 0100 0 0000
		DES_LDRB_P_IMM_OFF_POSTIND,	//010 0100 0 0000
		DES_LDRB_P_IMM_OFF_POSTIND,	//010 0110 1 1111
		//------------------------------------------
		DES_STRBT_P_IMM_OFF_POSTIND,	 //010 0111	0 0000
		DES_STRBT_P_IMM_OFF_POSTIND,	 //010 0100	0 0000
		DES_STRBT_P_IMM_OFF_POSTIND,	 //010 0100	0 0000
		DES_STRBT_P_IMM_OFF_POSTIND,	 //010 0100	0 0000
		DES_STRBT_P_IMM_OFF_POSTIND,	 //010 0100	0 0000
		DES_STRBT_P_IMM_OFF_POSTIND,	 //010 0100	0 0000
		DES_STRBT_P_IMM_OFF_POSTIND,	 //010 0100	0 0000
		DES_STRBT_P_IMM_OFF_POSTIND,	 //010 0100	0 0000
		DES_STRBT_P_IMM_OFF_POSTIND,	 //010 0100	0 0000
		DES_STRBT_P_IMM_OFF_POSTIND,	 //010 0100	0 0000
		DES_STRBT_P_IMM_OFF_POSTIND,	 //010 0100	0 0000
		DES_STRBT_P_IMM_OFF_POSTIND,	 //010 0100	0 0000
		DES_STRBT_P_IMM_OFF_POSTIND,	 //010 0100	0 0000
		DES_STRBT_P_IMM_OFF_POSTIND,	 //010 0100	0 0000
		DES_STRBT_P_IMM_OFF_POSTIND,	 //010 0100	0 0000
		DES_STRBT_P_IMM_OFF_POSTIND,	 //010 0100	0 0000

		DES_LDRBT_P_IMM_OFF_POSTIND,	 //010 0100	0 0000
		DES_LDRBT_P_IMM_OFF_POSTIND,	 //010 0100	0 0000
		DES_LDRBT_P_IMM_OFF_POSTIND,	 //010 0100	0 0000
		DES_LDRBT_P_IMM_OFF_POSTIND,	 //010 0100	0 0000
		DES_LDRBT_P_IMM_OFF_POSTIND,	 //010 0100	0 0000
		DES_LDRBT_P_IMM_OFF_POSTIND,	 //010 0100	0 0000
		DES_LDRBT_P_IMM_OFF_POSTIND,	 //010 0100	0 0000
		DES_LDRBT_P_IMM_OFF_POSTIND,	 //010 0100	0 0000
		DES_LDRBT_P_IMM_OFF_POSTIND,	 //010 0100	0 0000
		DES_LDRBT_P_IMM_OFF_POSTIND,	 //010 0100	0 0000
		DES_LDRBT_P_IMM_OFF_POSTIND,	 //010 0100	0 0000
		DES_LDRBT_P_IMM_OFF_POSTIND,	 //010 0100	0 0000
		DES_LDRBT_P_IMM_OFF_POSTIND,	 //010 0100	0 0000
		DES_LDRBT_P_IMM_OFF_POSTIND,	 //010 0100	0 0000
		DES_LDRBT_P_IMM_OFF_POSTIND,	 //010 0100	0 0000
		DES_LDRBT_P_IMM_OFF_POSTIND,	 //010 0111	1 1111
		//------------------------------------------
		DES_STR_M_IMM_OFF,  //010 1000 0	0000
		DES_STR_M_IMM_OFF,
		DES_STR_M_IMM_OFF,
		DES_STR_M_IMM_OFF,
		DES_STR_M_IMM_OFF,
		DES_STR_M_IMM_OFF,
		DES_STR_M_IMM_OFF,
		DES_STR_M_IMM_OFF,
		DES_STR_M_IMM_OFF,
		DES_STR_M_IMM_OFF,
		DES_STR_M_IMM_OFF,
		DES_STR_M_IMM_OFF,
		DES_STR_M_IMM_OFF,
		DES_STR_M_IMM_OFF,
		DES_STR_M_IMM_OFF,
		DES_STR_M_IMM_OFF,

		DES_LDR_M_IMM_OFF,
		DES_LDR_M_IMM_OFF,
		DES_LDR_M_IMM_OFF,
		DES_LDR_M_IMM_OFF,
		DES_LDR_M_IMM_OFF,
		DES_LDR_M_IMM_OFF,
		DES_LDR_M_IMM_OFF,
		DES_LDR_M_IMM_OFF,
		DES_LDR_M_IMM_OFF,
		DES_LDR_M_IMM_OFF,
		DES_LDR_M_IMM_OFF,
		DES_LDR_M_IMM_OFF,
		DES_LDR_M_IMM_OFF,
		DES_LDR_M_IMM_OFF,
		DES_LDR_M_IMM_OFF,
		DES_LDR_M_IMM_OFF,  //010 1000 1	1111
		//------------------------------------------
		DES_STR_M_IMM_OFF_PREIND,  //010	1001 0 0000
		DES_STR_M_IMM_OFF_PREIND,
		DES_STR_M_IMM_OFF_PREIND,
		DES_STR_M_IMM_OFF_PREIND,
		DES_STR_M_IMM_OFF_PREIND,
		DES_STR_M_IMM_OFF_PREIND,
		DES_STR_M_IMM_OFF_PREIND,
		DES_STR_M_IMM_OFF_PREIND,
		DES_STR_M_IMM_OFF_PREIND,
		DES_STR_M_IMM_OFF_PREIND,
		DES_STR_M_IMM_OFF_PREIND,
		DES_STR_M_IMM_OFF_PREIND,
		DES_STR_M_IMM_OFF_PREIND,
		DES_STR_M_IMM_OFF_PREIND,
		DES_STR_M_IMM_OFF_PREIND,
		DES_STR_M_IMM_OFF_PREIND,

		DES_LDR_M_IMM_OFF_PREIND,
		DES_LDR_M_IMM_OFF_PREIND,
		DES_LDR_M_IMM_OFF_PREIND,
		DES_LDR_M_IMM_OFF_PREIND,
		DES_LDR_M_IMM_OFF_PREIND,
		DES_LDR_M_IMM_OFF_PREIND,
		DES_LDR_M_IMM_OFF_PREIND,
		DES_LDR_M_IMM_OFF_PREIND,
		DES_LDR_M_IMM_OFF_PREIND,
		DES_LDR_M_IMM_OFF_PREIND,
		DES_LDR_M_IMM_OFF_PREIND,
		DES_LDR_M_IMM_OFF_PREIND,
		DES_LDR_M_IMM_OFF_PREIND,
		DES_LDR_M_IMM_OFF_PREIND,
		DES_LDR_M_IMM_OFF_PREIND,
		DES_LDR_M_IMM_OFF_PREIND,  //010	1001 1 1111
		//------------------------------------------
		DES_STRB_M_IMM_OFF,	//010 1010 0 0000
		DES_STRB_M_IMM_OFF,
		DES_STRB_M_IMM_OFF,
		DES_STRB_M_IMM_OFF,
		DES_STRB_M_IMM_OFF,
		DES_STRB_M_IMM_OFF,
		DES_STRB_M_IMM_OFF,
		DES_STRB_M_IMM_OFF,
		DES_STRB_M_IMM_OFF,
		DES_STRB_M_IMM_OFF,
		DES_STRB_M_IMM_OFF,
		DES_STRB_M_IMM_OFF,
		DES_STRB_M_IMM_OFF,
		DES_STRB_M_IMM_OFF,
		DES_STRB_M_IMM_OFF,
		DES_STRB_M_IMM_OFF,

		DES_LDRB_M_IMM_OFF,
		DES_LDRB_M_IMM_OFF,
		DES_LDRB_M_IMM_OFF,
		DES_LDRB_M_IMM_OFF,
		DES_LDRB_M_IMM_OFF,
		DES_LDRB_M_IMM_OFF,
		DES_LDRB_M_IMM_OFF,
		DES_LDRB_M_IMM_OFF,
		DES_LDRB_M_IMM_OFF,
		DES_LDRB_M_IMM_OFF,
		DES_LDRB_M_IMM_OFF,
		DES_LDRB_M_IMM_OFF,
		DES_LDRB_M_IMM_OFF,
		DES_LDRB_M_IMM_OFF,
		DES_LDRB_M_IMM_OFF,
		DES_LDRB_M_IMM_OFF,	//010 1010 1 1111
		//------------------------------------------
		DES_STRB_M_IMM_OFF_PREIND,  //010 1011 0	0000
		DES_STRB_M_IMM_OFF_PREIND,
		DES_STRB_M_IMM_OFF_PREIND,
		DES_STRB_M_IMM_OFF_PREIND,
		DES_STRB_M_IMM_OFF_PREIND,
		DES_STRB_M_IMM_OFF_PREIND,
		DES_STRB_M_IMM_OFF_PREIND,
		DES_STRB_M_IMM_OFF_PREIND,
		DES_STRB_M_IMM_OFF_PREIND,
		DES_STRB_M_IMM_OFF_PREIND,
		DES_STRB_M_IMM_OFF_PREIND,
		DES_STRB_M_IMM_OFF_PREIND,
		DES_STRB_M_IMM_OFF_PREIND,
		DES_STRB_M_IMM_OFF_PREIND,
		DES_STRB_M_IMM_OFF_PREIND,
		DES_STRB_M_IMM_OFF_PREIND,

		DES_LDRB_M_IMM_OFF_PREIND,
		DES_LDRB_M_IMM_OFF_PREIND,
		DES_LDRB_M_IMM_OFF_PREIND,
		DES_LDRB_M_IMM_OFF_PREIND,
		DES_LDRB_M_IMM_OFF_PREIND,
		DES_LDRB_M_IMM_OFF_PREIND,
		DES_LDRB_M_IMM_OFF_PREIND,
		DES_LDRB_M_IMM_OFF_PREIND,
		DES_LDRB_M_IMM_OFF_PREIND,
		DES_LDRB_M_IMM_OFF_PREIND,
		DES_LDRB_M_IMM_OFF_PREIND,
		DES_LDRB_M_IMM_OFF_PREIND,
		DES_LDRB_M_IMM_OFF_PREIND,
		DES_LDRB_M_IMM_OFF_PREIND,
		DES_LDRB_M_IMM_OFF_PREIND,
		DES_LDRB_M_IMM_OFF_PREIND,  //010 1011 1	1111
		//------------------------------------------
		DES_STR_P_IMM_OFF,  //010 1100 0	0000
		DES_STR_P_IMM_OFF,
		DES_STR_P_IMM_OFF,
		DES_STR_P_IMM_OFF,
		DES_STR_P_IMM_OFF,
		DES_STR_P_IMM_OFF,
		DES_STR_P_IMM_OFF,
		DES_STR_P_IMM_OFF,
		DES_STR_P_IMM_OFF,
		DES_STR_P_IMM_OFF,
		DES_STR_P_IMM_OFF,
		DES_STR_P_IMM_OFF,
		DES_STR_P_IMM_OFF,
		DES_STR_P_IMM_OFF,
		DES_STR_P_IMM_OFF,
		DES_STR_P_IMM_OFF,

		DES_LDR_P_IMM_OFF,
		DES_LDR_P_IMM_OFF,
		DES_LDR_P_IMM_OFF,
		DES_LDR_P_IMM_OFF,
		DES_LDR_P_IMM_OFF,
		DES_LDR_P_IMM_OFF,
		DES_LDR_P_IMM_OFF,
		DES_LDR_P_IMM_OFF,
		DES_LDR_P_IMM_OFF,
		DES_LDR_P_IMM_OFF,
		DES_LDR_P_IMM_OFF,
		DES_LDR_P_IMM_OFF,
		DES_LDR_P_IMM_OFF,
		DES_LDR_P_IMM_OFF,
		DES_LDR_P_IMM_OFF,
		DES_LDR_P_IMM_OFF,  //010 1100 1	1111
		//------------------------------------------
		DES_STR_P_IMM_OFF_PREIND,  //010	1101 0 0000
		DES_STR_P_IMM_OFF_PREIND,
		DES_STR_P_IMM_OFF_PREIND,
		DES_STR_P_IMM_OFF_PREIND,
		DES_STR_P_IMM_OFF_PREIND,
		DES_STR_P_IMM_OFF_PREIND,
		DES_STR_P_IMM_OFF_PREIND,
		DES_STR_P_IMM_OFF_PREIND,
		DES_STR_P_IMM_OFF_PREIND,
		DES_STR_P_IMM_OFF_PREIND,
		DES_STR_P_IMM_OFF_PREIND,
		DES_STR_P_IMM_OFF_PREIND,
		DES_STR_P_IMM_OFF_PREIND,
		DES_STR_P_IMM_OFF_PREIND,
		DES_STR_P_IMM_OFF_PREIND,
		DES_STR_P_IMM_OFF_PREIND,

		DES_LDR_P_IMM_OFF_PREIND,
		DES_LDR_P_IMM_OFF_PREIND,
		DES_LDR_P_IMM_OFF_PREIND,
		DES_LDR_P_IMM_OFF_PREIND,
		DES_LDR_P_IMM_OFF_PREIND,
		DES_LDR_P_IMM_OFF_PREIND,
		DES_LDR_P_IMM_OFF_PREIND,
		DES_LDR_P_IMM_OFF_PREIND,
		DES_LDR_P_IMM_OFF_PREIND,
		DES_LDR_P_IMM_OFF_PREIND,
		DES_LDR_P_IMM_OFF_PREIND,
		DES_LDR_P_IMM_OFF_PREIND,
		DES_LDR_P_IMM_OFF_PREIND,
		DES_LDR_P_IMM_OFF_PREIND,
		DES_LDR_P_IMM_OFF_PREIND,
		DES_LDR_P_IMM_OFF_PREIND,  //010	1101 1 1111
		//------------------------------------------
		DES_STRB_P_IMM_OFF,	//010 1110 0 0000
		DES_STRB_P_IMM_OFF,
		DES_STRB_P_IMM_OFF,
		DES_STRB_P_IMM_OFF,
		DES_STRB_P_IMM_OFF,
		DES_STRB_P_IMM_OFF,
		DES_STRB_P_IMM_OFF,
		DES_STRB_P_IMM_OFF,
		DES_STRB_P_IMM_OFF,
		DES_STRB_P_IMM_OFF,
		DES_STRB_P_IMM_OFF,
		DES_STRB_P_IMM_OFF,
		DES_STRB_P_IMM_OFF,
		DES_STRB_P_IMM_OFF,
		DES_STRB_P_IMM_OFF,
		DES_STRB_P_IMM_OFF,

		DES_LDRB_P_IMM_OFF,
		DES_LDRB_P_IMM_OFF,
		DES_LDRB_P_IMM_OFF,
		DES_LDRB_P_IMM_OFF,
		DES_LDRB_P_IMM_OFF,
		DES_LDRB_P_IMM_OFF,
		DES_LDRB_P_IMM_OFF,
		DES_LDRB_P_IMM_OFF,
		DES_LDRB_P_IMM_OFF,
		DES_LDRB_P_IMM_OFF,
		DES_LDRB_P_IMM_OFF,
		DES_LDRB_P_IMM_OFF,
		DES_LDRB_P_IMM_OFF,
		DES_LDRB_P_IMM_OFF,
		DES_LDRB_P_IMM_OFF,
		DES_LDRB_P_IMM_OFF,	//010 1110 1 1111
		//------------------------------------------
		DES_STRB_P_IMM_OFF_PREIND,  //010 1111 0	0000
		DES_STRB_P_IMM_OFF_PREIND,
		DES_STRB_P_IMM_OFF_PREIND,
		DES_STRB_P_IMM_OFF_PREIND,
		DES_STRB_P_IMM_OFF_PREIND,
		DES_STRB_P_IMM_OFF_PREIND,
		DES_STRB_P_IMM_OFF_PREIND,
		DES_STRB_P_IMM_OFF_PREIND,
		DES_STRB_P_IMM_OFF_PREIND,
		DES_STRB_P_IMM_OFF_PREIND,
		DES_STRB_P_IMM_OFF_PREIND,
		DES_STRB_P_IMM_OFF_PREIND,
		DES_STRB_P_IMM_OFF_PREIND,
		DES_STRB_P_IMM_OFF_PREIND,
		DES_STRB_P_IMM_OFF_PREIND,
		DES_STRB_P_IMM_OFF_PREIND,

		DES_LDRB_P_IMM_OFF_PREIND,
		DES_LDRB_P_IMM_OFF_PREIND,
		DES_LDRB_P_IMM_OFF_PREIND,
		DES_LDRB_P_IMM_OFF_PREIND,
		DES_LDRB_P_IMM_OFF_PREIND,
		DES_LDRB_P_IMM_OFF_PREIND,
		DES_LDRB_P_IMM_OFF_PREIND,
		DES_LDRB_P_IMM_OFF_PREIND,
		DES_LDRB_P_IMM_OFF_PREIND,
		DES_LDRB_P_IMM_OFF_PREIND,
		DES_LDRB_P_IMM_OFF_PREIND,
		DES_LDRB_P_IMM_OFF_PREIND,
		DES_LDRB_P_IMM_OFF_PREIND,
		DES_LDRB_P_IMM_OFF_PREIND,
		DES_LDRB_P_IMM_OFF_PREIND,
		DES_LDRB_P_IMM_OFF_PREIND,  //010 1111 1	1111
		//------------------------------------------
		DES_STR_M_LSL_IMM_OFF_POSTIND,  //011 0000 0	0000
		DES_UND,
		DES_STR_M_LSR_IMM_OFF_POSTIND,
		DES_UND,
		DES_STR_M_ASR_IMM_OFF_POSTIND,
		DES_UND,
		DES_STR_M_ROR_IMM_OFF_POSTIND,
		DES_UND,
		DES_STR_M_LSL_IMM_OFF_POSTIND,
		DES_UND,
		DES_STR_M_LSR_IMM_OFF_POSTIND,
		DES_UND,
		DES_STR_M_ASR_IMM_OFF_POSTIND,
		DES_UND,
		DES_STR_M_ROR_IMM_OFF_POSTIND,
		DES_UND,

		DES_LDR_M_LSL_IMM_OFF_POSTIND,  
		DES_UND,
		DES_LDR_M_LSR_IMM_OFF_POSTIND,
		DES_UND,
		DES_LDR_M_ASR_IMM_OFF_POSTIND,
		DES_UND,
		DES_LDR_M_ROR_IMM_OFF_POSTIND,
		DES_UND,
		DES_LDR_M_LSL_IMM_OFF_POSTIND,
		DES_UND,
		DES_LDR_M_LSR_IMM_OFF_POSTIND,
		DES_UND,
		DES_LDR_M_ASR_IMM_OFF_POSTIND,
		DES_UND,
		DES_LDR_M_ROR_IMM_OFF_POSTIND,
		DES_UND,						   //011 0000 1	1111
		//------------------------------------------
		DES_UND,	//011 0001 0 0000
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,	//011 0001 1 1111
		//------------------------------------------
		DES_STRB_M_LSL_IMM_OFF_POSTIND,	//011 0010 0 0000
		DES_UND,
		DES_STRB_M_LSR_IMM_OFF_POSTIND,
		DES_UND,
		DES_STRB_M_ASR_IMM_OFF_POSTIND,
		DES_UND,
		DES_STRB_M_ROR_IMM_OFF_POSTIND,
		DES_UND,
		DES_STRB_M_LSL_IMM_OFF_POSTIND,
		DES_UND,
		DES_STRB_M_LSR_IMM_OFF_POSTIND,
		DES_UND,
		DES_STRB_M_ASR_IMM_OFF_POSTIND,
		DES_UND,
		DES_STRB_M_ROR_IMM_OFF_POSTIND,
		DES_UND,

		DES_LDRB_M_LSL_IMM_OFF_POSTIND,	
		DES_UND,
		DES_LDRB_M_LSR_IMM_OFF_POSTIND,
		DES_UND,
		DES_LDRB_M_ASR_IMM_OFF_POSTIND,
		DES_UND,
		DES_LDRB_M_ROR_IMM_OFF_POSTIND,
		DES_UND,
		DES_LDRB_M_LSL_IMM_OFF_POSTIND,
		DES_UND,
		DES_LDRB_M_LSR_IMM_OFF_POSTIND,
		DES_UND,
		DES_LDRB_M_ASR_IMM_OFF_POSTIND,
		DES_UND,
		DES_LDRB_M_ROR_IMM_OFF_POSTIND,
		DES_UND,							//011 0010 1 1111
		//------------------------------------------
		DES_STRBT_M_LSL_IMM_OFF_POSTIND,	 //011 0011	0 0000
		DES_UND,
		DES_STRBT_M_LSR_IMM_OFF_POSTIND,
		DES_UND,
		DES_STRBT_M_ASR_IMM_OFF_POSTIND,
		DES_UND,
		DES_STRBT_M_ROR_IMM_OFF_POSTIND,
		DES_UND,
		DES_STRBT_M_LSL_IMM_OFF_POSTIND,
		DES_UND,
		DES_STRBT_M_LSR_IMM_OFF_POSTIND,
		DES_UND,
		DES_STRBT_M_ASR_IMM_OFF_POSTIND,
		DES_UND,
		DES_STRBT_M_ROR_IMM_OFF_POSTIND,
		DES_UND,

		DES_LDRBT_M_LSL_IMM_OFF_POSTIND,	
		DES_UND,
		DES_LDRBT_M_LSR_IMM_OFF_POSTIND,
		DES_UND,
		DES_LDRBT_M_ASR_IMM_OFF_POSTIND,
		DES_UND,
		DES_LDRBT_M_ROR_IMM_OFF_POSTIND,
		DES_UND,
		DES_LDRBT_M_LSL_IMM_OFF_POSTIND,
		DES_UND,
		DES_LDRBT_M_LSR_IMM_OFF_POSTIND,
		DES_UND,
		DES_LDRBT_M_ASR_IMM_OFF_POSTIND,
		DES_UND,
		DES_LDRBT_M_ROR_IMM_OFF_POSTIND,
		DES_UND,							   //011 0011 1	1111
		//------------------------------------------
		DES_STR_P_LSL_IMM_OFF_POSTIND,  //011 0100 0	0000
		DES_UND,
		DES_STR_P_LSR_IMM_OFF_POSTIND,
		DES_UND,
		DES_STR_P_ASR_IMM_OFF_POSTIND,
		DES_UND,
		DES_STR_P_ROR_IMM_OFF_POSTIND,
		DES_UND,
		DES_STR_P_LSL_IMM_OFF_POSTIND,
		DES_UND,
		DES_STR_P_LSR_IMM_OFF_POSTIND,
		DES_UND,
		DES_STR_P_ASR_IMM_OFF_POSTIND,
		DES_UND,
		DES_STR_P_ROR_IMM_OFF_POSTIND,
		DES_UND,

		DES_LDR_P_LSL_IMM_OFF_POSTIND,  
		DES_UND,
		DES_LDR_P_LSR_IMM_OFF_POSTIND,
		DES_UND,
		DES_LDR_P_ASR_IMM_OFF_POSTIND,
		DES_UND,
		DES_LDR_P_ROR_IMM_OFF_POSTIND,
		DES_UND,
		DES_LDR_P_LSL_IMM_OFF_POSTIND,
		DES_UND,
		DES_LDR_P_LSR_IMM_OFF_POSTIND,
		DES_UND,
		DES_LDR_P_ASR_IMM_OFF_POSTIND,
		DES_UND,
		DES_LDR_P_ROR_IMM_OFF_POSTIND,
		DES_UND,		//011 0100 1 1111
		//------------------------------------------
		DES_UND,	 //011 0101	0 0000
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,

		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,
		DES_UND,	  //011	0101 1 1111
		//------------------------------------------
		DES_STRB_P_LSL_IMM_OFF_POSTIND,	//011 0110 0 0000
		DES_UND,
		DES_STRB_P_LSR_IMM_OFF_POSTIND,
		DES_UND,
		DES_STRB_P_ASR_IMM_OFF_POSTIND,
		DES_UND,
		DES_STRB_P_ROR_IMM_OFF_POSTIND,
		DES_UND,
		DES_STRB_P_LSL_IMM_OFF_POSTIND,
		DES_UND,
		DES_STRB_P_LSR_IMM_OFF_POSTIND,
		DES_UND,
		DES_STRB_P_ASR_IMM_OFF_POSTIND,
		DES_UND,
		DES_STRB_P_ROR_IMM_OFF_POSTIND,
		DES_UND,

		DES_LDRB_P_LSL_IMM_OFF_POSTIND,	
		DES_UND,
		DES_LDRB_P_LSR_IMM_OFF_POSTIND,
		DES_UND,
		DES_LDRB_P_ASR_IMM_OFF_POSTIND,
		DES_UND,
		DES_LDRB_P_ROR_IMM_OFF_POSTIND,
		DES_UND,
		DES_LDRB_P_LSL_IMM_OFF_POSTIND,
		DES_UND,
		DES_LDRB_P_LSR_IMM_OFF_POSTIND,
		DES_UND,
		DES_LDRB_P_ASR_IMM_OFF_POSTIND,
		DES_UND,
		DES_LDRB_P_ROR_IMM_OFF_POSTIND,
		DES_UND,		
		//------------------------------------------
		DES_STRBT_P_LSL_IMM_OFF_POSTIND,	 //011 0111	0 0000
		DES_UND,
		DES_STRBT_P_LSR_IMM_OFF_POSTIND,
		DES_UND,
		DES_STRBT_P_ASR_IMM_OFF_POSTIND,
		DES_UND,
		DES_STRBT_P_ROR_IMM_OFF_POSTIND,
		DES_UND,
		DES_STRBT_P_LSL_IMM_OFF_POSTIND,
		DES_UND,
		DES_STRBT_P_LSR_IMM_OFF_POSTIND,
		DES_UND,
		DES_STRBT_P_ASR_IMM_OFF_POSTIND,
		DES_UND,
		DES_STRBT_P_ROR_IMM_OFF_POSTIND,
		DES_UND,

		DES_LDRBT_P_LSL_IMM_OFF_POSTIND,	
		DES_UND,
		DES_LDRBT_P_LSR_IMM_OFF_POSTIND,
		DES_UND,
		DES_LDRBT_P_ASR_IMM_OFF_POSTIND,
		DES_UND,
		DES_LDRBT_P_ROR_IMM_OFF_POSTIND,
		DES_UND,
		DES_LDRBT_P_LSL_IMM_OFF_POSTIND,
		DES_UND,
		DES_LDRBT_P_LSR_IMM_OFF_POSTIND,
		DES_UND,
		DES_LDRBT_P_ASR_IMM_OFF_POSTIND,
		DES_UND,
		DES_LDRBT_P_ROR_IMM_OFF_POSTIND,
		DES_UND,		
		//------------------------------------------
		DES_STR_M_LSL_IMM_OFF,  //011 1000 0	0000
		DES_UND,
		DES_STR_M_LSR_IMM_OFF,
		DES_UND,
		DES_STR_M_ASR_IMM_OFF,
		DES_UND,
		DES_STR_M_ROR_IMM_OFF,
		DES_UND,
		DES_STR_M_LSL_IMM_OFF,
		DES_UND,
		DES_STR_M_LSR_IMM_OFF,
		DES_UND,
		DES_STR_M_ASR_IMM_OFF,
		DES_UND,
		DES_STR_M_ROR_IMM_OFF,
		DES_UND,

		DES_LDR_M_LSL_IMM_OFF,
		DES_UND,
		DES_LDR_M_LSR_IMM_OFF,
		DES_UND,
		DES_LDR_M_ASR_IMM_OFF,
		DES_UND,
		DES_LDR_M_ROR_IMM_OFF,
		DES_UND,
		DES_LDR_M_LSL_IMM_OFF,
		DES_UND,
		DES_LDR_M_LSR_IMM_OFF,
		DES_UND,
		DES_LDR_M_ASR_IMM_OFF,
		DES_UND,
		DES_LDR_M_ROR_IMM_OFF,
		DES_UND,
		//------------------------------------------
		DES_STR_M_LSL_IMM_OFF_PREIND,  //011	1001 0 0000
		DES_UND,
		DES_STR_M_LSR_IMM_OFF_PREIND,
		DES_UND,
		DES_STR_M_ASR_IMM_OFF_PREIND,
		DES_UND,
		DES_STR_M_ROR_IMM_OFF_PREIND,
		DES_UND,
		DES_STR_M_LSL_IMM_OFF_PREIND,
		DES_UND,
		DES_STR_M_LSR_IMM_OFF_PREIND,
		DES_UND,
		DES_STR_M_ASR_IMM_OFF_PREIND,
		DES_UND,
		DES_STR_M_ROR_IMM_OFF_PREIND,
		DES_UND,

		DES_LDR_M_LSL_IMM_OFF_PREIND,
		DES_UND,
		DES_LDR_M_LSR_IMM_OFF_PREIND,
		DES_UND,
		DES_LDR_M_ASR_IMM_OFF_PREIND,
		DES_UND,
		DES_LDR_M_ROR_IMM_OFF_PREIND,
		DES_UND,
		DES_LDR_M_LSL_IMM_OFF_PREIND,
		DES_UND,
		DES_LDR_M_LSR_IMM_OFF_PREIND,
		DES_UND,
		DES_LDR_M_ASR_IMM_OFF_PREIND,
		DES_UND,
		DES_LDR_M_ROR_IMM_OFF_PREIND,
		DES_UND,
		//------------------------------------------
		DES_STRB_M_LSL_IMM_OFF,	//011 1010 0 0000
		DES_UND,
		DES_STRB_M_LSR_IMM_OFF,
		DES_UND,
		DES_STRB_M_ASR_IMM_OFF,
		DES_UND,
		DES_STRB_M_ROR_IMM_OFF,
		DES_UND,
		DES_STRB_M_LSL_IMM_OFF,
		DES_UND,
		DES_STRB_M_LSR_IMM_OFF,
		DES_UND,
		DES_STRB_M_ASR_IMM_OFF,
		DES_UND,
		DES_STRB_M_ROR_IMM_OFF,
		DES_UND,

		DES_LDRB_M_LSL_IMM_OFF,
		DES_UND,
		DES_LDRB_M_LSR_IMM_OFF,
		DES_UND,
		DES_LDRB_M_ASR_IMM_OFF,
		DES_UND,
		DES_LDRB_M_ROR_IMM_OFF,
		DES_UND,
		DES_LDRB_M_LSL_IMM_OFF,
		DES_UND,
		DES_LDRB_M_LSR_IMM_OFF,
		DES_UND,
		DES_LDRB_M_ASR_IMM_OFF,
		DES_UND,
		DES_LDRB_M_ROR_IMM_OFF,
		DES_UND,
		//------------------------------------------
		DES_STRB_M_LSL_IMM_OFF_PREIND, //011	1011 0 0000
		DES_UND,
		DES_STRB_M_LSR_IMM_OFF_PREIND,
		DES_UND,
		DES_STRB_M_ASR_IMM_OFF_PREIND,
		DES_UND,
		DES_STRB_M_ROR_IMM_OFF_PREIND,
		DES_UND,
		DES_STRB_M_LSL_IMM_OFF_PREIND,
		DES_UND,
		DES_STRB_M_LSR_IMM_OFF_PREIND,
		DES_UND,
		DES_STRB_M_ASR_IMM_OFF_PREIND,
		DES_UND,
		DES_STRB_M_ROR_IMM_OFF_PREIND,
		DES_UND,

		DES_LDRB_M_LSL_IMM_OFF_PREIND,
		DES_UND,
		DES_LDRB_M_LSR_IMM_OFF_PREIND,
		DES_UND,
		DES_LDRB_M_ASR_IMM_OFF_PREIND,
		DES_UND,
		DES_LDRB_M_ROR_IMM_OFF_PREIND,
		DES_UND,
		DES_LDRB_M_LSL_IMM_OFF_PREIND,
		DES_UND,
		DES_LDRB_M_LSR_IMM_OFF_PREIND,
		DES_UND,
		DES_LDRB_M_ASR_IMM_OFF_PREIND,
		DES_UND,
		DES_LDRB_M_ROR_IMM_OFF_PREIND,
		DES_UND,
		//------------------------------------------
		DES_STR_P_LSL_IMM_OFF, //011	1100 0 0000
		DES_UND,
		DES_STR_P_LSR_IMM_OFF,
		DES_UND,
		DES_STR_P_ASR_IMM_OFF,
		DES_UND,
		DES_STR_P_ROR_IMM_OFF,
		DES_UND,
		DES_STR_P_LSL_IMM_OFF,
		DES_UND,
		DES_STR_P_LSR_IMM_OFF,
		DES_UND,
		DES_STR_P_ASR_IMM_OFF,
		DES_UND,
		DES_STR_P_ROR_IMM_OFF,
		DES_UND,

		DES_LDR_P_LSL_IMM_OFF,
		DES_UND,
		DES_LDR_P_LSR_IMM_OFF,
		DES_UND,
		DES_LDR_P_ASR_IMM_OFF,
		DES_UND,
		DES_LDR_P_ROR_IMM_OFF,
		DES_UND,
		DES_LDR_P_LSL_IMM_OFF,
		DES_UND,
		DES_LDR_P_LSR_IMM_OFF,
		DES_UND,
		DES_LDR_P_ASR_IMM_OFF,
		DES_UND,
		DES_LDR_P_ROR_IMM_OFF,
		DES_UND,
		//------------------------------------------
		DES_STR_P_LSL_IMM_OFF_PREIND, //011 1101	0 0000
		DES_UND,
		DES_STR_P_LSR_IMM_OFF_PREIND,
		DES_UND,
		DES_STR_P_ASR_IMM_OFF_PREIND,
		DES_UND,
		DES_STR_P_ROR_IMM_OFF_PREIND,
		DES_UND,
		DES_STR_P_LSL_IMM_OFF_PREIND,
		DES_UND,
		DES_STR_P_LSR_IMM_OFF_PREIND,
		DES_UND,
		DES_STR_P_ASR_IMM_OFF_PREIND,
		DES_UND,
		DES_STR_P_ROR_IMM_OFF_PREIND,
		DES_UND,

		DES_LDR_P_LSL_IMM_OFF_PREIND,
		DES_UND,
		DES_LDR_P_LSR_IMM_OFF_PREIND,
		DES_UND,
		DES_LDR_P_ASR_IMM_OFF_PREIND,
		DES_UND,
		DES_LDR_P_ROR_IMM_OFF_PREIND,
		DES_UND,
		DES_LDR_P_LSL_IMM_OFF_PREIND,
		DES_UND,
		DES_LDR_P_LSR_IMM_OFF_PREIND,
		DES_UND,
		DES_LDR_P_ASR_IMM_OFF_PREIND,
		DES_UND,
		DES_LDR_P_ROR_IMM_OFF_PREIND,
		DES_UND,
		//------------------------------------------
		DES_STRB_P_LSL_IMM_OFF, //011 1110 0	0000
		DES_UND,
		DES_STRB_P_LSR_IMM_OFF,
		DES_UND,
		DES_STRB_P_ASR_IMM_OFF,
		DES_UND,
		DES_STRB_P_ROR_IMM_OFF,
		DES_UND,
		DES_STRB_P_LSL_IMM_OFF,
		DES_UND,
		DES_STRB_P_LSR_IMM_OFF,
		DES_UND,
		DES_STRB_P_ASR_IMM_OFF,
		DES_UND,
		DES_STRB_P_ROR_IMM_OFF,
		DES_UND,

		DES_LDRB_P_LSL_IMM_OFF,
		DES_UND,
		DES_LDRB_P_LSR_IMM_OFF,
		DES_UND,
		DES_LDRB_P_ASR_IMM_OFF,
		DES_UND,
		DES_LDRB_P_ROR_IMM_OFF,
		DES_UND,
		DES_LDRB_P_LSL_IMM_OFF,
		DES_UND,
		DES_LDRB_P_LSR_IMM_OFF,
		DES_UND,
		DES_LDRB_P_ASR_IMM_OFF,
		DES_UND,
		DES_LDRB_P_ROR_IMM_OFF,
		DES_UND,
		//------------------------------------------
		DES_STRB_P_LSL_IMM_OFF_PREIND, //011	1111 0 0000
		DES_UND,
		DES_STRB_P_LSR_IMM_OFF_PREIND,
		DES_UND,
		DES_STRB_P_ASR_IMM_OFF_PREIND,
		DES_UND,
		DES_STRB_P_ROR_IMM_OFF_PREIND,
		DES_UND,
		DES_STRB_P_LSL_IMM_OFF_PREIND,
		DES_UND,
		DES_STRB_P_LSR_IMM_OFF_PREIND,
		DES_UND,
		DES_STRB_P_ASR_IMM_OFF_PREIND,
		DES_UND,
		DES_STRB_P_ROR_IMM_OFF_PREIND,
		DES_UND,

		DES_LDRB_P_LSL_IMM_OFF_PREIND,
		DES_UND,
		DES_LDRB_P_LSR_IMM_OFF_PREIND,
		DES_UND,
		DES_LDRB_P_ASR_IMM_OFF_PREIND,
		DES_UND,
		DES_LDRB_P_ROR_IMM_OFF_PREIND,
		DES_UND,
		DES_LDRB_P_LSL_IMM_OFF_PREIND,
		DES_UND,
		DES_LDRB_P_LSR_IMM_OFF_PREIND,
		DES_UND,
		DES_LDRB_P_ASR_IMM_OFF_PREIND,
		DES_UND,
		DES_LDRB_P_ROR_IMM_OFF_PREIND,
		DES_UND,			  //011	1111 1 1111
		//------------------------------------------
		DES_STMDA, //100	0000 0 0000
		DES_STMDA,
		DES_STMDA,
		DES_STMDA,
		DES_STMDA,
		DES_STMDA,
		DES_STMDA,
		DES_STMDA,
		DES_STMDA,
		DES_STMDA,
		DES_STMDA,
		DES_STMDA,
		DES_STMDA,
		DES_STMDA,
		DES_STMDA,
		DES_STMDA,

		DES_LDMDA,
		DES_LDMDA,
		DES_LDMDA,
		DES_LDMDA,
		DES_LDMDA,
		DES_LDMDA,
		DES_LDMDA,
		DES_LDMDA,
		DES_LDMDA,
		DES_LDMDA,
		DES_LDMDA,
		DES_LDMDA,
		DES_LDMDA,
		DES_LDMDA,
		DES_LDMDA,
		DES_LDMDA,
		//------------------------------------------
		DES_STMDA_W,	//100 0001 0 0000
		DES_STMDA_W,
		DES_STMDA_W,
		DES_STMDA_W,
		DES_STMDA_W,
		DES_STMDA_W,
		DES_STMDA_W,
		DES_STMDA_W,
		DES_STMDA_W,
		DES_STMDA_W,
		DES_STMDA_W,
		DES_STMDA_W,
		DES_STMDA_W,
		DES_STMDA_W,
		DES_STMDA_W,
		DES_STMDA_W,

		DES_LDMDA_W,
		DES_LDMDA_W,
		DES_LDMDA_W,
		DES_LDMDA_W,
		DES_LDMDA_W,
		DES_LDMDA_W,
		DES_LDMDA_W,
		DES_LDMDA_W,
		DES_LDMDA_W,
		DES_LDMDA_W,
		DES_LDMDA_W,
		DES_LDMDA_W,
		DES_LDMDA_W,
		DES_LDMDA_W,
		DES_LDMDA_W,
		DES_LDMDA_W,
		//------------------------------------------
		DES_STMDA2, //100 0010 0	0000
		DES_STMDA2,
		DES_STMDA2,
		DES_STMDA2,
		DES_STMDA2,
		DES_STMDA2,
		DES_STMDA2,
		DES_STMDA2,
		DES_STMDA2,
		DES_STMDA2,
		DES_STMDA2,
		DES_STMDA2,
		DES_STMDA2,
		DES_STMDA2,
		DES_STMDA2,
		DES_STMDA2,

		DES_LDMDA2,
		DES_LDMDA2,
		DES_LDMDA2,
		DES_LDMDA2,
		DES_LDMDA2,
		DES_LDMDA2,
		DES_LDMDA2,
		DES_LDMDA2,
		DES_LDMDA2,
		DES_LDMDA2,
		DES_LDMDA2,
		DES_LDMDA2,
		DES_LDMDA2,
		DES_LDMDA2,
		DES_LDMDA2,
		DES_LDMDA2,
		//------------------------------------------
		DES_STMDA2_W, //100 0011	0 0000
		DES_STMDA2_W,
		DES_STMDA2_W,
		DES_STMDA2_W,
		DES_STMDA2_W,
		DES_STMDA2_W,
		DES_STMDA2_W,
		DES_STMDA2_W,
		DES_STMDA2_W,
		DES_STMDA2_W,
		DES_STMDA2_W,
		DES_STMDA2_W,
		DES_STMDA2_W,
		DES_STMDA2_W,
		DES_STMDA2_W,
		DES_STMDA2_W,

		DES_LDMDA2_W,
		DES_LDMDA2_W,
		DES_LDMDA2_W,
		DES_LDMDA2_W,
		DES_LDMDA2_W,
		DES_LDMDA2_W,
		DES_LDMDA2_W,
		DES_LDMDA2_W,
		DES_LDMDA2_W,
		DES_LDMDA2_W,
		DES_LDMDA2_W,
		DES_LDMDA2_W,
		DES_LDMDA2_W,
		DES_LDMDA2_W,
		DES_LDMDA2_W,
		DES_LDMDA2_W,
		//------------------------------------------
		DES_STMIA, //100	0100 0 0000
		DES_STMIA,
		DES_STMIA,
		DES_STMIA,
		DES_STMIA,
		DES_STMIA,
		DES_STMIA,
		DES_STMIA,
		DES_STMIA,
		DES_STMIA,
		DES_STMIA,
		DES_STMIA,
		DES_STMIA,
		DES_STMIA,
		DES_STMIA,
		DES_STMIA,

		DES_LDMIA,
		DES_LDMIA,
		DES_LDMIA,
		DES_LDMIA,
		DES_LDMIA,
		DES_LDMIA,
		DES_LDMIA,
		DES_LDMIA,
		DES_LDMIA,
		DES_LDMIA,
		DES_LDMIA,
		DES_LDMIA,
		DES_LDMIA,
		DES_LDMIA,
		DES_LDMIA,
		DES_LDMIA,
		//------------------------------------------
		DES_STMIA_W,	//100 0101 0 0000
		DES_STMIA_W,
		DES_STMIA_W,
		DES_STMIA_W,
		DES_STMIA_W,
		DES_STMIA_W,
		DES_STMIA_W,
		DES_STMIA_W,
		DES_STMIA_W,
		DES_STMIA_W,
		DES_STMIA_W,
		DES_STMIA_W,
		DES_STMIA_W,
		DES_STMIA_W,
		DES_STMIA_W,
		DES_STMIA_W,

		DES_LDMIA_W,
		DES_LDMIA_W,
		DES_LDMIA_W,
		DES_LDMIA_W,
		DES_LDMIA_W,
		DES_LDMIA_W,
		DES_LDMIA_W,
		DES_LDMIA_W,
		DES_LDMIA_W,
		DES_LDMIA_W,
		DES_LDMIA_W,
		DES_LDMIA_W,
		DES_LDMIA_W,
		DES_LDMIA_W,
		DES_LDMIA_W,
		DES_LDMIA_W,
		//------------------------------------------
		DES_STMIA2, //100 0110 0	0000
		DES_STMIA2,
		DES_STMIA2,
		DES_STMIA2,
		DES_STMIA2,
		DES_STMIA2,
		DES_STMIA2,
		DES_STMIA2,
		DES_STMIA2,
		DES_STMIA2,
		DES_STMIA2,
		DES_STMIA2,
		DES_STMIA2,
		DES_STMIA2,
		DES_STMIA2,
		DES_STMIA2,

		DES_LDMIA2,
		DES_LDMIA2,
		DES_LDMIA2,
		DES_LDMIA2,
		DES_LDMIA2,
		DES_LDMIA2,
		DES_LDMIA2,
		DES_LDMIA2,
		DES_LDMIA2,
		DES_LDMIA2,
		DES_LDMIA2,
		DES_LDMIA2,
		DES_LDMIA2,
		DES_LDMIA2,
		DES_LDMIA2,
		DES_LDMIA2,
		//------------------------------------------
		DES_STMIA2_W, //100 0111	0 0000
		DES_STMIA2_W,
		DES_STMIA2_W,
		DES_STMIA2_W,
		DES_STMIA2_W,
		DES_STMIA2_W,
		DES_STMIA2_W,
		DES_STMIA2_W,
		DES_STMIA2_W,
		DES_STMIA2_W,
		DES_STMIA2_W,
		DES_STMIA2_W,
		DES_STMIA2_W,
		DES_STMIA2_W,
		DES_STMIA2_W,
		DES_STMIA2_W,

		DES_LDMIA2_W,
		DES_LDMIA2_W,
		DES_LDMIA2_W,
		DES_LDMIA2_W,
		DES_LDMIA2_W,
		DES_LDMIA2_W,
		DES_LDMIA2_W,
		DES_LDMIA2_W,
		DES_LDMIA2_W,
		DES_LDMIA2_W,
		DES_LDMIA2_W,
		DES_LDMIA2_W,
		DES_LDMIA2_W,
		DES_LDMIA2_W,
		DES_LDMIA2_W,
		DES_LDMIA2_W,
		//------------------------------------------
		DES_STMDB, //100	1000 0 0000
		DES_STMDB,
		DES_STMDB,
		DES_STMDB,
		DES_STMDB,
		DES_STMDB,
		DES_STMDB,
		DES_STMDB,
		DES_STMDB,
		DES_STMDB,
		DES_STMDB,
		DES_STMDB,
		DES_STMDB,
		DES_STMDB,
		DES_STMDB,
		DES_STMDB,

		DES_LDMDB,
		DES_LDMDB,
		DES_LDMDB,
		DES_LDMDB,
		DES_LDMDB,
		DES_LDMDB,
		DES_LDMDB,
		DES_LDMDB,
		DES_LDMDB,
		DES_LDMDB,
		DES_LDMDB,
		DES_LDMDB,
		DES_LDMDB,
		DES_LDMDB,
		DES_LDMDB,
		DES_LDMDB,
		//------------------------------------------
		DES_STMDB_W,	//100 1001 0 0000
		DES_STMDB_W,
		DES_STMDB_W,
		DES_STMDB_W,
		DES_STMDB_W,
		DES_STMDB_W,
		DES_STMDB_W,
		DES_STMDB_W,
		DES_STMDB_W,
		DES_STMDB_W,
		DES_STMDB_W,
		DES_STMDB_W,
		DES_STMDB_W,
		DES_STMDB_W,
		DES_STMDB_W,
		DES_STMDB_W,

		DES_LDMDB_W,
		DES_LDMDB_W,
		DES_LDMDB_W,
		DES_LDMDB_W,
		DES_LDMDB_W,
		DES_LDMDB_W,
		DES_LDMDB_W,
		DES_LDMDB_W,
		DES_LDMDB_W,
		DES_LDMDB_W,
		DES_LDMDB_W,
		DES_LDMDB_W,
		DES_LDMDB_W,
		DES_LDMDB_W,
		DES_LDMDB_W,
		DES_LDMDB_W,
		//------------------------------------------
		DES_STMDB2, //100 1010 0	0000
		DES_STMDB2,
		DES_STMDB2,
		DES_STMDB2,
		DES_STMDB2,
		DES_STMDB2,
		DES_STMDB2,
		DES_STMDB2,
		DES_STMDB2,
		DES_STMDB2,
		DES_STMDB2,
		DES_STMDB2,
		DES_STMDB2,
		DES_STMDB2,
		DES_STMDB2,
		DES_STMDB2,

		DES_LDMDB2,
		DES_LDMDB2,
		DES_LDMDB2,
		DES_LDMDB2,
		DES_LDMDB2,
		DES_LDMDB2,
		DES_LDMDB2,
		DES_LDMDB2,
		DES_LDMDB2,
		DES_LDMDB2,
		DES_LDMDB2,
		DES_LDMDB2,
		DES_LDMDB2,
		DES_LDMDB2,
		DES_LDMDB2,
		DES_LDMDB2,
		//------------------------------------------
		DES_STMDB2_W, //100 1011	0 0000
		DES_STMDB2_W,
		DES_STMDB2_W,
		DES_STMDB2_W,
		DES_STMDB2_W,
		DES_STMDB2_W,
		DES_STMDB2_W,
		DES_STMDB2_W,
		DES_STMDB2_W,
		DES_STMDB2_W,
		DES_STMDB2_W,
		DES_STMDB2_W,
		DES_STMDB2_W,
		DES_STMDB2_W,
		DES_STMDB2_W,
		DES_STMDB2_W,

		DES_LDMDB2_W,
		DES_LDMDB2_W,
		DES_LDMDB2_W,
		DES_LDMDB2_W,
		DES_LDMDB2_W,
		DES_LDMDB2_W,
		DES_LDMDB2_W,
		DES_LDMDB2_W,
		DES_LDMDB2_W,
		DES_LDMDB2_W,
		DES_LDMDB2_W,
		DES_LDMDB2_W,
		DES_LDMDB2_W,
		DES_LDMDB2_W,
		DES_LDMDB2_W,
		DES_LDMDB2_W,
		//------------------------------------------


		DES_STMIB, //100	1100 0 0000
		DES_STMIB,
		DES_STMIB,
		DES_STMIB,
		DES_STMIB,
		DES_STMIB,
		DES_STMIB,
		DES_STMIB,
		DES_STMIB,
		DES_STMIB,
		DES_STMIB,
		DES_STMIB,
		DES_STMIB,
		DES_STMIB,
		DES_STMIB,
		DES_STMIB,

		DES_LDMIB,
		DES_LDMIB,
		DES_LDMIB,
		DES_LDMIB,
		DES_LDMIB,
		DES_LDMIB,
		DES_LDMIB,
		DES_LDMIB,
		DES_LDMIB,
		DES_LDMIB,
		DES_LDMIB,
		DES_LDMIB,
		DES_LDMIB,
		DES_LDMIB,
		DES_LDMIB,
		DES_LDMIB,
		//------------------------------------------
		DES_STMIB_W,	//100 1101 0 0000
		DES_STMIB_W,
		DES_STMIB_W,
		DES_STMIB_W,
		DES_STMIB_W,
		DES_STMIB_W,
		DES_STMIB_W,
		DES_STMIB_W,
		DES_STMIB_W,
		DES_STMIB_W,
		DES_STMIB_W,
		DES_STMIB_W,
		DES_STMIB_W,
		DES_STMIB_W,
		DES_STMIB_W,
		DES_STMIB_W,

		DES_LDMIB_W,
		DES_LDMIB_W,
		DES_LDMIB_W,
		DES_LDMIB_W,
		DES_LDMIB_W,
		DES_LDMIB_W,
		DES_LDMIB_W,
		DES_LDMIB_W,
		DES_LDMIB_W,
		DES_LDMIB_W,
		DES_LDMIB_W,
		DES_LDMIB_W,
		DES_LDMIB_W,
		DES_LDMIB_W,
		DES_LDMIB_W,
		DES_LDMIB_W,
		//------------------------------------------
		DES_STMIB2, //100 1110 0	0000
		DES_STMIB2,
		DES_STMIB2,
		DES_STMIB2,
		DES_STMIB2,
		DES_STMIB2,
		DES_STMIB2,
		DES_STMIB2,
		DES_STMIB2,
		DES_STMIB2,
		DES_STMIB2,
		DES_STMIB2,
		DES_STMIB2,
		DES_STMIB2,
		DES_STMIB2,
		DES_STMIB2,

		DES_LDMIB2,
		DES_LDMIB2,
		DES_LDMIB2,
		DES_LDMIB2,
		DES_LDMIB2,
		DES_LDMIB2,
		DES_LDMIB2,
		DES_LDMIB2,
		DES_LDMIB2,
		DES_LDMIB2,
		DES_LDMIB2,
		DES_LDMIB2,
		DES_LDMIB2,
		DES_LDMIB2,
		DES_LDMIB2,
		DES_LDMIB2,
		//------------------------------------------
		DES_STMIB2_W, //100 1111	0 0000
		DES_STMIB2_W,
		DES_STMIB2_W,
		DES_STMIB2_W,
		DES_STMIB2_W,
		DES_STMIB2_W,
		DES_STMIB2_W,
		DES_STMIB2_W,
		DES_STMIB2_W,
		DES_STMIB2_W,
		DES_STMIB2_W,
		DES_STMIB2_W,
		DES_STMIB2_W,
		DES_STMIB2_W,
		DES_STMIB2_W,
		DES_STMIB2_W,

		DES_LDMIB2_W,
		DES_LDMIB2_W,
		DES_LDMIB2_W,
		DES_LDMIB2_W,
		DES_LDMIB2_W,
		DES_LDMIB2_W,
		DES_LDMIB2_W,
		DES_LDMIB2_W,
		DES_LDMIB2_W,
		DES_LDMIB2_W,
		DES_LDMIB2_W,
		DES_LDMIB2_W,
		DES_LDMIB2_W,
		DES_LDMIB2_W,
		DES_LDMIB2_W,
		DES_LDMIB2_W,
		//------------------------------------------
		DES_B, //101	0000 0 0000
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		//------------------------------------------
		DES_B, //101	0001 0 0000
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		//------------------------------------------
		DES_B, //101	0010 0 0000
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		//------------------------------------------
		DES_B, //101	0011 0 0000
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		//------------------------------------------
		DES_B, //101	0100 0 0000
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		//------------------------------------------
		DES_B, //101	0101 0 0000
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		//------------------------------------------
		DES_B, //101	0110 0 0000
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		//------------------------------------------
		DES_B, //101	0111 0 0000
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		DES_B,
		//------------------------------------------
		DES_BL, //101 1000 0	0000
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		//------------------------------------------
		DES_BL, //101 1001 0	0000
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		//------------------------------------------
		DES_BL, //101 1010 0	0000
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		//------------------------------------------
		DES_BL, //101 1011 0	0000
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		//------------------------------------------
		DES_BL, //101 1100 0	0000
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		//------------------------------------------
		DES_BL, //101 1101 0	0000
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		//------------------------------------------
		DES_BL, //101 1110 0	0000
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		//------------------------------------------
		DES_BL, //101 1111 0	0000
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		DES_BL,
		//------------------------------------------
		DES_STC_OPTION,	//110 0000 0 0000
		DES_STC_OPTION,
		DES_STC_OPTION,
		DES_STC_OPTION,
		DES_STC_OPTION,
		DES_STC_OPTION,
		DES_STC_OPTION,
		DES_STC_OPTION,
		DES_STC_OPTION,
		DES_STC_OPTION,
		DES_STC_OPTION,
		DES_STC_OPTION,
		DES_STC_OPTION,
		DES_STC_OPTION,
		DES_STC_OPTION,
		DES_STC_OPTION,

		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		//------------------------------------------
		DES_STC_M_POSTIND,  //110 0001 0	0000
		DES_STC_M_POSTIND,
		DES_STC_M_POSTIND,
		DES_STC_M_POSTIND,
		DES_STC_M_POSTIND,
		DES_STC_M_POSTIND,
		DES_STC_M_POSTIND,
		DES_STC_M_POSTIND,
		DES_STC_M_POSTIND,
		DES_STC_M_POSTIND,
		DES_STC_M_POSTIND,
		DES_STC_M_POSTIND,
		DES_STC_M_POSTIND,
		DES_STC_M_POSTIND,
		DES_STC_M_POSTIND,
		DES_STC_M_POSTIND,

		DES_LDC_M_POSTIND,
		DES_LDC_M_POSTIND,
		DES_LDC_M_POSTIND,
		DES_LDC_M_POSTIND,
		DES_LDC_M_POSTIND,
		DES_LDC_M_POSTIND,
		DES_LDC_M_POSTIND,
		DES_LDC_M_POSTIND,
		DES_LDC_M_POSTIND,
		DES_LDC_M_POSTIND,
		DES_LDC_M_POSTIND,
		DES_LDC_M_POSTIND,
		DES_LDC_M_POSTIND,
		DES_LDC_M_POSTIND,
		DES_LDC_M_POSTIND,
		DES_LDC_M_POSTIND,
		//------------------------------------------
		DES_STC_OPTION,	//110 0010 0 0000
		DES_STC_OPTION,
		DES_STC_OPTION,
		DES_STC_OPTION,
		DES_STC_OPTION,
		DES_STC_OPTION,
		DES_STC_OPTION,
		DES_STC_OPTION,
		DES_STC_OPTION,
		DES_STC_OPTION,
		DES_STC_OPTION,
		DES_STC_OPTION,
		DES_STC_OPTION,
		DES_STC_OPTION,
		DES_STC_OPTION,
		DES_STC_OPTION,

		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		//------------------------------------------
		DES_STC_M_POSTIND,  //110 0011 0	0000
		DES_STC_M_POSTIND,
		DES_STC_M_POSTIND,
		DES_STC_M_POSTIND,
		DES_STC_M_POSTIND,
		DES_STC_M_POSTIND,
		DES_STC_M_POSTIND,
		DES_STC_M_POSTIND,
		DES_STC_M_POSTIND,
		DES_STC_M_POSTIND,
		DES_STC_M_POSTIND,
		DES_STC_M_POSTIND,
		DES_STC_M_POSTIND,
		DES_STC_M_POSTIND,
		DES_STC_M_POSTIND,
		DES_STC_M_POSTIND,

		DES_LDC_M_POSTIND,
		DES_LDC_M_POSTIND,
		DES_LDC_M_POSTIND,
		DES_LDC_M_POSTIND,
		DES_LDC_M_POSTIND,
		DES_LDC_M_POSTIND,
		DES_LDC_M_POSTIND,
		DES_LDC_M_POSTIND,
		DES_LDC_M_POSTIND,
		DES_LDC_M_POSTIND,
		DES_LDC_M_POSTIND,
		DES_LDC_M_POSTIND,
		DES_LDC_M_POSTIND,
		DES_LDC_M_POSTIND,
		DES_LDC_M_POSTIND,
		DES_LDC_M_POSTIND,
		//------------------------------------------
		DES_STC_OPTION,	//110 0100 0 0000
		DES_STC_OPTION,
		DES_STC_OPTION,
		DES_STC_OPTION,
		DES_STC_OPTION,
		DES_STC_OPTION,
		DES_STC_OPTION,
		DES_STC_OPTION,
		DES_STC_OPTION,
		DES_STC_OPTION,
		DES_STC_OPTION,
		DES_STC_OPTION,
		DES_STC_OPTION,
		DES_STC_OPTION,
		DES_STC_OPTION,
		DES_STC_OPTION,

		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		//------------------------------------------
		DES_STC_P_POSTIND,  //110 0101 0	0000
		DES_STC_P_POSTIND,
		DES_STC_P_POSTIND,
		DES_STC_P_POSTIND,
		DES_STC_P_POSTIND,
		DES_STC_P_POSTIND,
		DES_STC_P_POSTIND,
		DES_STC_P_POSTIND,
		DES_STC_P_POSTIND,
		DES_STC_P_POSTIND,
		DES_STC_P_POSTIND,
		DES_STC_P_POSTIND,
		DES_STC_P_POSTIND,
		DES_STC_P_POSTIND,
		DES_STC_P_POSTIND,
		DES_STC_P_POSTIND,

		DES_LDC_P_POSTIND,
		DES_LDC_P_POSTIND,
		DES_LDC_P_POSTIND,
		DES_LDC_P_POSTIND,
		DES_LDC_P_POSTIND,
		DES_LDC_P_POSTIND,
		DES_LDC_P_POSTIND,
		DES_LDC_P_POSTIND,
		DES_LDC_P_POSTIND,
		DES_LDC_P_POSTIND,
		DES_LDC_P_POSTIND,
		DES_LDC_P_POSTIND,
		DES_LDC_P_POSTIND,
		DES_LDC_P_POSTIND,
		DES_LDC_P_POSTIND,
		DES_LDC_P_POSTIND,
		//------------------------------------------
		DES_STC_OPTION,	//110 0110 0 0000
		DES_STC_OPTION,
		DES_STC_OPTION,
		DES_STC_OPTION,
		DES_STC_OPTION,
		DES_STC_OPTION,
		DES_STC_OPTION,
		DES_STC_OPTION,
		DES_STC_OPTION,
		DES_STC_OPTION,
		DES_STC_OPTION,
		DES_STC_OPTION,
		DES_STC_OPTION,
		DES_STC_OPTION,
		DES_STC_OPTION,
		DES_STC_OPTION,

		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		DES_LDC_OPTION,
		//------------------------------------------
		DES_STC_P_POSTIND,  //110 0111 0	0000
		DES_STC_P_POSTIND,
		DES_STC_P_POSTIND,
		DES_STC_P_POSTIND,
		DES_STC_P_POSTIND,
		DES_STC_P_POSTIND,
		DES_STC_P_POSTIND,
		DES_STC_P_POSTIND,
		DES_STC_P_POSTIND,
		DES_STC_P_POSTIND,
		DES_STC_P_POSTIND,
		DES_STC_P_POSTIND,
		DES_STC_P_POSTIND,
		DES_STC_P_POSTIND,
		DES_STC_P_POSTIND,
		DES_STC_P_POSTIND,

		DES_LDC_P_POSTIND,
		DES_LDC_P_POSTIND,
		DES_LDC_P_POSTIND,
		DES_LDC_P_POSTIND,
		DES_LDC_P_POSTIND,
		DES_LDC_P_POSTIND,
		DES_LDC_P_POSTIND,
		DES_LDC_P_POSTIND,
		DES_LDC_P_POSTIND,
		DES_LDC_P_POSTIND,
		DES_LDC_P_POSTIND,
		DES_LDC_P_POSTIND,
		DES_LDC_P_POSTIND,
		DES_LDC_P_POSTIND,
		DES_LDC_P_POSTIND,
		DES_LDC_P_POSTIND,
		//------------------------------------------
		DES_STC_M_IMM_OFF,  //110 1000 0	0000
		DES_STC_M_IMM_OFF,
		DES_STC_M_IMM_OFF,
		DES_STC_M_IMM_OFF,
		DES_STC_M_IMM_OFF,
		DES_STC_M_IMM_OFF,
		DES_STC_M_IMM_OFF,
		DES_STC_M_IMM_OFF,
		DES_STC_M_IMM_OFF,
		DES_STC_M_IMM_OFF,
		DES_STC_M_IMM_OFF,
		DES_STC_M_IMM_OFF,
		DES_STC_M_IMM_OFF,
		DES_STC_M_IMM_OFF,
		DES_STC_M_IMM_OFF,
		DES_STC_M_IMM_OFF,

		DES_LDC_M_IMM_OFF,
		DES_LDC_M_IMM_OFF,
		DES_LDC_M_IMM_OFF,
		DES_LDC_M_IMM_OFF,
		DES_LDC_M_IMM_OFF,
		DES_LDC_M_IMM_OFF,
		DES_LDC_M_IMM_OFF,
		DES_LDC_M_IMM_OFF,
		DES_LDC_M_IMM_OFF,
		DES_LDC_M_IMM_OFF,
		DES_LDC_M_IMM_OFF,
		DES_LDC_M_IMM_OFF,
		DES_LDC_M_IMM_OFF,
		DES_LDC_M_IMM_OFF,
		DES_LDC_M_IMM_OFF,
		DES_LDC_M_IMM_OFF,
		//------------------------------------------
		DES_STC_M_PREIND,  //110	1001 0 0000
		DES_STC_M_PREIND,
		DES_STC_M_PREIND,
		DES_STC_M_PREIND,
		DES_STC_M_PREIND,
		DES_STC_M_PREIND,
		DES_STC_M_PREIND,
		DES_STC_M_PREIND,
		DES_STC_M_PREIND,
		DES_STC_M_PREIND,
		DES_STC_M_PREIND,
		DES_STC_M_PREIND,
		DES_STC_M_PREIND,
		DES_STC_M_PREIND,
		DES_STC_M_PREIND,
		DES_STC_M_PREIND,

		DES_LDC_M_PREIND,
		DES_LDC_M_PREIND,
		DES_LDC_M_PREIND,
		DES_LDC_M_PREIND,
		DES_LDC_M_PREIND,
		DES_LDC_M_PREIND,
		DES_LDC_M_PREIND,
		DES_LDC_M_PREIND,
		DES_LDC_M_PREIND,
		DES_LDC_M_PREIND,
		DES_LDC_M_PREIND,
		DES_LDC_M_PREIND,
		DES_LDC_M_PREIND,
		DES_LDC_M_PREIND,
		DES_LDC_M_PREIND,
		DES_LDC_M_PREIND,
		//------------------------------------------
		DES_STC_M_IMM_OFF,  //110 1010 0	0000
		DES_STC_M_IMM_OFF,
		DES_STC_M_IMM_OFF,
		DES_STC_M_IMM_OFF,
		DES_STC_M_IMM_OFF,
		DES_STC_M_IMM_OFF,
		DES_STC_M_IMM_OFF,
		DES_STC_M_IMM_OFF,
		DES_STC_M_IMM_OFF,
		DES_STC_M_IMM_OFF,
		DES_STC_M_IMM_OFF,
		DES_STC_M_IMM_OFF,
		DES_STC_M_IMM_OFF,
		DES_STC_M_IMM_OFF,
		DES_STC_M_IMM_OFF,
		DES_STC_M_IMM_OFF,

		DES_LDC_M_IMM_OFF,
		DES_LDC_M_IMM_OFF,
		DES_LDC_M_IMM_OFF,
		DES_LDC_M_IMM_OFF,
		DES_LDC_M_IMM_OFF,
		DES_LDC_M_IMM_OFF,
		DES_LDC_M_IMM_OFF,
		DES_LDC_M_IMM_OFF,
		DES_LDC_M_IMM_OFF,
		DES_LDC_M_IMM_OFF,
		DES_LDC_M_IMM_OFF,
		DES_LDC_M_IMM_OFF,
		DES_LDC_M_IMM_OFF,
		DES_LDC_M_IMM_OFF,
		DES_LDC_M_IMM_OFF,
		DES_LDC_M_IMM_OFF,
		//------------------------------------------
		DES_STC_M_PREIND,  //110	1011 0 0000
		DES_STC_M_PREIND,
		DES_STC_M_PREIND,
		DES_STC_M_PREIND,
		DES_STC_M_PREIND,
		DES_STC_M_PREIND,
		DES_STC_M_PREIND,
		DES_STC_M_PREIND,
		DES_STC_M_PREIND,
		DES_STC_M_PREIND,
		DES_STC_M_PREIND,
		DES_STC_M_PREIND,
		DES_STC_M_PREIND,
		DES_STC_M_PREIND,
		DES_STC_M_PREIND,
		DES_STC_M_PREIND,

		DES_LDC_M_PREIND,
		DES_LDC_M_PREIND,
		DES_LDC_M_PREIND,
		DES_LDC_M_PREIND,
		DES_LDC_M_PREIND,
		DES_LDC_M_PREIND,
		DES_LDC_M_PREIND,
		DES_LDC_M_PREIND,
		DES_LDC_M_PREIND,
		DES_LDC_M_PREIND,
		DES_LDC_M_PREIND,
		DES_LDC_M_PREIND,
		DES_LDC_M_PREIND,
		DES_LDC_M_PREIND,
		DES_LDC_M_PREIND,
		DES_LDC_M_PREIND,
		//------------------------------------------
		DES_STC_P_IMM_OFF,  //110 1100 0	0000
		DES_STC_P_IMM_OFF,
		DES_STC_P_IMM_OFF,
		DES_STC_P_IMM_OFF,
		DES_STC_P_IMM_OFF,
		DES_STC_P_IMM_OFF,
		DES_STC_P_IMM_OFF,
		DES_STC_P_IMM_OFF,
		DES_STC_P_IMM_OFF,
		DES_STC_P_IMM_OFF,
		DES_STC_P_IMM_OFF,
		DES_STC_P_IMM_OFF,
		DES_STC_P_IMM_OFF,
		DES_STC_P_IMM_OFF,
		DES_STC_P_IMM_OFF,
		DES_STC_P_IMM_OFF,

		DES_LDC_P_IMM_OFF,
		DES_LDC_P_IMM_OFF,
		DES_LDC_P_IMM_OFF,
		DES_LDC_P_IMM_OFF,
		DES_LDC_P_IMM_OFF,
		DES_LDC_P_IMM_OFF,
		DES_LDC_P_IMM_OFF,
		DES_LDC_P_IMM_OFF,
		DES_LDC_P_IMM_OFF,
		DES_LDC_P_IMM_OFF,
		DES_LDC_P_IMM_OFF,
		DES_LDC_P_IMM_OFF,
		DES_LDC_P_IMM_OFF,
		DES_LDC_P_IMM_OFF,
		DES_LDC_P_IMM_OFF,
		DES_LDC_P_IMM_OFF,
		//------------------------------------------
		DES_STC_P_PREIND,  //110	1101 0 0000
		DES_STC_P_PREIND,
		DES_STC_P_PREIND,
		DES_STC_P_PREIND,
		DES_STC_P_PREIND,
		DES_STC_P_PREIND,
		DES_STC_P_PREIND,
		DES_STC_P_PREIND,
		DES_STC_P_PREIND,
		DES_STC_P_PREIND,
		DES_STC_P_PREIND,
		DES_STC_P_PREIND,
		DES_STC_P_PREIND,
		DES_STC_P_PREIND,
		DES_STC_P_PREIND,
		DES_STC_P_PREIND,

		DES_LDC_P_PREIND,
		DES_LDC_P_PREIND,
		DES_LDC_P_PREIND,
		DES_LDC_P_PREIND,
		DES_LDC_P_PREIND,
		DES_LDC_P_PREIND,
		DES_LDC_P_PREIND,
		DES_LDC_P_PREIND,
		DES_LDC_P_PREIND,
		DES_LDC_P_PREIND,
		DES_LDC_P_PREIND,
		DES_LDC_P_PREIND,
		DES_LDC_P_PREIND,
		DES_LDC_P_PREIND,
		DES_LDC_P_PREIND,
		DES_LDC_P_PREIND,
		//------------------------------------------
		DES_STC_P_IMM_OFF,  //110 1110 0	0000
		DES_STC_P_IMM_OFF,
		DES_STC_P_IMM_OFF,
		DES_STC_P_IMM_OFF,
		DES_STC_P_IMM_OFF,
		DES_STC_P_IMM_OFF,
		DES_STC_P_IMM_OFF,
		DES_STC_P_IMM_OFF,
		DES_STC_P_IMM_OFF,
		DES_STC_P_IMM_OFF,
		DES_STC_P_IMM_OFF,
		DES_STC_P_IMM_OFF,
		DES_STC_P_IMM_OFF,
		DES_STC_P_IMM_OFF,
		DES_STC_P_IMM_OFF,
		DES_STC_P_IMM_OFF,

		DES_LDC_P_IMM_OFF,
		DES_LDC_P_IMM_OFF,
		DES_LDC_P_IMM_OFF,
		DES_LDC_P_IMM_OFF,
		DES_LDC_P_IMM_OFF,
		DES_LDC_P_IMM_OFF,
		DES_LDC_P_IMM_OFF,
		DES_LDC_P_IMM_OFF,
		DES_LDC_P_IMM_OFF,
		DES_LDC_P_IMM_OFF,
		DES_LDC_P_IMM_OFF,
		DES_LDC_P_IMM_OFF,
		DES_LDC_P_IMM_OFF,
		DES_LDC_P_IMM_OFF,
		DES_LDC_P_IMM_OFF,
		DES_LDC_P_IMM_OFF,
		//------------------------------------------
		DES_STC_P_PREIND,  //110	1111 0 0000
		DES_STC_P_PREIND,
		DES_STC_P_PREIND,
		DES_STC_P_PREIND,
		DES_STC_P_PREIND,
		DES_STC_P_PREIND,
		DES_STC_P_PREIND,
		DES_STC_P_PREIND,
		DES_STC_P_PREIND,
		DES_STC_P_PREIND,
		DES_STC_P_PREIND,
		DES_STC_P_PREIND,
		DES_STC_P_PREIND,
		DES_STC_P_PREIND,
		DES_STC_P_PREIND,
		DES_STC_P_PREIND,

		DES_LDC_P_PREIND,
		DES_LDC_P_PREIND,
		DES_LDC_P_PREIND,
		DES_LDC_P_PREIND,
		DES_LDC_P_PREIND,
		DES_LDC_P_PREIND,
		DES_LDC_P_PREIND,
		DES_LDC_P_PREIND,
		DES_LDC_P_PREIND,
		DES_LDC_P_PREIND,
		DES_LDC_P_PREIND,
		DES_LDC_P_PREIND,
		DES_LDC_P_PREIND,
		DES_LDC_P_PREIND,
		DES_LDC_P_PREIND,
		DES_LDC_P_PREIND,
		//------------------------------------------
		DES_CDP,	 //111 0000	0 0000
		DES_MCR,
		DES_CDP,
		DES_MCR,
		DES_CDP,
		DES_MCR,
		DES_CDP,
		DES_MCR,
		DES_CDP,
		DES_MCR,
		DES_CDP,
		DES_MCR,
		DES_CDP,
		DES_MCR,
		DES_CDP,
		DES_MCR,

		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		//---------------------------------------------
		DES_CDP,	 //111 0001	0 0000
		DES_MCR,
		DES_CDP,
		DES_MCR,
		DES_CDP,
		DES_MCR,
		DES_CDP,
		DES_MCR,
		DES_CDP,
		DES_MCR,
		DES_CDP,
		DES_MCR,
		DES_CDP,
		DES_MCR,
		DES_CDP,
		DES_MCR,

		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		//---------------------------------------------
		DES_CDP,	 //111 0010	0 0000
		DES_MCR,
		DES_CDP,
		DES_MCR,
		DES_CDP,
		DES_MCR,
		DES_CDP,
		DES_MCR,
		DES_CDP,
		DES_MCR,
		DES_CDP,
		DES_MCR,
		DES_CDP,
		DES_MCR,
		DES_CDP,
		DES_MCR,

		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		//---------------------------------------------
		DES_CDP,	 //111 0011	0 0000
		DES_MCR,
		DES_CDP,
		DES_MCR,
		DES_CDP,
		DES_MCR,
		DES_CDP,
		DES_MCR,
		DES_CDP,
		DES_MCR,
		DES_CDP,
		DES_MCR,
		DES_CDP,
		DES_MCR,
		DES_CDP,
		DES_MCR,

		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		//---------------------------------------------
		DES_CDP,	 //111 0100	0 0000
		DES_MCR,
		DES_CDP,
		DES_MCR,
		DES_CDP,
		DES_MCR,
		DES_CDP,
		DES_MCR,
		DES_CDP,
		DES_MCR,
		DES_CDP,
		DES_MCR,
		DES_CDP,
		DES_MCR,
		DES_CDP,
		DES_MCR,

		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		//---------------------------------------------
		DES_CDP,	 //111 0101	0 0000
		DES_MCR,
		DES_CDP,
		DES_MCR,
		DES_CDP,
		DES_MCR,
		DES_CDP,
		DES_MCR,
		DES_CDP,
		DES_MCR,
		DES_CDP,
		DES_MCR,
		DES_CDP,
		DES_MCR,
		DES_CDP,
		DES_MCR,

		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		//---------------------------------------------
		DES_CDP,	 //111 0110	0 0000
		DES_MCR,
		DES_CDP,
		DES_MCR,
		DES_CDP,
		DES_MCR,
		DES_CDP,
		DES_MCR,
		DES_CDP,
		DES_MCR,
		DES_CDP,
		DES_MCR,
		DES_CDP,
		DES_MCR,
		DES_CDP,
		DES_MCR,

		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		//---------------------------------------------
		DES_CDP,	 //111 0111	0 0000
		DES_MCR,
		DES_CDP,
		DES_MCR,
		DES_CDP,
		DES_MCR,
		DES_CDP,
		DES_MCR,
		DES_CDP,
		DES_MCR,
		DES_CDP,
		DES_MCR,
		DES_CDP,
		DES_MCR,
		DES_CDP,
		DES_MCR,

		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		DES_CDP,
		DES_MRC,
		//---------------------------------------------
		DES_SWI,	 //111 1000	0 0000
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,	
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
		DES_SWI,
};

TYPE_RETOUR	(CALLTYPE	*NOM_THUMB_TAB[1024])(PARAMETRES)={
	DES_LSL_0,  //00	0000 0000
		DES_LSL,	 //00 0000 0001
		DES_LSL,	 //00 0000 0010
		DES_LSL,	 //00 0000 0011
		DES_LSL,	 //00 0000 0100
		DES_LSL,	 //00 0000 0101
		DES_LSL,	 //00 0000 0110
		DES_LSL,	 //00 0000 0111
		DES_LSL,	 //00 0000 1000
		DES_LSL,	 //00 0000 1001
		DES_LSL,	 //00 0000 1010
		DES_LSL,	 //00 0000 1011
		DES_LSL,	 //00 0000 1100
		DES_LSL,	 //00 0000 1101
		DES_LSL,	 //00 0000 1110
		DES_LSL,	 //00 0000 1111

		DES_LSL,	 //00 0001 0000
		DES_LSL,	 //00 0001 0001
		DES_LSL,	 //00 0001 0010
		DES_LSL,	 //00 0001 0011
		DES_LSL,	 //00 0001 0100
		DES_LSL,	 //00 0001 0101
		DES_LSL,	 //00 0001 0110
		DES_LSL,	 //00 0001 0111
		DES_LSL,	 //00 0001 1000
		DES_LSL,	 //00 0001 1001
		DES_LSL,	 //00 0001 1010
		DES_LSL,	 //00 0001 1011
		DES_LSL,	 //00 0001 1100
		DES_LSL,	 //00 0001 1101
		DES_LSL,	 //00 0001 1110
		DES_LSL,	 //00 0001 1111

		DES_LSR_0,  //00	0010 0000
		DES_LSR,	 //00 0010 0001
		DES_LSR,	 //00 0010 0010
		DES_LSR,	 //00 0010 0011
		DES_LSR,	 //00 0010 0100
		DES_LSR,	 //00 0010 0101
		DES_LSR,	 //00 0010 0110
		DES_LSR,	 //00 0010 0111
		DES_LSR,	 //00 0010 1000
		DES_LSR,	 //00 0010 1001
		DES_LSR,	 //00 0010 1010
		DES_LSR,	 //00 0010 1011
		DES_LSR,	 //00 0010 1100
		DES_LSR,	 //00 0010 1101
		DES_LSR,	 //00 0010 1110
		DES_LSR,	 //00 0010 1111

		DES_LSR,	 //00 0011 0000
		DES_LSR,	 //00 0011 0001
		DES_LSR,	 //00 0011 0010
		DES_LSR,	 //00 0011 0011
		DES_LSR,	 //00 0011 0100
		DES_LSR,	 //00 0011 0101
		DES_LSR,	 //00 0011 0110
		DES_LSR,	 //00 0011 0111
		DES_LSR,	 //00 0011 1000
		DES_LSR,	 //00 0011 1001
		DES_LSR,	 //00 0011 1010
		DES_LSR,	 //00 0011 1011
		DES_LSR,	 //00 0011 1100
		DES_LSR,	 //00 0011 1101
		DES_LSR,	 //00 0011 1110
		DES_LSR,	 //00 0011 1111

		DES_ASR_0,  //00	0100 0000
		DES_ASR,	 //00 0100 0001
		DES_ASR,	 //00 0100 0010
		DES_ASR,	 //00 0100 0011
		DES_ASR,	 //00 0100 0100
		DES_ASR,	 //00 0100 0101
		DES_ASR,	 //00 0100 0110
		DES_ASR,	 //00 0100 0111
		DES_ASR,	 //00 0100 1000
		DES_ASR,	 //00 0100 1001
		DES_ASR,	 //00 0100 1010
		DES_ASR,	 //00 0100 1011
		DES_ASR,	 //00 0100 1100
		DES_ASR,	 //00 0100 1101
		DES_ASR,	 //00 0100 1110
		DES_ASR,	 //00 0100 1111

		DES_ASR,	 //00 0101 0000
		DES_ASR,	 //00 0101 0001
		DES_ASR,	 //00 0101 0010
		DES_ASR,	 //00 0101 0011
		DES_ASR,	 //00 0101 0100
		DES_ASR,	 //00 0101 0101
		DES_ASR,	 //00 0101 0110
		DES_ASR,	 //00 0101 0111
		DES_ASR,	 //00 0101 1000
		DES_ASR,	 //00 0101 1001
		DES_ASR,	 //00 0101 1010
		DES_ASR,	 //00 0101 1011
		DES_ASR,	 //00 0101 1100
		DES_ASR,	 //00 0101 1101
		DES_ASR,	 //00 0101 1110
		DES_ASR,	 //00 0101 1111

		DES_ADD_REG,	 //00 0110 0000
		DES_ADD_REG,	 //00 0110 0001
		DES_ADD_REG,	 //00 0110 0010
		DES_ADD_REG,	 //00 0110 0011
		DES_ADD_REG,	 //00 0110 0100
		DES_ADD_REG,	 //00 0110 0101
		DES_ADD_REG,	 //00 0110 0110
		DES_ADD_REG,	 //00 0110 0111
		DES_SUB_REG,	 //00 0110 1000
		DES_SUB_REG,	 //00 0110 1001
		DES_SUB_REG,	 //00 0110 1010
		DES_SUB_REG,	 //00 0110 1011
		DES_SUB_REG,	 //00 0110 1100
		DES_SUB_REG,	 //00 0110 1101
		DES_SUB_REG,	 //00 0110 1110
		DES_SUB_REG,	 //00 0110 1111

		DES_ADD_IMM3,  //00 0111	0000
		DES_ADD_IMM3,  //00 0111	0001
		DES_ADD_IMM3,  //00 0111	0010
		DES_ADD_IMM3,  //00 0111	0011
		DES_ADD_IMM3,  //00 0111	0100
		DES_ADD_IMM3,  //00 0111	0101
		DES_ADD_IMM3,  //00 0111	0110
		DES_ADD_IMM3,  //00 0111	0111
		DES_SUB_IMM3,  //00 0111	1000
		DES_SUB_IMM3,  //00 0111	1001
		DES_SUB_IMM3,  //00 0111	1010
		DES_SUB_IMM3,  //00 0111	1011
		DES_SUB_IMM3,  //00 0111	1100
		DES_SUB_IMM3,  //00 0111	1101
		DES_SUB_IMM3,  //00 0111	1110
		DES_SUB_IMM3,  //00 0111	1111

		DES_MOV_IMM8,  //00 1000	0000
		DES_MOV_IMM8,  //00 1000	0001
		DES_MOV_IMM8,  //00 1000	0010
		DES_MOV_IMM8,  //00 1000	0011
		DES_MOV_IMM8,  //00 1000	0100
		DES_MOV_IMM8,  //00 1000	0101
		DES_MOV_IMM8,  //00 1000	0110
		DES_MOV_IMM8,  //00 1000	0111
		DES_MOV_IMM8,  //00 1000	1000
		DES_MOV_IMM8,  //00 1000	1001
		DES_MOV_IMM8,  //00 1000	1010
		DES_MOV_IMM8,  //00 1000	1011
		DES_MOV_IMM8,  //00 1000	1100
		DES_MOV_IMM8,  //00 1000	1101
		DES_MOV_IMM8,  //00 1000	1110
		DES_MOV_IMM8,  //00 1000	1111

		DES_MOV_IMM8,  //00 1001	0000
		DES_MOV_IMM8,  //00 1001	0001
		DES_MOV_IMM8,  //00 1001	0010
		DES_MOV_IMM8,  //00 1001	0011
		DES_MOV_IMM8,  //00 1001	0100
		DES_MOV_IMM8,  //00 1001	0101
		DES_MOV_IMM8,  //00 1001	0110
		DES_MOV_IMM8,  //00 1001	0111
		DES_MOV_IMM8,  //00 1001	1000
		DES_MOV_IMM8,  //00 1001	1001
		DES_MOV_IMM8,  //00 1001	1010
		DES_MOV_IMM8,  //00 1001	1011
		DES_MOV_IMM8,  //00 1001	1100
		DES_MOV_IMM8,  //00 1001	1101
		DES_MOV_IMM8,  //00 1001	1110
		DES_MOV_IMM8,  //00 1001	1111

		DES_CMP_IMM8,  //00 1010	0000
		DES_CMP_IMM8,  //00 1010	0001
		DES_CMP_IMM8,  //00 1010	0010
		DES_CMP_IMM8,  //00 1010	0011
		DES_CMP_IMM8,  //00 1010	0100
		DES_CMP_IMM8,  //00 1010	0101
		DES_CMP_IMM8,  //00 1010	0110
		DES_CMP_IMM8,  //00 1010	0111
		DES_CMP_IMM8,  //00 1010	1000
		DES_CMP_IMM8,  //00 1010	1001
		DES_CMP_IMM8,  //00 1010	1010
		DES_CMP_IMM8,  //00 1010	1011
		DES_CMP_IMM8,  //00 1010	1100
		DES_CMP_IMM8,  //00 1010	1101
		DES_CMP_IMM8,  //00 1010	1110
		DES_CMP_IMM8,  //00 1010	1111

		DES_CMP_IMM8,  //00 1011	0000
		DES_CMP_IMM8,  //00 1011	0001
		DES_CMP_IMM8,  //00 1011	0010
		DES_CMP_IMM8,  //00 1011	0011
		DES_CMP_IMM8,  //00 1011	0100
		DES_CMP_IMM8,  //00 1011	0101
		DES_CMP_IMM8,  //00 1011	0110
		DES_CMP_IMM8,  //00 1011	0111
		DES_CMP_IMM8,  //00 1011	1000
		DES_CMP_IMM8,  //00 1011	1001
		DES_CMP_IMM8,  //00 1011	1010
		DES_CMP_IMM8,  //00 1011	1011
		DES_CMP_IMM8,  //00 1011	1100
		DES_CMP_IMM8,  //00 1011	1101
		DES_CMP_IMM8,  //00 1011	1110
		DES_CMP_IMM8,  //00 1011	1111

		DES_ADD_IMM8,  //00 1100	0000
		DES_ADD_IMM8,  //00 1100	0001
		DES_ADD_IMM8,  //00 1100	0010
		DES_ADD_IMM8,  //00 1100	0011
		DES_ADD_IMM8,  //00 1100	0100
		DES_ADD_IMM8,  //00 1100	0101
		DES_ADD_IMM8,  //00 1100	0110
		DES_ADD_IMM8,  //00 1100	0111
		DES_ADD_IMM8,  //00 1100	1000
		DES_ADD_IMM8,  //00 1100	1001
		DES_ADD_IMM8,  //00 1100	1010
		DES_ADD_IMM8,  //00 1100	1011
		DES_ADD_IMM8,  //00 1100	1100
		DES_ADD_IMM8,  //00 1100	1101
		DES_ADD_IMM8,  //00 1100	1110
		DES_ADD_IMM8,  //00 1100	1111

		DES_ADD_IMM8,  //00 1101	0000
		DES_ADD_IMM8,  //00 1101	0001
		DES_ADD_IMM8,  //00 1101	0010
		DES_ADD_IMM8,  //00 1101	0011
		DES_ADD_IMM8,  //00 1101	0100
		DES_ADD_IMM8,  //00 1101	0101
		DES_ADD_IMM8,  //00 1101	0110
		DES_ADD_IMM8,  //00 1101	0111
		DES_ADD_IMM8,  //00 1101	1000
		DES_ADD_IMM8,  //00 1101	1001
		DES_ADD_IMM8,  //00 1101	1010
		DES_ADD_IMM8,  //00 1101	1011
		DES_ADD_IMM8,  //00 1101	1100
		DES_ADD_IMM8,  //00 1101	1101
		DES_ADD_IMM8,  //00 1101	1110
		DES_ADD_IMM8,  //00 1101	1111

		DES_SUB_IMM8,  //00 1110	0000
		DES_SUB_IMM8,  //00 1110	0001
		DES_SUB_IMM8,  //00 1110	0010
		DES_SUB_IMM8,  //00 1110	0011
		DES_SUB_IMM8,  //00 1110	0100
		DES_SUB_IMM8,  //00 1110	0101
		DES_SUB_IMM8,  //00 1110	0110
		DES_SUB_IMM8,  //00 1110	0111
		DES_SUB_IMM8,  //00 1110	1000
		DES_SUB_IMM8,  //00 1110	1001
		DES_SUB_IMM8,  //00 1110	1010
		DES_SUB_IMM8,  //00 1110	1011
		DES_SUB_IMM8,  //00 1110	1100
		DES_SUB_IMM8,  //00 1110	1101
		DES_SUB_IMM8,  //00 1110	1110
		DES_SUB_IMM8,  //00 1110	1111

		DES_SUB_IMM8,  //00 1111	0000
		DES_SUB_IMM8,  //00 1111	0001
		DES_SUB_IMM8,  //00 1111	0010
		DES_SUB_IMM8,  //00 1111	0011
		DES_SUB_IMM8,  //00 1111	0100
		DES_SUB_IMM8,  //00 1111	0101
		DES_SUB_IMM8,  //00 1111	0110
		DES_SUB_IMM8,  //00 1111	0111
		DES_SUB_IMM8,  //00 1111	1000
		DES_SUB_IMM8,  //00 1111	1001
		DES_SUB_IMM8,  //00 1111	1010
		DES_SUB_IMM8,  //00 1111	1011
		DES_SUB_IMM8,  //00 1111	1100
		DES_SUB_IMM8,  //00 1111	1101
		DES_SUB_IMM8,  //00 1111	1110
		DES_SUB_IMM8,  //00 1111	1111

		DES_AND,	 //01 0000 0000
		DES_EOR,	 //01 0000 0001
		DES_LSL_REG,	 //01 0000 0010
		DES_LSR_REG,	 //01 0000 0011
		DES_ASR_REG,	 //01 0000 0100
		DES_ADC_REG,	 //01 0000 0101
		DES_SBC_REG,	 //01 0000 0110
		DES_ROR_REG,	 //01 0000 0111
		DES_TST,	 //01 0000 1000
		DES_NEG,	 //01 0000 1001
		DES_CMP,	 //01 0000 1010
		DES_CMN,	 //01 0000 1011
		DES_ORR,	 //01 0000 1100
		DES_MUL_REG,	 //01 0000 1101
		DES_BIC,	 //01 0000 1110
		DES_MVN,	 //01 0000 1111

		DES_ADD_SPE,	 //01 0001 0000
		DES_ADD_SPE,	 //01 0001 0001
		DES_ADD_SPE,	 //01 0001 0010
		DES_ADD_SPE,	 //01 0001 0011
		DES_CMP_SPE,	 //01 0001 0100
		DES_CMP_SPE,	 //01 0001 0101
		DES_CMP_SPE,	 //01 0001 0110
		DES_CMP_SPE,	 //01 0001 0111
		DES_MOV_SPE,	 //01 0001 1000
		DES_MOV_SPE,	 //01 0001 1001
		DES_MOV_SPE,	 //01 0001 1010
		DES_MOV_SPE,	 //01 0001 1011
		DES_BX_THUMB,  //01 0001	1100
		DES_BX_THUMB,  //01 0001	1101
		DES_BLX_THUMB,  //01	0001 1110
		DES_BLX_THUMB,  //01	0001 1111 

		DES_LDR_PCREL,  //01	0010 0000
		DES_LDR_PCREL,  //01	0010 0001
		DES_LDR_PCREL,  //01	0010 0010
		DES_LDR_PCREL,  //01	0010 0011
		DES_LDR_PCREL,  //01	0010 0100
		DES_LDR_PCREL,  //01	0010 0101
		DES_LDR_PCREL,  //01	0010 0110
		DES_LDR_PCREL,  //01	0010 0111
		DES_LDR_PCREL,  //01	0010 1000
		DES_LDR_PCREL,  //01	0010 1001
		DES_LDR_PCREL,  //01	0010 1010
		DES_LDR_PCREL,  //01	0010 1011
		DES_LDR_PCREL,  //01	0010 1100
		DES_LDR_PCREL,  //01	0010 1101
		DES_LDR_PCREL,  //01	0010 1110
		DES_LDR_PCREL,  //01	0010 1111

		DES_LDR_PCREL,  //01	0011 0000
		DES_LDR_PCREL,  //01	0011 0001
		DES_LDR_PCREL,  //01	0011 0010
		DES_LDR_PCREL,  //01	0011 0011
		DES_LDR_PCREL,  //01	0011 0100
		DES_LDR_PCREL,  //01	0011 0101
		DES_LDR_PCREL,  //01	0011 0110
		DES_LDR_PCREL,  //01	0011 0111
		DES_LDR_PCREL,  //01	0011 1000
		DES_LDR_PCREL,  //01	0011 1001
		DES_LDR_PCREL,  //01	0011 1010
		DES_LDR_PCREL,  //01	0011 1011
		DES_LDR_PCREL,  //01	0011 1100
		DES_LDR_PCREL,  //01	0011 1101
		DES_LDR_PCREL,  //01	0011 1110
		DES_LDR_PCREL,  //01	0011 1111

		DES_STR_REG_OFF,	 //01 0100 0000
		DES_STR_REG_OFF,	 //01 0100 0001
		DES_STR_REG_OFF,	 //01 0100 0010
		DES_STR_REG_OFF,	 //01 0100 0011
		DES_STR_REG_OFF,	 //01 0100 0100
		DES_STR_REG_OFF,	 //01 0100 0101
		DES_STR_REG_OFF,	 //01 0100 0110
		DES_STR_REG_OFF,	 //01 0100 0111
		DES_STRH_REG_OFF,  //01 0100	1000
		DES_STRH_REG_OFF,  //01 0100	1001
		DES_STRH_REG_OFF,  //01 0100	1010
		DES_STRH_REG_OFF,  //01 0100	1011
		DES_STRH_REG_OFF,  //01 0100	1100
		DES_STRH_REG_OFF,  //01 0100	1101
		DES_STRH_REG_OFF,  //01 0100	1110
		DES_STRH_REG_OFF,  //01 0100	1111

		DES_STRB_REG_OFF,  //01 0101	0000
		DES_STRB_REG_OFF,  //01 0101	0001
		DES_STRB_REG_OFF,  //01 0101	0010
		DES_STRB_REG_OFF,  //01 0101	0011
		DES_STRB_REG_OFF,  //01 0101	0100
		DES_STRB_REG_OFF,  //01 0101	0101
		DES_STRB_REG_OFF,  //01 0101	0110
		DES_STRB_REG_OFF,  //01 0101	0111
		DES_LDRSB_REG_OFF,  //01	0101 1000
		DES_LDRSB_REG_OFF,  //01	0101 1001
		DES_LDRSB_REG_OFF,  //01	0101 1010
		DES_LDRSB_REG_OFF,  //01	0101 1011
		DES_LDRSB_REG_OFF,  //01	0101 1100
		DES_LDRSB_REG_OFF,  //01	0101 1101
		DES_LDRSB_REG_OFF,  //01	0101 1110
		DES_LDRSB_REG_OFF,  //01	0101 1111

		DES_LDR_REG_OFF,	 //01 0110 0000
		DES_LDR_REG_OFF,	 //01 0110 0001
		DES_LDR_REG_OFF,	 //01 0110 0010
		DES_LDR_REG_OFF,	 //01 0110 0011
		DES_LDR_REG_OFF,	 //01 0110 0100
		DES_LDR_REG_OFF,	 //01 0110 0101
		DES_LDR_REG_OFF,	 //01 0110 0110
		DES_LDR_REG_OFF,	 //01 0110 0111
		DES_LDRH_REG_OFF,  //01 0110	1000
		DES_LDRH_REG_OFF,  //01 0110	1001
		DES_LDRH_REG_OFF,  //01 0110	1010
		DES_LDRH_REG_OFF,  //01 0110	1011
		DES_LDRH_REG_OFF,  //01 0110	1100
		DES_LDRH_REG_OFF,  //01 0110	1101
		DES_LDRH_REG_OFF,  //01 0110	1110
		DES_LDRH_REG_OFF,  //01 0110	1111

		DES_LDRB_REG_OFF,  //01 0111	0000
		DES_LDRB_REG_OFF,  //01 0111	0001
		DES_LDRB_REG_OFF,  //01 0111	0010
		DES_LDRB_REG_OFF,  //01 0111	0011
		DES_LDRB_REG_OFF,  //01 0111	0100
		DES_LDRB_REG_OFF,  //01 0111	0101
		DES_LDRB_REG_OFF,  //01 0111	0110
		DES_LDRB_REG_OFF,  //01 0111	0111
		DES_LDRSH_REG_OFF,  //01	0111 1000
		DES_LDRSH_REG_OFF,  //01	0111 1001
		DES_LDRSH_REG_OFF,  //01	0111 1010
		DES_LDRSH_REG_OFF,  //01	0111 1011
		DES_LDRSH_REG_OFF,  //01	0111 1100
		DES_LDRSH_REG_OFF,  //01	0111 1101
		DES_LDRSH_REG_OFF,  //01	0111 1110
		DES_LDRSH_REG_OFF,  //01	0111 1111

		DES_STR_IMM_OFF,	 //01 1000 0000
		DES_STR_IMM_OFF,	 //01 1000 0001
		DES_STR_IMM_OFF,	 //01 1000 0010
		DES_STR_IMM_OFF,	 //01 1000 0011
		DES_STR_IMM_OFF,	 //01 1000 0100
		DES_STR_IMM_OFF,	 //01 1000 0101
		DES_STR_IMM_OFF,	 //01 1000 0110
		DES_STR_IMM_OFF,	 //01 1000 0111
		DES_STR_IMM_OFF,	 //01 1000 1000
		DES_STR_IMM_OFF,	 //01 1000 1001
		DES_STR_IMM_OFF,	 //01 1000 1010
		DES_STR_IMM_OFF,	 //01 1000 1011
		DES_STR_IMM_OFF,	 //01 1000 1100
		DES_STR_IMM_OFF,	 //01 1000 1101
		DES_STR_IMM_OFF,	 //01 1000 1110
		DES_STR_IMM_OFF,	 //01 1000 1111

		DES_STR_IMM_OFF,	 //01 1001 0000
		DES_STR_IMM_OFF,	 //01 1001 0001
		DES_STR_IMM_OFF,	 //01 1001 0010
		DES_STR_IMM_OFF,	 //01 1001 0011
		DES_STR_IMM_OFF,	 //01 1001 0100
		DES_STR_IMM_OFF,	 //01 1001 0101
		DES_STR_IMM_OFF,	 //01 1001 0110
		DES_STR_IMM_OFF,	 //01 1001 0111
		DES_STR_IMM_OFF,	 //01 1001 1000
		DES_STR_IMM_OFF,	 //01 1001 1001
		DES_STR_IMM_OFF,	 //01 1001 1010
		DES_STR_IMM_OFF,	 //01 1001 1011
		DES_STR_IMM_OFF,	 //01 1001 1100
		DES_STR_IMM_OFF,	 //01 1001 1101
		DES_STR_IMM_OFF,	 //01 1001 1110
		DES_STR_IMM_OFF,	 //01 1001 1111

		DES_LDR_IMM_OFF,	 //01 1010 0000
		DES_LDR_IMM_OFF,	 //01 1010 0001
		DES_LDR_IMM_OFF,	 //01 1010 0010
		DES_LDR_IMM_OFF,	 //01 1010 0011
		DES_LDR_IMM_OFF,	 //01 1010 0100
		DES_LDR_IMM_OFF,	 //01 1010 0101
		DES_LDR_IMM_OFF,	 //01 1010 0110
		DES_LDR_IMM_OFF,	 //01 1010 0111
		DES_LDR_IMM_OFF,	 //01 1010 1000
		DES_LDR_IMM_OFF,	 //01 1010 1001
		DES_LDR_IMM_OFF,	 //01 1010 1010
		DES_LDR_IMM_OFF,	 //01 1010 1011
		DES_LDR_IMM_OFF,	 //01 1010 1100
		DES_LDR_IMM_OFF,	 //01 1010 1101
		DES_LDR_IMM_OFF,	 //01 1010 1110
		DES_LDR_IMM_OFF,	 //01 1010 1111

		DES_LDR_IMM_OFF,	 //01 1011 0000
		DES_LDR_IMM_OFF,	 //01 1011 0001
		DES_LDR_IMM_OFF,	 //01 1011 0010
		DES_LDR_IMM_OFF,	 //01 1011 0011
		DES_LDR_IMM_OFF,	 //01 1011 0100
		DES_LDR_IMM_OFF,	 //01 1011 0101
		DES_LDR_IMM_OFF,	 //01 1011 0110
		DES_LDR_IMM_OFF,	 //01 1011 0111
		DES_LDR_IMM_OFF,	 //01 1011 1000
		DES_LDR_IMM_OFF,	 //01 1011 1001
		DES_LDR_IMM_OFF,	 //01 1011 1010
		DES_LDR_IMM_OFF,	 //01 1011 1011
		DES_LDR_IMM_OFF,	 //01 1011 1100
		DES_LDR_IMM_OFF,	 //01 1011 1101
		DES_LDR_IMM_OFF,	 //01 1011 1110
		DES_LDR_IMM_OFF,	 //01 1011 1111

		DES_STRB_IMM_OFF,  //01 1100	0000
		DES_STRB_IMM_OFF,  //01 1100	0001
		DES_STRB_IMM_OFF,  //01 1100	0010
		DES_STRB_IMM_OFF,  //01 1100	0011
		DES_STRB_IMM_OFF,  //01 1100	0100
		DES_STRB_IMM_OFF,  //01 1100	0101
		DES_STRB_IMM_OFF,  //01 1100	0110
		DES_STRB_IMM_OFF,  //01 1100	0111
		DES_STRB_IMM_OFF,  //01 1100	1000
		DES_STRB_IMM_OFF,  //01 1100	1001
		DES_STRB_IMM_OFF,  //01 1100	1010
		DES_STRB_IMM_OFF,  //01 1100	1011
		DES_STRB_IMM_OFF,  //01 1100	1100
		DES_STRB_IMM_OFF,  //01 1100	1101
		DES_STRB_IMM_OFF,  //01 1100	1110
		DES_STRB_IMM_OFF,  //01 1100	1111

		DES_STRB_IMM_OFF,  //01 1101	0000
		DES_STRB_IMM_OFF,  //01 1101	0001
		DES_STRB_IMM_OFF,  //01 1101	0010
		DES_STRB_IMM_OFF,  //01 1101	0011
		DES_STRB_IMM_OFF,  //01 1101	0100
		DES_STRB_IMM_OFF,  //01 1101	0101
		DES_STRB_IMM_OFF,  //01 1101	0110
		DES_STRB_IMM_OFF,  //01 1101	0111
		DES_STRB_IMM_OFF,  //01 1101	1000
		DES_STRB_IMM_OFF,  //01 1101	1001
		DES_STRB_IMM_OFF,  //01 1101	1010
		DES_STRB_IMM_OFF,  //01 1101	1011
		DES_STRB_IMM_OFF,  //01 1101	1100
		DES_STRB_IMM_OFF,  //01 1101	1101
		DES_STRB_IMM_OFF,  //01 1101	1110
		DES_STRB_IMM_OFF,  //01 1101	1111

		DES_LDRB_IMM_OFF,  //01 1110	0000
		DES_LDRB_IMM_OFF,  //01 1110	0001
		DES_LDRB_IMM_OFF,  //01 1110	0010
		DES_LDRB_IMM_OFF,  //01 1110	0011
		DES_LDRB_IMM_OFF,  //01 1110	0100
		DES_LDRB_IMM_OFF,  //01 1110	0101
		DES_LDRB_IMM_OFF,  //01 1110	0110
		DES_LDRB_IMM_OFF,  //01 1110	0111
		DES_LDRB_IMM_OFF,  //01 1110	1000
		DES_LDRB_IMM_OFF,  //01 1110	1001
		DES_LDRB_IMM_OFF,  //01 1110	1010
		DES_LDRB_IMM_OFF,  //01 1110	1011
		DES_LDRB_IMM_OFF,  //01 1110	1100
		DES_LDRB_IMM_OFF,  //01 1110	1101
		DES_LDRB_IMM_OFF,  //01 1110	1110
		DES_LDRB_IMM_OFF,  //01 1110	1111

		DES_LDRB_IMM_OFF,  //01 1111	0000
		DES_LDRB_IMM_OFF,  //01 1111	0001
		DES_LDRB_IMM_OFF,  //01 1111	0010
		DES_LDRB_IMM_OFF,  //01 1111	0011
		DES_LDRB_IMM_OFF,  //01 1111	0100
		DES_LDRB_IMM_OFF,  //01 1111	0101
		DES_LDRB_IMM_OFF,  //01 1111	0110
		DES_LDRB_IMM_OFF,  //01 1111	0111
		DES_LDRB_IMM_OFF,  //01 1111	1000
		DES_LDRB_IMM_OFF,  //01 1111	1001
		DES_LDRB_IMM_OFF,  //01 1111	1010
		DES_LDRB_IMM_OFF,  //01 1111	1011
		DES_LDRB_IMM_OFF,  //01 1111	1100
		DES_LDRB_IMM_OFF,  //01 1111	1101
		DES_LDRB_IMM_OFF,  //01 1111	1110
		DES_LDRB_IMM_OFF,  //01 1111	1111

		DES_STRH_IMM_OFF,  //10 0000	0000
		DES_STRH_IMM_OFF,  //10 0000	0001
		DES_STRH_IMM_OFF,  //10 0000	0010
		DES_STRH_IMM_OFF,  //10 0000	0011
		DES_STRH_IMM_OFF,  //10 0000	0100
		DES_STRH_IMM_OFF,  //10 0000	0101
		DES_STRH_IMM_OFF,  //10 0000	0110
		DES_STRH_IMM_OFF,  //10 0000	0111
		DES_STRH_IMM_OFF,  //10 0000	1000
		DES_STRH_IMM_OFF,  //10 0000	1001
		DES_STRH_IMM_OFF,  //10 0000	1010
		DES_STRH_IMM_OFF,  //10 0000	1011
		DES_STRH_IMM_OFF,  //10 0000	1100
		DES_STRH_IMM_OFF,  //10 0000	1101
		DES_STRH_IMM_OFF,  //10 0000	1110
		DES_STRH_IMM_OFF,  //10 0000	1111

		DES_STRH_IMM_OFF,  //10 0001	0000
		DES_STRH_IMM_OFF,  //10 0001	0001
		DES_STRH_IMM_OFF,  //10 0001	0010
		DES_STRH_IMM_OFF,  //10 0001	0011
		DES_STRH_IMM_OFF,  //10 0001	0100
		DES_STRH_IMM_OFF,  //10 0001	0101
		DES_STRH_IMM_OFF,  //10 0001	0110
		DES_STRH_IMM_OFF,  //10 0001	0111
		DES_STRH_IMM_OFF,  //10 0001	1000
		DES_STRH_IMM_OFF,  //10 0001	1001
		DES_STRH_IMM_OFF,  //10 0001	1010
		DES_STRH_IMM_OFF,  //10 0001	1011
		DES_STRH_IMM_OFF,  //10 0001	1100
		DES_STRH_IMM_OFF,  //10 0001	1101
		DES_STRH_IMM_OFF,  //10 0001	1110
		DES_STRH_IMM_OFF,  //10 0001	1111

		DES_LDRH_IMM_OFF,  //10 0010	0000
		DES_LDRH_IMM_OFF,  //10 0010	0001
		DES_LDRH_IMM_OFF,  //10 0010	0010
		DES_LDRH_IMM_OFF,  //10 0010	0011
		DES_LDRH_IMM_OFF,  //10 0010	0100
		DES_LDRH_IMM_OFF,  //10 0010	0101
		DES_LDRH_IMM_OFF,  //10 0010	0110
		DES_LDRH_IMM_OFF,  //10 0010	0111
		DES_LDRH_IMM_OFF,  //10 0010	1000
		DES_LDRH_IMM_OFF,  //10 0010	1001
		DES_LDRH_IMM_OFF,  //10 0010	1010
		DES_LDRH_IMM_OFF,  //10 0010	1011
		DES_LDRH_IMM_OFF,  //10 0010	1100
		DES_LDRH_IMM_OFF,  //10 0010	1101
		DES_LDRH_IMM_OFF,  //10 0010	1110
		DES_LDRH_IMM_OFF,  //10 0010	1111

		DES_LDRH_IMM_OFF,  //10 0011	0000
		DES_LDRH_IMM_OFF,  //10 0011	0001
		DES_LDRH_IMM_OFF,  //10 0011	0010
		DES_LDRH_IMM_OFF,  //10 0011	0011
		DES_LDRH_IMM_OFF,  //10 0011	0100
		DES_LDRH_IMM_OFF,  //10 0011	0101
		DES_LDRH_IMM_OFF,  //10 0011	0110
		DES_LDRH_IMM_OFF,  //10 0011	0111
		DES_LDRH_IMM_OFF,  //10 0011	1000
		DES_LDRH_IMM_OFF,  //10 0011	1001
		DES_LDRH_IMM_OFF,  //10 0011	1010
		DES_LDRH_IMM_OFF,  //10 0011	1011
		DES_LDRH_IMM_OFF,  //10 0011	1100
		DES_LDRH_IMM_OFF,  //10 0011	1101
		DES_LDRH_IMM_OFF,  //10 0011	1110
		DES_LDRH_IMM_OFF,  //10 0011	1111

		DES_STR_SPREL,  //10	0100 0000
		DES_STR_SPREL,  //10	0100 0001
		DES_STR_SPREL,  //10	0100 0010
		DES_STR_SPREL,  //10	0100 0011
		DES_STR_SPREL,  //10	0100 0100
		DES_STR_SPREL,  //10	0100 0101
		DES_STR_SPREL,  //10	0100 0110
		DES_STR_SPREL,  //10	0100 0111
		DES_STR_SPREL,  //10	0100 1000
		DES_STR_SPREL,  //10	0100 1001
		DES_STR_SPREL,  //10	0100 1010
		DES_STR_SPREL,  //10	0100 1011
		DES_STR_SPREL,  //10	0100 1100
		DES_STR_SPREL,  //10	0100 1101
		DES_STR_SPREL,  //10	0100 1110
		DES_STR_SPREL,  //10	0100 1111

		DES_STR_SPREL,  //10	0101 0000
		DES_STR_SPREL,  //10	0101 0001
		DES_STR_SPREL,  //10	0101 0010
		DES_STR_SPREL,  //10	0101 0011
		DES_STR_SPREL,  //10	0101 0100
		DES_STR_SPREL,  //10	0101 0101
		DES_STR_SPREL,  //10	0101 0110
		DES_STR_SPREL,  //10	0101 0111
		DES_STR_SPREL,  //10	0101 1000
		DES_STR_SPREL,  //10	0101 1001
		DES_STR_SPREL,  //10	0101 1010
		DES_STR_SPREL,  //10	0101 1011
		DES_STR_SPREL,  //10	0101 1100
		DES_STR_SPREL,  //10	0101 1101
		DES_STR_SPREL,  //10	0101 1110
		DES_STR_SPREL,  //10	0101 1111

		DES_LDR_SPREL,  //10	0110 0000
		DES_LDR_SPREL,  //10	0110 0001
		DES_LDR_SPREL,  //10	0110 0010
		DES_LDR_SPREL,  //10	0110 0011
		DES_LDR_SPREL,  //10	0110 0100
		DES_LDR_SPREL,  //10	0110 0101
		DES_LDR_SPREL,  //10	0110 0110
		DES_LDR_SPREL,  //10	0110 0111
		DES_LDR_SPREL,  //10	0110 1000
		DES_LDR_SPREL,  //10	0110 1001
		DES_LDR_SPREL,  //10	0110 1010
		DES_LDR_SPREL,  //10	0110 1011
		DES_LDR_SPREL,  //10	0110 1100
		DES_LDR_SPREL,  //10	0110 1101
		DES_LDR_SPREL,  //10	0110 1110
		DES_LDR_SPREL,  //10	0110 1111

		DES_LDR_SPREL,  //10	0111 0000
		DES_LDR_SPREL,  //10	0111 0001
		DES_LDR_SPREL,  //10	0111 0010
		DES_LDR_SPREL,  //10	0111 0011
		DES_LDR_SPREL,  //10	0111 0100
		DES_LDR_SPREL,  //10	0111 0101
		DES_LDR_SPREL,  //10	0111 0110
		DES_LDR_SPREL,  //10	0111 0111
		DES_LDR_SPREL,  //10	0111 1000
		DES_LDR_SPREL,  //10	0111 1001
		DES_LDR_SPREL,  //10	0111 1010
		DES_LDR_SPREL,  //10	0111 1011
		DES_LDR_SPREL,  //10	0111 1100
		DES_LDR_SPREL,  //10	0111 1101
		DES_LDR_SPREL,  //10	0111 1110
		DES_LDR_SPREL,  //10	0111 1111

		DES_ADD_2PC,	 //10 1000 0000
		DES_ADD_2PC,	 //10 1000 0001
		DES_ADD_2PC,	 //10 1000 0010
		DES_ADD_2PC,	 //10 1000 0011
		DES_ADD_2PC,	 //10 1000 0100
		DES_ADD_2PC,	 //10 1000 0101
		DES_ADD_2PC,	 //10 1000 0110
		DES_ADD_2PC,	 //10 1000 0111
		DES_ADD_2PC,	 //10 1000 1000
		DES_ADD_2PC,	 //10 1000 1001
		DES_ADD_2PC,	 //10 1000 1010
		DES_ADD_2PC,	 //10 1000 1011
		DES_ADD_2PC,	 //10 1000 1100
		DES_ADD_2PC,	 //10 1000 1101
		DES_ADD_2PC,	 //10 1000 1110
		DES_ADD_2PC,	 //10 1000 1111

		DES_ADD_2PC,	 //10 1001 0000
		DES_ADD_2PC,	 //10 1001 0001
		DES_ADD_2PC,	 //10 1001 0010
		DES_ADD_2PC,	 //10 1001 0011
		DES_ADD_2PC,	 //10 1001 0100
		DES_ADD_2PC,	 //10 1001 0101
		DES_ADD_2PC,	 //10 1001 0110
		DES_ADD_2PC,	 //10 1001 0111
		DES_ADD_2PC,	 //10 1001 1000
		DES_ADD_2PC,	 //10 1001 1001
		DES_ADD_2PC,	 //10 1001 1010
		DES_ADD_2PC,	 //10 1001 1011
		DES_ADD_2PC,	 //10 1001 1100
		DES_ADD_2PC,	 //10 1001 1101
		DES_ADD_2PC,	 //10 1001 1110
		DES_ADD_2PC,	 //10 1001 1111

		DES_ADD_2SP,	 //10 1010 0000
		DES_ADD_2SP,	 //10 1010 0001
		DES_ADD_2SP,	 //10 1010 0010
		DES_ADD_2SP,	 //10 1010 0011
		DES_ADD_2SP,	 //10 1010 0100
		DES_ADD_2SP,	 //10 1010 0101
		DES_ADD_2SP,	 //10 1010 0110
		DES_ADD_2SP,	 //10 1010 0111
		DES_ADD_2SP,	 //10 1010 1000
		DES_ADD_2SP,	 //10 1010 1001
		DES_ADD_2SP,	 //10 1010 1010
		DES_ADD_2SP,	 //10 1010 1011
		DES_ADD_2SP,	 //10 1010 1100
		DES_ADD_2SP,	 //10 1010 1101
		DES_ADD_2SP,	 //10 1010 1110
		DES_ADD_2SP,	 //10 1010 1111

		DES_ADD_2SP,	 //10 1011 0000
		DES_ADD_2SP,	 //10 1011 0001
		DES_ADD_2SP,	 //10 1011 0010
		DES_ADD_2SP,	 //10 1011 0011
		DES_ADD_2SP,	 //10 1011 0100
		DES_ADD_2SP,	 //10 1011 0101
		DES_ADD_2SP,	 //10 1011 0110
		DES_ADD_2SP,	 //10 1011 0111
		DES_ADD_2SP,	 //10 1011 1000
		DES_ADD_2SP,	 //10 1011 1001
		DES_ADD_2SP,	 //10 1011 1010
		DES_ADD_2SP,	 //10 1011 1011
		DES_ADD_2SP,	 //10 1011 1100
		DES_ADD_2SP,	 //10 1011 1101
		DES_ADD_2SP,	 //10 1011 1110
		DES_ADD_2SP,	 //10 1011 1111

		DES_ADJUST_P_SP,	 //10 1100 0000
		DES_ADJUST_P_SP,	 //10 1100 0001
		DES_ADJUST_M_SP,	 //10 1100 0010
		DES_ADJUST_M_SP,	 //10 1100 0011
		DES_UND_THUMB,  //10	1100 0100
		DES_UND_THUMB,  //10	1100 0101
		DES_UND_THUMB,  //10	1100 0110
		DES_UND_THUMB,  //10	1100 0111
		DES_UND_THUMB,  //10	1100 1000
		DES_UND_THUMB,  //10	1100 1001
		DES_UND_THUMB,  //10	1100 1010
		DES_UND_THUMB,  //10	1100 1011
		DES_UND_THUMB,  //10	1100 1100
		DES_UND_THUMB,  //10	1100 1101
		DES_UND_THUMB,  //10	1100 1110
		DES_UND_THUMB,  //10	1100 1111

		DES_PUSH,  //10 1101	0000
		DES_PUSH,  //10 1101	0001
		DES_PUSH,  //10 1101	0010
		DES_PUSH,  //10 1101	0011
		DES_PUSH_LR,	 //10 1101 0100
		DES_PUSH_LR,	 //10 1101 0101
		DES_PUSH_LR,	 //10 1101 0110
		DES_PUSH_LR,	 //10 1101 0111
		DES_UND_THUMB,  //10	1101 1000
		DES_UND_THUMB,  //10	1101 1001
		DES_UND_THUMB,  //10	1101 1010
		DES_UND_THUMB,  //10	1101 1011
		DES_UND_THUMB,  //10	1101 1100
		DES_UND_THUMB,  //10	1101 1101
		DES_UND_THUMB,  //10	1101 1110
		DES_UND_THUMB,  //10	1101 1111

		DES_UND_THUMB,  //10	1110 0000
		DES_UND_THUMB,  //10	1110 0001
		DES_UND_THUMB,  //10	1110 0010
		DES_UND_THUMB,  //10	1110 0011
		DES_UND_THUMB,  //10	1110 0100
		DES_UND_THUMB,  //10	1110 0101
		DES_UND_THUMB,  //10	1110 0110
		DES_UND_THUMB,  //10	1110 0111
		DES_UND_THUMB,  //10	1110 1000
		DES_UND_THUMB,  //10	1110 1001
		DES_UND_THUMB,  //10	1110 1010
		DES_UND_THUMB,  //10	1110 1011
		DES_UND_THUMB,  //10	1110 1100
		DES_UND_THUMB,  //10	1110 1101
		DES_UND_THUMB,  //10	1110 1110
		DES_UND_THUMB,  //10	1110 1111

		DES_POP,	 //10 1111 0000
		DES_POP,	 //10 1111 0001
		DES_POP,	 //10 1111 0010
		DES_POP,	 //10 1111 0011
		DES_POP_PC,	//10 1111 0100
		DES_POP_PC,	//10 1111 0101
		DES_POP_PC,	//10 1111 0110
		DES_POP_PC,	//10 1111 0111
		DES_BKPT_THUMB,	//10 1111 1000
		DES_BKPT_THUMB,	//10 1111 1001
		DES_BKPT_THUMB,	//10 1111 1010
		DES_BKPT_THUMB,	//10 1111 1011
		DES_UND_THUMB,  //10	1111 1100
		DES_UND_THUMB,  //10	1111 1101
		DES_UND_THUMB,  //10	1111 1110
		DES_UND_THUMB,  //10	1111 1111

		DES_STMIA_THUMB,	 //11 0000 0000
		DES_STMIA_THUMB,	 //11 0000 0001
		DES_STMIA_THUMB,	 //11 0000 0010
		DES_STMIA_THUMB,	 //11 0000 0011
		DES_STMIA_THUMB,	 //11 0000 0100
		DES_STMIA_THUMB,	 //11 0000 0101
		DES_STMIA_THUMB,	 //11 0000 0110
		DES_STMIA_THUMB,	 //11 0000 0111
		DES_STMIA_THUMB,	 //11 0000 1000
		DES_STMIA_THUMB,	 //11 0000 1001
		DES_STMIA_THUMB,	 //11 0000 1010
		DES_STMIA_THUMB,	 //11 0000 1011
		DES_STMIA_THUMB,	 //11 0000 1100
		DES_STMIA_THUMB,	 //11 0000 1101
		DES_STMIA_THUMB,	 //11 0000 1110
		DES_STMIA_THUMB,	 //11 0000 1111

		DES_STMIA_THUMB,	 //11 0001 0000
		DES_STMIA_THUMB,	 //11 0001 0001
		DES_STMIA_THUMB,	 //11 0001 0010
		DES_STMIA_THUMB,	 //11 0001 0011
		DES_STMIA_THUMB,	 //11 0001 0100
		DES_STMIA_THUMB,	 //11 0001 0101
		DES_STMIA_THUMB,	 //11 0001 0110
		DES_STMIA_THUMB,	 //11 0001 0111
		DES_STMIA_THUMB,	 //11 0001 1000
		DES_STMIA_THUMB,	 //11 0001 1001
		DES_STMIA_THUMB,	 //11 0001 1010
		DES_STMIA_THUMB,	 //11 0001 1011
		DES_STMIA_THUMB,	 //11 0001 1100
		DES_STMIA_THUMB,	 //11 0001 1101
		DES_STMIA_THUMB,	 //11 0001 1110
		DES_STMIA_THUMB,	 //11 0001 1111

		DES_LDMIA_THUMB,	 //11 0010 0000
		DES_LDMIA_THUMB,	 //11 0010 0001
		DES_LDMIA_THUMB,	 //11 0010 0010
		DES_LDMIA_THUMB,	 //11 0010 0011
		DES_LDMIA_THUMB,	 //11 0010 0100
		DES_LDMIA_THUMB,	 //11 0010 0101
		DES_LDMIA_THUMB,	 //11 0010 0110
		DES_LDMIA_THUMB,	 //11 0010 0111
		DES_LDMIA_THUMB,	 //11 0010 1000
		DES_LDMIA_THUMB,	 //11 0010 1001
		DES_LDMIA_THUMB,	 //11 0010 1010
		DES_LDMIA_THUMB,	 //11 0010 1011
		DES_LDMIA_THUMB,	 //11 0010 1100
		DES_LDMIA_THUMB,	 //11 0010 1101
		DES_LDMIA_THUMB,	 //11 0010 1110
		DES_LDMIA_THUMB,	 //11 0010 1111

		DES_LDMIA_THUMB,	 //11 0011 0000
		DES_LDMIA_THUMB,	 //11 0011 0001
		DES_LDMIA_THUMB,	 //11 0011 0010
		DES_LDMIA_THUMB,	 //11 0011 0011
		DES_LDMIA_THUMB,	 //11 0011 0100
		DES_LDMIA_THUMB,	 //11 0011 0101
		DES_LDMIA_THUMB,	 //11 0011 0110
		DES_LDMIA_THUMB,	 //11 0011 0111
		DES_LDMIA_THUMB,	 //11 0011 1000
		DES_LDMIA_THUMB,	 //11 0011 1001
		DES_LDMIA_THUMB,	 //11 0011 1010
		DES_LDMIA_THUMB,	 //11 0011 1011
		DES_LDMIA_THUMB,	 //11 0011 1100
		DES_LDMIA_THUMB,	 //11 0011 1101
		DES_LDMIA_THUMB,	 //11 0011 1110
		DES_LDMIA_THUMB,	 //11 0011 1111

		DES_B_COND,	//11 0100 0000
		DES_B_COND,	//11 0100 0001
		DES_B_COND,	//11 0100 0010
		DES_B_COND,	//11 0100 0011
		DES_B_COND,	//11 0100 0100
		DES_B_COND,	//11 0100 0101
		DES_B_COND,	//11 0100 0110
		DES_B_COND,	//11 0100 0111
		DES_B_COND,	//11 0100 1000
		DES_B_COND,	//11 0100 1001
		DES_B_COND,	//11 0100 1010
		DES_B_COND,	//11 0100 1011
		DES_B_COND,	//11 0100 1100
		DES_B_COND,	//11 0100 1101
		DES_B_COND,	//11 0100 1110
		DES_B_COND,	//11 0100 1111

		DES_B_COND,	//11 0101 0000
		DES_B_COND,	//11 0101 0001
		DES_B_COND,	//11 0101 0010
		DES_B_COND,	//11 0101 0011
		DES_B_COND,	//11 0101 0100
		DES_B_COND,	//11 0101 0101
		DES_B_COND,	//11 0101 0110
		DES_B_COND,	//11 0101 0111
		DES_B_COND,	//11 0101 1000
		DES_B_COND,	//11 0101 1001
		DES_B_COND,	//11 0101 1010
		DES_B_COND,	//11 0101 1011
		DES_B_COND,	//11 0101 1100
		DES_B_COND,	//11 0101 1101
		DES_B_COND,	//11 0101 1110
		DES_B_COND,	//11 0101 1111

		DES_B_COND,	//11 0110 0000
		DES_B_COND,	//11 0110 0001
		DES_B_COND,	//11 0110 0010
		DES_B_COND,	//11 0110 0011
		DES_B_COND,	//11 0110 0100
		DES_B_COND,	//11 0110 0101
		DES_B_COND,	//11 0110 0110
		DES_B_COND,	//11 0110 0111
		DES_B_COND,	//11 0110 1000
		DES_B_COND,	//11 0110 1001
		DES_B_COND,	//11 0110 1010
		DES_B_COND,	//11 0110 1011
		DES_B_COND,	//11 0110 1100
		DES_B_COND,	//11 0110 1101
		DES_B_COND,	//11 0110 1110
		DES_B_COND,	//11 0110 1111

		DES_B_COND,	//11 0111 0000
		DES_B_COND,	//11 0111 0001
		DES_B_COND,	//11 0111 0010
		DES_B_COND,	//11 0111 0011
		DES_B_COND,	//11 0111 0100
		DES_B_COND,	//11 0111 0101
		DES_B_COND,	//11 0111 0110
		DES_B_COND,	//11 0111 0111
		DES_B_COND,	//11 0111 1000
		DES_B_COND,	//11 0111 1001
		DES_B_COND,	//11 0111 1010
		DES_B_COND,	//11 0111 1011
		DES_SWI_THUMB,  //11	0111 1100
		DES_SWI_THUMB,  //11	0111 1101
		DES_SWI_THUMB,  //11	0111 1110
		DES_SWI_THUMB,  //11	0111 1111

		DES_B_UNCOND,  //11 1000	0000
		DES_B_UNCOND,  //11 1000	0001
		DES_B_UNCOND,  //11 1000	0010
		DES_B_UNCOND,  //11 1000	0011
		DES_B_UNCOND,  //11 1000	0100
		DES_B_UNCOND,  //11 1000	0101
		DES_B_UNCOND,  //11 1000	0110
		DES_B_UNCOND,  //11 1000	0111
		DES_B_UNCOND,  //11 1000	1000
		DES_B_UNCOND,  //11 1000	1001
		DES_B_UNCOND,  //11 1000	1010
		DES_B_UNCOND,  //11 1000	1011
		DES_B_UNCOND,  //11 1000	1100
		DES_B_UNCOND,  //11 1000	1101
		DES_B_UNCOND,  //11 1000	1110
		DES_B_UNCOND,  //11 1000	1111

		DES_B_UNCOND,  //11 1001	0000
		DES_B_UNCOND,  //11 1001	0001
		DES_B_UNCOND,  //11 1001	0010
		DES_B_UNCOND,  //11 1001	0011
		DES_B_UNCOND,  //11 1001	0100
		DES_B_UNCOND,  //11 1001	0101
		DES_B_UNCOND,  //11 1001	0110
		DES_B_UNCOND,  //11 1001	0111
		DES_B_UNCOND,  //11 1001	1000
		DES_B_UNCOND,  //11 1001	1001
		DES_B_UNCOND,  //11 1001	1010
		DES_B_UNCOND,  //11 1001	1011
		DES_B_UNCOND,  //11 1001	1100
		DES_B_UNCOND,  //11 1001	1101
		DES_B_UNCOND,  //11 1001	1110
		DES_B_UNCOND,  //11 1001	1111

		DES_BLX,	 //11 1010 0000
		DES_BLX,	 //11 1010 0001
		DES_BLX,	 //11 1010 0010
		DES_BLX,	 //11 1010 0011
		DES_BLX,	 //11 1010 0100
		DES_BLX,	 //11 1010 0101
		DES_BLX,	 //11 1010 0110
		DES_BLX,	 //11 1010 0111
		DES_BLX,	 //11 1010 1000
		DES_BLX,	 //11 1010 1001
		DES_BLX,	 //11 1010 1010
		DES_BLX,	 //11 1010 1011
		DES_BLX,	 //11 1010 1100
		DES_BLX,	 //11 1010 1101
		DES_BLX,	 //11 1010 1110
		DES_BLX,	 //11 1010 1111

		DES_BLX,	 //11 1011 0000
		DES_BLX,	 //11 1011 0001
		DES_BLX,	 //11 1011 0010
		DES_BLX,	 //11 1011 0011
		DES_BLX,	 //11 1011 0100
		DES_BLX,	 //11 1011 0101
		DES_BLX,	 //11 1011 0110
		DES_BLX,	 //11 1011 0111
		DES_BLX,	 //11 1011 1000
		DES_BLX,	 //11 1011 1001
		DES_BLX,	 //11 1011 1010
		DES_BLX,	 //11 1011 1011
		DES_BLX,	 //11 1011 1100
		DES_BLX,	 //11 1011 1101
		DES_BLX,	 //11 1011 1110
		DES_BLX,	 //11 1011 1111

		DES_BL_10,  //11	1100 0000
		DES_BL_10,  //11	1100 0001
		DES_BL_10,  //11	1100 0010
		DES_BL_10,  //11	1100 0011
		DES_BL_10,  //11	1100 0100
		DES_BL_10,  //11	1100 0101
		DES_BL_10,  //11	1100 0110
		DES_BL_10,  //11	1100 0111
		DES_BL_10,  //11	1100 1000
		DES_BL_10,  //11	1100 1001
		DES_BL_10,  //11	1100 1010
		DES_BL_10,  //11	1100 1011
		DES_BL_10,  //11	1100 1100
		DES_BL_10,  //11	1100 1101
		DES_BL_10,  //11	1100 1110
		DES_BL_10,  //11	1100 1111

		DES_BL_10,  //11	1101 0000
		DES_BL_10,  //11	1101 0001
		DES_BL_10,  //11	1101 0010
		DES_BL_10,  //11	1101 0011
		DES_BL_10,  //11	1101 0100
		DES_BL_10,  //11	1101 0101
		DES_BL_10,  //11	1101 0110
		DES_BL_10,  //11	1101 0111
		DES_BL_10,  //11	1101 1000
		DES_BL_10,  //11	1101 1001
		DES_BL_10,  //11	1101 1010
		DES_BL_10,  //11	1101 1011
		DES_BL_10,  //11	1101 1100
		DES_BL_10,  //11	1101 1101
		DES_BL_10,  //11	1101 1110
		DES_BL_10,  //11	1101 1111

		DES_BL_THUMB,  //11 1110	0000
		DES_BL_THUMB,  //11 1110	0001
		DES_BL_THUMB,  //11 1110	0010
		DES_BL_THUMB,  //11 1110	0011
		DES_BL_THUMB,  //11 1110	0100
		DES_BL_THUMB,  //11 1110	0101
		DES_BL_THUMB,  //11 1110	0110
		DES_BL_THUMB,  //11 1110	0111
		DES_BL_THUMB,  //11 1110	1000
		DES_BL_THUMB,  //11 1110	1001
		DES_BL_THUMB,  //11 1110	1010
		DES_BL_THUMB,  //11 1110	1011
		DES_BL_THUMB,  //11 1110	1100
		DES_BL_THUMB,  //11 1110	1101
		DES_BL_THUMB,  //11 1110	1110
		DES_BL_THUMB,  //11 1110	1111

		DES_BL_THUMB,  //11 1111	0000
		DES_BL_THUMB,  //11 1111	0001
		DES_BL_THUMB,  //11 1111	0010
		DES_BL_THUMB,  //11 1111	0011
		DES_BL_THUMB,  //11 1111	0100
		DES_BL_THUMB,  //11 1111	0101
		DES_BL_THUMB,  //11 1111	0110
		DES_BL_THUMB,  //11 1111	0111
		DES_BL_THUMB,  //11 1111	1000
		DES_BL_THUMB,  //11 1111	1001
		DES_BL_THUMB,  //11 1111	1010
		DES_BL_THUMB,  //11 1111	1011
		DES_BL_THUMB,  //11 1111	1100
		DES_BL_THUMB,  //11 1111	1101
		DES_BL_THUMB,  //11 1111	1110
		DES_BL_THUMB,  //11 1111	1111
}; //1089
