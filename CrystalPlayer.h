#define CRYSTALPLAYER

#ifdef CRYSTALPLAYER
#pragma once

#include <mmsystem.h>

class CCrystalPlayer
{
public:
	UINT m_uTimerID;
	LPSTR m_lpData;
	WAVEFORMATEX m_wfx;
	WAVEHDR m_wh;

	WORD m_wAdpcmTable[89];
	char m_sIndexTable[8];

	CCrystalPlayer(void);
	virtual ~CCrystalPlayer(void);

	MMRESULT Open(WAVEFORMATEX *pwfx);
	MMRESULT Close();

	MMRESULT SetVolume(DWORD dwVolume);
	MMRESULT GetVolume(LPDWORD pdwVolume);

	MMRESULT PrepareHeader(LPSTR lpData, UINT nSize, BOOL bAutoFree=TRUE);
	MMRESULT UnprepareHeader();

	MMRESULT Write(LPSTR lpData, UINT nSize, BOOL bAutoFree=TRUE);

	MMRESULT Reset();
	MMRESULT Pause();

	void Clear();
	MMRESULT Message(UINT uMsg, DWORD_PTR dw1=0, DWORD_PTR dw2=0);
	CString GetErrorText(MMRESULT err);

	void Play();
	void Play(LPSTR lpData, UINT nSize);
};
#endif
