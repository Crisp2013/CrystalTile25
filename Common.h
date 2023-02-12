#ifndef CT2_COMMON_H
#define CT2_COMMON_H

//support x64
#ifdef _WIN64
#define GWL_WNDPROC GWLP_WNDPROC
#define GWL_HINSTANCE GWLP_HINSTANCE
#define GWL_HWNDPARENT GWLP_HWNDPARENT
#define GWL_USERDATA GWLP_USERDATA
//#define GWL_ID GWLP_ID
#define GCL_MENUNAME GCLP_MENUNAME
#define GCL_HBRBACKGROUND GCLP_HBRBACKGROUND
#define GCL_HCURSOR GCLP_HCURSOR
#define GCL_HICON GCLP_HICON
#define GCL_HMODULE GCLP_HMODULE
#define GCL_WNDPROC GCLP_WNDPROC
#define GCL_HICONSM GCLP_HICONSM
//#define GetClassLong GetClassLongPtr
//#define GetWindowLong GetWindowLongPtr
//#define SetClassLong SetClassLongPtr
//#define SetWindowLong SetWindowLongPtr
#endif



// 变量
extern CString g_strMFile;
extern UINT g_nMFileSize;
extern UINT g_nRomType;
extern HIMAGELIST g_hCTImageList;
extern TCHAR g_szBuffer[_MAX_PATH];
//extern TCHAR g_szBuffer2[_MAX_PATH];
extern CHAR g_szBufferA[_MAX_PATH];
//extern CHAR g_szBufferA2[_MAX_PATH];
extern BYTE g_bBreak;

// 结构

struct PALETTEFILEHEADER
{
	DWORD	Signature;	//0x46464952 RIFF
	DWORD	FileSize;
};
struct PALETTEINFOHEADER
{
	DWORD	RIFF;		//0x204C4150 Pal 
	DWORD	Signature;	//0x61746164 data
	DWORD	ChunkSize;
	WORD	Version;	//0x0300
	WORD	Entries;
};
struct PALETTEINFO
{
	PALETTEFILEHEADER	FileHeader;
	PALETTEINFOHEADER	Palette;
};

struct UNDO {
	BYTE *pData;
	UINT nOffset;
	UINT nSize;
};

struct MAINFRMPOS {
	RECT rcMainFrm;
	BYTE bShowMax;
};

#define CT2_FAVLEN 32
struct FAVORITES{
	TCHAR Doc[_MAX_PATH];		// 文档名
	TCHAR Desc[CT2_FAVLEN];		// 书签注释
	UINT nOffset;				// 偏移
	WORD nCurView;				// 当前视图
	RGBQUAD pPal[256];			// 调色板

	// TileView
	BYTE nTileFormat;			// 颜色格式
	WORD nWidth;				// 宽度
	WORD nHeight;				// 高度
	WORD nScale;				// 缩放
	BYTE nDrawMode:4;			// 绘制模式
	BYTE nHCheck:1;				// 水平翻转
	BYTE nVCheck:1;				// 垂直翻转
	BYTE nL90Check:1;			// 左转九十度
	BYTE nR90Check:1;			// 右转九十度
	BYTE n24Check;				// 2/4层的Tile
	BYTE nSkipSize;				// 跳过字节
	BYTE nPalIndex;				// 调色板号
	WORD nColCount;				// Tile视图列数
	WORD nLineCount;			// Tile视图行数
	UINT nMoOffset;				// Map/Oam地址
	WORD nBkPixType;			// 底纹
	BYTE nAutoColFit;			// 自适应行列

	// AsmView
	UINT nBaseAddress;
	BYTE nCpuType;
	BYTE nAsmViewSize;

	// HexView
	BYTE nTblMode;
	BYTE nSwpCode;

	// CT2Doc
	UINT nScriptCodePage;
};

struct STATEDATA {
	WORD nSize;					// 本结构大小
	// Doc
	RGBQUAD pPal[256];			// 调色板
	UINT nBlockOffset;			// 映射块偏移
	BYTE nStateMapFile;			// 文档打开模式
	WORD nTblCodePage;			// 码表编码格式
	WORD nScriptCodePage;		// 处理脚本的编码格式
	// SHUQIAN Sq[10];				// 10个书签
	TCHAR szPathName[3][_MAX_PATH];	// 多文件单文档

	// ChidFrm
	MAINFRMPOS wpChildFrm;		// 文档框架位置
	WORD nCurView;				// 当前视图

	// CT2View

	// TileView
	UINT nTileViewOffset;		// 偏移
	BYTE nTileFormat;			// 颜色格式
	WORD nWidth;				// 宽度
	WORD nHeight;				// 高度
	WORD nScale;				// 缩放
	BYTE nDrawMode:4;			// 绘制模式
	BYTE nHCheck:1;				// 水平翻转
	BYTE nVCheck:1;				// 垂直翻转
	BYTE nL90Check:1;			// 左转九十度
	BYTE nR90Check:1;			// 右转九十度
	BYTE nTileViewGrid:1;		// 网格
	BYTE nSkipSize;				// 跳过字节
	BYTE n24Check;				// 2/4层的Tile
	BYTE nNextSize;				// 相关偏移
	BYTE nPalIndex;				// 调色板号
	WORD nColCount;				// Tile视图列数
	WORD nLineCount;			// Tile视图行数
	UINT nMoOffset;				// Map/Oam地址
	WORD nBkPixType;			// 底纹

	// HexView
	UINT nHexViewOffset;
	TCHAR lpszTblName[_MAX_PATH];// 码表文件路径

	// AsmView
	UINT nAsmViewOffset;
	UINT nAsmBaseAddress;
	BYTE nAsmSize;
	BYTE nAsmCpuType;

	// TileEditorView
	WORD nTileEditorViewScale;
};

struct GBAHEADER
{
	BYTE	RomEntryPoint[4];	// 32bit ARM branch opcode, eg. "B rom_start"
	BYTE	NintendoLogo[156];	// compressed bitmap, required!
	char	GameTitle[12];		// 大写
	char	GameCode[4];		// 大写 AGB-XXXX
	char	MakerCode[2];		// 大写 "01"=Nintendo.
	BYTE	FixedValue;			// must be 96h, required!
	BYTE	MainUnitCode;		// 00h for current GBA models
	BYTE	DeviceType;			// huh ???
	BYTE	ReservedArea[7];	// should be zero filled
	BYTE	SoftwareVersion;	// usually 00h
	BYTE	ComplementCheck;	// header checksum, required!
	BYTE	ReservedArea2[2];	// should be zero filled
	BYTE	RAMEntryPoint[4];	// 32bit ARM branch opcode, eg. "B ram_start"
	BYTE	BootMode;			// init as 00h - BIOS overwrites this value!	BootModeValue	Expl.			SlaceIDNumberValue  Expl.
	BYTE	SlaceIDNumber;		// init as 00h - BIOS overwrites this value!		01h			Joybus mode			01h				Slave #1
	BYTE	NotUsed[26];		// seems to be unused								02h			Normal mode			02h				Slave #2
	BYTE	JOYBUSEntryPt[4];	// 32bit ARM branch opcode, eg. "B joy_start"		03h			Multiplay mode		03h				Slave #3
};

struct NDSHEADER
{
	char GameTitle[0x0C];
	char GameCode[0x04];
	char MakerCode[0x02];
	BYTE UnitCode;
	BYTE DeviceCode;						// type of device in the game card
	BYTE DeviceCap;							// device capacity (128kb<<n Mbit)
	BYTE Reserved_0x015[0x09];				// (zero filled)
	BYTE RomVersion;
	BYTE Autostart;							// (Bit2: Skip "Press Button" after Health and Safety) (Also skips bootmenu, even in Manual mode & even Start pressed)
	UINT Arm9_Rom_Offset;					// copy src
	UINT Arm9_Entry_Address;				// entry point
	UINT Arm9_Ram_Address;					// copy dst
	UINT Arm9_Size;							// size
	UINT Arm7_Rom_Offset;
	UINT Arm7_Entry_Address;
	UINT Arm7_Ram_Address;
	UINT Arm7_Size;
	UINT Fnt_Offset;
	UINT Fnt_Size;
	UINT Fat_Offset;
	UINT Fat_Size;
	UINT Arm9_Overlay_Offset;
	UINT Arm9_Overlay_Size;
	UINT Arm7_Overlay_Offset;
	UINT Arm7_Overlay_Size;
	UINT Port_40001A4h_Normal_Commands;		// Port 40001A4h setting for normal commands (usually 00586000h)
	UINT Port_40001A4h_KEY1_Commands;		// Port 40001A4h setting for KEY1 commands   (usually 001808F8h)
 	UINT Banner_Offset;
	WORD Secure_Area_CRC;
	WORD Secure_Area_Loading_Timeout;
	UINT ARM9_Auto_Load_List_RAM_Address;	// ?
	UINT ARM7_Auto_Load_List_RAM_Address;	// ?
	UINT Secure_Area_Disable1;				// unique ID for homebrew
	UINT Secure_Area_Disable2;				// unique ID for homebrew
	UINT Application_End_Offset;			// Total Used ROM size
	UINT Rom_Header_Size;
	BYTE Reserved_0x088[0x38];				// Reserved (zero filled)
	BYTE Nintendo_Logo[0x9C];
	WORD Nintendo_Logo_CRC;
	WORD Header_CRC;
	UINT Debug_Rom_Offset;					// (0=none) (8000h and up)       ;only if debug
	UINT Debug_Size;						// (0=none) (max 3BFE00h)        ;version with
	UINT Debug_Ram_Address;					// (0=none) (2400000h..27BFE00h) ;SIO and 8MB
	UINT Reserved_0x16C;
	BYTE Zero[0x90];
};

struct NDSBANNER
{
	WORD wVer;				// Version  (0001h)
	WORD wCrc16;			// CRC16 across entries 020h..83Fh
	BYTE Reserved[0x1C];	// 00
	BYTE Icon[0x200];		// (32x32 pix) (4x4 tiles, each 4x8 bytes, 4bit depth)
	BYTE IconPalette[0x20];	// (16 colors, 16bit, range 0000h-7FFFh) (Color 0 is transparent, so the 1st palette entry is ignored)

	WCHAR JapaneseTitle[0x80];	// (128 characters, 16bit Unicode)
	WCHAR EnglishTitle[0x80];
	WCHAR FrenchTitle[0x80];
	WCHAR GermanTitle[0x80];
	WCHAR ItalianTitle[0x80];
	WCHAR SpanishTitle[0x80];
	WCHAR ChineseTitle[0x80];	// ?
};

struct NDSFIRMWARE_USERSETTINGS
{
	WORD wVer;
	BYTE Favorite_Color;
	BYTE Birthday_Month;
	BYTE Birthday_Day;
	BYTE Not_used;
	char Nickname[20];
	BYTE Nickname_Length[2];	// utf-16
	char Message[52];
	BYTE Message_Length[2];
	BYTE Alarm_Hour;
	BYTE Alarm_Minute;
	BYTE Unknown[2];
	BYTE Enable_Alarm;
	BYTE Zero;
	BYTE Adc_x1[2];
	BYTE Adc_y1[2];
	BYTE Scr_x1;
	BYTE Scr_y1;
	BYTE Adc_x2[2];
	BYTE Adc_xy[2];
	BYTE Scr_x2;
	BYTE Scr_y2;
	BYTE Language[2];	// 0-2bit
	BYTE Unknown2[2];
	BYTE RTC_Offset[4];
	BYTE Not_used2[4];
	BYTE Update_Counter[2];
	BYTE CRC16[2];	// 00h..6Fh(Update_Counter-wVer)
	BYTE Not_used3[0x8C];
};

struct NDSDIRREC
{
	UINT entry_start;
	WORD top_file_id;
	WORD parent_id_or_count;
};

struct NDSFILEREC
{
	UINT top;
	UINT bottom;	// size = bottom-top
};

struct OVERLAYENTRY
{
	UINT id;
	UINT ram_address;
	UINT ram_size;
	UINT bss_size;
	UINT sinit_init;
	UINT sinit_init_end;
	UINT file_id;
	UINT reserved;
};

struct NDSSPECREC {
	UINT nTop;
	UINT nBottom;
	CString FileName;
};

//ndsfile .sdat文件结构
// size:0x40
struct NDSSDAT
{
	char flags[4];			// 'S' 'D' 'A' 'T'
	char unknown1[2];		// 0xFEFF ?
	BYTE unknown2;			// 0x00
	BYTE unknown3;			// 0x01
	UINT nSDATSize;			// .sdat文件大小
	WORD nSize;				// tagNDSSDAT结构大小(0x40)
	WORD wChunkMax;			//
	UINT nSymbPos;			// 0x40 tagSDATSYMB
	UINT nSymbSize;			// tagSDATSYMB结构实际大小
	UINT nSDatInfoPos;		// tagSDATINFO
	UINT nSDatInfoSize;		// tagSDATINFO结构大小
	UINT nSDatFatPos;		// tagSDATFAT
	UINT nSDatFatSize;		// tagSDATFAT结构大小
	UINT nSDatFileInfoPos;	// tagSDATFILEINFO
	UINT nRecSize;			// 去掉文件头结构的文件大小 nSDATSize-nSDatFileInfoPos
	char unknown5[0x10];	// zero ?
};

// SYMB
struct SDATSYMB
{
	char falgs[4];			// 'S' 'Y' 'M' 'B'
	UINT nSize;				// tagSDATSYMB结构对齐大小
	UINT nSequences;
	UINT nSoundEffects;
	UINT nBanks;
	UINT nWaves;
	UINT nPlayers;
	UINT nGroups;
	UINT nStreams;
	UINT nUnknownMD;
	UINT unknown[8];
};

struct SDATSYMBREC
{
	UINT nCount;			// Rec Count
	UINT nSymbPos[1];
};
// _SYMB

// INFO
struct SDATINFO
{
	char falgs[4];			// 'I' 'N' 'F' 'O'
	UINT nSize;				// tagSDATINFO结构大小
	UINT nSequences;
	UINT nSoundEffects;
	UINT nBanks;
	UINT nWaves;
	UINT nPlayers;
	UINT nGroups;
	UINT nStreams;
	UINT nUnknownMD;
	UINT unknown[8];
};

struct SDATINFORECPOS
{
	UINT nCount;			// Rec Count
	UINT nInfoPos[1];
};

struct SDATINFOREC
{
	WORD wFileID;
	WORD wFileType;
	WORD wBnk;
	BYTE nVol;
	BYTE nVar1;
	BYTE nVar2;
	BYTE nVar3;
	BYTE nVar4;
	BYTE nVar5;
};
// _INFO

// FAT size:0x10
struct SDATFATREC
{
	UINT nPos;				// SoundPos
	UINT nSize;				// SoundSize
	char unknown[8];		// zero ?
};

struct SDATFAT
{
	char flags[4];			// 'F' 'A' 'T' ' '
	UINT nSize;				// tagSDATFAT结构大小
	UINT nFatCount;			// FatCount
	SDATFATREC rec[1];		// tagSDATFATREC...
};
// _FAT

// FILE
struct SDATFILEINFO
{
	char flags[4];			// 'F' 'I' 'L' 'E'
	UINT nSoundFileSize;	// 去掉文件头结构的文件大小 tagSDAT.nSDATSize-tagSDAT.nSDatFileInfoPos
	UINT nSoundCount;		// 曲目数量
	UINT unknown;			// zero ?
};
// _FILE

// FILEDATA
struct NDSFILEHEADER
{
	char falgs[4];			// 'S' 'S' 'E' 'Q' / 'S' 'T' 'R' 'M' / Other...
	WORD unknown1;			// 0xFEFF / 0xFFFE
	BYTE unknown2;			// 0x00 0x01 0x02.. ?
	BYTE unknown3;			// 0x00 0x01.. ?
	UINT nFileSize;			// FileSize
	WORD nSize;				// HEADER SIZE
	WORD nSubHeaderCount;	// ? 0x0001(NO HEAD) . 0x0002(SUBHEAD) ... count/type ?
	WORD nSize2;			// 由unknown2而定
};

struct SUBHEADER
{
	char falgs[4];			// 'H' 'E' 'A' 'D' / Other...
	UINT nSize;				// SUBHEADER结构大小
	// data... ?
};

struct SDATSOUNDDATA
{
	char flags[4];			// 'D' 'A' 'T' 'A'
	UINT nSize;				// SDATSOUNDDATA结构大小
	UINT nOffsetBase;		// 0x1C...
	// 声音数据...?
	/*
	1c statusByte
	1d velocity
	1e IF <0x80 ++ &0x7F<<7
	*/
};
// _FILEDATA

// CMAP
struct NDSCMAP
{
	char flags[4];
	UINT nSize;
	WORD nBeginCode;	// 开始编码(0000)
	WORD nEndCode;		// 结束编码(FFFF)
	WORD nType;			// =1 wChar有效 =0 nOffset＋wChar有效 ? =2 isTbl
	WORD unknown;		// zero?
	UINT nOffset;		// ?
	WORD wCount;
	struct
	{
		WORD wChar;
		WORD wIndex;	// TileOffset = TileBase + wIndex*TileSize
	}Tbl[1];
};
// _CMAP

// CWDH
struct NDSCWDH
{
	char flags[4];
	UINT nSize;
	WORD unknown1;
	WORD wCount;
	UINT unknown2;
	BYTE nWDH[1][3];		// L Width ? / Draw Char Width? / R Width ?
};
// _CWDH

// CGLP
struct NDSCGLP
{
	char flags[4];
	UINT nSize;
	BYTE nWidth;
	BYTE nHeight;
	BYTE nTileSize;
	BYTE unknown1;	// 00?
	BYTE unknown2;	// ?
	BYTE unknown3;	// ?
	BYTE nBits;		// bits?
};
// _CGLP

// LABL
struct NDSLABL
{
	char LABL[4];	// "LABL"
	UINT nSize;
	UINT nNamePos[1];// 标签名偏移
};
// _LABL

// CrystalTile Macro
/*
#define CT_MACRO_CLOSE	0
#define CT_MACRO_OPEN	1
#define CT_MACRO_PAUSE	2
#define CT_MACRO_VER	0x00000001

#define CT_MACRO_TYPE_NORMAL	0
#define CT_MACRO_TYPE_NDS		1
#define CT_MACRO_TYPE_ISO		2

enum CT_MACRO_NAME
{
	// 图片处理
	CT2PALETTE_LOADPAL,
	CT2PALETTE_LOADDEFPAL,
	CT2TILEVIEW_EDITIMPORT,
	CT2TILEVIEW_IMPROTBMP,
	CT2TILEEDITOR_TBL,
	CT2TILEEDITOR_SETTILE,
	CT2HEXVIEW_SETDATA,
	CT2HEXVIEW_IMPORTDATA,
	CT2HEXVIEW_IMPORTTEXT,
	CT2HEXVIEW_PAL2DATA,
	CT2HEXVIEW_DATA2PAL,
	CT2TILEVIEW_SETPIXEL,
	CT2SCRIPTVIEW_OPENSCRIPT,
	CT2SCRIPTVIEW_IMPORT,
	CT2DOC_LOADTBL,
	CT2TILEVIEW_EDITCOPY,
	CT2TILEVIEW_EDITPASTE,
	CT2NDSFSI_LOADPAL,
	CT2NDSFSI_SETNTFRWH,
	CT2NDSFSI_SETFILESIZE,
	CT2NDSFSI_IMPORT,
	CT2NDSFSI_CHECKCRC16,
	CT2NDSFSI_NDSINFO,
};

struct CT_MACRO_HEADER
{
	char	Flags[4];
	DWORD	dwSize;
	DWORD	dwVer;
	BYTE	nType;
};
struct CT_MACRO
{
	WORD	wSize;
	WORD	wActive;
};
struct CT_MACRO_1VAL_1FILE
{
	CT_MACRO	m;
	UINT		nVal;
	TCHAR		szFile[_MAX_PATH];
};
struct CT_MACRO_ONTBL
{
	CT_MACRO	m;
	UINT		nVal;
	UINT		nTextColor;
	UINT		nBkColor;
	UINT		nShadowColor;
	int			nAlignLeft;
	int			nAlignTop;
	RECT		rcText;
	BYTE		nTransparent;
	BYTE		nShadowType;
	POINT		ptPos;
	TCHAR		szFile[_MAX_PATH];
	LOGFONT		lfFont;
	STATEDATA	StateData;
};
struct CT_MACRO_CT2TILEVIEW_IMPROTBMP
{
	CT_MACRO	m;
	POINT		ptImport;
	STATEDATA	StateData;
};
struct CT_MACRO_CT2HEXVIEW_SETDATA
{
	CT_MACRO	m;
	UINT		nOffset;
	UINT		nData;
	BYTE		nSize;
	BYTE		nFlags;
};
struct CT_MACRO_CT2TILEVIEW_SETPIXEL
{
	CT_MACRO	m;
	UINT		nOffset;
	UINT		nPixel;
	POINT		ptPos;
	WORD		nTilePixelNO;
	STATEDATA	StateData;
};
struct CT_MACRO_NDS_IMPORTFILE
{
	CT_MACRO	m;
	UINT		nOffset;
	UINT		nSize;
	UINT		nNewOffset;
	WORD		wFileID;
	TCHAR		szFile[_MAX_PATH];
};
struct CT_MACRO_NDS_SETFILESIZE
{
	CT_MACRO	m;
	UINT		nNewSize;
	UINT		nNewOffset;
	WORD		wSub;
	WORD		wFileID;
};
struct CT_MACRO_NDS_NDSINFO
{
	CT_MACRO	m;
	TCHAR Info[0x100*7];
};
struct CT_MACRO_IMPORTTEXT
{
	CT_MACRO	m;
	UINT		nNewAddress;
	UINT		nCodePage;
	BYTE		nSwpCode;
	BYTE		nAlign2;
	BYTE		bNewAddress;
	BYTE		nNewCode;
	BYTE		nNoDefHint;
	BYTE		nSaveTbl;
	BYTE		nTblMode;
	BYTE		bRelativePoint;
};

#define INITMACRO(Macro, wActive)	InitMacro(&Macro, sizeof(Macro), wActive)
void InitMacro(void *pMacro, WORD wSize, WORD wActive);
*/
// WAVE
#include <mmsystem.h>
struct WAVEFILE
{
	char RIFF[4];		// 0x46464952
	UINT nFileSize;
	char WAVE[4];		// 0x45564157
	char fmt[4];		// 0x20746D66
	UINT nFmtSize;		// sizeof(WAVEFORMATEX)
};

struct WAVEDATA
{
	char data[4];		// 0x61746164
	UINT nWaveSize;
};

struct VISUALSCRIPT
{
	CString strCtrl;
	CString strVisual;
};

enum SFI_FILETYPE {SFI_UNKNOWN, SFI_SDAT, SFI_PAC, SFI_UTILITY_BIN, SFI_NARC};

// PAC FILE
#define PAC_FILEALIGN	0x200

struct PACFILE_HEADER
{
	WORD nSize;
};
struct PACFILE_FNT
{
	BYTE	nFNTCount;	// nIndex*0x200 每个FAT块0x200字节
	BYTE	nFileNameLen;
	BYTE	nUnknown2;
	char	FileName[1];
};
struct PACFILE_FAT_REC
{
	UINT nOffset;
	UINT nSize;
};
struct PACFILE_FAT
{
	WORD wUnknown1;
	WORD wFileCount;
	UINT nUnknown2;
	//PACFILE_FAT_REC rec[1];
};

struct UTITLITY_HEADER
{
	UINT	nFntOffset;
	UINT	nFntSize;
	UINT	nFatOffset;
	UINT	nFatSize;
};

struct NINTENDOFILE_HEADER
{
	char HeaderName[4];
	WORD unknown1;
	WORD unknown2;
	UINT nFileSize;
	WORD nSize;
	WORD wChunkMax;
};

struct NARCFILE_FAT
{
	char FAT[4];
	UINT nSize;
	UINT nFileCount;
	UINT nFat_Offset;
};

struct NARCFILE_FNT
{
	char FNT[4];
	UINT nSize;
	UINT nFnt_Offset;
};

struct NARCFILE_FIMG
{
	char FIMG[4];
	UINT nSize;
};

struct NANRFILE_ABNK
{
	char ABNK[4];
	UINT nSize;
	WORD wNumOfAni;
	WORD wTotalFrames;
	UINT unknown1;
	UINT unknown2;
	UINT unknown3;
	UINT unknown4;
	UINT unknown5;
	BYTE ab[3][16];
	UINT nFrames;
	WORD unknown6;
	WORD unknown7;
	UINT unknown8;
	UINT nFrameOffset;
};

struct NANRFILE_UEXT
{
	char UEXT[4];
	UINT nSize;
	UINT unknown;
};

struct NCERFILE_CEBK
{
	char CEBK[4];
	UINT nSize;
	WORD wCells;
	WORD unknown;
	UINT unknown2;
	UINT unknown3;
	UINT unknown4;
	UINT unknown5;
	UINT unknown6;

};

struct SBNKFILE_DATA
{
	char DATA[4];
	UINT nSize;
	UINT unknown[8];
	UINT nSamples;
	BYTE unknown2;
	BYTE nDataOffset[3];
};

struct SWARFILE_HEADER
{
	char SWAR[4];
	UINT unknown;
	UINT nFileSize;
	WORD nSize;
	WORD wChunkMax;
};

struct SWARFILE_DATA
{
	char DATA[4];
	UINT nSize;
	UINT unknown[8];
	UINT nWaves;
	UINT nWaveOffset;
};

struct SWARFILE_WAVE
{
	BYTE nWaveType;	// 0=PCM8 1=PCM16 2=ADPCM
	BYTE nLoop;		// 0=no loop 1=loop;
	WORD nSampleRate;
	WORD nSampleTime;
	WORD nLoopStart;
	UINT nLoopLength;
	BYTE Data[1];
};

// WAVE
class CWaveFile : public CFile
{
public:
	WAVEFILE m_wf;
	WAVEFORMATEX m_wfx;
	WAVEDATA m_wd;

	CWaveFile();
	virtual ~CWaveFile();

	BOOL Open(CString wavename);
	BOOL AddSound(BYTE *pWav, UINT nSize);
	void Close();
};
/*
// 动态数据交换
class CT2DataExchange
{
public:
	BOOL m_bSaveAndValidate;   // TRUE => save and validate data
	CWnd* m_pDlgWnd;           // container usually a dialog
	BOOL m_bUpdate;
	CT2DataExchange(CWnd* pDlgWnd, BOOL bSaveAndValidate);
	CT2DataExchange(CT2DataExchange& dx);

	void CT2_DDX_Text(CT2DataExchange& dx, UINT nIDC, BYTE& Byte, BOOL bHex=FALSE, BYTE nMin=0, BYTE nMax=0);
	void CT2_DDX_Text(CT2DataExchange& dx, UINT nIDC, WORD& wWord, BOOL bHex=FALSE, WORD nMin=0, WORD nMax=0);
	void CT2_DDX_Text(CT2DataExchange& dx, UINT nIDC, UINT& nUint, BOOL bHex=FALSE, UINT nMin=0, UINT nMax=0);
	void CT2_DDX_Text(CT2DataExchange& dx, UINT nIDC, CString& strText);
	void CT2_DDX_CBIndex(CT2DataExchange& dx, UINT nIDC, BYTE& nIndex);
	//void CT2_DDX_TBCheck(CT2DataExchange& dx, UINT nIDC, BYTE nCheck);
};
*/

// 宏
#define FSI_ICON_UNK	0
#define FSI_ICON_TILE	1
#define FSI_ICON_SOUND	2
#define FSI_ICON_ANIME	3
#define FSI_ICON_NDS	4
#define FSI_ICON_PAL	5
#define FSI_ICON_TEXT	6
#define FSI_ICON_MAP	7
#define FSI_ICON_LZ		8
#define FSI_ICON_HAN	9
#define FSI_ICON_OT		10
#define FSI_ICON_CT		11

// 代码页 CodePage
// HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Nls\CodePage
// HKEY_LOCAL_MACHINE\SOFTWARE\Classes\MIME\Database\Codepage
#define CP_UNICODE				1200	// Unicode
#define CP_UNICODE_BIG_ENDIAN	1201	// Unicode Big Endian
#define CP_UNICODE_UTF8			65001	// Unicode UTF 8
#define CP_SHIFTJIS				932		// ANSI/OEM	日文 Shift-JIS
#define CP_HZGB2312				52936	// HZ-GB2312简体中文
#define CP_GB18030				54936	// GB18030	简体中文
#define CP_GBK					936		// ANSI/OEM	简体中文 GBK
#define CP_BIG5					950		// ANSI/OEM	繁体中文 GBK

// 文档类型
#define CT2_RT_NORMAL	0
#define CT2_RT_GBA		1
#define CT2_RT_NDS		2
#define CT2_RT_N64		3
#define CT2_RT_PCMEM	4
// 多文件
#define CT2_RT_MFILE	4
// MAP文件
#define CT2_RT_NSCR		5

// 系统编码
#define CT2_TBL_SYS		0
// 使用用户码表
#define CT2_TBL_USERTBL	1
// 系统编码使用用户自定义控制符
#define CT2_TBL_USERCTL	2

#define WM_OPENFILE ((WM_USER)+10)
#define GetShiftJIS(code) {\
					BYTE nLow=(BYTE)code;\
					if( (code>=0x8140) && (code<=0xEAA4) && \
						(nLow>=0x40) && (nLow<=0xFC) && (nLow!=0x7F) ) \
					{\
						int code2=(code>>8)&0x7F;\
						if(code<=0x9FFC)\
						{\
							code-=0x8140;\
							if(code>0x7E)\
								code-=code2-1;\
							code-=(code2-1)*0x43;\
							if(nLow>0x7E) code--;\
							code = *(theApp.m_pSJisTbl+wCode);\
						}else\
						if(code>=0xE040)\
						{\
							code-=0xC97C;\
							if(code>0x7E)\
								code-=code2-0x60;\
							code-=(code2-0x60)*0x43;\
							if(nLow>0x7E) code--;\
							code = *(theApp.m_pSJisTbl+wCode);\
						}else\
							code=0;\
						if(code==0x30FB) code=0;\
					}else\
						code=0;\
					}
// 字符转换
#define mcharTowchar(lpWchar, lpMchar, nCodePage) MultiByteToWideChar\
	(nCodePage, 0, (LPCSTR)(lpMchar), -1, (LPWSTR)(lpWchar), 1)

#define sFindStr			_tcsstr
#define sFindChar			_tcschr
#define sReverseFindChar	_tcsrchr
#define sCompareNoCase		_tcsicmp
#define sCompare			_twcscmp
#define sMakeLower			_tcslwr
#define sMakeUpper			_wcsupr

#define SFIMAKEPARAM(nFileID, iItem) (MAKELONG(nFileID, iItem))

// DrawMode
#define CT2_DM_TILE 0
#define CT2_DM_OBJH 1
#define CT2_DM_OBJV 2
#define CT2_DM_MAP	3
#define CT2_DM_OAM	4

// TileFormat
// #define TF_FIRST	0
// #define TF_1BPP		0	// 单色 1bpp
// #define TF_NDS1BPP	1	// NDS  1bpp
// #define TF_GB2BPP	2	// GB   2bpp
// #define TF_4C2BPP	3	// 4色  2bpp
// #define TF_VB2BPP	4	// VB   2bpp
// #define TF_NGP2BPP	5	// NGP  2bpp
// #define TF_GBA4BPP	6	// GBA  4bpp
// #define TF_N644BPP	7	// N64  4bpp
// #define TF_GBA24BPP	8	// GBA2 4bpp
// #define TF_GBA8BPP	9	// GBA  8bpp
// #define TF_SNES4BPP	10	// SNES 4bpp
// #define TF_NES2BPP	11	// NES  2bpp
// #define TF_SMS4BPP	12	// SMS  4bpp
// #define TF_GBA3XBPP	13	// GBA3 Xbpp 不支持Obj
// #define TF_CT0XBPP	14	// CT0  Xbpp 不支持Obj
// #define TF_RGBA16BPP	15	// 16位位图
// #define TF_RGB24BPP	16	// 24位位图
// #define TF_RGBA32BPP	17	// 32位位图
// #define TF_RHYTHM	18	// 旋律天国
// #define TF_LAST		18

#define TF_FIRST	0
#define TF_1BPP		0	// 单色 1bpp
#define TF_NDS1BPP	1	// NDS  1bpp

#define TF_4C2BPP	2	// 4色  2bpp
#define TF_VB2BPP	3	// VB   2bpp
#define TF_GB2BPP	4	// GB   2bpp
#define TF_NES2BPP	5	// NES  2bpp
#define TF_NGP2BPP	6	// NGP  2bpp

#define TF_8C3BPP	7	// 8色  2bpp
// #define TF_8CR3BPP	8	// 8色R  2bpp

#define TF_N644BPP	8	// N64  4bpp
#define TF_GBA4BPP	9	// GBA  4bpp
#define TF_GBA24BPP	10	// GBA2 4bpp
#define TF_SNES4BPP	11	// SNES 4bpp
#define TF_SMS4BPP	12	// SMS  4bpp

#define TF_GBA8BPP	13	// GBA  8bpp

#define TF_GBA3XBPP	14	// GBA3 Xbpp 不支持Obj
#define TF_CT0XBPP	15	// CT0  Xbpp 不支持Obj

#define TF_RGBA16BPP	16	// 16位位图
//#define TF_BGRA16BPP	17	// 16位位图
#define TF_RGB24BPP	17	// 24位位图
#define TF_RGBA32BPP	18	// 32位位图
//#define TF_ABGR32BPP	20	// 32位位图
#define TF_RHYTHM	19	// 旋律天国
#define TF_LAST		19

#define ColorToWord(color) ((WORD)((((color)&0xF8)>>3) |\
							(((color)&0xF800)>>6) |\
							(((color)&0xF80000)>>9) |\
							(((~(color>>24)+1)&1)<<15) ))

#define WordToColor(wcolor) ((((wcolor)&0x1F)<<3) |\
							(((wcolor)&0x03E0)<<6) |\
							(((wcolor)&0x7C00)<<9) |\
							((~(wcolor>>15)+1)<<24) )

#define WordToRgbQuad(wcolor) ((((wcolor)&0x1F)<<19) |\
							(((wcolor)&0x03E0)<<6) |\
							(((wcolor)&0x7C00)>>7) |\
							((~(wcolor>>15)+1)<<24) )

#define RgbQuadToWord(rgb) ((((rgb)&0xF8)<<7) |\
							(((rgb)&0xF800)>>6) |\
							(((rgb)&0xF80000)>>19) |\
							(((~(rgb>>24)+1)&1)<<15) )

#define ColorToRgbQuad(color) ((((color)&0xFF)<<16) |\
								((color)>>16)&0xFF |\
								((color)&0xFF00) |\
								((color)&0xFF000000) )

#define RgbQuadToColor(rgb) ((((rgb)&0xFF)<<16) |\
							((rgb)>>16)&0xFF |\
							((rgb)&0xFF00) |\
							((rgb)&0xFF000000) )
// 类
class CHyperlink :
	public CStatic
{
protected:
	virtual void PreSubclassWindow();
public:
	CHyperlink(void);
	virtual ~CHyperlink(void);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnStnClicked();
	CFont m_Font;
	CString m_strLink;
	HCURSOR m_hCursor;
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
};

class CVerInfo
{
public:
	CVerInfo();
	virtual ~CVerInfo();
	//BYTE *VerData;
	CString ProductName;
	CString FileVersion;
	CString LegalCopyright;
	CString PrivateBuild;
};

// 函数

CString LoadStr(UINT nID);

void ShowError();

BOOL exeProgram(CString lpProgram, CString lpParameters);

BOOL Lz77uncomp(BYTE* pRes, BYTE** pDst, int& nLzSize, int& nBinSize);
BOOL Lz77uncomp(BYTE* pRes, BYTE* pDst, int& nLzSize, int& nBinSize);
// 字符串转换为数值
int StrToIntEX(CString& strhex, BOOL bHex=TRUE, BOOL bFix=FALSE);
//  提示
int Hint(UINT nIDS, UINT nType=MB_OK|MB_ICONINFORMATION);
int Hint(CString strHint, UINT nType=MB_OK|MB_ICONINFORMATION);
// 更新菜单
void OnInitMenuPopup(CWnd* pTarget, HMENU hMenu, BOOL bDisableIfNoHndler = TRUE);
// 取路径
CString GetPath(HWND hWnd);

void EnumSystemCodePages(UINT **pCodePage, UINT &nCount);
CString GetCodePageDescription(UINT nCodePage, CString *lpstrFontName=NULL);

// 载入内存图片
LPPICTURE LoadPic(HANDLE hHandle, UINT nSize, BOOL bLoadAndFree);
HANDLE LoadRes(UINT nIDR, DWORD &dwSize, LPCTSTR lpType=NULL);

//如果uFlages=SHGFI_SMALLICON|SHGFI_LARGEICON，图标将是小/大图标。
BOOL LoadSystemImageList(HIMAGELIST& hImages, UINT uFlags = SHGFI_SMALLICON);

//如果uFlages=SHGFI_OPENICON|SHGFI_SMALLICON|SHGFI_LARGEICON，目录图标将是打开的。小/大图标。
int GetIconIndex(LPCTSTR pszPath, UINT uFlags = SHGFI_SMALLICON);

int GetMenuItemIndex(HMENU hMenu, UINT nID);

//CString EnumFontFamilies();

LPWSTR GetUText(CString strPathName, UINT nCodePage);

UINT GetAsmCode(CString strAsm, UINT nCurOffset, BYTE &nSize, BYTE nThumb=0);

void CreateImageList();

int GetSFIIconIndex(CString name, BYTE *pFile=NULL);

UINT SFIExtractDirectory(CString& ParentDir, UINT nID, BYTE *pRom, UINT nFnt_Offset, UINT nFat_Offset, CListCtrl& FileList);

void GetTempFileName(LPTSTR pTempFileName);

int GetListImage(CListCtrl &lc, int iItem);

#endif // CT2_COMMON_H