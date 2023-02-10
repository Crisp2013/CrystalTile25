// CT2ScriptView.cpp : CT2ScriptView 类的实现
//

#include "stdafx.h"
#include "CrystalTile2.h"

#include "CT2Doc.h"
#include ".\CT2ScriptView.h"
#include "TextFileDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CT2ScriptView

IMPLEMENT_DYNCREATE(CT2ScriptView, CT2View)

BEGIN_MESSAGE_MAP(CT2ScriptView, CT2View)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_COMMAND_RANGE(ID_SEARCH_TEXT, ID_SEARCH_TEXTEX, OnSearchText)
	ON_NOTIFY(LVN_ITEMACTIVATE, IDC_FSILIST, OnLvnItemActivateFsilist)
	ON_COMMAND(IDC_HEXVIEW_TBL, OnTblMode)
	ON_UPDATE_COMMAND_UI(IDC_HEXVIEW_TBL, OnUpdateTblMode)
	ON_COMMAND(ID_HEXVIEW_SWPCODE, OnSwpcode)
	ON_UPDATE_COMMAND_UI(ID_HEXVIEW_SWPCODE, OnUpdateSwpcode)
	ON_COMMAND(ID_EDIT_COPY, OnScriptCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateScriptCopy)
	ON_COMMAND(ID_EDIT_DELETE, OnScriptviewDelete)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, OnUpdateEditDelete)
	ON_COMMAND(ID_EDIT_EXPORT, OnEditExport)
	ON_UPDATE_COMMAND_UI(ID_EDIT_EXPORT, OnUpdateEditExport)
	ON_COMMAND(ID_SCRIPTVIEW_FINDPOINT, OnScriptviewFindpoint)
	ON_UPDATE_COMMAND_UI(ID_SCRIPTVIEW_FINDPOINT, OnUpdateEditExport)
	ON_COMMAND(ID_SCRIPTVIEW_INVALIDATEDELETE, OnScriptviewInvalidatedelete)
	ON_UPDATE_COMMAND_UI(ID_SCRIPTVIEW_INVALIDATEDELETE, OnUpdateEditExport)
	ON_COMMAND(ID_SEARCH_ALIGN2, OnSearchAlign2)
	ON_UPDATE_COMMAND_UI(ID_SEARCH_ALIGN2, OnUpdateSearchAlign2)
	ON_COMMAND(ID_SEARCH_NOWORD, OnSearchNoword)
	ON_UPDATE_COMMAND_UI(ID_SEARCH_NOWORD, OnUpdateSearchNoword)
	ON_COMMAND(ID_EDIT_IMPORT, OnEditImport)
	ON_UPDATE_COMMAND_UI(ID_EDIT_IMPORT, OnUpdateEditImport)
	ON_COMMAND(ID_SCRIPTVIEW_OPENSCRIPT, OnScriptviewOpenscript)
	ON_UPDATE_COMMAND_UI(IDC_EDIT_FINDTEXT, OnUpdateViewScriptFind)
	ON_COMMAND(ID_EDIT_FIND, OnScriptFind)
	ON_UPDATE_COMMAND_UI(ID_EDIT_FIND, OnUpdateViewScriptFind)
	ON_COMMAND(ID_SCRIPTVIEW_FILTERPOINT, OnScriptviewFilterpoint)
	ON_COMMAND(ID_SCRIPTVIEW_FILTERSITEM, OnScriptviewFiltersitem)
	ON_COMMAND(ID_SCRIPTVIEW_FILTERDITEM, OnScriptviewFilterditem)
	ON_COMMAND(ID_SCRIPTVIEW_FILTERMIN, OnScriptviewFiltermin)
	ON_COMMAND(ID_SCRIPTVIEW_FILTERMAX, OnScriptviewFiltermax)
	ON_COMMAND(ID_SCRIPTVIEW_FILTERSADDRESS, OnScriptviewFiltersaddress)
	ON_COMMAND(ID_SCRIPTVIEW_FILTERDADDRESS, OnScriptviewFilterdaddress)
	ON_COMMAND(ID_SCRIPTVIEW_FILTERCHECKWORD, OnScriptviewFiltercheckword)
	ON_COMMAND(ID_SCRIPTVIEW_FILTERSPECWORD, OnScriptviewFilterspecword)
	ON_COMMAND(ID_SCRIPTVIEW_FILTERACHECK, OnScriptviewFilteracheck)
	ON_COMMAND(ID_SCRIPTVIEW_CHECKALL, OnScriptviewCheckall)
	ON_COMMAND(ID_SCRIPTVIEW_FILTERUNCHECK, OnScriptviewFilteruncheck)
	ON_COMMAND(ID_SCRIPTVIEW_DELCHECKITEM, OnScriptviewDelcheckitem)
	ON_COMMAND(ID_SCRIPTVIEW_EXPORTCHECKITEM, OnScriptviewExportcheckitem)
	ON_UPDATE_COMMAND_UI_RANGE(ID_SCRIPTVIEW_FILTERPOINT, ID_SCRIPTVIEW_EXPORTCHECKITEM, OnUpdateEditExport)
	ON_COMMAND(IDC_EDIT_FINDTEXT, OnEditFindtext)
	ON_COMMAND(ID_SEARCH_CTRLENDONLY, OnSearchCtrlendonly)
	ON_UPDATE_COMMAND_UI(ID_SEARCH_CTRLENDONLY, OnUpdateSearchCtrlendonly)
	ON_COMMAND(ID_TBL_TOTALWORD, OnTblTotalword)
	ON_UPDATE_COMMAND_UI(ID_TBL_TOTALWORD, OnUpdateEditExport)
	ON_WM_SETFOCUS()
	ON_COMMAND(IDC_HEXVIEW_TBLCTRL, OnScriptviewTblctrl)
	ON_UPDATE_COMMAND_UI(IDC_HEXVIEW_TBLCTRL, OnUpdateScriptviewTblctrl)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_COMMAND(ID_EDIT_EXPORT_EX, OnEditExportEx)
	ON_UPDATE_COMMAND_UI(ID_EDIT_EXPORT_EX, OnUpdateEditExport)
END_MESSAGE_MAP()

// CT2ScriptView 构造/析构

CT2ScriptView::CT2ScriptView()
: m_nTblMode(0)
, m_nBegin(0)
, m_nEnd(0)
, m_nMinLen(1)
, m_nMaxLen(250)
, m_nSwpCode(0)
, m_nBaseAddress(0x08000000)
, m_nPointBegin(0)
, m_nPointEnd(0)
, m_nAlign2(0)
, m_nNoWord(0)
, m_nNewAddress(0)
, m_nCtrlEndOnly(0)
, m_bNewAddress(0)
, m_bRelativePoint(0)
{
}

CT2ScriptView::~CT2ScriptView()
{
}

// CT2ScriptView 消息处理程序

void CT2ScriptView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
}

void CT2ScriptView::OnUpdateData()
{
	if(m_nBegin>m_nEnd)
	{
		UINT tmp=m_nBegin;
		m_nBegin = m_nEnd;
		m_nEnd=tmp;
	}

	CT2View::OnUpdateData();
}

void CT2ScriptView::OnInitialUpdate()
{
	CT2View::OnInitialUpdate();

}

void CT2ScriptView::OnSize(UINT nType, int cx, int cy)
{
	CT2View::OnSize(nType, cx, cy);

	m_ScriptList.SetWindowPos(NULL,
		0, 0,
		cx, cy,
		SWP_NOMOVE);
}

int CT2ScriptView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CT2View::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_ScriptList.Create(WS_CHILD|WS_VISIBLE|
		LVS_REPORT|LVS_SHOWSELALWAYS|LVS_NOSORTHEADER,
		CRect(0,0,0,0), this, IDC_FSILIST);
	m_ScriptList.SetExtendedStyle(m_ScriptList.GetExtendedStyle()|
		LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_CHECKBOXES|LVS_EX_INFOTIP);

	BYTE nWidth[] = {78, 62, 36, 255};
	BYTE nFormat[]={LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_RIGHT, LVCFMT_LEFT};
	CString Header, text;
	Header.LoadString(IDS_SCRIPTVIEW_HEADER);
	for(int c=0; c<4; c++)
	{
		AfxExtractSubString(text, Header, c);
		m_ScriptList.InsertColumn(c, text, 
			nFormat[c], nWidth[c]);
	}

	return 0;
}

struct FINDPOINT {
	CT2StatusBar *pPro;
	CT2ScriptView *pView;
	CListCtrl *pSL;
	BOOL bSuper;
	UINT nSize;
	BYTE *pRom;
};

UINT FindPoint(LPVOID lpParam)
{
	FINDPOINT& fp = *(FINDPOINT*)lpParam;
	CT2StatusBar &sb = *fp.pPro;

	UINT *pFirst, *pBase, *pBegin; UINT nSize;
	BYTE bRelativePoint = fp.pView->m_bRelativePoint;
	CListCtrl &sl = *fp.pSL;
	if(fp.bSuper)
	{
		pFirst	= (UINT*)fp.pRom;
		pBase	= (UINT*)fp.pRom;
		pBegin	= (UINT*)pBase;
		nSize	= fp.nSize;
	}else
	{
		pFirst	= (UINT*)fp.pView->GetDocument()->m_pRom;
		if((int)fp.pView->m_nPointEnd<0)
		{
			nSize	= (UINT)(-(int)fp.pView->m_nPointEnd);
		}else
		{
			nSize	= fp.pView->m_nPointBegin>fp.pView->m_nPointEnd?
							fp.pView->m_nPointBegin-fp.pView->m_nPointEnd:
							fp.pView->m_nPointEnd-fp.pView->m_nPointBegin;
		}
		pBase	= (UINT*)fp.pView->GetDocument()->m_pRom+
						(((fp.pView->m_nPointBegin>fp.pView->m_nPointBegin?
						fp.pView->m_nPointEnd:fp.pView->m_nPointBegin)&0xFFFFFFFC)/4);
		pBegin	= pBase;
		sl.SetRedraw(FALSE);
	}
	nSize/=4;
	UINT *pEnd = pBase+nSize;
	UINT nBaseAddress = fp.pView->m_nBaseAddress;

	CString Offset, PointList;
	int nCount=sl.GetItemCount();
	POSITION pos = sl.GetFirstSelectedItemPosition();
	BOOL bAll = pos==NULL || bRelativePoint;
	g_bBreak = FALSE;
	while(!g_bBreak && pBegin<pEnd)
	{
		int nIndex=0;
		UINT *pOldBegin=pBegin;
		while(TRUE)
		{
			if(bAll)
			{
				if(nIndex>=nCount)
					break;
			}else
			{
				while(nIndex<nCount)
				{
					if(sl.GetItemState(nIndex, LVIS_SELECTED))
						break;
					nIndex++;
				}
				if(nIndex>=nCount)
					break;
			}

			Offset = sl.GetItemText(nIndex, 1);
			UINT nOffset = StrToIntEX(Offset);
			if(bRelativePoint)
				nOffset-=nBaseAddress;
			else
				nOffset+=nBaseAddress;

			if(*pBegin>nOffset)
				break;
			else
			if(*pBegin==nOffset)
			{
				PointList = sl.GetItemText(nIndex, 0);
				if(PointList.GetAt(0)==_T('?'))
					PointList.Format(_T("%08X"), ((BYTE*)pBegin-(BYTE*)pFirst));
				else
				{
					Offset.Format(_T("%08X"), ((BYTE*)pBegin-(BYTE*)pFirst));
					if(PointList.Find(Offset)!=-1)
						continue;
					PointList.AppendFormat(_T(",%s"), Offset);
				}
				sl.SetItemText(nIndex, 0, PointList);
				if(bRelativePoint)
					pBegin++;
			}else
			if(bRelativePoint)
				break;

			nIndex++;
		}
		if(bRelativePoint)
		{
			if(nIndex>=nCount) break;
			pBegin = pOldBegin;
		}

		pBegin++;
		sb.SetProgressPos((UINT)(pBegin-pBase)*100/nSize);
	}

	if(!fp.bSuper)
	{
		fp.pPro->EndModalLoop(IDCANCEL);
		sl.SetRedraw();
	}
	return 0;
}

struct SEARCHTEXT {
	CT2StatusBar* pPro;
	CT2ScriptView *pView;
	CListCtrl *pScriptList;
	BOOL bSuper;
	UINT nSize;
	BYTE *pRom;
};

UINT SearchText(LPVOID lpParam)
{
	SEARCHTEXT &st = *(SEARCHTEXT*)lpParam;
	CT2StatusBar &sb = *st.pPro;

	CString path;
	HANDLE hFind=INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA fd;
	if(st.bSuper)
	{
		path = GetPath(st.pView->GetSafeHwnd());
		if(path.IsEmpty()) goto End;
		path.AppendChar(_T('\\'));
		hFind = FindFirstFile(path+_T("*.*"), &fd);
		if(hFind==INVALID_HANDLE_VALUE)
			goto End;;
		if(!CreateDirectory(path+_T("Script"), (LPSECURITY_ATTRIBUTES)NULL) && (GetLastError()!=0xB7))
			{ShowError(); goto End;}
	}

	UINT nMinLen = min(st.pView->m_nMinLen, st.pView->m_nMaxLen);
	UINT nMaxLen = max(st.pView->m_nMinLen, st.pView->m_nMaxLen);
	CListCtrl &ScriptList = *st.pScriptList;
	CString *DTBL = st.pView->GetDocument()->m_DTBL;
	CString *STBL = st.pView->GetDocument()->m_STBL;
	BYTE nTblMode = st.pView->m_nTblMode;
	BYTE nSwpCode = st.pView->m_nSwpCode;
	UINT nItem = 0;
	UINT nTextCount = 0, nScriptCount = 0;;
	BYTE nAlign2 = st.pView->m_nAlign2;
	BYTE nNoWordCheck = st.pView->m_nNoWord;
	BYTE bWord = FALSE;
	UINT nCodePage = st.pView->GetDocument()->m_nScriptCodePage;
	WORD nDefaultChar = st.pView->GetDocument()->m_nDefaultChar;
	BYTE nCtrlEndOnly = st.pView->m_nCtrlEndOnly;

	// 测试发现CString类的速度很慢所以直接使用内存
	// 防止溢出, 控制符的占用空间是原字符的4倍如:00 = [00]
	int nBufferSize = nMaxLen*10;
	LPTSTR lpszScript = new TCHAR[nBufferSize];
	LPTSTR lpszScriptEnd = lpszScript+nBufferSize-1;
	*lpszScriptEnd=0;
	TCHAR strVal[9];
	g_bBreak = FALSE;

	// InertItem
	LVITEM lvi;
	BYTE *pBase, *pRom, *pEnd;
	UINT nSize;
	//ScriptList.SetRedraw(FALSE);
	while(TRUE)
	{
		if(st.bSuper)
		{
			if(!FindNextFile(hFind, &fd))
				break;
			if(fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) continue;
			CFile f;
			if(!f.Open(path+fd.cFileName, CFile::modeRead|CFile::typeBinary))
				break;
			nSize = (UINT)f.GetLength();
			pBase = new BYTE[nSize];
			f.Read(pBase, nSize);
			pRom = pBase;
			pEnd = pBase+nSize-1;
		}else
		{
			pBase = st.pView->GetDocument()->m_pRom;
			if((int)st.pView->m_nEnd<0)
			{
				pRom = pBase+st.pView->m_nBegin;
				pEnd = pRom-(int)st.pView->m_nEnd;
			}else
			{
				pRom = pBase+(st.pView->m_nBegin>st.pView->m_nEnd?
								st.pView->m_nEnd:st.pView->m_nBegin);
				pEnd = pBase+(st.pView->m_nBegin>st.pView->m_nEnd?
								st.pView->m_nBegin:st.pView->m_nEnd);
			}
			nSize = (UINT)(pEnd-pRom)+1;
		}
		if(nTblMode==CT2_TBL_USERTBL)
		while(!g_bBreak && pRom<=pEnd)
		{	// 码表模式
			UINT nOffset = (UINT)(pRom-pBase);
			if(nAlign2 && nOffset&1)
			{
				pRom++;
				continue;
			}
			//CString Script;
			LPTSTR lpScript = lpszScript;
			UINT nLen = 0;
			BYTE nCtrlOnly = 1;
			BYTE bCtrlEnd = 0;
			while(pRom<=pEnd && lpScript<lpszScriptEnd)
			{
				UINT wCode = -1;
				if(pRom+1<pEnd)
					wCode = nSwpCode?
							MAKEWORD(*pRom, *(pRom+1)):
							MAKEWORD(*(pRom+1), *pRom);
				if( (wCode!=-1) && !DTBL[wCode].IsEmpty() )
				{
					if(DTBL[wCode].GetAt(0)==_T('\n'))
					{
						//CString sLen = DTBL[wCode]; sLen.Delete(0);
						lstrcpy(strVal, (LPCTSTR)DTBL[wCode]+1);
						//char nCLen = StrToIntEX(strVal, FALSE);
						int nCLen=0;
						swscanf(strVal, _T("%d"), &nCLen);
						if(!nCLen) {bCtrlEnd=TRUE; break;}
						//Script.AppendChar(_T('['));
						*lpScript++=_T('[');
						nLen+=nCLen;
						while(nCLen-- && (pRom<pEnd))
						{
							//Script.AppendFormat(_T("%02X"), *pRom++);
							wsprintf(lpScript, _T("%02X"), *pRom++);
							lpScript+=2;
						}
						//Script.AppendChar(_T(']'));
						*lpScript++=_T(']');
					}else
					{
						//Script.Append(DTBL[wCode]);
						lstrcpy(lpScript, DTBL[wCode]);
						lpScript+=DTBL[wCode].GetLength();

						pRom+=2;
						nLen+=2;
						bWord = TRUE;
						nCtrlOnly = 0;
						nTextCount++;
					}
					continue;
				}else
				if(!STBL[*pRom].IsEmpty())
				{
					if(STBL[*pRom].GetAt(0)==_T('\n'))
					{
						//CString sLen = STBL[*pRom]; sLen.Delete(0);
						//char nCLen = StrToIntEX(sLen, FALSE);
						lstrcpy(strVal, (LPCTSTR)STBL[*pRom]+1);
						int nCLen = 0;
						swscanf(strVal, _T("%d"), &nCLen);
						if(!nCLen) {bCtrlEnd=TRUE; break;}
						//Script.AppendChar(_T('['));
						*lpScript++=_T('[');
						nLen+=nCLen;
						while(nCLen-- && (pRom<pEnd))
						{
							//Script.AppendFormat(_T("%02X"), *pRom++);
							wsprintf(lpScript, _T("%02X"), *pRom++);
							lpScript+=2;
						}
						//Script.AppendChar(_T(']'));
						*lpScript++=_T(']');
					}else
					{
						//Script.Append(STBL[*pRom]);
						lstrcpy(lpScript, STBL[*pRom]);
						lpScript+=STBL[*pRom].GetLength();
						pRom++;
						nLen++;
						nCtrlOnly = 0;
						nTextCount++;
					}
					continue;
				}
				break;
			}
			if( ((nLen>=nMinLen) && (nLen<=nMaxLen)) &&
				(!nCtrlEndOnly || bCtrlEnd) &&
				(!nNoWordCheck || bWord) &&
				!nCtrlOnly)
			{
				*lpScript = 0;

				ScriptList.InsertItem(nItem, _T("?"));
				//CString tmp;
				//tmp.Format(_T("%08X"), nOffset);
				wsprintf(strVal, _T("%08X"), nOffset);
				//ScriptList.SetItemText(nItem, 1, strVal);
				lvi.iSubItem=1;
				lvi.pszText=strVal;
				SendMessage(ScriptList.m_hWnd, LVM_SETITEMTEXT, nItem, (LPARAM)&lvi);
				//tmp.Format(_T("%d"), nLen);
				wsprintf(strVal, _T("%d"), nLen);
				//ScriptList.SetItemText(nItem, 2, strVal);
				lvi.iSubItem=2;
				lvi.pszText=strVal;
				SendMessage(ScriptList.m_hWnd, LVM_SETITEMTEXT, nItem, (LPARAM)&lvi);
				*lpScript=0;
				//ScriptList.SetItemText(nItem, 3, lpszScript);
				lvi.iSubItem=3;
				lvi.pszText=lpszScript;
				SendMessage(ScriptList.m_hWnd, LVM_SETITEMTEXT, nItem, (LPARAM)&lvi);

				nItem++;
				nScriptCount += nTextCount;
			}
			nTextCount = 0;
			bWord = FALSE;
			pRom++;
			sb.SetProgressPos(nOffset*100/nSize);
		}else
		while(!g_bBreak && pRom<=pEnd)
		{	// 系统代码页模式
			UINT nOffset = (UINT)(pRom-pBase);
			if(nAlign2 && nOffset&1)
			{
				pRom++;
				continue;
			}
			//CString Script;
			LPTSTR lpScript = lpszScript;
			UINT nLen = 0;
			BYTE bCtrlEnd = 0;
			BYTE nCtrlOnly = 1;
			while(pRom<=pEnd && lpScript<lpszScriptEnd)
			{
				UINT wCode = -1;
				if(pRom+1<pEnd)
					wCode = nSwpCode?
							MAKEWORD(*(pRom+1), *pRom):
							MAKEWORD(*pRom, *(pRom+1));

				if(wCode!=-1 && nTblMode==CT2_TBL_USERCTL && DTBL[wCode].GetAt(0)==_T('\n'))
				{
					//CString sLen = DTBL[wCode]; sLen.Delete(0);
					lstrcpy(strVal, (LPCTSTR)DTBL[wCode]+1);
					//char nCLen = StrToIntEX(strVal, FALSE);
					int nCLen=0;
					swscanf(strVal, _T("%d"), &nCLen);
					if(!nCLen) {bCtrlEnd=TRUE; break;}
					//Script.AppendChar(_T('['));
					*lpScript++=_T('[');
					nLen+=nCLen;
					while(nCLen-- && (pRom<pEnd))
					{
						//Script.AppendFormat(_T("%02X"), *pRom++);
						wsprintf(lpScript, _T("%02X"), *pRom++);
						lpScript+=2;
					}
					//Script.AppendChar(_T(']'));
					*lpScript++=_T(']');
					continue;
				}else
				{
					UINT nCode=0;
					if(nTblMode==CT2_TBL_USERCTL)
					{
						WORD nTCode = (WORD)(wCode>>8 | wCode<<8);
						if(wCode !=-1 && !DTBL[nTCode].IsEmpty() && DTBL[nTCode].GetAt(0)!=_T('\n'))
						{
							lstrcpy(lpScript, DTBL[nTCode]);
							lpScript+=DTBL[nTCode].GetLength();

							pRom+=2;
							nLen+=2;
							bWord = TRUE;
							nCtrlOnly = 0;
							nTextCount++;
							continue;
						}else
						if(!STBL[*pRom].IsEmpty() && STBL[*pRom].GetAt(0)!=_T('\n'))
						{
							lstrcpy(lpScript, STBL[*pRom]);
							lpScript+=STBL[*pRom].GetLength();
							pRom++;
							nLen++;
							nCtrlOnly = 0;
							nTextCount++;
							continue;
						}else
							goto SYSCODE;
						continue;

					}else
					{
					SYSCODE:

						if(nCodePage==65001)
							mcharTowchar(&nCode, pRom, nCodePage);
						else
							mcharTowchar(&nCode, &wCode, nCodePage);
						if( (pRom<=pEnd) &&
							//mcharTowchar(&wCode, pRom, nCodePage) &&
							(nCode!=nDefaultChar) /*&&
							!(wCode>>16)*/ ||
							(nCodePage==1200 || nCodePage==1201) )
						{	// Shift JIS
							if(nCodePage==1200)
							{
								if(*(WORD*)pRom==_T('\r') || *(WORD*)pRom==_T('\n') || *(WORD*)pRom==0xFEFF)
									goto Ctrl;
								if(*(WORD*)pRom==0) {pRom++; break;}
								*lpScript++=*(WCHAR*)pRom;
							}else
							if(nCodePage==1201)
							{
								if(*(WORD*)pRom==0x0A00 || *(WORD*)pRom==0x0D00 || *(WORD*)pRom==0xFFFE)
									goto Ctrl;
								if(*(WORD*)pRom==0) {pRom++; break;}
								*lpScript++=(*pRom<<8)|*(pRom+1);
							}else
							if((WORD)nCode<=_T('~'))
								goto Ansi;
							else
							if(nCodePage==65001)
							{
								pRom++;
								nLen++;
								*lpScript++=nCode;
							}
							else
								//Script.AppendChar(wCode);
								*lpScript++=nCode;
							if(wCode<0xFF61)	// 半角检查
							{
								pRom+=2;
								nLen+=2;
							}else
							{
								pRom++;
								nLen++;
							}
							bWord=TRUE;
							nCtrlOnly = 0;
							nTextCount++;
							continue;
							Ctrl:
							wsprintf(lpScript, _T("[%02X%02X]"), *pRom, *(pRom+1));
							lpScript+=6;
							pRom+=2;
							nLen+=2;
							continue;
						}else
						Ansi:
						{
							if(nTblMode==CT2_TBL_USERCTL && STBL[*pRom].GetAt(0)==_T('\n'))
							{
								//CString sLen = STBL[*pRom]; sLen.Delete(0);
								//char nCLen = StrToIntEX(sLen, FALSE);
								lstrcpy(strVal, (LPCTSTR)STBL[*pRom]+1);
								int nCLen = 0;
								swscanf(strVal, _T("%d"), &nCLen);
								if(!nCLen) {bCtrlEnd=TRUE; break;}
								//Script.AppendChar(_T('['));
								*lpScript++=_T('[');
								nLen+=nCLen;
								while(nCLen-- && (pRom<pEnd))
								{
									//Script.AppendFormat(_T("%02X"), *pRom++);
									wsprintf(lpScript, _T("%02X"), *pRom++);
									lpScript+=2;
								}
								//Script.AppendChar(_T(']'));
								*lpScript++=_T(']');
								continue;
							}else
							if(*pRom>=' ' && *pRom<='~' || *pRom=='\t')
							{
								// Ascii
								//Script.AppendChar(*pRom);
								*lpScript++=*pRom;
								pRom++;
								nLen++;
								nCtrlOnly = 0;
								nTextCount++;
								continue;
							}else
							{
								if(*pRom=='\n' || *pRom=='\r')
								{
									// 回车
									//Script.AppendFormat(_T("[%02X]"), *pRom);
									wsprintf(lpScript, _T("[%02X]"), *pRom);
									lpScript+=4;
									pRom++;
									nLen++;
									continue;
								}
							}
						}
					}
				}
				break;
			}
			//if( ((nLen>=nMinLen) && (nLen<=nMaxLen)) &&
			//	(!nNoWordCheck || bWord) && !nCtrlOnly)
			if( ((nLen>=nMinLen) && (nLen<=nMaxLen)) &&
				(!nCtrlEndOnly || bCtrlEnd) &&
				(!nNoWordCheck || bWord) &&
				!nCtrlOnly)
			{
				*lpScript = 0;

				ScriptList.InsertItem(nItem, _T("?"));
				//CString tmp;
				//tmp.Format(_T("%08X"), nOffset);
				wsprintf(strVal, _T("%08X"), nOffset);
				//ScriptList.SetItemText(nItem, 1, strVal);
				lvi.iSubItem=1;
				lvi.pszText=strVal;
				SendMessage(ScriptList.m_hWnd, LVM_SETITEMTEXT, nItem, (LPARAM)&lvi);
				//tmp.Format(_T("%d"), nLen);
				wsprintf(strVal, _T("%d"), nLen);
				//ScriptList.SetItemText(nItem, 2, strVal);
				lvi.iSubItem=2;
				SendMessage(ScriptList.m_hWnd, LVM_SETITEMTEXT, nItem, (LPARAM)&lvi);
				//Script.Replace(_T("]["), _T(""));
				*lpScript=0;
				//ScriptList.SetItemText(nItem, 3, lpszScript);
				lvi.iSubItem=3;

				lvi.pszText=lpszScript;
				SendMessage(ScriptList.m_hWnd, LVM_SETITEMTEXT, nItem, (LPARAM)&lvi);

				nItem++;
				nScriptCount += nTextCount;
			}
			nTextCount = 0;
			bWord=FALSE;
			pRom++;
			sb.SetProgressPos(nOffset*100/nSize);
		}

		if(st.bSuper)
		{
			st.pRom = pBase;
			st.nSize = nSize;
			FindPoint(&st);
			CStdioFile f;
			if(!f.Open(path+_T("Script/")+fd.cFileName+_T(".txt"), CStdioFile::modeCreate|CStdioFile::modeWrite|CStdioFile::typeBinary))
				break;
			UINT n=0xFEFF;
			f.Write(&n, 2);
			for(int i=0; i<ScriptList.GetItemCount(); i++)
			{
				CString t = ScriptList.GetItemText(i, 0);
				if(t.GetAt(0)!=_T('?'))
				{
					CString t;
					t.Format(_T("*%s\r\n%s,%s,%s\r\n\r\n"),
						ScriptList.GetItemText(i,0),
						ScriptList.GetItemText(i,1),
						ScriptList.GetItemText(i,2),
						ScriptList.GetItemText(i,3));

					f.WriteString(t);
				}
			}
			nItem = 0;
			ScriptList.DeleteAllItems();

			//
			delete[] pBase;
		}else
			break;
	}
	delete[] lpszScript;
 
	{
	CString f,h;
	f.LoadString(IDS_SEARCHTEXT_COUNT);
	h.Format(f, nItem, nScriptCount);
	Hint(h);
	}
	//ScriptList.SetRedraw();

End:
	//if(hFind!=INVALID_HANDLE_VALUE)
	//	CloseHandle(hFind);

	sb.EndModalLoop(IDCANCEL);
	return 0;
}

void CT2ScriptView::OnSearchText(UINT nID)
{
	m_ScriptList.SetRedraw(FALSE);
	m_ScriptList.DeleteAllItems();

	SEARCHTEXT st;
	st.pPro = (CT2StatusBar*) theApp.GetStatusBar();
	st.pPro->BeginProgress(IDS_SCRIPTIVEW_SEARCHING);

	st.pScriptList = &m_ScriptList;
	st.pView = this;

	st.bSuper = nID==ID_SEARCH_TEXTEX;

	CWinThread *pOpenThread = AfxBeginThread(SearchText,
		&st, THREAD_PRIORITY_IDLE, 1024, 0, NULL);
	st.pPro->RunModalLoop();

	m_ScriptList.SetRedraw();

	st.pPro->EndProgress();
}

void CT2ScriptView::OnUpdateViewCtrl(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(theApp.GetCurView()==this);
}

void CT2ScriptView::OnLvnItemActivateFsilist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	SetCurView(ID_VIEW_HEXVIEW);
	CString Off = m_ScriptList.GetItemText(pNMIA->iItem, 1);
	theApp.GetCurView()->ScrollTo(StrToIntEX(Off));

	*pResult = 0;
}

void CT2ScriptView::OnTblMode()
{
	m_nTblMode=m_nTblMode==CT2_TBL_USERTBL?CT2_TBL_SYS:CT2_TBL_USERTBL;

	if(m_nTblMode==CT2_TBL_USERTBL)
	{
		CT2Doc *pDoc = GetDocument();
		if(pDoc->m_strTblName.IsEmpty())
		{
			pDoc->OnTblSeltbl();
			m_nTblMode = pDoc->m_strTblName.IsEmpty()?CT2_TBL_SYS:CT2_TBL_USERTBL;
		}
	}
}

void CT2ScriptView::OnUpdateTblMode(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nTblMode==CT2_TBL_USERTBL);
}

void CT2ScriptView::OnSwpcode()
{
	m_nSwpCode=!m_nSwpCode;
}

void CT2ScriptView::OnUpdateSwpcode(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nSwpCode);
	//pCmdUI->Enable(m_nTblMode);
}

void CT2ScriptView::OnScriptCopy()
{
	if(GetFocus()!=&m_ScriptList)
		GetFocus()->SendMessage(WM_COPY);
	else
	{
		POSITION pos = m_ScriptList.GetFirstSelectedItemPosition();
		int nIndex = m_ScriptList.GetNextSelectedItem(pos);
		CEdit e;e.Create(WS_CHILD, CRect(0,0,0,0),this, 0);
		e.SetWindowText(m_ScriptList.GetItemText(nIndex, 3));
		e.SetSel(0,-1); e.Copy();
	}
}

void CT2ScriptView::OnUpdateScriptCopy(CCmdUI *pCmdUI)
{
	pCmdUI->Enable((m_ScriptList.GetFirstSelectedItemPosition()!=NULL) ||
		GetFocus()!=&m_ScriptList);
}

void CT2ScriptView::OnScriptviewDelete()
{
	if(GetFocus()!=&m_ScriptList)
	{
		GetFocus()->SendMessage(WM_KEYDOWN, VK_DELETE);
		return;
	}
	m_ScriptList.SetRedraw(FALSE);

	/*POSITION pos;
	while(pos = m_ScriptList.GetFirstSelectedItemPosition())
		m_ScriptList.DeleteItem(m_ScriptList.GetNextSelectedItem(pos));
*/
	for(int i=m_ScriptList.GetItemCount()-1; i>=0; i--)
	{
		if(m_ScriptList.GetItemState(i, LVIS_SELECTED))
			m_ScriptList.DeleteItem(i);
	}
	m_ScriptList.SetRedraw();
}

void CT2ScriptView::OnUpdateEditDelete(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( (GetFocus()!=&m_ScriptList) ||
		m_ScriptList.GetItemCount() );
}

void CT2ScriptView::OnEditExport()
{
	static CTextFileDialog fd(FALSE, IDS_SCRIPTFILE);
	CString text;
	if(fd.DoModal()!=IDOK) return;
	text = PathFindExtension(fd.GetPathName());
	text.MakeLower();
	if(text!=_T(".txt"))
		text = fd.GetPathName()+_T(".txt");
	else
		text = fd.GetPathName();
    CStdioFile ScriptFile;
	if(!ScriptFile.Open(text,
		CStdioFile::modeCreate|CStdioFile::modeWrite|CStdioFile::typeBinary))
	{
		ShowError();
		return;
	}
	POSITION pos = m_ScriptList.GetFirstSelectedItemPosition();
	BOOL bAll = pos==NULL;
	char *lpMulit = NULL;
	UINT nBufferSize=m_nMaxLen;
	if(fd.m_nCodePage==1200)
	{
		WORD c=0xFEFF;
		ScriptFile.Write(&c, 2);
	}else
	if(fd.m_nCodePage==1201)
	{
		WORD c=0xFFFE;
		ScriptFile.Write(&c, 2);
	}else
	if(fd.m_nCodePage==65001)// UTF-8
	{
		UINT c=0xBFBBEF;
		ScriptFile.Write(&c, 3);
		lpMulit = new char[nBufferSize*=3];
	}else
		lpMulit = new char[nBufferSize];

	BOOL bUsed=FALSE;
	CPINFO cpInfo;
	GetCPInfo(fd.m_nCodePage, &cpInfo);
	LPCSTR lpDefChar = fd.m_nCodePage==65001?NULL:(LPCSTR)cpInfo.DefaultChar;
	int nIndex=-1;
	while(pos || bAll)
	{
		if(bAll)
		{
			nIndex++;
			if(nIndex>=m_ScriptList.GetItemCount())
				break;
		}else
			nIndex = m_ScriptList.GetNextSelectedItem(pos);
		CString Script;
		text = m_ScriptList.GetItemText(nIndex, 0);
		if(text.GetAt(0)!=_T('?'))// 存在有效指针
			Script.AppendFormat(_T("*%s\r\n"), text);

		for(int i=1; i<4; i++)
		{// 地址, 大小 文本
			text = m_ScriptList.GetItemText(nIndex, i);
			if(i==3)
				Script.AppendFormat(_T("%s\r\n\r\n"), text);
			else
				Script.AppendFormat(_T("%s,"), text);
		}
		if(fd.m_nCodePage==1200)
			ScriptFile.WriteString(Script);
		else
		if(fd.m_nCodePage==1201)
		{
			LPTSTR lpChar = Script.GetBuffer();
			for(int i=0; i<Script.GetLength(); i++)
				lpChar[i] = (lpChar[i]>>8)|((lpChar[i]&0xFF)<<8);
			Script.ReleaseBuffer();
			ScriptFile.WriteString(Script);
		}else
		{
			LPCTSTR lpText = Script.GetBuffer();
			BOOL bUseddefchar=FALSE;
			::WideCharToMultiByte(fd.m_nCodePage, 0, lpText, -1,
				(LPSTR)lpMulit, nBufferSize/*Script.GetLength()*sizeof(TCHAR)*/,
				lpDefChar, fd.m_nCodePage==65001?NULL:&bUseddefchar);
			Script.ReleaseBuffer();
			if(bUseddefchar && !bUsed)
			{
				bUsed = bUseddefchar;
				Hint(IDS_INVALIDCHAR, MB_OK|MB_ICONERROR);
			}
			ScriptFile.Write(lpMulit, (UINT)strlen(lpMulit));
		}
	}

	if(lpMulit) delete[] lpMulit;
}

void CT2ScriptView::OnUpdateEditExport(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_ScriptList.GetItemCount());
}

void CT2ScriptView::OnScriptviewFindpoint()
{
	FINDPOINT fp;
	fp.pPro = (CT2StatusBar*) theApp.GetStatusBar();
	fp.pPro->BeginProgress(IDS_SCRIPTIVEW_FINDPOINT);

	fp.pView = this;
	fp.pSL = &m_ScriptList;

	fp.bSuper = FALSE;

	CWinThread *pOpenThread = AfxBeginThread(FindPoint,
		&fp, THREAD_PRIORITY_IDLE, 1024, 0, NULL);
	fp.pPro->RunModalLoop();

	fp.pPro->EndProgress();
}

void CT2ScriptView::OnScriptviewInvalidatedelete()
{
	m_ScriptList.SetRedraw(FALSE);
	//CString point;

	TCHAR Char[2];
	LVITEM lvi;
	memset(&lvi, 0, sizeof(LVITEM));
	lvi.iSubItem = 0;
	CString str;
	lvi.cchTextMax = sizeof(TCHAR);
	lvi.pszText = Char;

	for(int i=m_ScriptList.GetItemCount()-1; i>=0; i--)
	{
		// 比GetItemText快
		//point = m_ScriptList.GetItemText(i, 0);

		m_ScriptList.SendMessage(LVM_GETITEMTEXT, (WPARAM)i,
            (LPARAM)&lvi);

		//if(point.GetAt(0)==_T('?'))
		if(*lvi.pszText==_T('?'))
			m_ScriptList.DeleteItem(i);
	}
	m_ScriptList.SetRedraw();
}

void CT2ScriptView::OnSearchAlign2()
{
	m_nAlign2 = !m_nAlign2;
}

void CT2ScriptView::OnUpdateSearchAlign2(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nAlign2);
}

void CT2ScriptView::OnSearchNoword()
{
	m_nNoWord = !m_nNoWord;
}

void CT2ScriptView::OnUpdateSearchNoword(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nNoWord);
}

struct IMPORTDATA {
	CT2StatusBar* pPro;
	CListCtrl *pScriptList;
	CT2ScriptView *pView;
	BYTE nSaveTbl;
	BYTE nTblMode;
	BYTE nSysCodeLen;
	UINT nCodePage;
};

CString m_strNewCode;
BYTE m_nNewCode;
BYTE m_nNoDefHint;

BOOL DoPointImport(CString& Text, UINT& nNewAddress, CString& Point, IMPORTDATA &id)
{
	BYTE *pRom = id.pView->GetDocument()->m_pRom;
	CString *IDTBL = id.pView->GetDocument()->m_DTBL;
	CString *ISTBL = id.pView->GetDocument()->m_STBL;
	BYTE nSwpCode = id.pView->m_nSwpCode;
	UINT nCodePage = id.nCodePage;
	BYTE nTblMode = id.nTblMode;
	BYTE nSysCodeLen = id.nSysCodeLen;
	int &wLastCode = id.pView->GetDocument()->m_wLastCode;

	UINT nUpdateAddress = nNewAddress+id.pView->m_nBaseAddress;

	CString CtrlList, NoDefHint;
	NoDefHint.LoadString(IDS_NODEFHINT);

	// 导入文本
	int nTextPos=0, nTextLen=Text.GetLength();
	while(nTextPos<nTextLen)
	{
		TCHAR Char = Text.GetAt(nTextPos);
		if(Char==_T('['))
		{	// 控制符
			int nCtrlPos = Text.Find(_T(']'), nTextPos);
			if(nCtrlPos!=-1)
			{
				nTextPos++;
				CtrlList = Text.Mid(nTextPos, nCtrlPos-nTextPos);
				nCtrlPos = CtrlList.GetLength();
				if(nCtrlPos&1)
				{
					nCtrlPos++;
					CtrlList.Insert(0, _T('0'));
				}
				nTextPos+=nCtrlPos+1;
				for(int i=0; i<nCtrlPos; i+=2)
				{
					int dwCode=StrToIntEX(CtrlList.Mid(i, 2));
					if(dwCode!=-1)
					{
						memcpy(pRom+nNewAddress, &dwCode, 1);
						nNewAddress++;
					}
					
				}
				continue;
			}
		}
		if(!nTblMode)
		{
			// 系统码表
			LPSTR lpOut = (LPSTR)(pRom+nNewAddress);
			BYTE l = Char<0x80?1:nSysCodeLen;
			WideCharToMultiByte(nCodePage, 0, &Char, 2, lpOut, l, NULL, NULL);

			if(nSwpCode && l==2)
				*(WORD*)lpOut = (WORD)(*((BYTE*)lpOut+1) | (*(BYTE*)lpOut<<8));

			nNewAddress+=l;
			nTextPos++;
			continue;
		}else
		{
NEWCODE:	// 定义新的编码
			if(!IDTBL[Char].IsEmpty())
			{
				int dwCode = StrToIntEX(IDTBL[Char]);
				if(dwCode==-1) continue;
				memcpy(pRom+nNewAddress, &dwCode, 2);
				nNewAddress+=2;
				nTextPos++;
				//nWordCount++;
				continue;
			}
			if(!ISTBL[Char].IsEmpty())
			{
				int Code = StrToIntEX(ISTBL[Char]);
				if(Code==-1) continue;
				*(pRom+nNewAddress)=(BYTE)Code;
				nNewAddress++;
				nTextPos++;
				//nByteCount++;
				continue;
			}
			//
			if(IDTBL[Char].IsEmpty() && ISTBL[Char].IsEmpty())
			{
				if(m_nNoDefHint)
				{
					int nError=Hint(Text+_T(" -->")+Char+NoDefHint, MB_ICONQUESTION|MB_YESNOCANCEL);
					if(nError==IDNO) m_nNoDefHint=FALSE;
					if(nError==IDCANCEL) return FALSE;
				}
				if(m_nNewCode)
				{
					CString Neo;
					if(nSwpCode)
					{
						Neo.Format(_T("%02X%02X"), wLastCode&0xFF, (wLastCode>>8)&0xFF);
						IDTBL[Char].Format(_T("%04X"), wLastCode&0xFFFF);
					}else
					{
						Neo.Format(_T("%04X"), wLastCode&0xFFFF);
						IDTBL[Char].Format(_T("%02X%02X"), wLastCode&0xFF, (wLastCode>>8)&0xFF);
					}
					m_strNewCode.AppendFormat(_T("%s=%c\r\n"), Neo, Char);
					wLastCode++;
					goto NEWCODE;
				}
			}
		}
		nTextPos++;
	}

	// 更新指针表
	if(id.pView->m_bRelativePoint)
		nUpdateAddress -= id.pView->m_nBaseAddress;
	while(Point.GetAt(0)!=_T('?'))
	{
		UINT nPoint = StrToIntEX(Point.Left(8));
		if(nPoint!=-1)
			memcpy(pRom+nPoint, &nUpdateAddress, 4);
		Point.Delete(0, 9);
		if(Point.IsEmpty()) break;
	}

	return TRUE;
}

UINT Import(LPVOID lpParam)
{
	IMPORTDATA &id = *(IMPORTDATA*)lpParam;
	CT2StatusBar &sb = *id.pPro;
	CListCtrl &sl = *id.pScriptList;
	UINT nCodePage = id.nCodePage;
	BYTE nTblMode = id.nTblMode;
	BYTE nSysCodeLen = id.nSysCodeLen;
	BYTE *pRom = id.pView->GetDocument()->m_pRom;
	CString *IDTBL = id.pView->GetDocument()->m_DTBL;
	CString *ISTBL = id.pView->GetDocument()->m_STBL;
	BYTE nSwpCode = id.pView->m_nSwpCode;
	BYTE nAlign2 = id.pView->m_nAlign2;
	BYTE bNewAddress = id.pView->m_bNewAddress;

	int nSize, nCount = sl.GetItemCount();
	int &wLastCode = id.pView->GetDocument()->m_wLastCode;

	CString Offset, Point, Size, Text, CtrlList, Ctrl, NoDefHint;
	m_strNewCode.Empty();

	UINT nOffset, nNewAddress = id.pView->m_nNewAddress;
	if(nNewAddress>(id.pView->GetDocument()->m_nRomSize-4))
		nNewAddress=0;
	g_bBreak = FALSE;

	NoDefHint.LoadString(IDS_NODEFHINT);
	for(int nIndex=0; !g_bBreak && nIndex<nCount;)
	{
		Point = sl.GetItemText(nIndex, 0);
		Offset = sl.GetItemText(nIndex, 1);
		Size = sl.GetItemText(nIndex, 2);
		Text = sl.GetItemText(nIndex, 3);

		nOffset = StrToIntEX(Offset);
		nSize = StrToIntEX(Size);

		if(bNewAddress) goto supertext;
		// 导入文本
		int nTextAddPos=0;
		int nTextPos=0, nTextLen=Text.GetLength();
		while(nTextPos<nTextLen)
		{
			TCHAR Char = Text.GetAt(nTextPos);
			if(Char==_T('['))
			{	// 控制符
				int nCtrlPos = Text.Find(_T(']'), nTextPos);
				if(nCtrlPos!=-1)
				{
					nTextPos++;
					CtrlList = Text.Mid(nTextPos, nCtrlPos-nTextPos);
					nCtrlPos = CtrlList.GetLength();
					if(nCtrlPos&1)
					{
						nCtrlPos++;
						CtrlList.Insert(0, _T('0'));
					}
					nTextPos+=nCtrlPos+1;
					for(int i=0; i<nCtrlPos; i+=2)
					{
						int dwCode=StrToIntEX(CtrlList.Mid(i, 2));
						if( (dwCode!=-1) &&
							(nTextAddPos<nSize) )
						{
							memcpy(pRom+nOffset+nTextAddPos, &dwCode, 1);
							nTextAddPos++;
						}
						
					}
					continue;
				}
			}
			if(!nTblMode)
			{
				// 系统码表
				if(nTextAddPos<(int)(nSize-nSysCodeLen))
				{
					LPSTR lpOut = (LPSTR)(pRom+nOffset+nTextAddPos);
					BYTE l = Char<0x80?1:nSysCodeLen;
					WideCharToMultiByte(nCodePage, 0, &Char, 2, lpOut, l, NULL, NULL);

					if(nSwpCode && l==2)
						*(WORD*)lpOut = (WORD)(*((BYTE*)lpOut+1) | (*(BYTE*)lpOut<<8));

					nTextAddPos += l;
					nTextPos++;
					continue;
				}
			}else
			{
NEWCODE:		// 定义新的编码
				if(!IDTBL[Char].IsEmpty() && (nTextAddPos<(nSize-1)))
				{
					int dwCode = StrToIntEX(IDTBL[Char]);
					if(dwCode==-1) continue;
					memcpy(pRom+nOffset+nTextAddPos, &dwCode, 2);
					nTextAddPos+=2;
					nTextPos++;
					//nWordCount++;
					continue;
				}
				if(!ISTBL[Char].IsEmpty() && (nTextAddPos<nSize))
				{
					int Code = StrToIntEX(ISTBL[Char]);
					if(Code==-1) continue;
					*(pRom+nOffset+nTextAddPos)=(BYTE)Code;
					nTextAddPos++;
					nTextPos++;
					//nByteCount++;
					continue;
				}
			}
			//
			if(nTblMode && IDTBL[Char].IsEmpty() && ISTBL[Char].IsEmpty())
			{
				if(m_nNoDefHint)
				{
					int nError=Hint(Text+_T(" -->")+Char+NoDefHint, MB_ICONQUESTION|MB_YESNOCANCEL);
					if(nError==IDNO) m_nNoDefHint=FALSE;
					if(nError==IDCANCEL) goto End;
				}
				if(m_nNewCode)
				{
					CString Neo;
					if(nSwpCode)
					{
						Neo.Format(_T("%02X%02X"), wLastCode&0xFF, (wLastCode>>8)&0xFF);
						IDTBL[Char].Format(_T("%04X"), wLastCode&0xFFFF);
					}else
					{
						Neo.Format(_T("%04X"), wLastCode&0xFFFF);
						IDTBL[Char].Format(_T("%02X%02X"), wLastCode&0xFF, (wLastCode>>8)&0xFF);
					}
					m_strNewCode.AppendFormat(_T("%s=%c\r\n"), Neo, Char);
					wLastCode++;
					goto NEWCODE;
				}
			}else
			{	// 超长文本
				// nWordCount=nByteCount=0;
				supertext:if( nAlign2 && (nNewAddress&1) ) nNewAddress++;
				if(!nNewAddress) goto End;
				Offset.Format(_T("%08X"), nNewAddress);
				nSize = nNewAddress;
				if(!DoPointImport(Text, nNewAddress, Point, id)) goto End;
				nSize = nNewAddress-nSize;

				// 插入结束控制符符
				BYTE l = id.pView->GetDocument()->m_nEndCodeLen;
				memcpy(pRom+nNewAddress, &id.pView->GetDocument()->m_nEndCode, l);
				nNewAddress+=l;

				sl.SetItemText(nIndex, 1, Offset);
				Size.Format(_T("%d"), nSize);
				sl.SetItemText(nIndex, 2, Size);
				break;
			}
			nTextPos++;
		}

		nIndex++;
		sb.SetProgressPos(nIndex*100/nCount);
	}
End:
	if(!m_strNewCode.IsEmpty())
	{
		if(id.nSaveTbl || Hint(IDS_NEWCODE, MB_YESNO|MB_ICONQUESTION)==IDYES)
		{
			CStdioFile IT;
			if(IT.Open(id.pView->GetDocument()->m_strTblName, CStdioFile::modeReadWrite|CStdioFile::typeBinary))
			{
				IT.SeekToEnd();
				IT.WriteString(m_strNewCode);
				IT.Close();
				id.pView->GetDocument()->LoadTbl(id.pView->GetDocument()->m_strTblName, 1);
			}
		}else
			m_strNewCode.Empty();
	}

	sb.EndModalLoop(IDCANCEL);
	return 0;
}

void CT2ScriptView::OnEditImport()
{
	if(Hint(IDS_IMPORTSCRIPT, MB_YESNO|MB_ICONQUESTION)!=IDYES)
		return;

	OnImport(TRUE, Hint(IDS_DEFNEWCODE, MB_YESNO|MB_ICONQUESTION)==IDYES, m_nTblMode, GetDocument()->m_nScriptCodePage);
}

void CT2ScriptView::OnImport(BYTE nNoDefHint, BYTE nNewCode, BYTE nTblMode, UINT nCodePage, BYTE nSaveTbl)
{
	m_nNoDefHint = nNoDefHint;
	m_nNewCode = nNewCode;

	GetDocument()->SetModifiedFlag();

	IMPORTDATA id;
	id.pPro = (CT2StatusBar*) theApp.GetStatusBar();
	id.pPro->BeginProgress(IDS_SCRIPTIVEW_SEARCHING);

	m_nTblMode = nTblMode;

	id.pScriptList = &m_ScriptList;
	id.pView = this;
	id.nSaveTbl = nSaveTbl;
	id.nTblMode = nTblMode;
	id.nSysCodeLen = nCodePage==CP_UNICODE_UTF8?3:2;
	id.nCodePage = nCodePage;

	GetDocument()->LoadTbl(GetDocument()->m_strTblName, TRUE);

	CWinThread *pOpenThread = AfxBeginThread(Import,
		&id, THREAD_PRIORITY_IDLE, 1024, 0, NULL);
	id.pPro->RunModalLoop();

	//AddMacro_Import();

	GetDocument()->LoadTbl(GetDocument()->m_strTblName);

	id.pPro->EndProgress();
}

void CT2ScriptView::OnUpdateEditImport(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( m_ScriptList.GetItemCount() &&
					(!GetDocument()->m_strTblName.IsEmpty()||m_nTblMode!=CT2_TBL_USERTBL) &&
					!GetDocument()->m_nReadOnly );
}

void CT2ScriptView::OnScriptviewOpenscript()
{
	static CTextFileDialog fd(TRUE, IDS_SCRIPTFILE);
	if(fd.DoModal()!=IDOK) return;

	OnOpenScript(fd.GetPathName(), fd.m_nCodePage);
}

void CT2ScriptView::OnOpenScript(CString path, UINT nCodePage)
{
	LPTSTR lpScript = GetUText(path, nCodePage);
	if(!lpScript) return;
	CStdioFile Script;
	if(!Script.Open(path, CStdioFile::modeRead|CStdioFile::typeBinary))
	{
		ShowError();
		return;
	}
	//AddMacro_OpenScript(path, nCodePage);

	m_ScriptList.SetRedraw(FALSE);
	m_ScriptList.DeleteAllItems();
	CString Point, Text;
	char t=0; int nItem=0, nPos;

	UINT nSize = (UINT)Script.GetLength();

	nItem=0;
	LPTSTR lpText = lpScript;
	if(*(WORD*)lpText==0xFEFF) lpText=(LPTSTR)((BYTE*)lpText+2);
	//while(Script.ReadString(Text))
	CString Offset, Size, NewText;
	char isNew=0;
	CT2Doc *pDoc = GetDocument();
	CString B = pDoc->m_strScriptBegin,
		C = pDoc->m_strCHN,
		E = pDoc->m_strScriptEnd;
	B.Replace(_T("\r\n"), _T(""));
	C.Replace(_T("\r\n"), _T(""));
	E.Replace(_T("\r\n"), _T(""));

	while(*lpText)
	{
		Text = GetLine(lpText);
		if(Text.GetAt(0)==_T(';') || Text.IsEmpty())
			continue;

		if(t==0)
		{
			int pos;
			if(Text.GetAt(0)==_T('*'))
			{
				pos = Text.Find(_T(','));
				if(pos!=-1)
				{	// new
					Point = Text.Mid(1, pos-1);
					if(Point.IsEmpty()) Point = _T("?");
					pos++;
					int pos2 = Text.Find(_T(','), pos);
					if(pos2==-1) continue;
                    Offset = Text.Mid(pos, pos2-pos);
					Size = Text.Right(Text.GetLength()-pos2-1);
					if(Size.IsEmpty()) continue;
					isNew = 1;
				}else
				{
					Point = Text;
					Point.Delete(0);
					isNew = 0;
				}
			}else
			{
				Point = _T("?");
				goto t1;
			}
			t=1;
			continue;
		}else
		if(t==1)
		{
			t1:
			if(isNew)
			{
				if(Text.Compare(B)!=0) continue;
				while(*lpText)
				{
					Text = GetLine(lpText);
					if(Text.Compare(C)==0)
						break;
				}
				Text.Empty();
				while(*lpText)
				{
					NewText = GetLine(lpText);

					if(NewText.Compare(E)==0)
					{
						Text.Delete(Text.GetLength()-1);
						break;
					}else
					if(NewText!=_T("\r\n"))
						NewText.Append(_T("\r\n"));
					Text.Append(NewText);
				}
				if(Text.IsEmpty()) continue;
				POSITION pos = pDoc->m_VisualScript.GetHeadPosition();
				while(pos)
				{
					VISUALSCRIPT &vs = pDoc->m_VisualScript.GetNext(pos);
					Text.Replace(vs.strVisual, vs.strCtrl);
				}
			}else
			{
				Offset = Text.Left(8);
				if(StrToIntEX(Offset)==-1)
					continue;
				Text.Delete(0, 9);
				nPos = Text.Find(_T(','), 1);
				if(nPos==-1) continue;
				Size = Text.Left(nPos);
				Text.Delete(0, nPos+1);
			}
			m_ScriptList.InsertItem(nItem, Point);
			m_ScriptList.SetItemText(nItem, 1, Offset);
			m_ScriptList.SetItemText(nItem, 2, Size);
			m_ScriptList.SetItemText(nItem, 3, Text);
			nItem++;
			t=0;
		}
	}
	m_ScriptList.SetRedraw();
	delete[] lpScript;
}

void CT2ScriptView::OnScriptFind()
{
	CString Text=g_szBuffer, Script;
	Text.MakeLower();
	int i = 0;
	POSITION pos = m_ScriptList.GetFirstSelectedItemPosition();
	if(pos)
	{
		i = m_ScriptList.GetNextSelectedItem(pos);
		i++;
	}
	int nCount = m_ScriptList.GetItemCount();
	BYTE nA = i;
Loop:
	for(; i<nCount; i++)
	{
		Script = m_ScriptList.GetItemText(i, 3);
		Script.MakeLower();
		if(Script.Find(Text)!=-1)
		{
			m_ScriptList.SetItemState(i,
				LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
			m_ScriptList.SendMessage(WM_KEYDOWN, VK_LEFT);
			return;
		}
	}
	if(nA)
	{
		nCount = nA;
		i = 0;
		nA=0;
		goto Loop;
	}
}

void CT2ScriptView::OnUpdateViewScriptFind(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_ScriptList.GetItemCount());
}

void CT2ScriptView::OnScriptviewFilterpoint()
{
	for(int i=0; i<m_ScriptList.GetItemCount(); i++)
	{
		if(m_ScriptList.GetItemText(i, 0).GetAt(0)==_T('?'))
			m_ScriptList.SetCheck(i);
	}
}

void CT2ScriptView::OnScriptviewFiltersitem()
{
	for(int i=0; i<m_ScriptList.GetItemCount(); i++)
	{
		int nLen = StrToIntEX(m_ScriptList.GetItemText(i, 2), FALSE);
		if( (nLen&1) || (nLen==-1) )
			m_ScriptList.SetCheck(i);
	}
}

void CT2ScriptView::OnScriptviewFilterditem()
{
	for(int i=0; i<m_ScriptList.GetItemCount(); i++)
	{
		int nLen = StrToIntEX(m_ScriptList.GetItemText(i, 2), FALSE);
		if( !(nLen&1) || (nLen==-1))
			m_ScriptList.SetCheck(i);
	}
}

void CT2ScriptView::OnScriptviewFiltermin()
{
	for(int i=0; i<m_ScriptList.GetItemCount(); i++)
	{
		UINT nLen = StrToIntEX(m_ScriptList.GetItemText(i, 2), FALSE);
		if( (nLen<m_nMinLen) || (nLen==-1) )
			m_ScriptList.SetCheck(i);
	}
}

void CT2ScriptView::OnScriptviewFiltermax()
{
	for(int i=0; i<m_ScriptList.GetItemCount(); i++)
	{
		UINT nLen = StrToIntEX(m_ScriptList.GetItemText(i, 2), FALSE);
		if( (nLen>m_nMinLen) || (nLen==-1) )
			m_ScriptList.SetCheck(i);
	}
}

void CT2ScriptView::OnScriptviewFiltersaddress()
{
	for(int i=0; i<m_ScriptList.GetItemCount(); i++)
	{
		int nAddr = StrToIntEX(m_ScriptList.GetItemText(i, 1), FALSE);
		if( (nAddr&1) || (nAddr==-1) )
			m_ScriptList.SetCheck(i);
	}
}

void CT2ScriptView::OnScriptviewFilterdaddress()
{
	for(int i=0; i<m_ScriptList.GetItemCount(); i++)
	{
		int nAddr = StrToIntEX(m_ScriptList.GetItemText(i, 1), FALSE);
		if(!(nAddr&1) || (nAddr==-1))
			m_ScriptList.SetCheck(i);
	}
}

void CT2ScriptView::OnScriptviewFiltercheckword()
{	// 简单语法检查
	int nCount = m_ScriptList.GetItemCount();
	CString text;
	for(int nIndex=0; nIndex<nCount; nIndex++)
	{
		text = m_ScriptList.GetItemText(nIndex, 3);
		if(text.GetLength()<2)
			m_ScriptList.SetCheck(nIndex); // 单个文字
		/*else
		{
			// 其他
		}*/
	}
}

void CT2ScriptView::OnScriptviewFilterspecword()
{
	CString Text, Find=g_szBuffer;
	//theApp.GetExplorerBox()->GetDlgItem(IDC_EDIT_FINDTEXT)->GetWindowText(Find);
	if(Find.IsEmpty()) return;

	for(int i=0; i<m_ScriptList.GetItemCount(); i++)
	{
		Text = m_ScriptList.GetItemText(i, 3);
		if(Text.Find(Find)!=-1)
			m_ScriptList.SetCheck(i);
	}
}

void CT2ScriptView::OnScriptviewFilteracheck()
{
	for(int i=0; i<m_ScriptList.GetItemCount(); i++)
	{
		char c=
			(int)m_ScriptList.SendMessage(LVM_GETITEMSTATE, (WPARAM)i,
		(LPARAM)LVIS_STATEIMAGEMASK)>>13;
		m_ScriptList.SetCheck(i, !c);
	}
}

void CT2ScriptView::OnScriptviewCheckall()
{
	POSITION pos = m_ScriptList.GetFirstSelectedItemPosition();
	LVITEM lvi;
	lvi.stateMask = LVIS_STATEIMAGEMASK;
	lvi.state = 1<<13;
	if(pos)
	{
		do
		{
			int nIndex = m_ScriptList.GetNextSelectedItem(pos);
			//m_ScriptList.SetCheck(nIndex);
			m_ScriptList.SendMessage(LVM_SETITEMSTATE,
				(WPARAM)nIndex, (LPARAM)&lvi);
		}while(pos);
	}else
	{
		int nCount = m_ScriptList.GetItemCount();
		for(int nIndex=0; nIndex<nCount; nIndex++)
			//m_ScriptList.SetCheck(i);
			m_ScriptList.SendMessage(LVM_SETITEMSTATE,
				(WPARAM)nIndex, (LPARAM)&lvi);
	}
}

void CT2ScriptView::OnScriptviewFilteruncheck()
{
	POSITION pos = m_ScriptList.GetFirstSelectedItemPosition();
	LVITEM lvi;
	lvi.stateMask = LVIS_STATEIMAGEMASK;
	lvi.state = 1<<12;
	if(pos)
	{
		do
		{
			int nIndex = m_ScriptList.GetNextSelectedItem(pos);
			//m_ScriptList.SetCheck(nIndex, FALSE);
			m_ScriptList.SendMessage(LVM_SETITEMSTATE,
				(WPARAM)nIndex, (LPARAM)&lvi);
		}while(pos);
	}else
	{
		int nCount = m_ScriptList.GetItemCount();
		for(int nIndex=0; nIndex<nCount; nIndex++)
			//m_ScriptList.SetCheck(i, FALSE);
			m_ScriptList.SendMessage(LVM_SETITEMSTATE,
				(WPARAM)nIndex, (LPARAM)&lvi);
	}
}

void CT2ScriptView::OnScriptviewDelcheckitem()
{
	m_ScriptList.SetRedraw(FALSE);
	int nCount = m_ScriptList.GetItemCount();
	for(int i=nCount-1; i>=0; i--)
	{
		char c=
			(int)m_ScriptList.SendMessage(LVM_GETITEMSTATE, (WPARAM)i,
		(LPARAM)LVIS_STATEIMAGEMASK)>>13;
		//if(m_ScriptList.GetCheck(i))
		if(c)
			m_ScriptList.DeleteItem(i);
	}
	m_ScriptList.SetRedraw();
}

void CT2ScriptView::OnScriptviewExportcheckitem()
{
	static CTextFileDialog fd(FALSE, IDS_SCRIPTFILE);
	CString text;
	if(fd.DoModal()!=IDOK) return;
	CStdioFile ScriptFile;
	if(!ScriptFile.Open(fd.GetPathName(),
		CStdioFile::modeCreate|CStdioFile::modeWrite|CStdioFile::typeBinary))
	{
		ShowError();
		return;
	}

	char *lpMulit = NULL;
	if(fd.m_nCodePage==1200)
	{
		WORD c=0xFEFF;
		ScriptFile.Write(&c, 2);
	}else
	if(fd.m_nCodePage==1201)
	{
		WORD c=0xFFFE;
		ScriptFile.Write(&c, 2);
	}else
		lpMulit = new char[m_nMaxLen];
	BOOL bUsed=FALSE;
	CPINFO cpInfo;
	GetCPInfo(fd.m_nCodePage, &cpInfo);
	UINT nDefChar = *(WORD*)cpInfo.DefaultChar;

	int nCount = m_ScriptList.GetItemCount();
	for(int nIndex=0; nIndex<nCount; nIndex++)
	{
		//if(!m_ScriptList.GetCheck(nIndex)) continue;
		char c=
			(int)m_ScriptList.SendMessage(LVM_GETITEMSTATE, (WPARAM)nIndex,
		(LPARAM)LVIS_STATEIMAGEMASK)>>13;
		if(!c) continue;

		CString Script;
		text = m_ScriptList.GetItemText(nIndex, 0);
		if(text.GetAt(0)!=_T('?'))// 存在有效指针
			Script.AppendFormat(_T("*%s\r\n"), text);

		for(int i=1; i<4; i++)
		{// 地址, 大小 文本
			text = m_ScriptList.GetItemText(nIndex, i);
			if(i==3)
				Script.AppendFormat(_T("%s\r\n\r\n"), text);
			else
				Script.AppendFormat(_T("%s,"), text);
		}

		if(fd.m_nCodePage==1200)
			ScriptFile.WriteString(Script);
		else
		if(fd.m_nCodePage==1201)
		{
			LPTSTR lpChar = Script.GetBuffer();
			for(int i=0; i<Script.GetLength(); i++)
				lpChar[i] = (lpChar[i]>>8)|((lpChar[i]&0xFF)<<8);
			Script.ReleaseBuffer();
			ScriptFile.WriteString(Script);
		}else
		{
			LPCTSTR lpText = Script.GetBuffer();
			BOOL bUseddefchar=FALSE;
			::WideCharToMultiByte(fd.m_nCodePage, 0, lpText, -1,
				(LPSTR)lpMulit, Script.GetLength()*sizeof(TCHAR), (LPCSTR)&nDefChar, &bUseddefchar);
			Script.ReleaseBuffer();
			if(bUseddefchar && !bUsed)
			{
				bUsed = bUseddefchar;
				Hint(IDS_INVALIDCHAR, MB_OK|MB_ICONERROR);
			}
			ScriptFile.Write(lpMulit, (UINT)strlen(lpMulit));
		}
	}

	if(lpMulit) delete[] lpMulit;
}

void CT2ScriptView::OnEditFindtext()
{
	//theApp.GetExplorerBox()->Show(IDC_EDIT_FINDTEXT);
	OnScriptFind();
}

void CT2ScriptView::OnSearchCtrlendonly()
{
	m_nCtrlEndOnly = !m_nCtrlEndOnly;
}

void CT2ScriptView::OnUpdateSearchCtrlendonly(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nCtrlEndOnly);
}

void CT2ScriptView::OnTblTotalword()
{
	static CTextFileDialog fd(FALSE, IDS_TBLFILTER);
	CString text;
	if(fd.DoModal()!=IDOK) return;

	text = fd.GetPathName();
	CString tmp = PathFindExtension(text);
	tmp.MakeLower();
	if(tmp!=_T(".txt") && tmp!=_T(".tbl"))
		text.Append(_T(".TBL"));
	CFile file;
	if(!file.Open(text,
		CFile::modeCreate|CFile::modeWrite|CFile::typeBinary))
	{
		ShowError();
		return;
	}
	UINT nTmp;
	if(fd.m_nCodePage==1200)
	{
		nTmp=0xFEFF;
		file.Write(&nTmp, 2);
	}else
	if(fd.m_nCodePage==1201)
	{
		nTmp=0xFFFE;
		file.Write(&nTmp, 2);
	}

	TCHAR *pDTbl = new TCHAR[0x10000];
	TCHAR *pSTbl = new TCHAR[0x100];
	memset(pDTbl, 0, sizeof(TCHAR)*0x10000);
	memset(pSTbl, 0, sizeof(TCHAR)*0x100);

	int nCount = m_ScriptList.GetItemCount();
	for(int i=0; i<nCount; i++)
	{
		text = m_ScriptList.GetItemText(i, 3);
		int nLen = text.GetLength();
		for(int pos=0; pos<nLen; pos++)
		{
			TCHAR Char = text.GetAt(pos);

			if(m_nTblMode==CT2_TBL_USERTBL || fd.m_nCodePage<1200)
			{
				if(Char<=_T('~'))
					pSTbl[Char] = Char;
				else
					pDTbl[Char] = Char;
			}else
			if(fd.m_nCodePage==1200)
				pDTbl[Char] = Char;
			else
			if(fd.m_nCodePage==1201)
				pDTbl[Char] = (Char>>8)|(Char<<8);
		}
	}

	char out[4];
	CPINFO cpInfo;
	GetCPInfo(fd.m_nCodePage, &cpInfo);
	WORD nDefChar = *(WORD*)cpInfo.DefaultChar;
	BOOL bUsed=FALSE;

	if(fd.m_nCodePage==1200)
		nTmp = 0x000A000D;// \r\n
	else
	if(fd.m_nCodePage==1201)
		nTmp = 0x0A000D00;
	else
		nTmp = 0x0A0D;

	nCount = 0x100;
	char d=0;
	TCHAR *pt = pSTbl;
outd:
	for(int s=0; s<nCount; s++)
	{
		if(pt[s])
		{
			if(fd.m_nCodePage>=1200)
			{
				file.Write(&pt[s], 2);
				file.Write(&nTmp, 4);
			}else
			{
				BOOL bUseddefchar=FALSE;
				WideCharToMultiByte(fd.m_nCodePage, 0,
					&pt[s], 2,
					out, 4,
					(LPCSTR)&nDefChar, &bUseddefchar);
				if(bUseddefchar && !bUsed)
				{
					bUsed = bUseddefchar;
					Hint(IDS_INVALIDCHAR, MB_OK|MB_ICONERROR);
				}
				if(!bUseddefchar)
				{
					file.Write(out, pt[s]<=_T('~')?1:2);
					file.Write(&nTmp, 2);
				}
			}
		}
	}

	if(!d)
	{
		pt = pDTbl;
		nCount = 0x10000;
		d=1;
		goto outd;
	}
	delete[] pDTbl;
	delete[] pSTbl;
}

void CT2ScriptView::ScrollTo(UINT nOffset, BOOL bForce)
{
	//m_nBegin = nOffset;
	POINT Point;
	if(!GetDocument()->
		OffsetToPathName(nOffset, &Point).IsEmpty())
	{
		m_nBegin = m_nOffset = Point.x;
		m_nEnd = Point.y-1;
		if(m_nMaxLen>(UINT)(Point.y-Point.x))
			m_nMaxLen=Point.y-Point.x;
		//OnUpdateData();
	}
}

void CT2ScriptView::OnSetFocus(CWnd* pOldWnd)
{
	CT2View::OnSetFocus(pOldWnd);

	CRect rc; GetClientRect(&rc);
	m_ScriptList.SetWindowPos(NULL, 0, 0,
		rc.Width(), rc.Height(), SWP_NOMOVE);
	m_ScriptList.SetFocus();
}

void CT2ScriptView::OnScriptviewTblctrl()
{
	m_nTblMode=m_nTblMode==CT2_TBL_USERCTL?CT2_TBL_SYS:CT2_TBL_USERCTL;
	if(m_nTblMode==CT2_TBL_USERCTL)
	{
		CT2Doc *pDoc = GetDocument();
		if(pDoc->m_strTblName.IsEmpty())
		{
			pDoc->OnTblSeltbl();
			m_nTblMode = pDoc->m_strTblName.IsEmpty()?CT2_TBL_SYS:CT2_TBL_USERCTL;
		}
	}
	OnUpdateData();
	//Invalidate(FALSE);
}

void CT2ScriptView::OnUpdateScriptviewTblctrl(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nTblMode==CT2_TBL_USERCTL);
}

void CT2ScriptView::OnEditSelectAll()
{
	for(int i=0; i<m_ScriptList.GetItemCount(); i++)
	{
		m_ScriptList.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
	}
}

void CT2ScriptView::OnEditExportEx()
{
	static CTextFileDialog fd(FALSE, IDS_SCRIPTFILE);
	CString text;
	if(fd.DoModal()!=IDOK) return;
	text = PathFindExtension(fd.GetPathName());
	text.MakeLower();
	if(text!=_T(".txt"))
		text = fd.GetPathName()+_T(".txt");
	else
		text = fd.GetPathName();
    CStdioFile ScriptFile;
	if(!ScriptFile.Open(text,
		CStdioFile::modeCreate|CStdioFile::modeWrite|CStdioFile::typeBinary))
	{
		ShowError();
		return;
	}
	POSITION pos = m_ScriptList.GetFirstSelectedItemPosition();
	BOOL bAll = pos==NULL;
	char *lpMulit = NULL;
	UINT nBufferSize=m_nMaxLen;
	if(fd.m_nCodePage==1200)
	{
		WORD c=0xFEFF;
		ScriptFile.Write(&c, 2);
	}else
	if(fd.m_nCodePage==1201)
	{
		WORD c=0xFFFE;
		ScriptFile.Write(&c, 2);
	}else
	if(fd.m_nCodePage==65001)// UTF-8
	{
		UINT c=0xBFBBEF;
		ScriptFile.Write(&c, 3);
		lpMulit = new char[nBufferSize*=100];
	}else
		lpMulit = new char[nBufferSize*=100];

	BOOL bUsed=FALSE;
	CPINFO cpInfo;
	GetCPInfo(fd.m_nCodePage, &cpInfo);
	LPCSTR lpDefChar = fd.m_nCodePage==65001?NULL:(LPCSTR)cpInfo.DefaultChar;
	int nIndex=-1;

	CT2Doc *pDoc = GetDocument();
	/*CString RN = _T("\r\n");
	CString strPageLine=RN, strJPNLine=RN, strCHNLine=RN, strScriptEnd=RN;
	UINT pc=0, jc=0, cc=0;
	for(UINT i=0; i<pDoc->m_nMaxChar; i++)
	{
		UINT l = pDoc->m_strJPNDesc.GetLength();

		if(l && jc==((pDoc->m_nMaxChar-l)/2))
		{
			strJPNLine.Append(pDoc->m_strJPNDesc);
			jc += l;
		}
		l = pDoc->m_strCHNDesc.GetLength();
		if(l && cc==((pDoc->m_nMaxChar-l)/2))
		{
			strCHNLine.Append(pDoc->m_strCHNDesc);
			cc += l;
		}
		l = pDoc->m_strPAGEDesc.GetLength();
		if(l && pc==((pDoc->m_nMaxChar-l)/2))
		{
			strPageLine.Append(pDoc->m_strPAGEDesc);
			pc += l;
		}

		if(jc<pDoc->m_nMaxChar)
			strJPNLine.AppendChar(pDoc->m_nJPNChar);
		if(cc<pDoc->m_nMaxChar)
			strCHNLine.AppendChar(pDoc->m_nCHNChar);
		if(pc<pDoc->m_nMaxChar)
			strPageLine.AppendChar(pDoc->m_nPAGEChar);
		jc++; cc++; pc++;
		strScriptEnd.AppendChar(pDoc->m_nJPNChar);
	}
	strJPNLine.Append(RN);
	strCHNLine.Append(RN);
	strPageLine.Append(RN);
	strScriptEnd.Append(RN);

	CString line; line.Format(_T("[%s]") , GetDocument()->m_strLine);
	CString page; page.Format(_T("[%s]") , GetDocument()->m_strPage);
*/

	while(pos || bAll)
	{
		if(bAll)
		{
			nIndex++;
			if(nIndex>=m_ScriptList.GetItemCount())
				break;
		}else
			nIndex = m_ScriptList.GetNextSelectedItem(pos);
		CString Script;
		text = m_ScriptList.GetItemText(nIndex, 0);
		Script.AppendFormat(_T("*%s,"), text);

		for(int i=1; i<4; i++)
		{// 地址, 大小 文本
			text = m_ScriptList.GetItemText(nIndex, i);
			switch(i)
			{
			case 1:	// 地址
				Script.AppendFormat(_T("%s,"), text);
				break;
			case 2: // 大小
				Script.AppendFormat(_T("%s"), text);
				continue;
			case 3: // 文本
				//text.Replace(line, RN);
				//text.Replace(page, strPageLine);
				//Script.AppendFormat(_T("%s%s%s%s%s\r\n"), strJPNLine, text, strCHNLine, text, strScriptEnd);
				{
				POSITION pos = pDoc->m_VisualScript.GetHeadPosition();
				while(pos)
				{
					VISUALSCRIPT &vs = pDoc->m_VisualScript.GetNext(pos);
					text.Replace(vs.strCtrl, vs.strVisual);
				}
				Script.AppendFormat(_T("%s%s%s%s%s\r\n"), pDoc->m_strScriptBegin, text, pDoc->m_strCHN, text, pDoc->m_strScriptEnd);
				}break;
			}
		}

		if(fd.m_nCodePage==1200)
			ScriptFile.WriteString(Script);
		else
		if(fd.m_nCodePage==1201)
		{
			LPTSTR lpChar = Script.GetBuffer();
			for(int i=0; i<Script.GetLength(); i++)
				lpChar[i] = (lpChar[i]>>8)|((lpChar[i]&0xFF)<<8);
			Script.ReleaseBuffer();
			ScriptFile.WriteString(Script);
		}else
		{
			LPCTSTR lpText = Script.GetBuffer();
			BOOL bUseddefchar=FALSE;
			::WideCharToMultiByte(fd.m_nCodePage, 0, lpText, -1,
				(LPSTR)lpMulit, nBufferSize/*Script.GetLength()*sizeof(TCHAR)*/,
				lpDefChar, fd.m_nCodePage==65001?NULL:&bUseddefchar);
			Script.ReleaseBuffer();
			if(bUseddefchar && !bUsed)
			{
				bUsed = bUseddefchar;
				Hint(IDS_INVALIDCHAR, MB_OK|MB_ICONERROR);
			}
			ScriptFile.Write(lpMulit, (UINT)strlen(lpMulit));
		}
	}

	if(lpMulit) delete[] lpMulit;
}

void CT2ScriptView::AppendVar()
{
	CCrystalTreeCtrl &tc = ((CT2MainFrm*)theApp.GetMainWnd())->m_TreeBar.m_Tree;
	int i=1;
	UINT nMax=GetDocument()->m_nRomSize-1;
	tc.AppendVal(i++, &m_nBegin, CTCS_UINT, 0, nMax);
	tc.AppendVal(i++, &m_nEnd, CTCS_UINT, 0, nMax);
	tc.AppendVal(i++, &m_nMinLen, CTCS_UINT, 1, 9999);
	tc.AppendVal(i++, &m_nMaxLen, CTCS_UINT, 1, 9999);
	tc.AppendVal(i++, &m_nAlign2, CTCS_BYTE);
	tc.AppendVal(i++, &m_nNoWord, CTCS_BYTE);
	tc.AppendVal(i++, &m_nCtrlEndOnly, CTCS_BYTE);
	tc.AppendVal(i++, &m_nTblMode, CTCS_BYTE);
	tc.AppendVal(i++, &m_nSwpCode, CTCS_BYTE);

	tc.AppendVal(i, &GetDocument()->m_nCurCp, CTCS_BYTE);
	CString strItem;
	strItem.LoadString(IDS_DEFCODEPAGE);
	for(UINT i2=0; i2<theApp.m_nCPCount; i2++)
		strItem.AppendFormat(_T(";%s"), GetCodePageDescription(theApp.m_pCodePage[i2]));
	int v=0;
	HTREEITEM hItem = tc.IndexToItem(i, tc.GetRootItem(), v);
	tc.SetVarData(hItem, strItem);
	i+=2;
	tc.AppendVal(i++, &m_nBaseAddress, CTCS_UINT, 0, -1);
	tc.AppendVal(i++, &m_bRelativePoint, CTCS_BYTE);
	tc.AppendVal(i++, &m_nPointBegin, CTCS_UINT, 0, nMax);
	tc.AppendVal(i, &m_nPointEnd, CTCS_UINT, 0, nMax);
	i+=2;
	tc.AppendVal(i++, &m_bNewAddress, CTCS_BYTE);
	tc.AppendVal(i++, &m_nNewAddress, CTCS_BYTE, 0, nMax);
	tc.AppendVal(i, &g_szBuffer, CTCS_LPTSTR, 0, _MAX_PATH);
}
/*
void CT2ScriptView::AddMacro_OpenScript(CString path, UINT nCodePage)
{
	CT_MACRO_1VAL_1FILE m;
	INITMACRO(m, CT2SCRIPTVIEW_OPENSCRIPT);

	m.nVal = nCodePage;
	lstrcpyn(m.szFile, path, _MAX_PATH);

	AddMacro(&m);
}

void CT2ScriptView::AddMacro_Import()
{
	CT_MACRO_IMPORTTEXT m;
	INITMACRO(m, CT2SCRIPTVIEW_IMPORT);

	m.nNewAddress = m_nNewAddress;
	m.nSwpCode = m_nSwpCode;
	m.nAlign2 = m_nAlign2;
	m.bNewAddress = m_bNewAddress;
	m.nNewCode = m_nNewCode;
	m.nNoDefHint = m_nNoDefHint;
	m.nSaveTbl = !m_strNewCode.IsEmpty();
	m.nTblMode = m_nTblMode;
	m.nCodePage = GetDocument()->m_nScriptCodePage;
	m.bRelativePoint = m_bRelativePoint;
	m_strNewCode.Empty();

	AddMacro(&m);
}
*/