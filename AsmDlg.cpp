// AsmDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CrystalTile2.h"
#include "AsmDlg.h"
#include ".\asmdlg.h"

// CAsmHint

CAsmHint::CAsmHint()
	: m_strCompSrc(_T(""))
{
}

void CAsmHint::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	CString strHint, strText;
	GetWindowText(strHint);
	strHint.Replace(_T("\t"), _T("\n"));
	CDC dc;
	dc.Attach(lpDIS->hDC);
	dc.FillRect(&lpDIS->rcItem, &CBrush(::GetSysColor(COLOR_BTNFACE)));

	int x=lpDIS->rcItem.left;
	int y=lpDIS->rcItem.top;
	int nLen = m_strCompSrc.GetLength();

	CString strSrc;
	int nPos = m_strCompSrc.Find(_T(' '));
	if(nPos!=-1)
		strSrc = m_strCompSrc.Left(nPos);
	else
		strSrc = m_strCompSrc;
	strSrc.TrimLeft();
	strSrc.TrimRight();

	nPos = 0;
	CSize sz = dc.GetTextExtent(_T(" "));
	int nTab = 10*sz.cx;
	while(TRUE)
	{
		if(x>=lpDIS->rcItem.right)
		{
			x = lpDIS->rcItem.left;
			y += sz.cy;
		}

		AfxExtractSubString(strText, strHint, nPos++);
		if(strText.IsEmpty()) break;

		CString str = strText.Left(nLen);

		if(lstrcmp(str, strSrc)==0)
			dc.SetTextColor(0xFF0000);
		else
			dc.SetTextColor(0x00);

		dc.TextOut(x, y, strText);
		sz = dc.GetTextExtent(strText);
		x += sz.cx + nTab-sz.cx;
	}

	dc.Detach();
}


// CAsmDlg 对话框

IMPLEMENT_DYNAMIC(CAsmDlg, CDialog)
CAsmDlg::CAsmDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAsmDlg::IDD, pParent)
	, m_strAsmHint(_T(""))
	, m_nType(0)
	, m_strAsm(_T(""))
	, m_nCurOffset(0)
	, m_strAsmDesc(_T(""))
{
}

CAsmDlg::~CAsmDlg()
{
}

void CAsmDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ASMCB, m_AsmCB);
	DDX_Text(pDX, IDC_ASMHINT, m_strAsmHint);
	DDX_CBString(pDX, IDC_ASMCB, m_strAsm);
	DDX_Text(pDX, IDC_ASMDESC, m_strAsmDesc);
}


BEGIN_MESSAGE_MAP(CAsmDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_CBN_EDITCHANGE(IDC_ASMCB, OnCbnEditchangeAsmcb)
END_MESSAGE_MAP()

// CAsmDlg 消息处理程序

void CAsmDlg::OnBnClickedOk()
{
	UpdateData();

	BYTE nThumb = m_nType&1;
	m_nType = GetAsmCode(m_strAsm, m_nCurOffset, m_nSize, nThumb);

	if(m_strAsmList.Find(m_strAsm, 0)==-1)
	{
		CString sasm;
		sasm.Format(_T("\n%s"), m_strAsm);
		m_strAsmList.Insert(0, sasm);
	}

	if(!m_nSize)
	{
		Hint(IDS_ASMERROR, MB_OK|MB_ICONERROR);
		OnCancel();
	}else
		OnOK();
}

void CAsmDlg::OnCbnEditchangeAsmcb()
{
	UpdateData();
	//if(m_strAsm.GetLength()>1) return;

	CString strAsm = m_strAsm;
	strAsm.MakeLower();
	strAsm.TrimLeft();
	switch(strAsm.GetAt(0))
	{
	case 'a':
		m_strAsmHint = _T("adc\tadd\tand\tasr");
		break;

	case 'b':
		m_strAsmHint = _T("b\tbeq\tbne\tbcs\tbcc\tbmi\tbpl\tbvs\tbvc\tbhi\tbls\tbge\tblt\tbgt\tble\tbic\tbl\tbx");
		break;

	case 'c':
		m_strAsmHint = _T("cdp\tcmn\tcmp");
		break;

	case 'e':
		m_strAsmHint = _T("eor");
		break;

	case 'l':
		m_strAsmHint = _T("ldc\tldm\tldmia\tldr\tldrb\tldrh\tldrsb\tldrsh\tlsl\tlsr");
		break;

	case 'm':
		m_strAsmHint = _T("mcr\tmla\tmov\tmrc\tmrs\tmsr\tmul\tmvn");
		break;

	case 'n':
		m_strAsmHint = _T("nop");
		break;

	case 'o':
		m_strAsmHint = _T("orr");
		break;

	case 'r':
		m_strAsmHint = _T("ror\trrx\trsb\trsc");
		break;

	case 's':
		m_strAsmHint = _T("sbc\tsmlal\tsmull\tstc\tstm\tstmia\tstr\tstrb\tstrh\tstrsb\tstrsh\tsub\tswi\tswp");
		break;

	case 't':
		m_strAsmHint = _T("teq\ttst");
		break;

	case 'u':
		m_strAsmHint = _T("umlal\tumull");
		break;
	default:
		m_strAsmHint.Empty();
		break;
	}

	m_AsmHint.m_strCompSrc = m_strAsm;
	GetDlgItem(IDC_ASMHINT)->SetWindowText(m_strAsmHint);
	UpdateAsmDesc();
}

BOOL CAsmDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_AsmHint.SubclassDlgItem(IDC_ASMHINT, this);
	m_AsmHint.ModifyStyle(0, SS_OWNERDRAW);

	m_AsmCB.ResetContent();
	CString t;
	int i=1;
	while(TRUE)
	{
		AfxExtractSubString(t, m_strAsmList, i++);
		if(t.IsEmpty()) break;
		m_AsmCB.AddString(t);
	}

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

struct ASMDESC
{
	char* lpAsm;
	char* lpDesc;
};

// 汇编指令格式帮助
ASMDESC g_AsmDesc[] =
{
	{ "adc", "adc{cond}{s} Rd, Rn\nadc{cond}{s} Rd, Rn, Op2\nadc Rd,Rs\n带进位的加法" },
	{ "add", "add{cond}{s} Rd, Rn\tadd{cond}{s} Rd, Rn, Op2\nadd Rd,Rs,Imm3bit\tadd Rd,Imm8bit\nadd Rd,Rs,Rn\t\tadd R0-14,R8-15\nadd R8-14,R0-15\tadd R15,R0-15\nadd Rd,PC,Imm8bit*4\tadd Rd,SP,Imm8bit*4\nadd Rd,SP,Imm7bit*4\tadd Rd,SP,-Imm7bit*4" },
	{ "and", "and{cond}{s} Rd, Rn\nand{cond}{s} Rd, Rn, Op2\nand Rd,Rs" },
	{ "asr", "asr Rd, Rs\nasr Rd, Rs, shift\nRd/Rs = R0-R7\nshift = 0-31\n算术右移" },
	{ "b", "b{cond} offset\nAsm\toffset = -32M..+32M (step:4)\nThumb\toffset = $+4-256..$+4+254 (step:2)" },
	{ "bls", "无符号小于" },
	{ "ble", "有符号小于等于" },
	{ "blt", "有符号小于" },
	{ "beq", "等于" },
	{ "bge", "有符号大于等于" },
	{ "bgt", "有符号大于" },
	{ "bhi", "无符号大于" },
	{ "bcs", "无符合大于等于" },
	{ "bcc", "无符合小于" },
	{ "bmi", "无符合小于" },
	{ "bpl", "无符合大于等于" },
	{ "bne", "不等" },
	{ "bic", "bic{cond}{s} Rd, Rn, Op2\nbic Rd,Rs" },
	{ "bl", "bl{cond} offset\nAsm\toffset = -32M..+32M (step:4)\nThumb\toffset = (PC+4)-400000h..+3FFFFEh (step:2)" },
	{ "bx", "bx{cond} Rs" },
	{ "cmn", "cmn{cond} Rn, Op2\ncmn Rd,Rs" },
	{ "cmp", "cmp{cond} Rn, Op2\ncmp Rd,Rs\ncmp R0-15,R8-15\ncmp R8-15,R0-15" },
	{ "eor", "eor{cond}{s} Rd, Rn, Op2\neor Rd, Rs" },
	{ "ldc", "ldc{cond}{l} p#,cd,<Address>" },
	{ "ldm", "ldm{cond}{amod} Rn{!},<Rlist>{^}" },
	{ "ldr", "ldr{cond}[h|sb|sh] Rd,<Address>\nldr Rd,[Rb,5bit*4]\tldr Rd,[PC,8bit*4]\nldr Rd,[SP,8bit*4]\tldr Rd,[Rb,Ro]" },
	{ "ldrb", "ldrb Rd,[Rb,5bit*1]\nldrb Rd,[Rb,Ro]" },
	{ "ldrh", "ldrh Rd,[Rb,5bit*2]\nldrh Rd,[Rb,Ro]" },
	{ "ldsb", "ldsb Rd,[Rb,Ro]" },
	{ "ldsh", "ldsh Rd,[Rb,Ro]" },
	{ "lsl", "lsl Rd, Rs\nlsl Rd, Rs, Imm5bit\n逻辑左移" },
	{ "lsr", "lsr Rd, Rs\nlsr Rd, Rs, Imm5bit\n逻辑右移" },
	{ "mcr", "mcr{cond} p#,<cpopc>,Rd,cn,cm{,<cp>}" },
	{ "mla", "mla{cond}{s} Rd,Rm,Rs,Rn" },
	{ "mov", "mov{cond}{s} Rd,Op2\nmov Rd,Imm8bit\nmov Rd,Rs\nmov R0-14,R8-15\nmov R8-14,R0-15\nmov R15,R0-15" },
	{ "mrc", "mrc{cond} p#,<cpopc>,Rd,cn,cm{,<cp>}" },
	{ "mrs", "mrs{cond} Rd,Psr" },
	{ "msr", "msr{cond} Psr{_field},Op" },
	{ "mul", "mul{cond}{s} Rd,Rm,Rs\nmul Rd,Rs" },
	{ "mvn", "mvn{cond}{s} Rd,Op2\nmvn Rd,Rs" },
	{ "neg", "neg Rd,Rs" },
	{ "nop", "nop\n空指令" },
	{ "orr", "orr{cond}{s} Rd,Rn,Op2\norr Rd,Rs" },
	{ "ror", "ror Rd,Rs\n循环右移" },
	{ "rsb", "rsb{cond}{s} Rd,Rn,Op2\n反向减法" },
	{ "rsc", "rsc{cond}{s} Rd,Rn,Op2\n带借位的反向减法" },
	{ "sbc", "sbc{cond}{s} Rd,Rn,Op2\nsbc Rd,Rs\n带借位的减法" },
	{ "smlal", "smlal{cond}{s} RdLo,RdHi,Rm,Rs" },
	{ "smull", "smull{cond}{s} RdLo,RdHi,Rm,Rs" },
	{ "stc", "stc{cond}{l} p#,cd,<Rs>" },
	{ "stm", "stm{cond}{amod} Rn{!},<Rlist>{^}" },
	{ "str", "str{cond}[b]<t> Rd,[Rs],shift\nstr{cond}[b] Rd,[Rs][!]\nstr{cond}[b|h] Rd,[Rs]\nstr Rd,[Rs]\nstr Rd,[Rb,5bit*4]\tstr Rd,[SP,8bit*4]\nstr Rd,[Rb,Ro]" },
	{ "sub", "sub{cond}{s} Rd,Rn,Op2\nsub Rd,Rs,Imm3Bit\nsub Rd,Imm8bit\nsub Rd,Rs,Rn" },
	{ "swi", "swi{cond} Imm24bit\nswi Imm8bit" },
	{ "swp", "swp{cond}{b} Rd,Rm,[Rn]" },
	{ "teq", "teq{cond} Rn,Op2\n测试等价" },
	{ "tst", "tst{cond} Rn,Op2\ntst Rd,Rs\n测试位" },
	{ "umlal", "umlal{cond}{s} RdLo,RdHi,Rm,Rs" },
	{ "umull", "umull{cond}{s} RdLo,RdHi,Rm,Rs" },
	{ NULL, NULL }
};

void CAsmDlg::UpdateAsmDesc()
{
	CString strSrc = m_strAsm, strHint = m_strAsmHint;
	strHint.Replace(_T("\t"), _T("\n"));

	int i = strSrc.Find(_T(' '));
	if(i!=-1)
		strSrc = strSrc.Left(i);
	i = 0;
	CString strAsm;
	m_strAsmDesc.Empty();
	while(TRUE)
	{
		AfxExtractSubString(strAsm, strHint, i++);
		if(strAsm.IsEmpty()) break;

		if(strAsm.CompareNoCase(strSrc)==0)
		{
			m_strAsmDesc = _T("OK");
			char Asm[8];
			::CharToOem(strAsm, Asm);
			i = 0;
			while(g_AsmDesc[i].lpAsm)
			{
				if( _mbsicmp((const unsigned char*)g_AsmDesc[i].lpAsm,
					(const unsigned char*)Asm) == 0 )
				{
					::OemToChar(g_AsmDesc[i].lpDesc, m_strAsmDesc.GetBuffer(_MAX_PATH));
					m_strAsmDesc.ReleaseBuffer();
					break;
				}
				i++;
			}
			break;
		}
	}

	SetDlgItemText(IDC_ASMDESC, m_strAsmDesc);
	//DWORD dwSel = m_AsmCB.GetEditSel();
	//UpdateData(FALSE);
	//m_AsmCB.SetEditSel(dwSel&0xFFFF, dwSel>>16);
}
