// NdsFileSystemInfo.cpp : 实现文件
//

#include "stdafx.h"
#include "CrystalTile2.h"
#include ".\NdsFileSystemInfo.h"
#include "CT2ChildFrm.h"
#include "CT2TileView.h"
#include "NdsInfo.h"
#include "CT2MainFrm.h"
#include "CT2InputDlg.h"
#include "TextFileDialog.h"
#include "SDatInfo.h"
#include "CT2HexView.h"

// CNdsFileSystemInfo 对话框

// CRC
unsigned short crc16tab[] =
{
	0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
	0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
	0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
	0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
	0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
	0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
	0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
	0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
	0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
	0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
	0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
	0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
	0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
	0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
	0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
	0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
	0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
	0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
	0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
	0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
	0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
	0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
	0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
	0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
	0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
	0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
	0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
	0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
	0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
	0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
	0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
	0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
};

WORD GetCrc16(BYTE *pCrc, WORD nSize)
{
	WORD wCrc = -1;
	BYTE *pEnd = pCrc+nSize;

	while(pCrc<pEnd) wCrc = (wCrc>>8) ^ crc16tab[(wCrc ^ (*pCrc++)) & 0xFF];

	/*WORD tab[]={0x0000, 0xCC01, 0xD801, 0x1400, 0xF001, 0x3C00, 0x2800, 0xE401,
		0xA001, 0x6C00, 0x7800, 0xB401, 0x5000, 0x9C01, 0x8801, 0x4400};
	for(; pCrc < pEnd; pCrc+=2)
	{
		char c=0;
		while(c<0x10)
		{
			UINT t = tab[wCrc&0x0F];
			wCrc = (wCrc>>4) ^ t;
			t=*(WORD*)pCrc>>c;
			t = tab[t&0x0F];
			wCrc ^= t;
			c+=4;
		}
	}*/

	return wCrc;
}

const unsigned char encr_data[] =
{
	0x99,0xD5,0x20,0x5F,0x57,0x44,0xF5,0xB9,0x6E,0x19,0xA4,0xD9,0x9E,0x6A,0x5A,0x94,
	0xD8,0xAE,0xF1,0xEB,0x41,0x75,0xE2,0x3A,0x93,0x82,0xD0,0x32,0x33,0xEE,0x31,0xD5,
	0xCC,0x57,0x61,0x9A,0x37,0x06,0xA2,0x1B,0x79,0x39,0x72,0xF5,0x55,0xAE,0xF6,0xBE,
	0x5F,0x1B,0x69,0xFB,0xE5,0x9D,0xF1,0xE9,0xCE,0x2C,0xD9,0xA1,0x5E,0x32,0x05,0xE6,
	0xFE,0xD3,0xFE,0xCF,0xD4,0x62,0x04,0x0D,0x8B,0xF5,0xEC,0xB7,0x2B,0x60,0x79,0xBB,
	0x12,0x95,0x31,0x0D,0x6E,0x3F,0xDA,0x2B,0x88,0x84,0xF0,0xF1,0x3D,0x12,0x7E,0x25,
	0x45,0x22,0xF1,0xBB,0x24,0x06,0x1A,0x06,0x11,0xAD,0xDF,0x28,0x8B,0x64,0x81,0x34,
	0x2B,0xEB,0x33,0x29,0x99,0xAA,0xF2,0xBD,0x9C,0x14,0x95,0x9D,0x9F,0xF7,0xF5,0x8C,
	0x72,0x97,0xA1,0x29,0x9D,0xD1,0x5F,0xCF,0x66,0x4D,0x07,0x1A,0xDE,0xD3,0x4A,0x4B,
	0x85,0xC9,0xA7,0xA3,0x17,0x95,0x05,0x3A,0x3D,0x49,0x0A,0xBF,0x0A,0x89,0x8B,0xA2,
	0x4A,0x82,0x49,0xDD,0x27,0x90,0xF1,0x0B,0xE9,0xEB,0x1C,0x6A,0x83,0x76,0x45,0x05,
	0xBA,0x81,0x70,0x61,0x17,0x3F,0x4B,0xDE,0xAE,0xCF,0xAB,0x39,0x57,0xF2,0x3A,0x56,
	0x48,0x11,0xAD,0x8A,0x40,0xE1,0x45,0x3F,0xFA,0x9B,0x02,0x54,0xCA,0xA6,0x93,0xFB,
	0xEF,0x4D,0xFE,0x6F,0xA3,0xD8,0x87,0x9C,0x08,0xBA,0xD5,0x48,0x6A,0x8D,0x2D,0xFD,
	0x6E,0x15,0xF8,0x74,0xBD,0xBE,0x52,0x8B,0x18,0x22,0x8A,0x9E,0xFB,0x74,0x37,0x07,
	0x1B,0x36,0x6C,0x4A,0x19,0xBA,0x42,0x62,0xB9,0x79,0x91,0x10,0x7B,0x67,0x65,0x96,
	0xFE,0x02,0x23,0xE8,0xEE,0x99,0x8C,0x77,0x3E,0x5C,0x86,0x64,0x4D,0x6D,0x78,0x86,
	0xA5,0x4F,0x65,0xE2,0x1E,0xB2,0xDF,0x5A,0x0A,0xD0,0x7E,0x08,0x14,0xB0,0x71,0xAC,
	0xBD,0xDB,0x83,0x1C,0xB9,0xD7,0xA1,0x62,0xCD,0xC6,0x63,0x7C,0x52,0x69,0xC3,0xE6,
	0xBF,0x75,0xCE,0x12,0x44,0x5D,0x21,0x04,0xFA,0xFB,0xD3,0x3C,0x38,0x11,0x63,0xD4,
	0x95,0x85,0x41,0x49,0x46,0x09,0xF2,0x08,0x43,0x11,0xDC,0x1F,0x76,0xC0,0x15,0x6D,
	0x1F,0x3C,0x63,0x70,0xEA,0x87,0x80,0x6C,0xC3,0xBD,0x63,0x8B,0xC2,0x37,0x21,0x37,
	0xDC,0xEE,0x09,0x23,0x2E,0x37,0x6A,0x4D,0x73,0x90,0xF7,0x50,0x30,0xAC,0x1C,0x92,
	0x04,0x10,0x23,0x91,0x4F,0xD2,0x07,0xAA,0x68,0x3E,0x4F,0x9A,0xC9,0x64,0x60,0x6A,
	0xC8,0x14,0x21,0xF3,0xD6,0x22,0x41,0x12,0x44,0x24,0xCF,0xE6,0x8A,0x56,0xDD,0x0D,
	0x53,0x4D,0xE1,0x85,0x1E,0x8C,0x52,0x5A,0x9C,0x19,0x84,0xC2,0x03,0x57,0xF1,0x6F,
	0xE3,0x00,0xBE,0x58,0xF6,0x4C,0xED,0xD5,0x21,0x64,0x9C,0x1F,0xBE,0x55,0x03,0x3C,
	0x4A,0xDC,0xFF,0xAA,0xC9,0xDA,0xE0,0x5D,0x5E,0xBF,0xE6,0xDE,0xF5,0xD8,0xB1,0xF8,
	0xFF,0x36,0xB3,0xB9,0x62,0x67,0x95,0xDB,0x31,0x5F,0x37,0xED,0x4C,0x70,0x67,0x99,
	0x90,0xB5,0x18,0x31,0x6C,0x3D,0x99,0x99,0xE4,0x42,0xDA,0xD3,0x25,0x42,0x13,0xA0,
	0xAE,0xD7,0x70,0x6C,0xB1,0x55,0xCF,0xC7,0xD7,0x46,0xD5,0x43,0x61,0x17,0x3D,0x44,
	0x28,0xE9,0x33,0x85,0xD5,0xD0,0xA2,0x93,0xAA,0x25,0x12,0x1F,0xFB,0xC5,0x0B,0x46,
	0xF5,0x97,0x76,0x56,0x45,0xA6,0xBE,0x87,0xB1,0x94,0x6B,0xE8,0xB1,0xFE,0x33,0x99,
	0xAE,0x1F,0x3E,0x6C,0x39,0x71,0x1D,0x09,0x00,0x90,0x37,0xE4,0x10,0x3E,0x75,0x74,
	0xFF,0x8C,0x83,0x3B,0xB0,0xF1,0xB0,0xF9,0x01,0x05,0x47,0x42,0x95,0xF1,0xD6,0xAC,
	0x7E,0x38,0xE6,0x9E,0x95,0x74,0x26,0x3F,0xB4,0x68,0x50,0x18,0xD0,0x43,0x30,0xB4,
	0x4C,0x4B,0xE3,0x68,0xBF,0xE5,0x4D,0xB6,0x95,0x8B,0x0A,0xA0,0x74,0x25,0x32,0x77,
	0xCF,0xA1,0xF7,0x2C,0xD8,0x71,0x13,0x5A,0xAB,0xEA,0xC9,0x51,0xE8,0x0D,0xEE,0xEF,
	0xE9,0x93,0x7E,0x19,0xA7,0x1E,0x43,0x38,0x81,0x16,0x2C,0xA1,0x48,0xE3,0x73,0xCC,
	0x29,0x21,0x6C,0xD3,0x5D,0xCE,0xA0,0xD9,0x61,0x71,0x43,0xA0,0x15,0x13,0xB5,0x64,
	0x92,0xCF,0x2A,0x19,0xDC,0xAD,0xB7,0xA5,0x9F,0x86,0x65,0xF8,0x1A,0x9F,0xE7,0xFB,
	0xF7,0xFD,0xB8,0x13,0x6C,0x27,0xDB,0x6F,0xDF,0x35,0x1C,0xF7,0x8D,0x2C,0x5B,0x9B,
	0x12,0xAB,0x38,0x64,0x06,0xCC,0xDE,0x31,0xE8,0x4E,0x75,0x11,0x64,0xE3,0xFA,0xEA,
	0xEB,0x34,0x54,0xC2,0xAD,0x3F,0x34,0xEB,0x93,0x2C,0x7D,0x26,0x36,0x9D,0x56,0xF3,
	0x5A,0xE1,0xF6,0xB3,0x98,0x63,0x4A,0x9E,0x32,0x83,0xE4,0x9A,0x84,0x60,0x7D,0x90,
	0x2E,0x13,0x0E,0xEE,0x93,0x4B,0x36,0xA2,0x85,0xEC,0x16,0x38,0xE8,0x88,0x06,0x02,
	0xBF,0xF0,0xA0,0x3A,0xED,0xD7,0x6A,0x9A,0x73,0xE1,0x57,0xCF,0xF8,0x44,0xB8,0xDC,
	0x2E,0x23,0x59,0xD1,0xDF,0x95,0x52,0x71,0x99,0x61,0xA0,0x4B,0xD5,0x7F,0x6E,0x78,
	0xBA,0xA9,0xC5,0x30,0xD3,0x40,0x86,0x32,0x9D,0x32,0x0C,0x9C,0x37,0xB7,0x02,0x2F,
	0xBA,0x54,0x98,0xA9,0xC4,0x13,0x04,0xC9,0x8D,0xBE,0xC8,0xE7,0x5D,0x97,0x50,0x2E,
	0x93,0xD6,0x22,0x59,0x0C,0x27,0xBC,0x22,0x92,0xE0,0xA7,0x20,0x0F,0x93,0x6F,0x7F,
	0x4C,0x9F,0xD3,0xB5,0xA6,0x2A,0x0B,0x74,0x67,0x49,0x7D,0x10,0x26,0xCB,0xD1,0xC5,
	0x86,0x71,0xE7,0x8C,0xA0,0x9C,0xE9,0x5B,0xB2,0x1A,0xF6,0x01,0xEE,0x8C,0x9E,0x5E,
	0x83,0xF2,0x1A,0xDB,0xE6,0xE5,0xEA,0x84,0x59,0x76,0xD2,0x7C,0xF6,0x8D,0xA5,0x49,
	0x36,0x48,0xC2,0x16,0x52,0xBB,0x83,0xA3,0x74,0xB9,0x07,0x0C,0x3B,0xFF,0x61,0x28,
	0xE1,0x61,0xE9,0xE4,0xEF,0x6E,0x15,0xAA,0x4E,0xBA,0xE8,0x5D,0x05,0x96,0xBB,0x32,
	0x56,0xB0,0xFB,0x72,0x52,0x0F,0x0E,0xC8,0x42,0x25,0x65,0x76,0x89,0xAF,0xF2,0xDE,
	0x10,0x27,0xF0,0x01,0x4B,0x74,0xA7,0x97,0x07,0xD5,0x26,0x54,0x54,0x09,0x1F,0x82,
	0x0A,0x86,0x7D,0x30,0x39,0x0E,0xB3,0x26,0x9B,0x0B,0x57,0xBB,0x36,0x06,0x31,0xAF,
	0xFD,0x79,0xFC,0xD9,0x30,0x10,0x2B,0x0C,0xB3,0xE1,0x9B,0xD7,0x7B,0xDC,0x5F,0xEF,
	0xD2,0xF8,0x13,0x45,0x4D,0x47,0x75,0xBD,0x46,0x96,0x3C,0x7E,0x75,0xF3,0x3E,0xB5,
	0x67,0xC5,0x9A,0x3B,0xB0,0x5B,0x29,0x6B,0xDE,0x80,0x5B,0xC8,0x15,0x05,0xB1,0x31,
	0xB6,0xCE,0x49,0xDD,0xAD,0x84,0xB5,0xAE,0x60,0xDC,0x67,0x31,0x34,0x30,0xFE,0x4E,
	0xBD,0x80,0x2F,0xA6,0xBF,0x63,0x39,0x21,0x86,0xD9,0x35,0x7F,0x16,0x68,0x22,0x05,
	0x54,0xE9,0x90,0x26,0x8C,0x07,0x6C,0x51,0xA4,0x31,0x55,0xD7,0x09,0x07,0xA8,0x3E,
	0x2E,0x53,0x66,0xC1,0xF8,0xF2,0x7B,0xC4,0xF2,0x58,0xCF,0xF1,0x87,0xC5,0xA2,0xE7,
	0x27,0x8F,0x30,0x87,0x58,0xA0,0x64,0x62,0x23,0x18,0xB9,0x88,0x7C,0xFA,0xCE,0xC4,
	0x98,0xAE,0xAD,0x17,0xCC,0x4A,0x5B,0xF3,0xE9,0x48,0xD5,0x56,0xD3,0x0D,0xF2,0xC8,
	0x92,0x73,0x8C,0xDB,0xD7,0x2F,0x56,0xAC,0x81,0xF9,0x92,0x69,0x4D,0xC6,0x32,0xF6,
	0xE6,0xC0,0x8D,0x21,0xE2,0x76,0x80,0x61,0x11,0xBC,0xDC,0x6C,0x93,0xAF,0x19,0x69,
	0x9B,0xD0,0xBF,0xB9,0x31,0x9F,0x02,0x67,0xA3,0x51,0xEE,0x83,0x06,0x22,0x7B,0x0C,
	0xAB,0x49,0x42,0x40,0xB8,0xD5,0x01,0x7D,0xCE,0x5E,0xF7,0x55,0x53,0x39,0xC5,0x99,
	0x46,0xD8,0x87,0x9F,0xBA,0xF7,0x64,0xB4,0xE3,0x9A,0xFA,0xA1,0x6D,0x90,0x68,0x10,
	0x30,0xCA,0x8A,0x54,0xA7,0x9F,0x60,0xC3,0x19,0xF5,0x6B,0x0D,0x7A,0x51,0x98,0xE6,
	0x98,0x43,0x51,0xB4,0xD6,0x35,0xE9,0x4F,0xC3,0xDF,0x0F,0x7B,0xD6,0x2F,0x5C,0xBD,
	0x3A,0x15,0x61,0x19,0xF1,0x4B,0xCB,0xAA,0xDC,0x6D,0x64,0xC9,0xD3,0xC6,0x1E,0x56,
	0xEF,0x38,0x4C,0x50,0x71,0x86,0x75,0xCC,0x0D,0x0D,0x4E,0xE9,0x28,0xF6,0x06,0x5D,
	0x70,0x1B,0xAA,0xD3,0x45,0xCF,0xA8,0x39,0xAC,0x95,0xA6,0x2E,0xB4,0xE4,0x22,0xD4,
	0x74,0xA8,0x37,0x5F,0x48,0x7A,0x04,0xCC,0xA5,0x4C,0x40,0xD8,0x28,0xB4,0x28,0x08,
	0x0D,0x1C,0x72,0x52,0x41,0xF0,0x7D,0x47,0x19,0x3A,0x53,0x4E,0x58,0x84,0x62,0x6B,
	0x93,0xB5,0x8A,0x81,0x21,0x4E,0x0D,0xDC,0xB4,0x3F,0xA2,0xC6,0xFC,0xC9,0x2B,0x40,
	0xDA,0x38,0x04,0xE9,0x5E,0x5A,0x86,0x6B,0x0C,0x22,0x25,0x85,0x68,0x11,0x8D,0x7C,
	0x92,0x1D,0x95,0x55,0x4D,0xAB,0x8E,0xBB,0xDA,0xA6,0xE6,0xB7,0x51,0xB6,0x32,0x5A,
	0x05,0x41,0xDD,0x05,0x2A,0x0A,0x56,0x50,0x91,0x17,0x47,0xCC,0xC9,0xE6,0x7E,0xB5,
	0x61,0x4A,0xDB,0x73,0x67,0x51,0xC8,0x33,0xF5,0xDA,0x6E,0x74,0x2E,0x54,0xC3,0x37,
	0x0D,0x6D,0xAF,0x08,0xE8,0x15,0x8A,0x5F,0xE2,0x59,0x21,0xCD,0xA8,0xDE,0x0C,0x06,
	0x5A,0x77,0x6B,0x5F,0xDB,0x18,0x65,0x3E,0xC8,0x50,0xDE,0x78,0xE0,0xB8,0x82,0xB3,
	0x5D,0x4E,0x72,0x32,0x07,0x4F,0xC1,0x34,0x23,0xBA,0x96,0xB7,0x67,0x4E,0xA4,0x28,
	0x1E,0x34,0x62,0xEB,0x2D,0x6A,0x70,0xE9,0x2F,0x42,0xC4,0x70,0x4E,0x5A,0x31,0x9C,
	0xF9,0x5B,0x47,0x28,0xAA,0xDA,0x71,0x6F,0x38,0x1F,0xB3,0x78,0xC4,0x92,0x6B,0x1C,
	0x9E,0xF6,0x35,0x9A,0xB7,0x4D,0x0E,0xBF,0xCC,0x18,0x29,0x41,0x03,0x48,0x35,0x5D,
	0x55,0xD0,0x2B,0xC6,0x29,0xAF,0x5C,0x60,0x74,0x69,0x8E,0x5E,0x9B,0x7C,0xD4,0xBD,
	0x7B,0x44,0x64,0x7D,0x3F,0x92,0x5D,0x69,0xB6,0x1F,0x00,0x4B,0xD4,0x83,0x35,0xCF,
	0x7E,0x64,0x4E,0x17,0xAE,0x8D,0xD5,0x2E,0x9A,0x28,0x12,0x4E,0x2E,0x2B,0x49,0x08,
	0x5C,0xAE,0xC6,0x46,0x85,0xAE,0x41,0x61,0x1E,0x6F,0x82,0xD2,0x51,0x37,0x16,0x1F,
	0x0B,0xF6,0x59,0xA4,0x9A,0xCA,0x5A,0xAF,0x0D,0xD4,0x33,0x8B,0x20,0x63,0xF1,0x84,
	0x80,0x5C,0xCB,0xCF,0x08,0xB4,0xB9,0xD3,0x16,0x05,0xBD,0x62,0x83,0x31,0x9B,0x56,
	0x51,0x98,0x9F,0xBA,0xB2,0x5B,0xAA,0xB2,0x22,0x6B,0x2C,0xB5,0xD4,0x48,0xFA,0x63,
	0x2B,0x5F,0x58,0xFA,0x61,0xFA,0x64,0x09,0xBB,0x38,0xE0,0xB8,0x9D,0x92,0x60,0xA8,
	0x0D,0x67,0x6F,0x0E,0x37,0xF5,0x0D,0x01,0x9F,0xC2,0x77,0xD4,0xFE,0xEC,0xF1,0x73,
	0x30,0x39,0xE0,0x7D,0xF5,0x61,0x98,0xE4,0x2C,0x28,0x55,0x04,0x56,0x55,0xDB,0x2F,
	0x6B,0xEC,0xE5,0x58,0x06,0xB6,0x64,0x80,0x6A,0x2A,0x1A,0x4E,0x5B,0x0F,0xD8,0xC4,
	0x0A,0x2E,0x52,0x19,0xD9,0x62,0xF5,0x30,0x48,0xBE,0x8C,0x7B,0x4F,0x38,0x9B,0xA2,
	0xC3,0xAF,0xC9,0xD3,0xC7,0xC1,0x62,0x41,0x86,0xB9,0x61,0x21,0x57,0x6F,0x99,0x4F,
	0xC1,0xBA,0xCE,0x7B,0xB5,0x3B,0x4D,0x5E,0x8A,0x8B,0x44,0x57,0x5F,0x13,0x5F,0x70,
	0x6D,0x5B,0x29,0x47,0xDC,0x38,0xE2,0xEC,0x04,0x55,0x65,0x12,0x2A,0xE8,0x17,0x43,
	0xE1,0x8E,0xDD,0x2A,0xB3,0xE2,0x94,0xF7,0x09,0x6E,0x5C,0xE6,0xEB,0x8A,0xF8,0x6D,
	0x89,0x49,0x54,0x48,0xF5,0x2F,0xAD,0xBF,0xEA,0x94,0x4B,0xCA,0xFC,0x39,0x87,0x82,
	0x5F,0x8A,0x01,0xF2,0x75,0xF2,0xE6,0x71,0xD6,0xD8,0x42,0xDE,0xF1,0x2D,0x1D,0x28,
	0xA6,0x88,0x7E,0xA3,0xA0,0x47,0x1D,0x30,0xD9,0xA3,0x71,0xDF,0x49,0x1C,0xCB,0x01,
	0xF8,0x36,0xB1,0xF2,0xF0,0x22,0x58,0x5D,0x45,0x6B,0xBD,0xA0,0xBB,0xB2,0x88,0x42,
	0xC7,0x8C,0x28,0xCE,0x93,0xE8,0x90,0x63,0x08,0x90,0x7C,0x89,0x3C,0xF5,0x7D,0xB7,
	0x04,0x2D,0x4F,0x55,0x51,0x16,0xFD,0x7E,0x79,0xE8,0xBE,0xC1,0xF2,0x12,0xD4,0xF8,
	0xB4,0x84,0x05,0x23,0xA0,0xCC,0xD2,0x2B,0xFD,0xE1,0xAB,0xAD,0x0D,0xD1,0x55,0x6C,
	0x23,0x41,0x94,0x4D,0x77,0x37,0x4F,0x05,0x28,0x0C,0xBF,0x17,0xB3,0x12,0x67,0x6C,
	0x8C,0xC3,0x5A,0xF7,0x41,0x84,0x2A,0x6D,0xD0,0x94,0x12,0x27,0x2C,0xB4,0xED,0x9C,
	0x4D,0xEC,0x47,0x82,0x97,0xD5,0x67,0xB9,0x1B,0x9D,0xC0,0x55,0x07,0x7E,0xE5,0x8E,
	0xE2,0xA8,0xE7,0x3E,0x12,0xE4,0x0E,0x3A,0x2A,0x45,0x55,0x34,0xA2,0xF9,0x2D,0x5A,
	0x1B,0xAB,0x52,0x7C,0x83,0x10,0x5F,0x55,0xD2,0xF1,0x5A,0x43,0x2B,0xC6,0xA7,0xA4,
	0x89,0x15,0x95,0xE8,0xB4,0x4B,0x9D,0xF8,0x75,0xE3,0x9F,0x60,0x78,0x5B,0xD6,0xE6,
	0x0D,0x44,0xE6,0x21,0x06,0xBD,0x47,0x22,0x53,0xA4,0x00,0xAD,0x8D,0x43,0x13,0x85,
	0x39,0xF7,0xAA,0xFC,0x38,0xAF,0x7B,0xED,0xFC,0xE4,0x2B,0x54,0x50,0x98,0x4C,0xFC,
	0x85,0x80,0xF7,0xDF,0x3C,0x80,0x22,0xE1,0x94,0xDA,0xDE,0x24,0xC6,0xB0,0x7A,0x39,
	0x38,0xDC,0x0F,0xA1,0xA7,0xF4,0xF9,0x6F,0x63,0x18,0x57,0x8B,0x84,0x41,0x2A,0x2E,
	0xD4,0x53,0xF2,0xD9,0x00,0x0F,0xD0,0xDD,0x99,0x6E,0x19,0xA6,0x0A,0xD0,0xEC,0x5B,
	0x58,0x24,0xAB,0xC0,0xCB,0x06,0x65,0xEC,0x1A,0x13,0x38,0x94,0x0A,0x67,0x03,0x2F,
	0x3F,0xF7,0xE3,0x77,0x44,0x77,0x33,0xC6,0x14,0x39,0xD0,0xE3,0xC0,0xA2,0x08,0x79,
	0xBB,0x40,0x99,0x57,0x41,0x0B,0x01,0x90,0xCD,0xE1,0xCC,0x48,0x67,0xDB,0xB3,0xAF,
	0x88,0x74,0xF3,0x4C,0x82,0x8F,0x72,0xB1,0xB5,0x23,0x29,0xC4,0x12,0x6C,0x19,0xFC,
	0x8E,0x46,0xA4,0x9C,0xC4,0x25,0x65,0x87,0xD3,0x6D,0xBE,0x8A,0x93,0x11,0x03,0x38,
	0xED,0x83,0x2B,0xF3,0x46,0xA4,0x93,0xEA,0x3B,0x53,0x85,0x1D,0xCE,0xD4,0xF1,0x08,
	0x83,0x27,0xED,0xFC,0x9B,0x1A,0x18,0xBC,0xF9,0x8B,0xAE,0xDC,0x24,0xAB,0x50,0x38,
	0xE9,0x72,0x4B,0x10,0x22,0x17,0x7B,0x46,0x5D,0xAB,0x59,0x64,0xF3,0x40,0xAE,0xF8,
	0xBB,0xE5,0xC8,0xF9,0x26,0x03,0x4E,0x55,0x7D,0xEB,0xEB,0xFE,0xF7,0x39,0xE6,0xE0,
	0x0A,0x11,0xBE,0x2E,0x28,0xFF,0x98,0xED,0xC0,0xC9,0x42,0x56,0x42,0xC3,0xFD,0x00,
	0xF6,0xAF,0x87,0xA2,0x5B,0x01,0x3F,0x32,0x92,0x47,0x95,0x9A,0x72,0xA5,0x32,0x3D,
	0xAE,0x6B,0xD0,0x9B,0x07,0xD2,0x49,0x92,0xE3,0x78,0x4A,0xFA,0xA1,0x06,0x7D,0xF2,
	0x41,0xCF,0x77,0x74,0x04,0x14,0xB2,0x0C,0x86,0x84,0x64,0x16,0xD5,0xBB,0x51,0xA1,
	0xE5,0x6F,0xF1,0xD1,0xF2,0xE2,0xF7,0x5F,0x58,0x20,0x4D,0xB8,0x57,0xC7,0xCF,0xDD,
	0xC5,0xD8,0xBE,0x76,0x3D,0xF6,0x5F,0x7E,0xE7,0x2A,0x8B,0x88,0x24,0x1B,0x38,0x3F,
	0x0E,0x41,0x23,0x77,0xF5,0xF0,0x4B,0xD4,0x0C,0x1F,0xFA,0xA4,0x0B,0x80,0x5F,0xCF,
	0x45,0xF6,0xE0,0xDA,0x2F,0x34,0x59,0x53,0xFB,0x20,0x3C,0x52,0x62,0x5E,0x35,0xB5,
	0x62,0xFE,0x8B,0x60,0x63,0xE3,0x86,0x5A,0x15,0x1A,0x6E,0xD1,0x47,0x45,0xBC,0x32,
	0xB4,0xEB,0x67,0x38,0xAB,0xE4,0x6E,0x33,0x3A,0xB5,0xED,0xA3,0xAD,0x67,0xE0,0x4E,
	0x41,0x95,0xEE,0x62,0x62,0x71,0x26,0x1D,0x31,0xEF,0x62,0x30,0xAF,0xD7,0x82,0xAC,
	0xC2,0xDC,0x05,0x04,0xF5,0x97,0x07,0xBF,0x11,0x59,0x23,0x07,0xC0,0x64,0x02,0xE8,
	0x97,0xE5,0x3E,0xAF,0x18,0xAC,0x59,0xA6,0x8B,0x4A,0x33,0x90,0x1C,0x6E,0x7C,0x9C,
	0x20,0x7E,0x4C,0x3C,0x3E,0x61,0x64,0xBB,0xC5,0x6B,0x7C,0x7E,0x3E,0x9F,0xC5,0x4C,
	0x9F,0xEA,0x73,0xF5,0xD7,0x89,0xC0,0x4C,0xF4,0xFB,0xF4,0x2D,0xEC,0x14,0x1B,0x51,
	0xD5,0xC1,0x12,0xC8,0x10,0xDF,0x0B,0x4A,0x8B,0x9C,0xBC,0x93,0x45,0x6A,0x3E,0x3E,
	0x7D,0xC1,0xA9,0xBA,0xCD,0xC1,0xB4,0x07,0xE4,0xE1,0x68,0x86,0x43,0xB2,0x6D,0x38,
	0xF3,0xFB,0x0C,0x5C,0x66,0x37,0x71,0xDE,0x56,0xEF,0x6E,0xA0,0x10,0x40,0x65,0xA7,
	0x98,0xF7,0xD0,0xBE,0x0E,0xC8,0x37,0x36,0xEC,0x10,0xCA,0x7C,0x9C,0xAB,0x84,0x1E,
	0x05,0x17,0x76,0x02,0x1C,0x4F,0x52,0xAA,0x5F,0xC1,0xC6,0xA0,0x56,0xB9,0xD8,0x04,
	0x84,0x44,0x4D,0xA7,0x59,0xD8,0xDE,0x60,0xE6,0x38,0x0E,0x05,0x8F,0x03,0xE1,0x3B,
	0x6D,0x81,0x04,0x33,0x6F,0x30,0x0B,0xCE,0x69,0x05,0x21,0x33,0xFB,0x26,0xBB,0x89,
	0x7D,0xB6,0xAE,0x87,0x7E,0x51,0x07,0xE0,0xAC,0xF7,0x96,0x0A,0x6B,0xF9,0xC4,0x5C,
	0x1D,0xE4,0x44,0x47,0xB8,0x5E,0xFA,0xE3,0x78,0x84,0x55,0x42,0x4B,0x48,0x5E,0xF7,
	0x7D,0x47,0x35,0x86,0x1D,0x2B,0x43,0x05,0x03,0xEC,0x8A,0xB8,0x1E,0x06,0x3C,0x76,
	0x0C,0x48,0x1A,0x43,0xA7,0xB7,0x8A,0xED,0x1E,0x13,0xC6,0x43,0xEE,0x10,0xEF,0xDB,
	0xEC,0xFB,0x3C,0x83,0xB2,0x95,0x44,0xEF,0xD8,0x54,0x51,0x4E,0x2D,0x11,0x44,0x1D,
	0xFB,0x36,0x59,0x1E,0x7A,0x34,0xC1,0xC3,0xCA,0x57,0x00,0x61,0xEA,0x67,0xA5,0x16,
	0x9B,0x55,0xD0,0x55,0xE1,0x7F,0xD9,0x36,0xD2,0x40,0x76,0xAE,0xDC,0x01,0xCE,0xB0,
	0x7A,0x83,0xD5,0xCB,0x20,0x98,0xEC,0x6B,0xC1,0x72,0x92,0x34,0xF3,0x82,0x57,0x37,
	0x62,0x8A,0x32,0x36,0x0C,0x90,0x43,0xAE,0xAE,0x5C,0x9B,0x78,0x8E,0x13,0x65,0x02,
	0xFD,0x68,0x71,0xC1,0xFE,0xB0,0x31,0xA0,0x24,0x82,0xB0,0xC3,0xB1,0x79,0x69,0xA7,
	0xF5,0xD2,0xEB,0xD0,0x82,0xC0,0x32,0xDC,0x9E,0xC7,0x26,0x3C,0x6D,0x8D,0x98,0xC1,
	0xBB,0x22,0xD4,0xD0,0x0F,0x33,0xEC,0x3E,0xB9,0xCC,0xE1,0xDC,0x6A,0x4C,0x77,0x36,
	0x14,0x1C,0xF9,0xBF,0x81,0x9F,0x28,0x5F,0x71,0x85,0x32,0x29,0x90,0x75,0x48,0xC4,
	0xB3,0x4A,0xCE,0xD8,0x44,0x8F,0x14,0x2F,0xFD,0x40,0x57,0xEF,0xAA,0x08,0x75,0xD9,
	0x46,0xD1,0xD6,0x6E,0x32,0x55,0x1F,0xC3,0x18,0xFE,0x84,0x1F,0xFC,0x84,0xD5,0xFF,
	0x71,0x5E,0x1B,0x48,0xC3,0x86,0x95,0x0E,0x28,0x08,0x27,0xD3,0x38,0x83,0x71,0x7B,
	0x4C,0x80,0x63,0x54,0x9A,0x56,0xB0,0xAC,0xCF,0x80,0xCA,0x31,0x09,0xEF,0xFE,0xF3,
	0xBE,0xAF,0x24,0x7E,0xA6,0xFE,0x53,0x3F,0xC2,0x8D,0x4A,0x33,0x68,0xD1,0x22,0xA6,
	0x66,0xAD,0x7B,0xEA,0xDE,0xB6,0x43,0xB0,0xA1,0x25,0x95,0x00,0xA3,0x3F,0x75,0x46,
	0x14,0x11,0x44,0xEC,0xD7,0x95,0xBC,0x92,0xF0,0x4F,0xA9,0x16,0x53,0x62,0x97,0x60,
	0x2A,0x0F,0x41,0xF1,0x71,0x24,0xBE,0xEE,0x94,0x7F,0x08,0xCD,0x60,0x93,0xB3,0x85,
	0x5B,0x07,0x00,0x3F,0xD8,0x0F,0x28,0x83,0x9A,0xD1,0x69,0x9F,0xD1,0xDA,0x2E,0xC3,
	0x90,0x01,0xA2,0xB9,0x6B,0x4E,0x2A,0x66,0x9D,0xDA,0xAE,0xA6,0xEA,0x2A,0xD3,0x68,
	0x2F,0x0C,0x0C,0x9C,0xD2,0x8C,0x4A,0xED,0xE2,0x9E,0x57,0x65,0x9D,0x09,0x87,0xA3,
	0xB4,0xC4,0x32,0x5D,0xC9,0xD4,0x32,0x2B,0xB1,0xE0,0x71,0x1E,0x64,0x4D,0xE6,0x90,
	0x71,0xE3,0x1E,0x40,0xED,0x7D,0xF3,0x84,0x0E,0xED,0xC8,0x78,0x76,0xAE,0xC0,0x71,
	0x27,0x72,0xBB,0x05,0xEA,0x02,0x64,0xFB,0xF3,0x48,0x6B,0xB5,0x42,0x93,0x3F,0xED,
	0x9F,0x13,0x53,0xD2,0xF7,0xFE,0x2A,0xEC,0x1D,0x47,0x25,0xDB,0x3C,0x91,0x86,0xC6,
	0x8E,0xF0,0x11,0xFD,0x23,0x74,0x36,0xF7,0xA4,0xF5,0x9E,0x7A,0x7E,0x53,0x50,0x44,
	0xD4,0x47,0xCA,0xD3,0xEB,0x38,0x6D,0xE6,0xD9,0x71,0x94,0x7F,0x4A,0xC6,0x69,0x4B,
	0x11,0xF4,0x52,0xEA,0x22,0xFE,0x8A,0xB0,0x36,0x67,0x8B,0x59,0xE8,0xE6,0x80,0x2A,
	0xEB,0x65,0x04,0x13,0xEE,0xEC,0xDC,0x9E,0x5F,0xB1,0xEC,0x05,0x6A,0x59,0xE6,0x9F,
	0x5E,0x59,0x6B,0x89,0xBF,0xF7,0x1A,0xCA,0x44,0xF9,0x5B,0x6A,0x71,0x85,0x03,0xE4,
	0x29,0x62,0xE0,0x70,0x6F,0x41,0xC4,0xCF,0xB2,0xB1,0xCC,0xE3,0x7E,0xA6,0x07,0xA8,
	0x87,0xE7,0x7F,0x84,0x93,0xDB,0x52,0x4B,0x6C,0xEC,0x7E,0xDD,0xD4,0x24,0x48,0x10,
	0x69,0x9F,0x04,0x60,0x74,0xE6,0x48,0x18,0xF3,0xE4,0x2C,0xB9,0x4F,0x2E,0x50,0x7A,
	0xDF,0xD4,0x54,0x69,0x2B,0x8B,0xA7,0xF3,0xCE,0xFF,0x1F,0xF3,0x3E,0x26,0x01,0x39,
	0x17,0x95,0x84,0x89,0xB0,0xF0,0x4C,0x4B,0x82,0x91,0x9F,0xC4,0x4B,0xAC,0x9D,0xA5,
	0x74,0xAF,0x17,0x25,0xC9,0xCA,0x32,0xD3,0xBC,0x89,0x8A,0x84,0x89,0xCC,0x0D,0xAE,
	0x7C,0xA2,0xDB,0x9C,0x6A,0x78,0x91,0xEE,0xEA,0x76,0x5D,0x4E,0x87,0x60,0xF5,0x69,
	0x15,0x67,0xD4,0x02,0xCF,0xAF,0x48,0x36,0x07,0xEA,0xBF,0x6F,0x66,0x2D,0x06,0x8F,
	0xC4,0x9A,0xFE,0xF9,0xF6,0x90,0x87,0x75,0xB8,0xF7,0xAD,0x0F,0x76,0x10,0x5A,0x3D,
	0x59,0xB0,0x2E,0xB3,0xC7,0x35,0x2C,0xCC,0x70,0x56,0x2B,0xCB,0xE3,0x37,0x96,0xC5,
	0x2F,0x46,0x1B,0x8A,0x22,0x46,0xC7,0x88,0xA7,0x26,0x32,0x98,0x61,0xDF,0x86,0x22,
	0x8A,0xF4,0x1C,0x2F,0x87,0xA1,0x09,0xAA,0xCC,0xA9,0xAE,0xD3,0xBD,0x00,0x45,0x1C,
	0x9A,0x54,0x87,0x86,0x52,0x87,0xEF,0xFF,0x1E,0x8F,0xA1,0x8F,0xC1,0x89,0x5C,0x35,
	0x1B,0xDA,0x2D,0x3A,0x2C,0x16,0xB2,0xC2,0xF1,0x56,0xE2,0x78,0xC1,0x6B,0x63,0x97,
	0xC5,0x56,0x8F,0xC9,0x32,0x7F,0x2C,0xAA,0xAF,0xA6,0xA8,0xAC,0x20,0x91,0x22,0x88,
	0xDE,0xE4,0x60,0x8B,0xF9,0x4B,0x42,0x25,0x1A,0xE3,0x7F,0x9C,0x2C,0x19,0x89,0x3A,
	0x7E,0x05,0xD4,0x36,0xCC,0x69,0x58,0xC2,0xC1,0x32,0x8B,0x2F,0x90,0x85,0xEB,0x7A,
	0x39,0x50,0xA5,0xA1,0x27,0x92,0xC5,0x66,0xB0,0x20,0x4F,0x58,0x7E,0x55,0x83,0x43,
	0x2B,0x45,0xE2,0x9C,0xE4,0xD8,0x12,0x90,0x2C,0x16,0x83,0x56,0x16,0x79,0x03,0xB3,
	0xAD,0x2D,0x61,0x18,0x1A,0x13,0x1F,0x37,0xE2,0xE1,0x9C,0x73,0x7B,0x80,0xD5,0xFD,
	0x2D,0x51,0x87,0xFC,0x7B,0xAA,0xD7,0x1F,0x2C,0x7A,0x8E,0xAF,0xF4,0x8D,0xBB,0xCD,
	0x95,0x11,0x7C,0x72,0x0B,0xEE,0x6F,0xE2,0xB9,0xAF,0xDE,0x37,0x83,0xDE,0x8C,0x8D,
	0x62,0x05,0x67,0xB7,0x96,0xC6,0x8D,0x56,0xB6,0x0D,0xD7,0x62,0xBA,0xD6,0x46,0x36,
	0xBD,0x8E,0xC8,0xE6,0xEA,0x2A,0x6C,0x10,0x14,0xFF,0x6B,0x5B,0xFA,0x82,0x3C,0x46,
	0xB1,0x30,0x43,0x46,0x51,0x8A,0x7D,0x9B,0x92,0x3E,0x83,0x79,0x5B,0x55,0x5D,0xB2,
	0x6C,0x5E,0xCE,0x90,0x62,0x8E,0x53,0x98,0xC9,0x0D,0x6D,0xE5,0x2D,0x57,0xCD,0xC5,
	0x81,0x57,0xBA,0xE1,0xE8,0xB8,0x8F,0x72,0xE5,0x4F,0x13,0xDC,0xEA,0x9D,0x71,0x15,
	0x10,0xB2,0x11,0x88,0xD5,0x09,0xD4,0x7F,0x5B,0x65,0x7F,0x2C,0x3B,0x38,0x4C,0x11,
	0x68,0x50,0x8D,0xFB,0x9E,0xB0,0x59,0xBF,0x94,0x80,0x89,0x4A,0xC5,0x1A,0x18,0x12,
	0x89,0x53,0xD1,0x4A,0x10,0x29,0xE8,0x8C,0x1C,0xEC,0xB6,0xEA,0x46,0xC7,0x17,0x8B,
	0x25,0x15,0x31,0xA8,0xA2,0x6B,0x43,0xB1,0x9D,0xE2,0xDB,0x0B,0x87,0x9B,0xB0,0x11,
	0x04,0x0E,0x71,0xD2,0x29,0x77,0x89,0x82,0x0A,0x66,0x41,0x7F,0x1D,0x0B,0x48,0xFF,
	0x72,0xBB,0x24,0xFD,0xC2,0x48,0xA1,0x9B,0xFE,0x7B,0x7F,0xCE,0x88,0xDB,0x86,0xD9,
	0x85,0x3B,0x1C,0xB0,0xDC,0xA8,0x33,0x07,0xBF,0x51,0x2E,0xE3,0x0E,0x9A,0x00,0x97,
	0x1E,0x06,0xC0,0x97,0x43,0x9D,0xD8,0xB6,0x45,0xC4,0x86,0x67,0x5F,0x00,0xF8,0x88,
	0x9A,0xA4,0x52,0x9E,0xC7,0xAA,0x8A,0x83,0x75,0xEC,0xC5,0x18,0xAE,0xCE,0xC3,0x2F,
	0x1A,0x2B,0xF9,0x18,0xFF,0xAE,0x1A,0xF5,0x53,0x0B,0xB5,0x33,0x51,0xA7,0xFD,0xE8,
	0xA8,0xE1,0xA2,0x64,0xB6,0x22,0x17,0x43,0x80,0xCC,0x0A,0xD8,0xAE,0x3B,0xBA,0x40,
	0xD7,0xD9,0x92,0x4A,0x89,0xDF,0x04,0x10,0xEE,0x9B,0x18,0x2B,0x6A,0x77,0x69,0x8A,
	0x68,0xF4,0xF9,0xB9,0xA2,0x21,0x15,0x6E,0xE6,0x1E,0x3B,0x03,0x62,0x30,0x9B,0x60,
	0x41,0x7E,0x25,0x9B,0x9E,0x8F,0xC5,0x52,0x10,0x08,0xF8,0xC2,0x69,0xA1,0x21,0x11,
	0x88,0x37,0x5E,0x79,0x35,0x66,0xFF,0x10,0x42,0x18,0x6E,0xED,0x97,0xB6,0x6B,0x1C,
	0x4E,0x36,0xE5,0x6D,0x7D,0xB4,0xE4,0xBF,0x20,0xB9,0xE0,0x05,0x3A,0x69,0xD5,0xB8,
	0xE3,0xD5,0xDC,0xE0,0xB9,0xAC,0x53,0x3E,0x07,0xA4,0x57,0xAD,0x77,0xFF,0x48,0x18,
	0x76,0x2A,0xAC,0x49,0x2A,0x8E,0x47,0x75,0x6D,0x9F,0x67,0x63,0x30,0x35,0x8C,0x39,
	0x05,0x39,0xD5,0x6F,0x64,0x3A,0x5B,0xAD,0xCA,0x0B,0xBB,0x82,0x52,0x99,0x45,0xB1,
	0x93,0x36,0x36,0x99,0xAF,0x13,0x20,0x44,0x36,0xD8,0x02,0x44,0x09,0x39,0x92,0x85,
	0xFF,0x4A,0x4A,0x97,0x87,0xA6,0x63,0xD7,0xC7,0xB5,0xB5,0x24,0xED,0x0F,0xB4,0x6F,
	0x0C,0x58,0x52,0x14,0xD9,0xA6,0x7B,0xD3,0x79,0xBC,0x38,0x58,0xA1,0xBD,0x3B,0x84,
	0x06,0xD8,0x1A,0x06,0xFD,0x6B,0xA8,0xEA,0x4B,0x69,0x28,0x04,0x37,0xAD,0x82,0x99,
	0xFB,0x0E,0x1B,0x85,0xBD,0xA8,0x5D,0x73,0xCD,0xDC,0x58,0x75,0x0A,0xBE,0x63,0x6C,
	0x48,0xE7,0x4C,0xE4,0x30,0x2B,0x04,0x60,0xB9,0x15,0xD8,0xDA,0x86,0x81,0x75,0x8F,
	0x96,0xD4,0x8D,0x1C,0x5D,0x70,0x85,0x7C,0x1C,0x67,0x7B,0xD5,0x08,0x67,0xA6,0xCE,
	0x4B,0x0A,0x66,0x70,0xB7,0xE5,0x63,0xD4,0x5B,0x8A,0x82,0xEA,0x10,0x67,0xCA,0xE2,
	0xF4,0xEF,0x17,0x85,0x2F,0x2A,0x5F,0x8A,0x97,0x82,0xF8,0x6A,0xD6,0x34,0x10,0xEA,
	0xEB,0xC9,0x5C,0x3C,0xE1,0x49,0xF8,0x46,0xEB,0xDE,0xBD,0xF6,0xA9,0x92,0xF1,0xAA,
	0xA6,0xA0,0x18,0xB0,0x3A,0xD3,0x0F,0x1F,0xF3,0x6F,0xFF,0x31,0x45,0x43,0x44,0xD3,
	0x50,0x9A,0xF7,0x88,0x09,0x96,0xC1,0xCE,0x76,0xCC,0xF2,0x2C,0x2C,0xBA,0xAD,0x82,
	0x77,0x8F,0x18,0x84,0xC0,0xD2,0x07,0x9C,0x36,0x90,0x83,0x4E,0x0B,0xA5,0x4F,0x43,
	0x3E,0x04,0xAB,0x78,0x4F,0xD6,0xFB,0x09,0x01,0x24,0x90,0xDA,0x6F,0x3C,0x3A,0x61,
	0x0D,0x7F,0x69,0x4A,0xEB,0x2B,0x30,0x02,0xB4,0xDB,0xE0,0x84,0xA9,0xEC,0xD7,0x35,
	0xBF,0x37,0x7D,0x85,0x58,0xCE,0xA9,0x4E,0xE4,0x80,0xC7,0xA8,0xD3,0x30,0x67,0x48,
	0xEB,0x29,0xAF,0x2F,0x74,0x6A,0xB4,0xA7,0x3F,0x0F,0x3F,0x92,0xAF,0xF3,0xCA,0xAC,
	0xAF,0x4B,0xD9,0x94,0xC0,0x43,0xCA,0x81,0x0D,0x2F,0x48,0xA1,0xB0,0x27,0xD5,0xD2,
	0xEF,0x4B,0x05,0x85,0xA3,0xDE,0x4D,0x93,0x30,0x3C,0xF0,0xBB,0x4A,0x8F,0x30,0x27,
	0x4C,0xEB,0xE3,0x3E,0x64,0xED,0x9A,0x2F,0x3B,0xF1,0x82,0xF0,0xBA,0xF4,0xCF,0x7F,
	0x40,0xCB,0xB0,0xE1,0x7F,0xBC,0xAA,0x57,0xD3,0xC9,0x74,0xF2,0xFA,0x43,0x0D,0x22,
	0xD0,0xF4,0x77,0x4E,0x93,0xD7,0x85,0x70,0x1F,0x99,0xBF,0xB6,0xDE,0x35,0xF1,0x30,
	0xA7,0x5E,0x71,0xF0,0x6B,0x01,0x2D,0x7B,0x64,0xF0,0x33,0x53,0x0A,0x39,0x88,0xF3,
	0x6B,0x3A,0xA6,0x6B,0x35,0xD2,0x2F,0x43,0xCD,0x02,0xFD,0xB5,0xE9,0xBC,0x5B,0xAA,
	0xD8,0xA4,0x19,0x7E,0x0E,0x5D,0x94,0x81,0x9E,0x6F,0x77,0xAD,0xD6,0x0E,0x74,0x93,
	0x96,0xE7,0xC4,0x18,0x5F,0xAD,0xF5,0x19,
};

UINT lookup(UINT *magic, UINT v)
{
	UINT a = (v >> 24) & 0xFF;
	UINT b = (v >> 16) & 0xFF;
	UINT c = (v >> 8) & 0xFF;
	UINT d = (v >> 0) & 0xFF;

	a = magic[a+18+0];
	b = magic[b+18+256];
	c = magic[c+18+512];
	d = magic[d+18+768];

	return d + (c ^ (b + a));
}

void EnCrypt(UINT *magic, UINT *arg1, UINT *arg2)
{
	UINT a,b,c;
	a = *arg1;
	b = *arg2;
	for (int i=0; i<16; i++)
	{
		c = magic[i] ^ a;
		a = b ^ lookup(magic, c);
		b = c;
	}
	*arg2 = a ^ magic[16];
	*arg1 = b ^ magic[17];
}
/*
void DeCrypt(UINT *magic, UINT *arg1, UINT *arg2)
{
	UINT a,b,c;
	a = *arg1;
	b = *arg2;
	for (int i=17; i>1; i--)
	{
		c = magic[i] ^ a;
		a = b ^ lookup(magic, c);
		b = c;
	}
	*arg1 = b ^ magic[0];
	*arg2 = a ^ magic[1];
}
*/
void Update_HashTable(UINT * magic, BYTE arg1[8])
{
	for (int j=0;j<18;j++)
	{
		UINT r3=0;
		for (int i=0;i<4;i++)
		{
			r3 <<= 8;
			r3 |= arg1[(j*4 + i) & 7];
		}
		magic[j] ^= r3;
	}

	UINT tmp1 = 0;
	UINT tmp2 = 0;
	for (int i=0; i<18; i+=2)
	{
		EnCrypt(magic,&tmp1,&tmp2);
		magic[i+0] = tmp1;
		magic[i+1] = tmp2;
	}
	for (int i=0; i<0x400; i+=2)
	{
		EnCrypt(magic,&tmp1,&tmp2);
		magic[i+18+0] = tmp1;
		magic[i+18+1] = tmp2;
	}
}

void Init2(UINT *magic, UINT a[3])
{
	EnCrypt(magic, a+2, a+1);
	EnCrypt(magic, a+1, a+0);
	Update_HashTable(magic, (BYTE*)a);
}

void Init1(UINT *CardHash, UINT arg2[3], UINT nGameCode)
{
	memcpy(CardHash, encr_data, 4*(1024 + 18));
	arg2[0] = nGameCode;
	arg2[1] = nGameCode>>1;
	arg2[2] = nGameCode<<1;
	Init2(CardHash, arg2);
	Init2(CardHash, arg2);
}

#define MAGIC30		0x72636E65
#define MAGIC34		0x6A624F79
void EnCrypt_Arm9(UINT nGameCode, BYTE* pArm9)
{
	UINT *p = (UINT*)pArm9;
	if(p[0] != 0xE7FFDEFF || p[1] != 0xE7FFDEFF)
		return;

	p += 2;

	UINT CardHash[0x412];
	UINT arg2[3];

	Init1(CardHash, arg2, nGameCode);

	arg2[1] <<= 1;
	arg2[2] >>= 1;
	
	Init2(CardHash, arg2);

	UINT nSize = 0x800 - 8;
	while (nSize > 0)
	{
		EnCrypt(CardHash, p+1, p);
		p += 2;
		nSize -= 8;
	}

	// place header
	p = (UINT*)pArm9;
	p[0] = MAGIC30;
	p[1] = MAGIC34;
	EnCrypt(CardHash, p+1, p);
	Init1(CardHash, arg2, nGameCode);
	EnCrypt(CardHash, p+1, p);
}
/*
void DeCrypt_Arm9(UINT nGameCode, BYTE* pArm9)
{
	UINT *p = (UINT*)pArm9;

	UINT CardHash[0x412];
	UINT arg2[3];

	Init1(CardHash, arg2, nGameCode);
	DeCrypt(CardHash, p+1, p);
	arg2[1] <<= 1;
	arg2[2] >>= 1;	
	Init2(CardHash, arg2);
	DeCrypt(CardHash, p+1, p);

	if (p[0] != MAGIC30 || p[1] != MAGIC34)
		return;

	*p++ = 0xE7FFDEFF;
	*p++ = 0xE7FFDEFF;
	UINT size = 0x800 - 8;
	while (size > 0)
	{
		DeCrypt(CardHash, p+1, p);
		p += 2;
		size -= 8;
	}
}
*/
// _CRC

void __fastcall OnFsiLoadmap(CT2TileView *pView, UINT nPos)
{
	BYTE *pRom = pView->GetDocument()->m_pRom;

	pView->m_nMoOffset = nPos;
	NDSFILEHEADER *header = (NDSFILEHEADER*)(pRom+pView->m_nMoOffset);
	pView->m_nDrawMode = CT2_DM_MAP;
	if(*(UINT*)header->falgs==0x4E534352)
	{
		pView->m_nMoOffset+=header->nSize;

		pView->m_nWidth = *(WORD*)(pRom+pView->m_nMoOffset+0x08);
		pView->m_nHeight = *(WORD*)(pRom+pView->m_nMoOffset+0x0A);
		pView->m_nMoOffset+=0x14;
	}else
	{
		pView->m_nWidth = pView->m_nHeight = 256;
	}
	pView->OnUpdateData();
}

void __fastcall OnFsiLoadtile(CT2TileView *pView, UINT nPos, BYTE nType=0)
{
	BYTE *pRom = pView->GetDocument()->m_pRom;

	pView->m_nOffset = nPos;
	NDSFILEHEADER *header = (NDSFILEHEADER*)(pRom+pView->m_nOffset);
	if(*(UINT*)header->falgs==0x4E434752)
	{
		pView->m_nOffset += header->nSize;

		pView->m_nTileFormat = *(WORD*)(pRom+pView->m_nOffset+0x0C)==4?TF_GBA8BPP:TF_GBA4BPP;

		pView->m_nOffset += 0x20;
	}else
	{
		pView->m_nDrawMode=CT2_DM_TILE;
		if(pView->m_nTileFormat!=TF_GBA8BPP)
			pView->m_nTileFormat=TF_GBA4BPP;
		if(nType==1)
		{
			pView->m_nWidth=pView->m_nHeight=256;
			pView->m_nTileFormat=TF_GBA8BPP;
			pView->m_nColCount = pView->m_nLineCount = 1;
			//pView->m_nAutoColFit = 1;
		}else
		if(nType==2)
		{
			//pView->m_nTileFormat=TF_GBA8BPP;
			pView->m_nWidth=pView->m_nHeight=8;
		}else
		{
			//pView->m_nTileFormat=TF_GBA4BPP;
			pView->m_nColCount = 4;
			pView->m_nLineCount = 16;
			pView->m_nAutoColFit = 0;
			pView->m_nWidth=pView->m_nHeight=8;
		}
	}

	pView->OnUpdateData();
}

void __fastcall OnFsiLoadpal(CT2TileView *pView, UINT nPos, WORD nCount=256)
{
	BYTE *pRom = pView->GetDocument()->m_pRom;

	NDSFILEHEADER *header = (NDSFILEHEADER*)(pRom+nPos);
	if(*(UINT*)header->falgs==0x4E434C52)
	{
		nPos+=header->nSize;
		header = (NDSFILEHEADER*)(pRom + nPos);

		nPos += 0x18;
	}
	WORD *pWPal = (WORD*)(pRom+nPos);

	CNdsFileSystemInfo::LoadPal(pWPal, 	pView->GetDocument()->m_pPal, nCount);

	theApp.GetCurView()->OnUpdateData();
}

#define FSI_SUB_FILENAME	0
#define FSI_SUB_FILEID		1
#define FSI_SUB_FILEOFFSET	2
#define FSI_SUB_FILESIZE	3
#define FSI_SUB_FILEHEADER	4
#define FSI_SUB_FILEINFO	5

IMPLEMENT_DYNAMIC(CNdsFileSystemInfo, CDialog)
CNdsFileSystemInfo::CNdsFileSystemInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CNdsFileSystemInfo::IDD, pParent)
	, m_nRomFitSize(0)
{
}

CNdsFileSystemInfo::~CNdsFileSystemInfo()
{
}

void CNdsFileSystemInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FSILIST, m_NdsFSI);
}


BEGIN_MESSAGE_MAP(CNdsFileSystemInfo, CDialog)
	ON_WM_SIZE()
	ON_NOTIFY(LVN_GETDISPINFO, IDC_FSILIST, OnLvnGetdispinfoFsilist)
	ON_NOTIFY(LVN_ITEMACTIVATE, IDC_FSILIST, OnLvnItemActivateFsilist)
	ON_WM_CONTEXTMENU()
	ON_WM_INITMENUPOPUP()
	ON_COMMAND(ID_FSI_GO, OnFsiGo)
	ON_UPDATE_COMMAND_UI(ID_FSI_GO, OnUpdateFsiGo)
	ON_COMMAND(ID_FSI_REFALSH, OnFsiRefalsh)
	ON_COMMAND(ID_FSI_EXPORT, OnFsiExport)
	ON_UPDATE_COMMAND_UI(ID_FSI_EXPORT, OnUpdateFsiGo)
	ON_COMMAND(ID_FSI_IMPORT, OnFsiImport)
	ON_UPDATE_COMMAND_UI(ID_FSI_IMPORT, OnUpdateFsiImport)
	ON_COMMAND(ID_FSI_SPLITROM, OnFsiSplitrom)
	ON_COMMAND(ID_FSI_CREATEROM, OnFsiCreaterom)
	ON_UPDATE_COMMAND_UI(ID_FSI_CREATEROM, OnUpdateFsiCreaterom)
	ON_COMMAND_RANGE(ID_FSI_CHECKCRC16, ID_FSI_CHECKCRC16, OnFsiCheckcrc16)
	ON_COMMAND(ID_FSI_GAMEINFO, OnFsiGameinfo)
	ON_UPDATE_COMMAND_UI(ID_FSI_GAMEINFO, OnUpdateFsiGameinfo)
	ON_COMMAND(ID_FSI_GOFILE, OnFsiGoFile)
	ON_COMMAND(ID_FSI_SETMAP, OnFsiSetmap)
	ON_UPDATE_COMMAND_UI(ID_FSI_SETMAP, OnUpdateFsiSetmap)
	ON_COMMAND(ID_TOOLS_SFI, OnSFI)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_SFI, OnUpdateSFI)
	ON_COMMAND(ID_FSI_FIND, OnFsiFind)
	ON_COMMAND(ID_FSI_COPYTOCLIPBOARD, OnFsiCopytoclipboard)
	ON_UPDATE_COMMAND_UI(ID_FSI_COPYTOCLIPBOARD, OnUpdateFsiCopytoclipboard)
	ON_COMMAND(ID_FSI_NEXT, OnFsiNext)
	ON_NOTIFY(LVN_KEYDOWN, IDC_FSILIST, OnLvnKeydownFsilist)
	ON_COMMAND(ID_FSI_GOHAN, OnFsiGohan)
	ON_COMMAND_RANGE(ID_FSI_SETFILESIZE, (ID_FSI_SETFILESIZE+999), OnFsiSetfilesize)
	ON_UPDATE_COMMAND_UI_RANGE(ID_FSI_SETFILESIZE, (ID_FSI_SETFILESIZE+15), OnUpdateFsiSetfilesize)
	ON_COMMAND_RANGE(ID_FSI_GOTOHEADER, ID_FSI_GOTOHEADER+999, OnFsiGotoheader)
	ON_UPDATE_COMMAND_UI_RANGE(ID_FSI_GOTOHEADER, ID_FSI_GOTOHEADER+999, OnUpdateFsiGotoheader)
	ON_COMMAND(ID_FSI_EXPORTEX, OnFsiExportex)
	ON_UPDATE_COMMAND_UI(ID_FSIPOP_LOADNTFRWH, OnUpdateFsipopNtfr)
	ON_UPDATE_COMMAND_UI(ID_FSIPOP_SETNTFRWH, OnUpdateFsipopNtfrSetNftrWH)
	ON_COMMAND(ID_FSIPOP_LOADNTFRWH, OnFsipopLoadntfrwh)
	ON_COMMAND(ID_FSIPOP_SETNTFRWH, OnFsipopSetntfrwh)
	ON_COMMAND(ID_FSIPOP_EXPORTTBL, OnFsipopExporttbl)
	ON_UPDATE_COMMAND_UI(ID_FSIPOP_EXPORTTBL, OnUpdateFsipopNtfr)
	ON_NOTIFY(LVN_ENDSCROLL, IDC_FSILIST, OnLvnEndScrollFsilist)
	ON_COMMAND(ID_FSIPOP_SHOWICON, OnFsipopShowicon)
	ON_COMMAND(ID_FSIPOP_UPDATEICONPAL, OnFsipopUpdateiconpal)
	ON_COMMAND(ID_FSI_FIX, OnFsiFix)
	ON_COMMAND(ID_FSI_LOADDATA, OnFsiLoaddata)
	ON_UPDATE_COMMAND_UI(ID_FSI_LOADDATA, OnUpdateFsiLoaddata)
END_MESSAGE_MAP()


// CNdsFileSystemInfo 消息处理程序

BOOL CNdsFileSystemInfo::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_NdsFSI.ModifyStyle(LVS_SORTASCENDING|LVS_SORTDESCENDING|
		LVS_ICON|LVS_SMALLICON|LVS_LIST|LVS_EDITLABELS|LVS_NOSORTHEADER,
		LVS_REPORT|LVS_SHOWSELALWAYS|LVS_SHAREIMAGELISTS/*|LVS_OWNERDRAWFIXED*/);
	m_NdsFSI.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_INFOTIP);

	CString header, text;
	header.LoadString(IDS_NDSFSIHEADER);
	BYTE nFormat[]={LVCFMT_LEFT, LVCFMT_RIGHT, LVCFMT_LEFT, LVCFMT_RIGHT, LVCFMT_LEFT, LVCFMT_LEFT};
	BYTE nWidth[]={144, 48, 64, 92, 64, 64};
	for(int i=0; i<(sizeof(nWidth)/sizeof(BYTE)); i++)
	{
		AfxExtractSubString(text, header, i);
		m_NdsFSI.InsertColumn(i, text, nFormat[i], nWidth[i]);
	}

	GetWindowText(header);
	header.Append(m_pTileView->GetDocument()->GetTitle());
	SetWindowText(header);

	CreateImageList();

	m_NdsFSI.SendMessage(LVM_SETIMAGELIST, LVSIL_SMALL, (LPARAM)g_hCTImageList);

	SetIcon(ImageList_GetIcon(g_hCTImageList, FSI_ICON_NDS, 0), FALSE);

	OnLoadFSI();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CNdsFileSystemInfo::OnSize(UINT nType, int cx, int cy)
{
	if(m_NdsFSI.GetSafeHwnd())
	{
		CRect rc; m_NdsFSI.GetWindowRect(&rc);
		ScreenToClient(rc);
		m_NdsFSI.SetWindowPos(NULL,
			0, 0,
			cx-rc.left,
			cy-rc.top,
			SWP_NOMOVE);
	}

	CDialog::OnSize(nType, cx, cy);
}

void CNdsFileSystemInfo::OnUpdateNds()
{
	m_pTileView = ((CT2ChildFrm*)m_pParentWnd)->m_pTileView;
	m_nRomSize = m_pTileView->GetDocument()->m_nRomSize;
	m_pRom = m_pTileView->GetDocument()->m_pRom;
	m_bReadOnly = m_pTileView->GetDocument()->m_nReadOnly;
	m_pHeader = (NDSHEADER*)m_pRom;

	CString OverlayFiles = _T("FSI.CT/");

	UINT nArm9_Size = m_pHeader->Arm9_Size;
	if(*(UINT*)(m_pRom+m_pHeader->Arm9_Rom_Offset+nArm9_Size)==0xDEC00621)
		nArm9_Size+=4*4;	// arm9.bin with_footer

	UINT Rom_Header_Size=m_pHeader->Rom_Header_Size;
	if(Rom_Header_Size<sizeof(NDSHEADER))
		Rom_Header_Size = sizeof(NDSHEADER);
	NDSSPECREC SpecRec[HEADERCOUNT] = {
		//Top	Bottom			Name
		{0, Rom_Header_Size, OverlayFiles+_T("ndsheader.bin")},
		{m_pHeader->Arm9_Rom_Offset, m_pHeader->Arm9_Rom_Offset+nArm9_Size, OverlayFiles+_T("arm9.bin")},
		{m_pHeader->Arm7_Rom_Offset, m_pHeader->Arm7_Rom_Offset+m_pHeader->Arm7_Size, OverlayFiles+_T("arm7.bin")},
		{m_pHeader->Arm9_Overlay_Offset, m_pHeader->Arm9_Overlay_Offset+m_pHeader->Arm9_Overlay_Size, OverlayFiles+_T("arm9ovltable.bin")},
		{m_pHeader->Arm7_Overlay_Offset, m_pHeader->Arm7_Overlay_Offset+m_pHeader->Arm7_Overlay_Size, OverlayFiles+_T("arm7ovltable.bin")},
		{m_pHeader->Fnt_Offset, m_pHeader->Fnt_Offset+m_pHeader->Fnt_Size, OverlayFiles+_T("fnt.bin")},
		{m_pHeader->Fat_Offset, m_pHeader->Fat_Offset+m_pHeader->Fat_Size, OverlayFiles+_T("fat.bin")},
		{m_pHeader->Banner_Offset, m_pHeader->Banner_Offset+0x840, OverlayFiles+_T("banner.bin")}
	};
	for(int i=0; i<HEADERCOUNT; i++)
	{
		m_SpecRec[i].nTop=SpecRec[i].nTop;
		m_SpecRec[i].nBottom=SpecRec[i].nBottom;
		m_SpecRec[i].FileName=SpecRec[i].FileName;
	}
}

void CNdsFileSystemInfo::OnLoadFSI(void)
{
	m_NdsFSI.SetRedraw(FALSE);

	OnUpdateNds();

	m_nOverlayFiles9 = m_pHeader->Arm9_Overlay_Size/sizeof(OVERLAYENTRY);
	m_nOverlayFiles7 = m_pHeader->Arm7_Overlay_Size/sizeof(OVERLAYENTRY);

	int nSel = GetCurSel();

	m_NdsFSI.DeleteAllItems();
	m_nRomFitSize = 0;	int nItem = 0;

	for(; nItem<HEADERCOUNT; nItem++)
		m_NdsFSI.InsertItem(nItem, m_SpecRec[nItem].FileName, FSI_ICON_NDS);

	CString OverlayFiles;
	m_nOverlayFileSize = 0; 
	OVERLAYENTRY *OverlayEntry = (OVERLAYENTRY*)(m_pRom+m_pHeader->Arm9_Overlay_Offset);
	NDSFILEREC *pFileRec;
	for(UINT i=0; i<m_nOverlayFiles9; i++)
	{
		OverlayFiles.Format(OVERLAY_FMT, 9, i);

		pFileRec = (NDSFILEREC*)(m_pRom+m_pHeader->Fat_Offset+(OverlayEntry->file_id<<3));
		m_nOverlayFileSize += pFileRec->bottom-pFileRec->top;

		m_NdsFSI.InsertItem(LVIF_TEXT|LVIF_PARAM|LVIF_IMAGE, nItem,
			OverlayFiles, 0, 0, FSI_ICON_OT, SFIMAKEPARAM(OverlayEntry->file_id, nItem));
		nItem++;
		OverlayEntry++;
	}
	OverlayEntry = (OVERLAYENTRY*)(m_pRom+m_pHeader->Arm7_Overlay_Offset);
	for(UINT i=0; i<m_nOverlayFiles7; i++)
	{
		OverlayFiles.Format(OVERLAY_FMT, 7, i);

		pFileRec = (NDSFILEREC*)(m_pRom+m_pHeader->Fat_Offset+(OverlayEntry->file_id<<3));
		m_nOverlayFileSize += pFileRec->bottom-pFileRec->top;

		m_NdsFSI.InsertItem(LVIF_TEXT|LVIF_PARAM|LVIF_IMAGE, nItem,
			OverlayFiles, 0, 0, FSI_ICON_OT, SFIMAKEPARAM(OverlayEntry->file_id, nItem));
		nItem++;
		OverlayEntry++;
	}

	//OverlayFiles  = _T("/");
	OverlayFiles.Empty();

	// ExtractDirectory(OverlayFiles);
	m_nRomFitSize = SFIExtractDirectory(OverlayFiles, 0, m_pRom,
		m_pHeader->Fnt_Offset,
		m_pHeader->Fat_Offset,
		m_NdsFSI);

	m_nRomFitSize+=
		sizeof(NDSHEADER)				//ndsheader
		+m_pHeader->Arm9_Size			//arm9
		+m_pHeader->Arm7_Size			//arm7
		+m_pHeader->Arm9_Overlay_Size	//arm9ovltable
		+m_pHeader->Arm7_Overlay_Size	//arm7ovltable
		+0x840							//banner
		+m_nOverlayFileSize;			//OverlayFileSize

	LVCOLUMN col; memset(&col, 0, sizeof(col));
	col.mask = LVCF_TEXT; col.iSubItem = 2; m_NdsFSI.GetColumn(3, &col);
	OverlayFiles.Format(_T("大小(%d)"), m_nRomFitSize);
	col.pszText = (LPTSTR)(LPCTSTR)OverlayFiles; col.mask = LVCF_TEXT; m_NdsFSI.SetColumn(3, &col);

	m_NdsFSI.SetRedraw();
	if(nSel!=-1)
	{
		m_NdsFSI.SetItemState(nSel, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		m_NdsFSI.SendMessage(WM_KEYDOWN, VK_LEFT);
	}
}
/*
void CNdsFileSystemInfo::ExtractDirectory(CString& ParentDir, UINT nID)
{
	CString strFilePathName, ext, name; NDSDIRREC *pDirRec;
	BYTE nRecLen; BOOL bIsDir; BYTE* pRec;
	CString Dir = ParentDir;

	UINT nPos = m_pHeader->Fnt_Offset+(nID<<3);	if(m_nRomSize<nPos) return;
	pDirRec = (NDSDIRREC*)(m_pRom+nPos);
	nPos = m_pHeader->Fnt_Offset + pDirRec->entry_start;	if(m_nRomSize<nPos) return;
	pRec  = m_pRom+nPos;
	UINT FileID = pDirRec->top_file_id;
	while(1)
	{
		nRecLen = *(pRec++);	if(nRecLen==0) break;
		bIsDir = nRecLen&0x80; nRecLen&=0x7F;

		memcpy(m_FileName, pRec, nRecLen);	pRec+=nRecLen;
		m_FileName[nRecLen]=0;
		OemToChar(m_FileName, g_szBuffer);
		if(bIsDir)
		{
			WORD DirID = *(WORD*)(pRec);DirID&=0xFFF;
			pRec+=2;
			Dir.Format(_T("%s%s/"), ParentDir,g_szBuffer);
			ExtractDirectory(Dir, DirID); continue;
		}
		NDSFILEREC *pFileRec;
		pFileRec = (NDSFILEREC*)(m_pRom+m_pHeader->Fat_Offset+(FileID<<3));
		nPos = pFileRec->bottom-pFileRec->top;
		m_nRomFitSize+=nPos;

		strFilePathName.Format(_T("%s%s"), ParentDir,g_szBuffer);

		nPos = m_NdsFSI.GetItemCount();
		int nImage = GetSFIIconIndex(strFilePathName, m_pRom+pFileRec->top);// FSI_ICON_UNK;

		m_NdsFSI.InsertItem(LVIF_TEXT|LVIF_PARAM|LVIF_IMAGE,
			nPos, strFilePathName, 0, 0,
			nImage, SFIMAKEPARAM(FileID, nPos));
		FileID++;
	}
}
*/
void CNdsFileSystemInfo::OnLvnGetdispinfoFsilist(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

	UINT FileID = (UINT)pDispInfo->item.iItem; CString text;
	if(FileID>=HEADERCOUNT)
	{
		FileID = GETFILEID(pDispInfo->item.lParam);
		NDSFILEREC *pFileRec;
		//memcpy((BYTE*)&filerec, m_pRom+m_pHeader->fat_offset+(FileID<<3), sizeof(filerec));
		pFileRec = (NDSFILEREC*)(m_pRom+m_pHeader->Fat_Offset+(FileID<<3));
		if(pDispInfo->item.mask & LVIF_TEXT)
		{
			switch(pDispInfo->item.iSubItem)
			{
			case FSI_SUB_FILEID://ID
				text.Format(_T("%04d"), FileID);
				break;
			case FSI_SUB_FILEOFFSET://Offset
				text.Format(_T("%08X"), pFileRec->top);
				break;
			case FSI_SUB_FILESIZE://Size
				{
					text.Format(_T("%d"), pFileRec->bottom-pFileRec->top);
					int nPos=text.GetLength();
					while(nPos>3)	{nPos-=3; text.Insert(nPos, _T(','));}
				}break;
			case FSI_SUB_FILEHEADER://HeaderInfo
				{
					LVITEM lvi; memset(&lvi, 0, sizeof(lvi));
					lvi.iItem = pDispInfo->item.iItem; lvi.mask = LVIF_IMAGE;
					m_NdsFSI.GetItem(&lvi);
					if(lvi.iImage==FSI_ICON_TEXT)
					{
						UINT nSize = pFileRec->bottom-pFileRec->top;
						if(nSize>0x10) nSize=0x10;
						//text.Append(m_pRom+pFileRec->top, nSize);
						LPTSTR p = text.GetBuffer(0x20);
						memset(p, 0, sizeof(TCHAR)*0x20);
						MultiByteToWideChar(932, 0, (LPCSTR)(m_pRom+pFileRec->top), nSize,
							p, 0x20);
						text.ReleaseBuffer(0x20);
					}else
					{
						text = GetSubHeader(pFileRec->top, lvi.iItem);
						text.Replace(_T('\n'), _T(' '));
					}
				}
				break;
			case FSI_SUB_FILEINFO:
				{
					LVITEM lvi; memset(&lvi, 0, sizeof(lvi));
					lvi.iItem = pDispInfo->item.iItem; lvi.mask = LVIF_IMAGE;
					m_NdsFSI.GetItem(&lvi);
					CString h, sub;
					h.LoadString(IDS_NDSHEADERINFO);
					AfxExtractSubString(text, h, HEADERCOUNT+lvi.iImage);
					sub = m_NdsFSI.GetItemText(lvi.iItem, FSI_SUB_FILEOFFSET);
					UINT nOff = StrToIntEX(sub);
					if(nOff==-1) break;
					sub = GetSubHeader(nOff, lvi.iItem);
					UINT i=0, nSize;
					while(TRUE)
					{
						AfxExtractSubString(h, sub, i);
						nSize = StrToIntEX(h.Right(h.GetLength()-5), FALSE);
						h = h.Left(4);
						if(h==_T("CWDH"))
						{
							NDSCWDH *cwdh = (NDSCWDH*)(m_pRom+nOff);
							h.LoadString(IDS_NDSCWDH_INFO);
							text.AppendFormat(h, cwdh->wCount+1, cwdh->nWDH[0][0], cwdh->nWDH[0][1], cwdh->nWDH[0][2]);
						}else
						if(h==_T("CGLP"))
						{
							NDSCGLP *cglp = (NDSCGLP*)(m_pRom+nOff);
							h.LoadString(IDS_NDSCGLP_INFO);
							text.AppendFormat(h, cglp->nWidth, cglp->nHeight, cglp->nTileSize, cglp->nBits, cglp->nTileSize*8/(cglp->nWidth*cglp->nHeight));
						}else
						if(h.IsEmpty()) break;
						nOff += nSize;
						i++;
					}
				}break;
			}
			if(text.GetLength()>pDispInfo->item.cchTextMax)
			{
				text = text.Left(pDispInfo->item.cchTextMax-4);
				text.Append(_T("..."));
			}
			::lstrcpy(pDispInfo->item.pszText, text);
		}
	}else
	{
		if(pDispInfo->item.mask & LVIF_TEXT)
		{
			switch(pDispInfo->item.iSubItem)
			{
			case FSI_SUB_FILEID://ID
				text.LoadString(IDS_NDSHEADER);
				break;
			case FSI_SUB_FILEOFFSET://Offset
				text.Format(_T("%08X"), m_SpecRec[FileID].nTop);
				break;
			case FSI_SUB_FILESIZE://Size
				{
				text.Format(_T("%d"), m_SpecRec[FileID].nBottom-m_SpecRec[FileID].nTop);
				int nPos=text.GetLength();while(nPos>3){nPos-=3; text.Insert(nPos, _T(','));}
				}break;
			case FSI_SUB_FILEINFO:
				{
				CString h;h.LoadString(IDS_NDSHEADERINFO);
				AfxExtractSubString(text, h, FileID);
				}break;
			}
			::lstrcpy(pDispInfo->item.pszText, text);
		}
	}

	*pResult = 0;
}

void CNdsFileSystemInfo::OnLvnItemActivateFsilist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	OnUpdateNds();

	OnFsiGo();
	*pResult = 0;
}

void CNdsFileSystemInfo::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	CMenu Menu;
	Menu.LoadMenu(IDR_CT2POPMENU);
	CMenu *pMenu = Menu.GetSubMenu(3);

	long x = point.x,
		y = point.y;

	if(x<0 || y<0)
	{
		int nSel = GetCurSel();
		if(nSel!=-1)
		{
			CRect rc;
			m_NdsFSI.GetItemRect(nSel, &rc, 0);
			m_NdsFSI.ClientToScreen(&rc);
			x = rc.left;
			y = rc.bottom;
		}
	}
	pMenu->TrackPopupMenu(TPM_LEFTBUTTON, x, y, this);
}

void CNdsFileSystemInfo::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	CDialog::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);

	OnUpdateNds();
	::OnInitMenuPopup(this, pPopupMenu->GetSafeHmenu());
}

void CNdsFileSystemInfo::OnFsiGo()
{
	//POSITION pos = m_NdsFSI.GetFirstSelectedItemPosition();
	int nSel = GetCurSel();//m_NdsFSI.GetNextSelectedItem(pos);

	CT2View *pView = (CT2View*)((CT2ChildFrm*)m_pParentWnd)->GetCurView();
	if((nSel==-1) || !pView) return;

	if(((CMDIFrameWnd*)theApp.m_pMainWnd)->MDIGetActive()!=m_pTileView->GetParent())
		((CMDIFrameWnd*)theApp.m_pMainWnd)->MDIActivate(m_pTileView->GetParent());

	pView->SetFocus();

	CString Addr = m_NdsFSI.GetItemText(nSel, 2);
	//pView->m_nOffset = StrToIntEX(Addr);
	UINT nOffset = StrToIntEX(Addr);
	pView->ScrollTo(nOffset, TRUE);
	if(GetStyle()&WS_MAXIMIZE)
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE);
}

void CNdsFileSystemInfo::OnUpdateFsiGo(CCmdUI *pCmdUI)
{
	POSITION pos = m_NdsFSI.GetFirstSelectedItemPosition();
	pCmdUI->Enable(pos!=NULL);
}

void CNdsFileSystemInfo::OnFsiRefalsh()
{
	OnLoadFSI();
}

void CNdsFileSystemInfo::OnFsiExport()
{
	ExportFile(_T(""));
}

void CNdsFileSystemInfo::OnFsiImport()
{
	int nSel = GetCurSel();
	if(nSel==-1) return;

	UINT nOff, nSize; WORD wFileID;
	CString tmp = m_NdsFSI.GetItemText(nSel, 1);
	wFileID = StrToIntEX(tmp, FALSE);

	tmp = m_NdsFSI.GetItemText(nSel, 2);
	nOff = StrToIntEX(tmp);

	tmp = m_NdsFSI.GetItemText(nSel, 3);	tmp.Replace(_T(","), _T(""));
	nSize = StrToIntEX(tmp, FALSE);

	static CFileDialog fd(TRUE);
	TCHAR path[_MAX_PATH];
	fd.m_pOFN->lpstrFile = path;
	fd.m_pOFN->nMaxFile = _MAX_PATH;
	m_NdsFSI.GetItemText(nSel, 0, path, _MAX_PATH);
	lstrcpy(path, PathFindFileName(path));

	if(fd.DoModal()!=IDOK) return;

	OnImport(path, nOff, nSize, nSel<HEADERCOUNT?nSel|0x8000 : wFileID);

	m_pParentWnd->Invalidate(FALSE);
	OnLoadFSI();
}

void CNdsFileSystemInfo::OnImport(CString path, UINT nOff, UINT nSize, WORD wFileID)
{
	CFile file;
	if(!file.Open(path, CFile::modeRead|CFile::typeBinary)) {ShowError(); return;}

	UINT nNewOffset = ImportFile(&file, nOff, nSize, wFileID);

	//AddMacro_Import(path, nOff, nSize, wFileID, nNewOffset);
}

void CNdsFileSystemInfo::OnUpdateFsiImport(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( (m_bReadOnly==FALSE) &&
		(m_NdsFSI.GetFirstSelectedItemPosition()!=NULL) );
}

void CNdsFileSystemInfo::OnFsiSplitrom()
{
	CString ProjectPath = GetPath(m_hWnd);
	if(ProjectPath.IsEmpty()) return;
	ProjectPath.AppendChar(_T('/'));
	int nCount = 0;
	while(m_pHeader->GameTitle[nCount])
	{
		ProjectPath.AppendChar(m_pHeader->GameTitle[nCount++]);
		if(nCount>=0x0C) break;
	}

	if(!CreateDirectory(ProjectPath, (LPSECURITY_ATTRIBUTES)NULL) && (GetLastError()!=0xB7))
		{ShowError(); return;}

	nCount = m_NdsFSI.GetItemCount();

	CString path, text, FileID; CFile file; UINT nOff, nSize;
	text = ProjectPath+_T("/FSI.CT");

	if(!CreateDirectory(text, (LPSECURITY_ATTRIBUTES)NULL) && (GetLastError()!=0xB7))
		{ShowError(); return;}

	for(int nItem=0; nItem<nCount; nItem++)
	{
		if(nItem<HEADERCOUNT)//FSI.CT
		{
			text = m_NdsFSI.GetItemText(nItem, 0);//filename
			path.Format(_T("%s/%s"), ProjectPath, text);
			if(!file.Open(path, CFile::modeCreate|CFile::modeWrite|CFile::typeBinary)) {ShowError(); return;}

			text = m_NdsFSI.GetItemText(nItem, 2);//offset
			nOff = StrToIntEX(text); if(nOff==-1) return;
			text = m_NdsFSI.GetItemText(nItem, 3);//size
			text.Replace(_T(","), _T(""));
			nSize = StrToIntEX(text, FALSE); if(nSize==-1) return;

			file.Write(m_pRom+nOff, nSize);
			file.Close();
		}else//FileID
		{
			text = m_NdsFSI.GetItemText(nItem, 0);//filename
			//mkdir
			nOff = 1;
			while( (nOff=text.Find(_T('/'), nOff))!=-1 )
			{
				nSize = text.Find(_T('/'), nOff); if(nSize==-1) nSize=text.GetLength();
				FileID.Format(_T("%s/%s"), ProjectPath, text.Left(nSize));
				if(!CreateDirectory(FileID, (LPSECURITY_ATTRIBUTES)NULL) && (GetLastError()!=0xB7))
					{ShowError(); return;}
				nOff = nSize+1;
			}

			path.Format(_T("%s/%s"), ProjectPath, text);
			if(!file.Open(path, CFile::modeCreate|CFile::modeWrite|CFile::typeBinary)) {ShowError(); return;}

			text = m_NdsFSI.GetItemText(nItem, 2);//offset
			nOff = StrToIntEX(text); if(nOff==-1) return;
			text = m_NdsFSI.GetItemText(nItem, 3);//size
			text.Replace(_T(","), _T(""));
			nSize = StrToIntEX(text, FALSE); if(nSize==-1) return;

			file.Write(m_pRom+nOff, nSize);
			file.Close();
		}
	}
}

void CNdsFileSystemInfo::CopyFromBin(CString& Path, UINT& nSize)
{
	CFile file;
	if(!file.Open(Path, CFile::modeRead|CFile::typeBinary)) {ShowError(); return;}
	BYTE buffer[1024]; int nReadSize;
	nSize = 0;
	while(1)
	{
		nReadSize = file.Read(buffer, sizeof(buffer));
		if(nReadSize<=0) break;
		m_fNds.Write(buffer, nReadSize);
		nSize+=nReadSize;
	}
}

void CNdsFileSystemInfo::OnFsiCreaterom()
{
	CString ProjectPath = GetPath(m_hWnd);
	if(ProjectPath.IsEmpty()) return;

	OnUpdateNds();
	CT2TileView* pView = (CT2TileView*)m_pTileView;
	CT2Doc* pDoc = pView->GetDocument();
	CString nds = pDoc->GetPathName();
	if(!m_fNds.Open(nds, CFile::modeReadWrite|CFile::typeBinary))
		{ShowError(); return;}

	CFile file; CString path, text;
	//ndsheader.bin
	path.Format(_T("%s/FSI.CT/ndsheader.bin"), ProjectPath);
	if(!file.Open(path, CFile::modeRead|CFile::typeBinary))
		{ShowError(); return;}
	UINT nSize = (UINT)file.GetLength();
	if(nSize<sizeof(NDSHEADER))
		return;
	BYTE *pHeader = new BYTE[nSize];
	file.Read(pHeader, nSize);
	file.Close();

	NDSHEADER NdsHeader;
	memcpy(&NdsHeader, pHeader, sizeof(NDSHEADER));

	// header size
	NdsHeader.Rom_Header_Size = nSize;
	m_fNds.Write(pHeader, nSize);
	delete[] pHeader;

	BYTE BIN_NULL[0x4000]={0};
	// arm9.bin
	NdsHeader.Arm9_Rom_Offset = (UINT)m_fNds.GetPosition();
	nSize = (nSize<0x4000) ? 0x4000 : 0x1000;
	NdsHeader.Arm9_Rom_Offset += AlignFile(nSize, BIN_NULL);

	path.Format(_T("%s/FSI.CT/arm9.bin"), ProjectPath);
	CopyFromBin(path, nSize);
	NdsHeader.Arm9_Size = ((nSize + 3) &~ 3);

	// ARM9 overlay table
	NdsHeader.Arm9_Overlay_Offset = (UINT)m_fNds.GetPosition();	// do not align
	path.Format(_T("%s/FSI.CT/arm9ovltable.bin"), ProjectPath);
	CopyFromBin(path, nSize);
	NdsHeader.Arm9_Overlay_Size = nSize;
	m_nOverlayFiles9 = nSize/sizeof(OVERLAYENTRY);
	if(!nSize) NdsHeader.Arm9_Overlay_Offset = 0;

	// ARM7 binary
	NdsHeader.Arm7_Rom_Offset = (UINT)m_fNds.GetPosition();	// align to 512 bytes
	NdsHeader.Arm7_Rom_Offset += AlignFile(0x200, BIN_NULL);
	path.Format(_T("%s/FSI.CT/arm7.bin"), ProjectPath);
	CopyFromBin(path, nSize);
	NdsHeader.Arm7_Size = ((nSize + 3) &~ 3);

	// ARM7 overlay table
	NdsHeader.Arm7_Overlay_Offset = (UINT)m_fNds.GetPosition();	// do not align

	path.Format(_T("%s/FSI.CT/arm7ovltable.bin"), ProjectPath);
	CopyFromBin(path, nSize);
	NdsHeader.Arm7_Overlay_Size = nSize;
	m_nOverlayFiles7 = nSize/sizeof(OVERLAYENTRY);
	if(!nSize) NdsHeader.Arm7_Overlay_Offset = 0;

	// fnt_offset
	NdsHeader.Fnt_Offset = (UINT)m_fNds.GetPosition();	// align to 512 bytes
	NdsHeader.Fnt_Offset += AlignFile(0x200, BIN_NULL);
	path.Format(_T("%s/FSI.CT/fnt.bin"), ProjectPath);
	CopyFromBin(path, nSize);
	NdsHeader.Fnt_Size = nSize;

	// fat_offset
	NdsHeader.Fat_Offset = (UINT)m_fNds.GetPosition();	// align to 512 bytes
	NdsHeader.Fat_Offset += AlignFile(0x200, BIN_NULL);
	path.Format(_T("%s/FSI.CT/fat.bin"), ProjectPath);
	CopyFromBin(path, nSize);
	NdsHeader.Fat_Size = nSize;

	if(!file.Open(path, CFile::modeRead|CFile::typeBinary)) {ShowError(); return;}
	BYTE* pFat = NULL; pFat = new BYTE[nSize]; if(pFat==NULL) {ShowError(); return;}
	file.Read(pFat, nSize); file.Close();

	// banner.bin
	NdsHeader.Banner_Offset = (UINT)m_fNds.GetPosition();	// align to 512 bytes
	NdsHeader.Banner_Offset += AlignFile(0x200, BIN_NULL);
	path.Format(_T("%s/FSI.CT/banner.bin"), ProjectPath);
	CopyFromBin(path, nSize);

	// filesystem
	AlignFile(0x200, BIN_NULL);
	// addfiles align to 4 bytes
	int nItemCount = m_NdsFSI.GetItemCount();
	CString FileID;//, Offset, Size;
	UINT nFileID;//, nFSize;
	memset(BIN_NULL, 0xFF, 4);
	for(int i=HEADERCOUNT; i<nItemCount; i++)
	{
		text = m_NdsFSI.GetItemText(i, 0);	//filename
		FileID = m_NdsFSI.GetItemText(i, 1);//fileid

		nFileID = StrToIntEX(FileID, FALSE);
		if(nFileID==-1) continue;

		NDSFILEREC* pFR = (NDSFILEREC*)(pFat + (nFileID<<3));
		pFR->top = (UINT)m_fNds.GetPosition();
		path.Format(_T("%s/%s"), ProjectPath, text);
		CopyFromBin(path, nSize);
		pFR->bottom = pFR->top+nSize;

		AlignFile(4, BIN_NULL);	// align to 4 bytes
	}
	AlignFile(4, BIN_NULL);
	NdsHeader.Application_End_Offset = (UINT)m_fNds.GetPosition();
	//NdsHeader.Application_End_Offset = (NdsHeader.Application_End_Offset + 3) & ~3;

	m_fNds.Seek(NdsHeader.Fat_Offset, SEEK_SET);
	m_fNds.Write(pFat, NdsHeader.Fat_Size);
	delete[] pFat;

	// nMbits
	UINT nOffset = ((NdsHeader.Application_End_Offset+0x7FFFFF)&~0x7FFFFF) / 0x20000;
	NdsHeader.DeviceCap = 0xFF; while(nOffset) {NdsHeader.DeviceCap++; nOffset>>=1;}

	//* Crc16
	BYTE secure_area[0x4000];
	m_fNds.Seek(NdsHeader.Arm9_Rom_Offset, SEEK_SET); m_fNds.Read(secure_area, 0x4000);
	EnCrypt_Arm9(*(UINT*)NdsHeader.GameCode, secure_area);

	NdsHeader.Secure_Area_CRC = GetCrc16(secure_area, 0x4000);

	NdsHeader.Nintendo_Logo_CRC = GetCrc16(NdsHeader.Nintendo_Logo, 156);

	NdsHeader.Header_CRC = GetCrc16((BYTE*)&NdsHeader, 0x15E);
	// End
	m_fNds.Seek(0, SEEK_SET);
	m_fNds.Write(&NdsHeader, sizeof(NdsHeader));

	//m_fNds.Close();
	//if(!m_fNds.Open(nds, CFile::modeRead|CFile::typeBinary)) {ShowError(); return;}

	/*HANDLE hMapFile = ::CreateFileMapping(m_fNds.m_hFile, NULL, PAGE_READONLY, 0, NdsHeader.Application_End_Offset, NULL);
	if(hMapFile==NULL) {ShowError(); return;}m_fNds.Close();
	BYTE* pRom = (BYTE*)MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, 0);
	if(pRom==NULL) {ShowError(); return;}

	CString tmp = nds + _T(".tmp");
	if(!m_fNds.Open(tmp, CFile::modeCreate|CFile::modeWrite|CFile::typeBinary)) {ShowError(); return;}
	m_fNds.Write(pRom, NdsHeader.Application_End_Offset); m_fNds.Close();*/

	//BOOL bMap=pDoc->m_hMapFile!=NULL;
	//if(bMap)
		pDoc->DeleteContents();

	m_fNds.SetLength(NdsHeader.Application_End_Offset);
	m_fNds.Close();

	//UnmapViewOfFile(pRom);
	//CloseHandle(hMapFile);

	//if(bMap)
	{
		//pDoc->OnCloseDocument();
		//CFile::Remove(nds);
		//CFile::Rename(tmp, nds);
		//theApp.OpenDocumentFile(nds);
		pDoc->OpenRom(nds);
		//return;
	}

	//CFile::Remove(nds);
	//CFile::Rename(tmp, nds);

	m_pParentWnd->SetFocus();
	OnLoadFSI();
}

void CNdsFileSystemInfo::OnUpdateFsiCreaterom(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_bReadOnly==FALSE);
}

void CNdsFileSystemInfo::OnFsiCheckcrc16(UINT nID)
{
	OnUpdateNds();
	// Crc16
	BYTE Arm9[0x4000];
	memcpy(Arm9, m_pRom+m_SpecRec[1].nTop, 0x4000);
	EnCrypt_Arm9(*(UINT*)m_pHeader->GameCode, Arm9);

	WORD secure_area_crc = GetCrc16(Arm9, 0x4000);
	WORD logo_crc = GetCrc16(m_pHeader->Nintendo_Logo, 156);
	WORD header_crc = GetCrc16((BYTE*)m_pHeader, 0x15E);
	NDSBANNER *pBanner = (NDSBANNER*)(m_pRom+m_pHeader->Banner_Offset);
	WORD banner_crc = GetCrc16(pBanner->Icon, (WORD)((BYTE*)pBanner->ChineseTitle-pBanner->Icon));

	BOOL bBad = (m_pHeader->Secure_Area_CRC != secure_area_crc) ||
				(m_pHeader->Nintendo_Logo_CRC != logo_crc) ||
				(m_pHeader->Header_CRC != header_crc);

	if( bBad || (pBanner->wCrc16 != banner_crc))
	{
		CString Error, format;
		format.LoadString(IDS_CRCERROR);
		Error.Format(format,
			m_pHeader->Secure_Area_CRC, secure_area_crc,
			m_pHeader->Nintendo_Logo_CRC, logo_crc,
			m_pHeader->Header_CRC, header_crc,
			pBanner->wCrc16, banner_crc);
		if(nID==0 || Hint(Error, (m_pTileView->GetDocument()->m_nReadOnly?MB_OK:MB_YESNO)|MB_ICONQUESTION)==IDYES)
		{
			CT2TileView *pView = (CT2TileView*)((CT2ChildFrm*)m_pParentWnd)->m_pTileView;
			if(pBanner->wCrc16 != banner_crc)
			{
				pView->DoUndo((BYTE*)&pBanner->wCrc16, (UINT)((BYTE*)&pBanner->wCrc16-m_pRom), 2);
				pBanner->wCrc16 = banner_crc;
			}
			if(bBad)
			{
				pView->DoUndo(m_pRom, 0, sizeof(NDSHEADER));

				m_pHeader->Secure_Area_CRC = secure_area_crc;
				m_pHeader->Nintendo_Logo_CRC = logo_crc;
				m_pHeader->Header_CRC = GetCrc16((BYTE*)m_pHeader, 0x15E);
			}
			m_pParentWnd->Invalidate(FALSE);
			//AddMacro_Checkcrc16();
		}
	}else
		Hint(IDS_CRCOK, MB_OK|MB_ICONINFORMATION);
}

void CNdsFileSystemInfo::OnFsiGameinfo()
{
	OnUpdateNds();
	CNdsInfo ni(this);
	ni.DoModal();
	//	AddMacro_NdsInfo();
}

void CNdsFileSystemInfo::OnUpdateFsiGameinfo(CCmdUI *pCmdUI)
{
#ifndef UNICODE
	pCmdUI->Enable(FALSE);
#endif
}

#include "CT2ScriptView.h"
void CNdsFileSystemInfo::OnFsiGoFile()
{
	UINT nCurOffset;
	CT2View* pView = (CT2View*)((CT2ChildFrm*)m_pParentWnd)->GetCurView(&nCurOffset);
	switch(nCurOffset)
	{
	case ID_VIEW_TILEVIEW:
		nCurOffset = pView->m_ptCursorPos.x*((CT2TileView*)pView)->m_nTileSize;
		nCurOffset+=pView->m_nOffset;
		break;
	case ID_VIEW_HEXVIEW:
		nCurOffset = pView->m_ptCursorPos.x;
		break;
	case ID_VIEW_SCRIPTVIEW:
		nCurOffset = ((CT2ScriptView*)pView)->m_nBegin;
		break;
	default:
		nCurOffset = 0;
		break;
	}

	int nCount = m_NdsFSI.GetItemCount();
	CString off,size;UINT nOff, nSize;
	for(int i=0; i<nCount; i++)
	{
		off = m_NdsFSI.GetItemText(i, 2);
		size = m_NdsFSI.GetItemText(i, 3); size.Replace(_T(","), _T(""));

		nOff = StrToIntEX(off); nSize = StrToIntEX(size, FALSE);
		//if((nOff==-1) || (nSize==-1)) continue;
		if( (nCurOffset>=nOff) && (nCurOffset<(nOff+nSize)) )
		{
			m_NdsFSI.SetItemState(i, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
			m_NdsFSI.SendMessage(WM_KEYDOWN, VK_LEFT);
			return;
		}
	}
	POSITION pos = m_NdsFSI.GetFirstSelectedItemPosition();
	while(pos)
	{
		int nSel = m_NdsFSI.GetNextSelectedItem(pos);
		m_NdsFSI.SetItemState(nSel, 0, LVIS_SELECTED|LVIS_FOCUSED);
	}
}

BOOL CNdsFileSystemInfo::Create(LPCTSTR lpszTemplateName, CWnd* pParentWnd)
{
	m_pParentWnd = pParentWnd;
	OnUpdateNds();

	return CDialog::Create(lpszTemplateName, pParentWnd);
}

void CNdsFileSystemInfo::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CDialog::OnOK();
	OnUpdateNds();

	OnFsiGo();
}

void CNdsFileSystemInfo::LoadPal(WORD *pWPal, RGBQUAD *pPal, WORD nCount)
{
	if(nCount>256) nCount=256;
	for(UINT i=0; i<nCount; i++)
	{
			*(UINT*)pPal = WordToRgbQuad( *(pWPal+i) );
		pPal++;
	}
	theApp.GetPalDlg().Invalidate(FALSE);
}

void CNdsFileSystemInfo::OnFsiSetmap()
{
	if(((CMDIFrameWnd*)theApp.m_pMainWnd)->MDIGetActive()!=m_pTileView->GetParent())
		((CMDIFrameWnd*)theApp.m_pMainWnd)->MDIActivate(m_pTileView->GetParent());

	//POSITION POS = m_NdsFSI.GetFirstSelectedItemPosition();
	int nSel = GetCurSel();//m_NdsFSI.GetNextSelectedItem(POS);
	//if(nSel==-1) return;
	OnUpdateNds();

	CString strFile = m_NdsFSI.GetItemText(nSel, 0);
	strFile = strFile.Left(strFile.GetLength()-3);
	CString strNSCR = strFile+_T("scr");
	CString strNCLR = strFile+_T("clr");
	CString strNCGR = strFile+_T("cgr");

	UINT nPosNSCR = GetFilePos(strNSCR);
	UINT nPosNCLR = GetFilePos(strNCLR);
	UINT nPosNCGR = GetFilePos(strNCGR);
	if(nPosNSCR!=-1)
	{
		::OnFsiLoadpal((CT2TileView*)m_pTileView, nPosNCLR);
		::OnFsiLoadtile((CT2TileView*)m_pTileView, nPosNCGR);
		::OnFsiLoadmap((CT2TileView*)m_pTileView, nPosNSCR);
	}else
	{
		strFile = m_NdsFSI.GetItemText(nSel, 0);
		strFile = strFile.Left(strFile.GetLength()-1);
		strNSCR = strFile+_T("s");
		strNCLR = strFile+_T("p");
		strNCGR = strFile+_T("t");

		nPosNSCR = GetFilePos(strNSCR);
		nPosNCLR = GetFilePos(strNCLR);
		nPosNCGR = GetFilePos(strNCGR);

		//pos = m_NdsFSI.GetItemText(nSel, 3);	pos.Replace(_T(","), _T(""));
		//UINT nCount = StrToIntEX(pos, FALSE)>>1;

		::OnFsiLoadpal((CT2TileView*)m_pTileView, nPosNCLR);
		::OnFsiLoadtile((CT2TileView*)m_pTileView, nPosNCGR, 1);
		::OnFsiLoadmap((CT2TileView*)m_pTileView, nPosNSCR);
	}

	CT2TileView *pView = (CT2TileView*)m_pTileView;
	/*NDSFILEHEADER *header = (NDSFILEHEADER*)(m_pRom+nPosNCGR);
	nPosNCGR+=header->nSize;
	header = (NDSFILEHEADER*)(m_pRom + nPosNCGR);
	if(*(UINT*)header->falgs!=0x43484152) return;

	header = (NDSFILEHEADER*)(m_pRom+nPosNSCR);
	nPosNSCR+=header->nSize;
	header = (NDSFILEHEADER*)(m_pRom + nPosNSCR);
	if(*(UINT*)header->falgs!=0x5343524E) return;

	header = (NDSFILEHEADER*)(m_pRom+nPosNCLR);
	nPosNCLR+=header->nSize;
	header = (NDSFILEHEADER*)(m_pRom + nPosNCLR);
	if(*(UINT*)header->falgs!=0x504C5454) return;
	//nPosNCLR += 0x18;

	pView->SetCurView(ID_VIEW_TILEVIEW);

	pView->m_nOffset = nPosNCGR+0x20;
	pView->m_nMoOffset = nPosNSCR+0x14;

	pView->m_nTileFormat = *(WORD*)(m_pRom+nPosNCGR+0x0C)==4?TF_GBA8BPP:TF_GBA4BPP;
	pView->m_nDrawMode = CT2_DM_MAP;
	pView->m_nWidth = *(WORD*)(m_pRom+nPosNSCR+0x08);
	pView->m_nHeight = *(WORD*)(m_pRom+nPosNSCR+0x0A);

	nPosNCLR+=0x18;
	LoadPal((WORD*)(m_pRom+nPosNCLR), 	m_pTileView->GetDocument()->m_pPal);*/

	pView->m_nScale = 100;

	//pView->OnUpdateData();
}

void CNdsFileSystemInfo::OnUpdateFsiSetmap(CCmdUI *pCmdUI)
{
	//POSITION POS = m_NdsFSI.GetFirstSelectedItemPosition();
	int nSel = GetCurSel();//m_NdsFSI.GetNextSelectedItem(POS);
	if(nSel!=-1)
	{
		CString strFile = m_NdsFSI.GetItemText(nSel, 0);
		CString ext = PathFindExtension(strFile);
		ext.MakeLower();

		strFile = strFile.Left(strFile.GetLength()-3);
		CString strNSCR = strFile+_T("scr");
		CString strNCLR = strFile+_T("clr");
		CString strNCGR = strFile+_T("cgr");

		UINT nPosNSCR = GetFilePos(strNSCR);
		UINT nPosNCLR = GetFilePos(strNCLR);
		UINT nPosNCGR = GetFilePos(strNCGR);

		BOOL bMap = nPosNSCR!=-1 && nPosNCLR!=-1 &&  nPosNCGR!=-1;/* &&
			(ext==_T(".nscr") || ext==_T(".nclr") || ext==_T(".ncgr"));*/
		if(!bMap)
		{
			strFile = m_NdsFSI.GetItemText(nSel, 0);
			strFile = strFile.Left(strFile.GetLength()-1);
			strNSCR = strFile+_T("s");
			strNCLR = strFile+_T("p");
			strNCGR = strFile+_T("t");

			nPosNSCR = GetFilePos(strNSCR);
			nPosNCLR = GetFilePos(strNCLR);
			nPosNCGR = GetFilePos(strNCGR);

			bMap = nPosNSCR!=-1 && nPosNCLR!=-1 &&  nPosNCGR!=-1;/* &&
				(ext==_T(".nscr") || ext==_T(".nclr") || ext==_T(".ncgr"));*/
		}

		pCmdUI->Enable( bMap );
	}else
		pCmdUI->Enable(FALSE);
}

UINT CNdsFileSystemInfo::GetFilePos(CString& FileName, UINT nID)
{
	NDSDIRREC *pDirRec;
	BYTE nRecLen; BOOL bIsDir; BYTE* pRec;

	UINT nPos = m_pHeader->Fnt_Offset+(nID<<3);	if(m_nRomSize<nPos) return -1;
	pDirRec = (NDSDIRREC*)(m_pRom+nPos);
	nPos = m_pHeader->Fnt_Offset + pDirRec->entry_start;	if(m_nRomSize<nPos) return -1;
	pRec  = m_pRom+nPos;
	UINT FileID = pDirRec->top_file_id;

	CString sub;
	nPos = FileName.Find(_T('/'));
	if(nPos==-1) nPos = FileName.GetLength();
	sub = FileName.Left(nPos);

	while(1)
	{
		nRecLen = *(pRec++);	if(nRecLen==0) break;
		bIsDir = nRecLen&0x80; nRecLen&=0x7F;

		memcpy(m_FileName, pRec, nRecLen);	pRec+=nRecLen;
		m_FileName[nRecLen]=0;

		OemToChar(m_FileName, g_szBuffer);

		if(bIsDir)
		{
			WORD DirID = *(WORD*)(pRec);DirID&=0xFFF;
			pRec+=2;

			if(lstrcmpi(sub, g_szBuffer)!=0)
				continue;

			nPos = FileName.Find(_T('/'));
			FileName.Delete(0, nPos+1);

			nPos = GetFilePos(FileName, DirID);

			if(nPos!=-1)
				return nPos;

			continue;
		}
		NDSFILEREC *pFileRec;

		pFileRec = (NDSFILEREC*)(m_pRom+m_pHeader->Fat_Offset+(FileID<<3));
		nPos = pFileRec->bottom-pFileRec->top;
		m_nRomFitSize+=nPos;

		if(lstrcmpi(sub, g_szBuffer)==0)
			return pFileRec->top;

		FileID++;
	}

	return -1;
}

void CNdsFileSystemInfo::OnFsiLoaddata()
{
	//POSITION POS = m_NdsFSI.GetFirstSelectedItemPosition();
	int nSel = GetCurSel();//m_NdsFSI.GetNextSelectedItem(POS);
	if(nSel==-1) return;

	CString pos = m_NdsFSI.GetItemText(nSel, 2);
	UINT nPos = StrToIntEX(pos);
	pos = m_NdsFSI.GetItemText(nSel, 3);	pos.Replace(_T(","), _T(""));
	UINT nCount = StrToIntEX(pos, FALSE)>>1;

	if(CheckSelItemExt(_T(".ncgr")) || CheckSelItemExt(_T(".char")) || CheckSelItemExt(_T(".nbfc")))
		::OnFsiLoadtile((CT2TileView*)m_pTileView, nPos);
	else
	/*if(CheckSelItemExt(_T(".nbfc")))
		::OnFsiLoadtile((CT2TileView*)m_pTileView, nPos, 2);
	else*/
	if(CheckSelItemExt(_T(".ntft")))
		::OnFsiLoadtile((CT2TileView*)m_pTileView, nPos, 1);
	else
	if(CheckSelItemExt(_T(".nclr")))
		::OnFsiLoadpal((CT2TileView*)m_pTileView, nPos);
	else
	if(CheckSelItemExt(_T(".ntfp")) || CheckSelItemExt(_T(".plt")) || CheckSelItemExt(_T(".nbfp")))
		::OnFsiLoadpal((CT2TileView*)m_pTileView, nPos, nCount);
	else
	if(CheckSelItemExt(_T(".nscr")))
		::OnFsiLoadmap((CT2TileView*)m_pTileView, nPos);

	/*m_pTileView->m_nOffset = StrToIntEX(pos);
	NDSFILEHEADER *header = (NDSFILEHEADER*)(m_pRom+m_pTileView->m_nOffset);
	m_pTileView->m_nOffset += header->nSize;
	//((CT2TileView*)m_pTileView)->m_nTileFormat = *(WORD*)(m_pRom+m_pTileView->m_nOffset+0x0C)==4?TF_GBA8BPP:TF_GBA4BPP;
	m_pTileView->m_nOffset += 0x20;
	m_pTileView->OnUpdateData();*/
}


void CNdsFileSystemInfo::OnUpdateFsiLoaddata(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
	CString text=_T("?");
	if(CheckSelItemExt(_T(".ncgr")) || CheckSelItemExt(_T(".char")) || CheckSelItemExt(_T(".ntft")) || CheckSelItemExt(_T(".nbfc")))
		text.LoadString(IDS_LOADTITLE);
	else
	if(CheckSelItemExt(_T(".nclr")) || CheckSelItemExt(_T(".ntfp")) || CheckSelItemExt(_T(".plt")) || CheckSelItemExt(_T(".nbfp")))
		text.LoadString(IDS_LOADPAL);
	else
	if(CheckSelItemExt(_T(".nscr")))
		text.LoadString(IDS_LOADMAP);
	else
		pCmdUI->Enable(FALSE);
	pCmdUI->SetText(text);
}

BOOL CNdsFileSystemInfo::CheckSelItemExt(CString ext)
{
	//POSITION POS = m_NdsFSI.GetFirstSelectedItemPosition();
	int nSel = GetCurSel();//m_NdsFSI.GetNextSelectedItem(POS);
	if(nSel==-1) return FALSE;

	CString name = m_NdsFSI.GetItemText(nSel, 0);
	CString strext = PathFindExtension(name);
	
	return lstrcmpi(strext, ext)==0;
}

void CNdsFileSystemInfo::OnSFI()
{
	int nSel = GetCurSel();
	CString Name = m_NdsFSI.GetItemText(nSel, 0);
	Name = PathFindFileName(Name);

	CT2ChildFrm* pChild = (CT2ChildFrm*)m_pTileView->GetParent();
	if( !pChild->m_pSFI ||
		(pChild->m_pSFI && Name.CompareNoCase(((CSubFileInfo*)pChild->m_pSFI)->m_strTitle)!=0) )
	{
		pChild->OnCreateSFI(Name);
		if(pChild->m_pSFI)
		{
			UINT nOffset = StrToIntEX(m_NdsFSI.GetItemText(nSel, 2));
			UINT nSize = StrToIntEX(m_NdsFSI.GetItemText(nSel, 3), FALSE, TRUE);
			((CSDatInfo*)pChild->m_pSFI)->OnInit(Name, pChild, nOffset, nSize);
		}
	}

	pChild->OnSFI();
}

void CNdsFileSystemInfo::OnUpdateSFI(CCmdUI *pCmdUI)
{
	int nSel = GetCurSel();
	CString Name = m_NdsFSI.GetItemText(nSel, 0);
	Name = PathFindFileName(Name);

	CT2ChildFrm* pChild = (CT2ChildFrm*)m_pTileView->GetParent();
/*
	if( !pChild->m_pSFI ||
		(pChild->m_pSFI && Name.CompareNoCase(((CSubFileInfo*)pChild->m_pSFI)->m_strTitle)!=0) )
	{
		pChild->OnCreateSFI(Name);
		if(pChild->m_pSFI)
		{
			UINT nOffset = StrToIntEX(m_NdsFSI.GetItemText(nSel, 2));
			UINT nSize = StrToIntEX(m_NdsFSI.GetItemText(nSel, 3), FALSE, TRUE);
			((CSDatInfo*)pChild->m_pSFI)->OnInit(Name, pChild, nOffset, nSize);
		}
	}
*/
	pCmdUI->SetText(pChild->GetSubFileMenuText(Name));
	//pCmdUI->Enable(pChild->m_pSFI!=NULL);
	pCmdUI->Enable(pChild->GetFileType(Name)!=SFI_UNKNOWN);
}

void CNdsFileSystemInfo::OnFsiFind()
{
	static CT2InputDlg id(IDS_FSI_FIND);
	if(id.DoModal()!=IDOK) return;
	m_strFind = id.m_strVal;
	m_strFind.MakeLower();
    OnFsiNext();
}

void CNdsFileSystemInfo::OnFsiCopytoclipboard()
{
	//POSITION POS = m_NdsFSI.GetFirstSelectedItemPosition();
	int nSel = GetCurSel();//m_NdsFSI.GetNextSelectedItem(POS);
	CString tmp = m_NdsFSI.GetItemText(nSel, 2);
	UINT nPos = StrToIntEX(tmp);
	tmp = m_NdsFSI.GetItemText(nSel, 3);
	tmp.Replace(_T(","), _T(""));
	UINT nSize = StrToIntEX(tmp)*3;
	HANDLE hText = GlobalAlloc(GMEM_MOVEABLE, nSize);
	BYTE* pText = (BYTE*)GlobalLock(hText);
	//memcpy(pText, m_pRom+nPos, nSize);
	MultiByteToWideChar(m_pTileView->GetDocument()->m_nScriptCodePage, 0,
		(LPCSTR)(m_pRom+nPos), nSize/3, (LPWSTR)pText, nSize);
	while(nSize>1)
	{
		if(*(WORD*)pText==0xF8F3)
		{
			*(WORD*)pText=0;
			break;
		}
		pText+=2;
		nSize-=2;
	}

	GlobalUnlock(hText);

	OpenClipboard();
	EmptyClipboard();
	SetClipboardData(CF_UNICODETEXT, hText);
	CloseClipboard();
	//GlobalFree(hText);
}

void CNdsFileSystemInfo::OnUpdateFsiCopytoclipboard(CCmdUI *pCmdUI)
{
	//POSITION POS = m_NdsFSI.GetFirstSelectedItemPosition();
	int nSel = GetCurSel();//m_NdsFSI.GetNextSelectedItem(POS);
	LVITEM lvi;
	memset(&lvi, 0, sizeof(lvi));
	lvi.mask = LVIF_IMAGE;
	lvi.iItem = nSel;
	m_NdsFSI.GetItem(&lvi);
	pCmdUI->Enable(lvi.iImage==FSI_ICON_TEXT);
}

void CNdsFileSystemInfo::OnFsiNext()
{
	if(m_strFind.IsEmpty())
	{
		OnFsiFind();
		return;
	}
	//POSITION POS = m_NdsFSI.GetFirstSelectedItemPosition();
	int nSel = GetCurSel();//m_NdsFSI.GetNextSelectedItem(POS);
	if(nSel==-1)
		nSel=0;
	else
		nSel++;

	int nFind = nSel!=0?nSel:0;
	int nCount = m_NdsFSI.GetItemCount();

FIND:
	for(; nSel<nCount; nSel++)
	{
		CString si = m_NdsFSI.GetItemText(nSel, 0);
		si.MakeLower();
		if( si.Find(m_strFind)!=-1 )
		{
			m_NdsFSI.SetItemState(nSel, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
			m_NdsFSI.SendMessage(WM_KEYDOWN, VK_LEFT);
			return;
		}
	}
	if(nFind)
	{
		nSel = 0;
		nCount = nFind;
		nFind = 0;
		goto FIND;
	}
}

void CNdsFileSystemInfo::OnLvnKeydownFsilist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	if(pLVKeyDow->wVKey==VK_F3)
		OnFsiNext();
	else
	if(pLVKeyDow->wVKey==VK_F2)
		OnFsiGohan();

	*pResult = 0;
}

void CNdsFileSystemInfo::OnFsiGohan()
{
	LVITEM lvi;
	memset(&lvi, 0, sizeof(lvi));
	lvi.mask = LVIF_IMAGE;

	//POSITION POS = m_NdsFSI.GetFirstSelectedItemPosition();
	lvi.iItem = GetCurSel();//m_NdsFSI.GetNextSelectedItem(POS);
	if(lvi.iItem==-1)
		lvi.iItem=0;
	else
		lvi.iItem++;

	int nFind = lvi.iItem!=0?lvi.iItem:0;
	int nCount = m_NdsFSI.GetItemCount();

FIND:
	for(; lvi.iItem<nCount; lvi.iItem++)
	{
		m_NdsFSI.GetItem(&lvi);
		if( lvi.iImage == FSI_ICON_HAN )
		{
			m_NdsFSI.SetItemState(lvi.iItem,
				LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
			m_NdsFSI.SendMessage(WM_KEYDOWN, VK_LEFT);
			return;
		}
	}
	if(nFind)
	{
		lvi.iItem = 0;
		nCount = nFind;
		nFind = 0;
		goto FIND;
	}
}

UINT CNdsFileSystemInfo::ImportFile(CFile *File, UINT nOff, UINT nSize, WORD wFileID, UINT nNewSize, UINT nNewOffset)
{
	BOOL bIndex = wFileID&0x8000;
	UINT nSel = bIndex ? wFileID&0x7FFF : FileIDToIndex(wFileID);
	if(bIndex && nSel>=HEADERCOUNT)
		return nNewOffset;

	UINT nFSize = nNewSize?nNewSize:(UINT)File->GetLength();
	CString size, pos, name, format;
	if(nFSize>nSize)// return;
	{
		if(nSel==0 || nSel==7)// NdsHeader.bin / banner.bin
		{
			Hint(IDS_IMPORTSPECFILE_ERROR);
			return nNewOffset;
		}
		if(nNewOffset==-1)
		{
			CT2InputDlg id((UINT)IDS_FILETOOBIG, m_pTileView);
			id.m_strVal.Format(_T("%X"),
				m_pHeader->Application_End_Offset+(16-m_pHeader->Application_End_Offset%16));
			do
			{
			NEWOFF:
				if(id.DoModal()!=IDOK) return nNewOffset;
			}while( ((nOff=StrToIntEX(id.m_strVal))==-1) ||
					(nOff+nFSize)>m_nRomSize );
			//对齐
			if(nSel==1) nOff = (nOff+0x0FFF) & ~0x0FFF;	// arm9 0x1000
			else
			if(nSel<HEADERCOUNT)
				nOff = (nOff+0x01FF) & ~0x01FF;	// header 512
			else
				nOff = (nOff+3) & ~3;	// files 4

			//交错检查
			size.Format(_T("%08X"), nOff);
			for(int i=0; i<m_NdsFSI.GetItemCount(); i++)
			{
				pos = m_NdsFSI.GetItemText(i, 2);
				size = m_NdsFSI.GetItemText(i, 3);
				size.Replace(_T(","), _T(""));
				UINT nOff2 = StrToIntEX(pos);
				UINT nSize2 = StrToIntEX(size, FALSE);
				//if(pos.CompareNoCase(size)==0)
				if( (nOff>=nOff2) && (nOff<(nOff2+nSize2)) )
				{
					name = m_NdsFSI.GetItemText(i, 0);
					format.LoadString(IDS_FILEOFFERROR);
					size.Format(format, name);
					if(Hint(size, MB_YESNO|MB_ICONQUESTION)==IDYES)
						goto NEWOFF;
					break;
				}
			}
			nNewOffset = nOff;
		}else
			nOff = nNewOffset;
	}


	CT2TileView *TileView = (CT2TileView*)m_pTileView; 
	m_pTileView->DoUndo(m_pRom+nOff, nOff, nFSize);

	char f=0;
	if(nSel==1)
	{// arm9.bin with_footer
		if(*(UINT*)(m_pRom+m_pHeader->Arm9_Rom_Offset +
            m_pHeader->Arm9_Size) == 0xDEC00621)
			f=1;
	}

	if(nNewSize)
		memcpy(m_pRom+nOff, (BYTE*)File, nFSize);
	else
		File->Read(m_pRom+nOff, nFSize);

	if(f && (*(UINT*)(m_pRom+nOff+nFSize-4*4)!=0xDEC00621))
	{// arm9.bin with_footer
		if(Hint(IDS_ARM9_FOOTER, MB_YESNO|MB_ICONQUESTION)==IDYES)
			*(UINT*)(m_pRom+nOff+nFSize-0x10)=0xDEC00621;
		else
			f = 0;
	}

	if(nSel>=HEADERCOUNT)
	{
		NDSFILEREC *filerec = (NDSFILEREC *)
            (m_pRom+m_pHeader->Fat_Offset+(wFileID<<3));

		m_pTileView->DoUndo((BYTE*)filerec, m_pHeader->Fat_Offset+(wFileID<<3), sizeof(NDSFILEREC));

		filerec->top = nOff;
		filerec->bottom = nOff+nFSize;
	}else
	{
		struct tagSpecRec
		{
			UINT *pTop, *pSize;
		};
		tagSpecRec SpecRec[HEADERCOUNT-1] = {
			//Top							Size
			{&m_pHeader->Arm9_Rom_Offset, &m_pHeader->Arm9_Size,},
			{&m_pHeader->Arm7_Rom_Offset, &m_pHeader->Arm7_Size},
			{&m_pHeader->Arm9_Overlay_Offset, &m_pHeader->Arm9_Overlay_Size},
			{&m_pHeader->Arm7_Overlay_Offset, &m_pHeader->Arm7_Overlay_Size},
			{&m_pHeader->Fnt_Offset, &m_pHeader->Fnt_Size},
			{&m_pHeader->Fat_Offset, &m_pHeader->Fat_Size},
		};

		if(nSel==1 && f)// Arm9.bin
			nFSize-=4*4;

		*SpecRec[nSel-1].pTop = nOff;
		*SpecRec[nSel-1].pSize= nFSize;
	}
	if((nOff+nFSize)>m_pHeader->Application_End_Offset)
		m_pHeader->Application_End_Offset = nOff+nFSize;
	return nNewOffset;
}

void CNdsFileSystemInfo::OnFsiSetfilesize(UINT nID)
{
	WORD i = nID-ID_FSI_SETFILESIZE+1;
	int iItem = GetCurSel();
	CString Struct = m_NdsFSI.GetItemText(iItem, FSI_SUB_FILEOFFSET);

	UINT nOff = StrToIntEX(Struct);
	Struct = GetSubHeader(nOff, iItem);

	CString sub;
	AfxExtractSubString(sub, Struct, i);

	CT2InputDlg id(IDS_FSI_SETSTRUCTSIZE);
	id.m_strVal = sub.Right(sub.GetLength()-sub.ReverseFind(_T('='))-1);
	UINT nOldSize = StrToIntEX(id.m_strVal, FALSE);

	if(id.DoModal()!=IDOK) return;

	UINT nNewSize = StrToIntEX(id.m_strVal, FALSE);
	if(nNewSize==-1 || nNewSize==nOldSize) return;
	if(nNewSize<0x10) nNewSize = 0x10;
	if(nNewSize>1024*1024*8) nNewSize=1024*1024*8;

	LVITEM item;
	item.mask = LVIF_PARAM;
	item.iItem = iItem;
	m_NdsFSI.GetItem(&item);
	OnSetfilesize(nNewSize, i, GETFILEID(item.lParam));
}

void CNdsFileSystemInfo::OnSetfilesize(UINT nNewSize, WORD wSub, WORD wFileID, UINT nNewOffset)
{
	int iItem = FileIDToIndex(wFileID);
	if(iItem==-1) return;

	CString Struct = m_NdsFSI.GetItemText(iItem, FSI_SUB_FILEOFFSET);
	UINT nOff = StrToIntEX(Struct);
	Struct = GetSubHeader(nOff, iItem);

	CString sub;
	AfxExtractSubString(sub, Struct, wSub);
	sub.Right(sub.GetLength()-sub.ReverseFind(_T('='))-1);

	UINT nOldSize = StrToIntEX(sub.Right(sub.GetLength()-sub.ReverseFind(_T('='))-1), FALSE);

	UINT i2=0;
	UINT nNewOff = nOff;
	UINT nSize=0;
	while(TRUE)
	{
		if(i2==wSub)
			nSize += nNewSize;
		else
		{
			AfxExtractSubString(sub, Struct, i2);
			if(sub.IsEmpty()) break;

			sub = sub.Right(sub.GetLength()-sub.ReverseFind(_T('='))-1);
			UINT s = StrToIntEX(sub, FALSE);
			if(i2<wSub)
				nNewOff += s;
			nSize += s;
		}
		i2++;
	}

	NDSFILEHEADER *header = (NDSFILEHEADER*)(m_pRom + nOff);
	if(nNewSize>nOldSize)
	{
		BYTE *pData = new BYTE[nSize];
		memcpy(pData, m_pRom+nOff, nNewOff-nOff);
		memcpy(pData+nNewOff-nOff, m_pRom+nNewOff, nOldSize);
		memset(pData+nNewOff-nOff+nOldSize, 0, nNewSize-nOldSize);
		SUBHEADER *subh = (SUBHEADER*)(pData+nNewOff-nOff);
		subh->nSize = nNewSize;
		memcpy(pData+nNewOff-nOff+nNewSize, m_pRom+nNewOff+nOldSize, header->nFileSize-(nNewOff-nOff+nOldSize));
		((NDSFILEHEADER*)pData)->nFileSize = nSize;
		sub = m_NdsFSI.GetItemText(iItem, FSI_SUB_FILEID);
		nNewOffset = ImportFile((CFile*)pData, nOff, header->nFileSize, StrToIntEX(sub, FALSE), nSize, nNewOffset);
		delete[] pData;
	}else
	{
		m_pTileView->DoUndo(m_pRom+nOff, nOff, header->nFileSize);
		memcpy(m_pRom+nNewOff+nNewSize, m_pRom+nNewOff+nOldSize, header->nFileSize - (nNewOff-nOff+nOldSize));
		((SUBHEADER*)(m_pRom+nNewOff))->nSize = nNewSize;
		header->nFileSize = nSize;

		//
		sub = m_NdsFSI.GetItemText(iItem, FSI_SUB_FILEID);
		i2 = StrToIntEX(sub, FALSE);

		NDSFILEREC *filerec = (NDSFILEREC *)
            (m_pRom+m_pHeader->Fat_Offset+(i2<<3));

		m_pTileView->DoUndo((BYTE*)filerec, m_pHeader->Fat_Offset+(i2<<3), sizeof(NDSFILEREC));

		filerec->top = nOff;
		filerec->bottom = nOff+nSize;
	}
	//AddMacro_Setfilesize(nNewSize, wSub, wFileID, nNewOffset);
}

void CNdsFileSystemInfo::OnUpdateFsiSetfilesize(CCmdUI *pCmdUI)
{
	//POSITION POS = m_NdsFSI.GetFirstSelectedItemPosition();
	int iItem = GetCurSel();//m_NdsFSI.GetNextSelectedItem(POS);
	CString Struct;
	if(iItem!=-1)
		Struct = m_NdsFSI.GetItemText(iItem, FSI_SUB_FILEOFFSET);

	//pCmdUI->Enable(iItem!=-1 && !Struct.IsEmpty());

	CMenu *pMenu = GetMenu()->GetSubMenu(1);
	pMenu = pMenu->GetSubMenu(6);
	if(pMenu)
	{
		UINT c = pMenu->GetMenuItemCount();
		while(c--)
		{
			pMenu->DeleteMenu(0, MF_REMOVE|MF_BYPOSITION);
		}

		// SUB1=%d SUB2=%d
		UINT nOff = StrToIntEX(Struct);
		Struct = GetSubHeader(nOff, iItem);
		CString h;
		h.LoadString(IDS_FSI_SETHEADER);
		if(m_pTileView->GetDocument()->m_nReadOnly || Struct.IsEmpty() || iItem==-1)
		{
		EMPTY:
			pMenu->InsertMenu(0, MF_BYCOMMAND|MF_STRING|MF_DISABLED, ID_FSI_SETFILESIZE, h);
			pCmdUI->Enable(FALSE);
		}else
		{
			CString sub;
			c = ID_FSI_SETFILESIZE;
			int nSub=1;
			while(TRUE)
			{
				AfxExtractSubString(sub, Struct, nSub);
				if(sub.IsEmpty()) break;
				pMenu->InsertMenu(nSub++, MF_BYCOMMAND|MF_STRING, c++, h+sub);
			}
			if(nSub==1)
				goto EMPTY;
		}
	}
}

CString CNdsFileSystemInfo::GetSubHeader(UINT nOffset, int iItem)
{
	CString text;
	unsigned char ext[6]={0}, ext2[6], el=0;

	m_NdsFSI.GetItemText(iItem, FSI_SUB_FILENAME, g_szBuffer, _MAX_PATH);
	LPTSTR lpExt = PathFindExtension(g_szBuffer);
	if(lpExt)
	{
		CharToOemBuff((LPCTSTR)(lpExt+1), (LPSTR)ext, 5);
		el = (char)strlen((char*)ext);
		_mbslwr(ext);
	}

	BOOL bNintendoFile=iItem!=-1;
	if( bNintendoFile && (!lpExt || (*ext!='N' && *ext!='n')) )
		bNintendoFile = FALSE;

	BYTE *pEnd = m_pRom+m_nRomSize-1;
	NDSFILEHEADER *header = (NDSFILEHEADER*)(m_pRom+nOffset);
	if( (((BYTE*)header+sizeof(NDSFILEHEADER))>pEnd) ||
		nOffset == -1 ) return text;

	UINT nSize = header->nSize;
	//(header->unknown2==0x01 || header->unknown2==0x02) && header->unknown3==0?header->nSize2:header->nSize;
CHECK:
	SUBHEADER *sub = (SUBHEADER*) ((BYTE*)header+nSize);

	if( (BYTE*)header<pEnd &&
		((BYTE*)sub+sizeof(SUBHEADER))<pEnd &&
		//header->nSubHeaderCount<0x10 &&
		//(header->unknown1==0xFFFE || header->unknown1==0xFEFF) &&
		//(header->unknown2==0x0100 || header->unknown2==0x01 || header->unknown2==0x02) &&
		(IsCharUpper(header->falgs[0]) || (header->falgs[0]>='0' && header->falgs[0]<='9')) &&
		(IsCharUpper(header->falgs[1]) || (header->falgs[1]>='0' && header->falgs[1]<='9') || header->falgs[1]==' ') &&
		(IsCharUpper(header->falgs[2]) || (header->falgs[2]>='0' && header->falgs[2]<='9') || header->falgs[2]==' ') &&
		(IsCharUpper(header->falgs[3]) || (header->falgs[3]>='0' && header->falgs[3]<='9') || header->falgs[3]==' ') &&

		(!header->nSubHeaderCount || ((IsCharUpper(sub->falgs[0]) || (sub->falgs[0]>='0' && sub->falgs[0]<='9')) &&
		(IsCharUpper(sub->falgs[1]) || (sub->falgs[1]>='0' && sub->falgs[1]<='9') || sub->falgs[1]==' ') &&
		(IsCharUpper(sub->falgs[2]) || (sub->falgs[2]>='0' && sub->falgs[2]<='9') || sub->falgs[2]==' ') &&
		(IsCharUpper(sub->falgs[3]) || (sub->falgs[3]>='0' && sub->falgs[3]<='9') || sub->falgs[3]==' '))) )
	{
		if(bNintendoFile)
		{
			StrCpyNA((char*)ext2, header->falgs, 5);
			_mbslwr(ext2);
			bNintendoFile = header->falgs[3]=='N' || StrCmpNA((LPCSTR)ext2, (LPCSTR)ext, el)==0;
		}
		if( bNintendoFile )
			text.Format(_T("%c%c%c%c=%d\n"),
				header->falgs[3],header->falgs[2],header->falgs[1],header->falgs[0],
				nSize);
		else
			text.Format(_T("%c%c%c%c=%d\n"),
				header->falgs[0],header->falgs[1],header->falgs[2],header->falgs[3],
				nSize);
		nSize = 0;
		for(int i=0; /*nSize<m_nRomSize && */i<header->nSubHeaderCount; i++)
		{
			if( nSize>header->nFileSize ||
				//nSize>m_nRomSize ||
				((BYTE*)sub+sizeof(SUBHEADER))>=pEnd ||
				((!IsCharUpper(sub->falgs[0]) && !(sub->falgs[0]>='0' && sub->falgs[0]<='9')) &&
				(!IsCharUpper(sub->falgs[1]) && !(sub->falgs[1]>='0' && sub->falgs[1]<='9') && sub->falgs[1]!=' ') &&
				(!IsCharUpper(sub->falgs[2]) && !(sub->falgs[2]>='0' && sub->falgs[2]<='9') && sub->falgs[2]!=' ') &&
				(!IsCharUpper(sub->falgs[3]) && !(sub->falgs[3]>='0' && sub->falgs[3]<='9') && sub->falgs[3]!=' ')) )
			{
				text.Empty();
				break;
			}
			nSize += sub->nSize;
			if(bNintendoFile)
				text.AppendFormat(_T("%c%c%c%c=%d\n"),
					sub->falgs[3],sub->falgs[2],sub->falgs[1],sub->falgs[0],
					sub->nSize);
			else
				text.AppendFormat(_T("%c%c%c%c=%d\n"),
					sub->falgs[0],sub->falgs[1],sub->falgs[2],sub->falgs[3],
					sub->nSize);
			sub = (SUBHEADER*)((BYTE*)sub+sub->nSize);
			//侨胶
			if (i != header->nSubHeaderCount && header->nSubHeaderCount != 0)
				text.Empty();
		}

	}else
	if(nSize!=header->nSize2)
	{
		nSize = header->nSize2;
		goto CHECK;
	}
	return text;
}

void CNdsFileSystemInfo::OnFsiGotoheader(UINT nID)
{
	UINT i = nID-ID_FSI_GOTOHEADER+1;
	//POSITION POS = m_NdsFSI.GetFirstSelectedItemPosition();
	int iItem = GetCurSel();//m_NdsFSI.GetNextSelectedItem(POS);
	CString Struct = m_NdsFSI.GetItemText(iItem, FSI_SUB_FILEOFFSET);

	UINT nPos = StrToIntEX(Struct);
	Struct = GetSubHeader(nPos, iItem);
	UINT i2=0;
	CString sub;
	do
	{
		AfxExtractSubString(sub, Struct, i2++);
		if(sub.IsEmpty()) break;
		nPos += StrToIntEX(sub.Right(sub.GetLength()-sub.ReverseFind(_T('='))-1), FALSE);
	}while(i2<i);

	((CT2ChildFrm*)m_pTileView->GetParent())->GetCurView()->ScrollTo(nPos, TRUE);
}

void CNdsFileSystemInfo::OnUpdateFsiGotoheader(CCmdUI *pCmdUI)
{
	//POSITION POS = m_NdsFSI.GetFirstSelectedItemPosition();
	int iItem = GetCurSel();//m_NdsFSI.GetNextSelectedItem(POS);
	CString Struct;
	if(iItem!=-1)
		Struct = m_NdsFSI.GetItemText(iItem, FSI_SUB_FILEOFFSET);

	//pCmdUI->Enable(iItem!=-1 && !Struct.IsEmpty());

	CMenu *pMenu = GetMenu()->GetSubMenu(2);
	pMenu = pMenu->GetSubMenu(2);
	if(pMenu)
	{
		UINT c = pMenu->GetMenuItemCount();
		while(c--)
		{
			pMenu->DeleteMenu(0, MF_REMOVE|MF_BYPOSITION);
		}

		// SUB1=%d SUB2=%d
		UINT nOff = StrToIntEX(Struct);
		Struct = GetSubHeader(nOff, iItem);
		CString h;
		h.LoadString(IDS_FSI_GOTOHEADER);
		if(Struct.IsEmpty() || iItem==-1)
		{
		EMPTY:
			pMenu->InsertMenu(0, MF_BYCOMMAND|MF_STRING|MF_DISABLED, ID_FSI_GOTOHEADER, h);
			pCmdUI->Enable(FALSE);
		}else
		{
			CString sub;
			c = ID_FSI_GOTOHEADER;
			int nSub=1;
			while(TRUE)
			{
				AfxExtractSubString(sub, Struct, nSub);
				if(sub.IsEmpty()) break;
				pMenu->InsertMenu(nSub++, MF_BYCOMMAND|MF_STRING, c++, h+sub);
			}
			if(nSub==1)
				goto EMPTY;
		}
	}
}

void CNdsFileSystemInfo::OnFsiExportex()
{
	CT2InputDlg id(IDS_FSI_EXPORTFILEEX);
	if(id.DoModal()!=IDOK) return;

	id.m_strVal.MakeLower();
	ExportFile(id.m_strVal);
}

void CNdsFileSystemInfo::ExportFile(CString key)
{
	POSITION POS = m_NdsFSI.GetFirstSelectedItemPosition();
	int nSel = m_NdsFSI.GetNextSelectedItem(POS);
	BOOL bKey=!key.IsEmpty();

	if(nSel==-1 && !bKey) return;

	CString name = m_NdsFSI.GetItemText(nSel, 0);
	CString pos = m_NdsFSI.GetItemText(nSel, 2);
	CString size = m_NdsFSI.GetItemText(nSel, 3);	size.Replace(_T(","), _T(""));

	int nPos;	CString Path, Format;
	if(bKey || m_NdsFSI.GetSelectedCount()>1)
	{
		if(bKey) nSel = 0;
		Path = GetPath(m_hWnd);
		if(Path.IsEmpty()) return;
	}else
	{
		static CFileDialog fd(FALSE);
		fd.m_pOFN->lpstrFile = Path.GetBuffer(_MAX_PATH);
		Path.ReleaseBuffer(_MAX_PATH);
		fd.m_pOFN->nMaxFile = _MAX_PATH;
		lstrcpy(fd.m_pOFN->lpstrFile, PathFindFileName(name));
		if(fd.DoModal()!=IDOK) return;
		name.Empty();
	}

	UINT nOff, nSize;
	while(TRUE)
	{
		//mkdir
		nPos = 1;
		while( (nPos=name.Find(_T('/'), nPos))!=-1 )
		{
			nSize = name.Find(_T('/'), nPos); if(nSize==-1) nSize=name.GetLength();
			Format.Format(_T("%s%s"), Path, name.Left(nSize));
			if(!CreateDirectory(Format, (LPSECURITY_ATTRIBUTES)NULL) && (GetLastError()!=0xB7))
				{ShowError(); return;}
			nPos = nSize+1;
		}
		//lstrcpy(path, Path+name);
		CString pn = Path+name;
		pn.MakeLower();

		nOff = StrToIntEX(pos);
		nSize = StrToIntEX(size, FALSE);

		if(!bKey || (bKey && name.Find(key)!=-1))
		{
			CFile file;
			if(!file.Open(pn, CFile::modeCreate|CFile::modeWrite|CFile::typeBinary))
				{ShowError(); return;}

			file.Write(m_pRom+nOff, nSize);
		}
		if(bKey)
		{
			nSel++;
			if(nSel>=m_NdsFSI.GetItemCount()) break;
		}else
		{
			if(POS==NULL) break;
			nSel = m_NdsFSI.GetNextSelectedItem(POS);
		}
		name = m_NdsFSI.GetItemText(nSel, 0);
		pos = m_NdsFSI.GetItemText(nSel, 2);
		size = m_NdsFSI.GetItemText(nSel, 3);	size.Replace(_T(","), _T(""));
	}
}

void CNdsFileSystemInfo::OnUpdateFsipopNtfr(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(CheckSelItemExt(_T(".nftr")));
}

void CNdsFileSystemInfo::OnUpdateFsipopNtfrSetNftrWH(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(CheckSelItemExt(_T(".nftr")) && !m_pTileView->GetDocument()->m_nReadOnly);
}

void CNdsFileSystemInfo::OnFsipopLoadntfrwh()
{
	//POSITION POS = m_NdsFSI.GetFirstSelectedItemPosition();
	int nSel = GetCurSel();//m_NdsFSI.GetNextSelectedItem(POS);
	// if(nSel==-1) return;
	UINT nOff = GetSubHeaderOffset(nSel, _T("CGLP"));
	if(nOff!=-1)
	{
		NDSCGLP *cglp = (NDSCGLP*)(m_pRom+nOff);
		CT2TileView *pTV = (CT2TileView*)m_pTileView;
		pTV->m_nWidth = cglp->nWidth;
		pTV->m_nHeight = cglp->nHeight;
		pTV->OnUpdateData();
	}
/*
	CString pos = m_NdsFSI.GetItemText(nSel, 2);
	UINT nOff = StrToIntEX(pos);
	pos = GetSubHeader(nOff, nSel);

	CString sub;
	//nOff = 0;
	nSel = 0;
	while(TRUE)
	{
		AfxExtractSubString(sub, pos, nSel++);
		if(sub.IsEmpty()) break;

		if(sub.Left(5)==_T("CGLP="))
		{
			NDSCGLP *cglp = (NDSCGLP*)(m_pRom+nOff);
			CT2TileView *pTV = (CT2TileView*)m_pTileView;
			pTV->m_nWidth = cglp->nWidth;
			pTV->m_nHeight = cglp->nHeight;
			pTV->OnUpdateData();
			break;
		}
		int p = sub.Find(_T('='));
		nOff += StrToIntEX(sub.Right(sub.GetLength()-p-1), FALSE);
	}*/
}

void CNdsFileSystemInfo::OnFsipopSetntfrwh()
{
	static CT2InputDlg id(IDS_SETNFTRWH);
	if(id.DoModal()!=IDOK) return;
	CString strWH = id.m_strVal;
	strWH.MakeLower();

	int nPos = strWH.Find(_T('x'));
	if(nPos==-1)
		nPos = strWH.Find(_T(','));

	if(nPos==-1)
		return;

	int nWidth, nHeight;
	nWidth = StrToIntEX(strWH.Left(nPos), FALSE);
	nHeight = StrToIntEX(strWH.Right(strWH.GetLength()-nPos-1), FALSE);

	if(nWidth<1 || nHeight<1 ||
		nWidth>255 || nHeight>255)
		return;

	LVITEM item;
	item.mask = LVIF_PARAM;
	item.iItem=GetCurSel();
	m_NdsFSI.GetItem(&item);
	WORD wFileID = GETFILEID(item.lParam);

	OnSetntfrwh(nWidth, nHeight, wFileID);
}

void CNdsFileSystemInfo::OnSetntfrwh(BYTE nWidth, BYTE nHeight, WORD wFileID)
{
	int nSel=FileIDToIndex(wFileID);
	if(nSel==-1) return;

	UINT nOff = GetSubHeaderOffset(nSel, _T("CGLP"));
	if(nOff!=-1)
	{
		NDSCGLP *cglp = (NDSCGLP*)(m_pRom+nOff);
		m_pTileView->DoUndo(&cglp->nWidth, nOff+8, 2);

		cglp->nWidth = nWidth;
		cglp->nHeight = nHeight;

		theApp.GetCurView()->Invalidate(FALSE);
		m_NdsFSI.Invalidate(FALSE);

		//AddMacro_Setntfrwh(nWidth, nHeight, wFileID);
	}
}

UINT CNdsFileSystemInfo::GetSubHeaderOffset(int nItem, CString Header)
{
	CString pos = m_NdsFSI.GetItemText(nItem, 2);
	UINT nOff = StrToIntEX(pos);
	pos = GetSubHeader(nOff, nItem);

	CString sub;
	int i=0;
	int l = Header.GetLength()+1;
	Header.AppendChar(_T('='));
	while(TRUE)
	{
		AfxExtractSubString(sub, pos, i++);
		if(sub.IsEmpty()) break;

		if(sub.Left(l)==Header)
		{
			return nOff;
		}
		int p = sub.Find(_T('='));
		nOff += StrToIntEX(sub.Right(sub.GetLength()-p-1), FALSE);
	}

	return -1;
}

void CNdsFileSystemInfo::OnFsipopExporttbl()
{
	static CFileDialog fd(FALSE);
	CString f;
	f.LoadString(IDS_TBLFILTER);
	f.AppendChar(_T('\0'));
	fd.m_pOFN->lpstrFilter = f;

	if(fd.DoModal()!=IDOK) return;
	CStdioFile file;
	if(!file.Open(fd.GetPathName(), CStdioFile::modeCreate|CStdioFile::modeReadWrite|CStdioFile::typeBinary))
	{
		ShowError();
		return;
	}

	WORD c=0xFEFF;
	file.Write(&c, 2);

	int iSel = GetCurSel();
	CString Offset = m_NdsFSI.GetItemText(iSel, 2);
	UINT nOffset = StrToIntEX(Offset);
	CString strCWDH = GetSubHeader(nOffset, iSel);

	iSel = 0;
	UINT nSize;
	while(AfxExtractSubString(Offset, strCWDH, iSel++))
	{
		nSize = StrToIntEX(Offset.Right(Offset.GetLength()-5), FALSE);

		if(Offset.Left(4)==_T("CMAP"))
		{
			NDSCMAP *cmap = (NDSCMAP*)(m_pRom+nOffset);
			if(cmap->nType==2)
			{
				f.Format(_T(";[%08X] Chunk#:%d %s\r\n"), nOffset, iSel, Offset);
				file.WriteString(f);

				for(WORD i=0; i<cmap->wCount; i++)
				{
					/*if(!cmap->Tbl[i].wChar)
						break;*/
					WORD wChar = cmap->Tbl[i].wChar;
					if(wChar>0xFF)
						wChar = (wChar>>8) | (wChar<<8);

					UINT nTbl;
					mcharTowchar(&nTbl, &wChar, CP_SHIFTJIS);
					Offset.Format(_T("%04X=%c\r\n"), cmap->Tbl[i].wChar, nTbl);
					file.WriteString(Offset);
				}
				file.WriteString(_T("\r\n"));
			}
		}

		nOffset += nSize;
	}
}

int CNdsFileSystemInfo::GetCurSel()
{
	POSITION pos = m_NdsFSI.GetFirstSelectedItemPosition();
	return m_NdsFSI.GetNextSelectedItem(pos);
}

int CNdsFileSystemInfo::FileIDToIndex(WORD wFileID)
{
	LVITEM item;
	item.mask = LVIF_PARAM;
	for(item.iItem=HEADERCOUNT; item.iItem<m_NdsFSI.GetItemCount(); item.iItem++)
	{
		m_NdsFSI.GetItem(&item);
		if( GETFILEID(item.lParam) == wFileID )
			return item.iItem;
	}
	return -1;
}
/*
void CNdsFileSystemInfo::AddMacro_LoadPal(UINT nOffset)
{
	CT_MACRO_CT2HEXVIEW_SETDATA m;
	INITMACRO(m, CT2NDSFSI_LOADPAL);

	m.nOffset = nOffset;

	m_pTileView->AddMacro(&m);
}

void CNdsFileSystemInfo::AddMacro_Setntfrwh(BYTE nWidth, BYTE nHeight, WORD wFileID)
{
	CT_MACRO_CT2HEXVIEW_SETDATA m;
	INITMACRO(m, CT2NDSFSI_SETNTFRWH);

	m.nSize = nWidth;
	m.nFlags = nHeight;
	m.nOffset = wFileID;

	m_pTileView->AddMacro(&m);
}

void CNdsFileSystemInfo::AddMacro_Setfilesize(UINT nNewSize, WORD wSub, WORD wFileID, UINT nNewOffset)
{
	CT_MACRO_NDS_SETFILESIZE m;
	INITMACRO(m, CT2NDSFSI_SETFILESIZE);

	m.nNewSize = nNewSize;
	m.nNewOffset = nNewOffset;
	m.wSub = wSub;
	m.wFileID = wFileID;

	m_pTileView->AddMacro(&m);
}

void CNdsFileSystemInfo::AddMacro_Import(CString path, UINT nOff, UINT nSize, WORD wFileID, UINT nNewOffset)
{
	CT_MACRO_NDS_IMPORTFILE m;
	INITMACRO(m, CT2NDSFSI_IMPORT);

	m.nOffset = nOff;
	m.nSize = nSize;
	m.nNewOffset = nNewOffset;
	m.wFileID = wFileID;

	lstrcpyn(m.szFile, path, _MAX_PATH);

	m_pTileView->AddMacro(&m);
}

void CNdsFileSystemInfo::AddMacro_Checkcrc16()
{
	CT_MACRO m;
	INITMACRO(m, CT2NDSFSI_CHECKCRC16);

	m_pTileView->AddMacro(&m);
}

void CNdsFileSystemInfo::AddMacro_NdsInfo()
{
	CT_MACRO_NDS_NDSINFO m;
	INITMACRO(m, CT2NDSFSI_NDSINFO);

	NDSHEADER *pHeader = (NDSHEADER*)m_pRom;
	LPTSTR pStr = (LPTSTR)(m_pRom+pHeader->Banner_Offset+0x240);
	memcpy(m.Info, pStr, 0x100*7);

	m_pTileView->AddMacro(&m);
}
*/
void CNdsFileSystemInfo::OnLvnEndScrollFsilist(NMHDR *pNMHDR, LRESULT *pResult)
{
	// 此功能要求 Internet Explorer 5.5 或更高版本。
	// 符号 _WIN32_IE 必须是 >= 0x0560。
	LPNMLVSCROLL pStateChanged = reinterpret_cast<LPNMLVSCROLL>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	m_NdsFSI.Invalidate(FALSE);
}

UINT CNdsFileSystemInfo::AlignFile(UINT nAlignSize, BYTE* pData)
{
	UINT nPos = (UINT)m_fNds.GetPosition();
	nPos &= nAlignSize-1;
	if(nPos)
	{
		nAlignSize-=nPos;
		m_fNds.Write(pData, nAlignSize);
		return nAlignSize;
	}
	return 0;
}

void CNdsFileSystemInfo::OnFsipopShowicon()
{
	CT2ChildFrm *pChild = (CT2ChildFrm*)m_pParentWnd;
	CT2TileView *pView = (CT2TileView*)pChild->m_pTileView;
	NDSBANNER *pBanner = (NDSBANNER*)(m_pRom+m_pHeader->Banner_Offset);
	((CT2HexView*)pChild->m_pHexView)->OnEditData2pal((UINT)(pBanner->IconPalette-m_pRom),
		pView->m_nPalIndex, pView->m_nBitCount);

	if(pChild->m_nCurView!=ID_VIEW_TILEVIEW)
		pChild->SetCurView(ID_VIEW_TILEVIEW);

	pView->m_nWidth = pView->m_nHeight = 32;
	pView->m_nTileFormat = TF_GBA4BPP;
	pView->m_nDrawMode = CT2_DM_OBJH;
	pView->m_nOffset = (UINT)(pBanner->Icon-m_pRom);

	pView->OnUpdateData();
}

void CNdsFileSystemInfo::OnFsipopUpdateiconpal()
{
	CT2ChildFrm *pChild = (CT2ChildFrm*)m_pParentWnd;
	CT2TileView *pView = (CT2TileView*)pChild->m_pTileView;
	CT2HexView *pHexView = (CT2HexView*)pChild->m_pHexView;
	NDSBANNER *pBanner = (NDSBANNER*)(m_pRom+m_pHeader->Banner_Offset);

	BYTE nAlign=pHexView->m_nAlign;
	pHexView->m_nAlign=0;
	pHexView->OnEditPal2data((UINT)(pBanner->IconPalette-m_pRom), pView->m_nPalIndex&(~0x0F), 4);
	pHexView->m_nAlign=nAlign;

	pView->DoUndo((BYTE*)&pBanner->wCrc16, (UINT)((BYTE*)&pBanner->wCrc16-m_pRom), 2);
	pBanner->wCrc16 = GetCrc16((BYTE*)pBanner->Icon, (WORD)((BYTE*)pBanner->ChineseTitle-pBanner->Icon));

	theApp.GetCurView()->Invalidate(FALSE);
}

void CNdsFileSystemInfo::OnFsiFix()
{
	CT2Doc *pDoc = ((CT2ChildFrm*)m_pParentWnd)->m_pTileView->GetDocument();
	CString nds = pDoc->GetPathName();
	CFile file;
	if(!file.Open(nds, CFile::modeReadWrite|CFile::typeBinary))
	{
		ShowError();
		return;
	}

	UINT Application_End_Offset = m_pHeader->Application_End_Offset;

	pDoc->DeleteContents();

	file.SetLength(Application_End_Offset);
	file.Close();

	pDoc->OpenRom(nds);
}
