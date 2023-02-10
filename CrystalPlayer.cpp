#include "StdAfx.h"
#include ".\CrystalPlayer.h"

#ifdef CRYSTALPLAYER

#include "Common.h"

#pragma comment(lib, "winmm.lib")
/*
int indextbl[8] =
{
  -1, -1, -1, -1, 2, 4, 6, 8
};
*/
DWORD g_dwMilliSeconds = 0;	// ms
HWAVEOUT g_hWaveOut = NULL;
void CALLBACK MMTimeCallBack(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
{
}

void CALLBACK MMDrvCallBack(HDRVR hdrvr, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
{
	switch(uMsg)
	{
	case MM_WOM_OPEN:
		break;
	case WOM_CLOSE:
		break;
	case WOM_DONE:
		{
		}break;
	}
}

CCrystalPlayer::CCrystalPlayer(void)
{
	memset(&m_wfx, 0, sizeof(WAVEFORMATEX));
	m_wfx.wFormatTag = WAVE_FORMAT_PCM;
	m_wfx.nChannels = 1;
	m_wfx.nSamplesPerSec = 11025;
	//m_wfx.nAvgBytesPerSec = m_wfx.nSamplesPerSec * ((16 / 8) * m_wfx.nChannels);
	//m_wfx.nBlockAlign = (16 / 8) * m_wfx.nChannels;
	m_wfx.wBitsPerSample = 8;
	//m_wfx.cbSize = sizeof(WAVEFORMATEX);

	memset(&m_wh, 0, sizeof(m_wh));
	m_wh.dwUser = 1;

	m_lpData	= NULL;

	UINT X=0x000776D2;
	for(int i=0; i<89; i++)
	{
		m_wAdpcmTable[i] = X>>16;
		X += X/10;
	}
	m_wAdpcmTable[3]	= 0x0A;
	m_wAdpcmTable[4]	= 0x0B;
	m_wAdpcmTable[88]	= 0x7FFF;

	m_sIndexTable[0] = -1;
	m_sIndexTable[1] = -1;
	m_sIndexTable[2] = -1;
	m_sIndexTable[3] = -1;
	m_sIndexTable[4] = 2;
	m_sIndexTable[5] = 4;
	m_sIndexTable[6] = 6;
	m_sIndexTable[7] = 8;
}

CCrystalPlayer::~CCrystalPlayer(void)
{
	Close();
}

MMRESULT CCrystalPlayer::Open(WAVEFORMATEX *pwfx)
{
	Close();
	//m_uTimerID = timeSetEvent(120, 220, MMTimeCallBack, m_wh.dwUser, TIME_CALLBACK_FUNCTION|TIME_PERIODIC);

	memcpy(&m_wfx, pwfx, sizeof(m_wfx));
	m_wfx.nBlockAlign = pwfx->wBitsPerSample/8 * pwfx->nChannels;
	m_wfx.nAvgBytesPerSec = pwfx->nSamplesPerSec * pwfx->nBlockAlign;

	return waveOutOpen(&g_hWaveOut, 0, pwfx,
		(DWORD_PTR)MMDrvCallBack, (DWORD_PTR)AfxGetInstanceHandle(), CALLBACK_FUNCTION);
}

MMRESULT CCrystalPlayer::Close()
{
	if(g_hWaveOut)
	{
		Reset();
		Clear();
		//timeKillEvent(m_uTimerID);
		return waveOutClose(g_hWaveOut);
	}
	return MMSYSERR_NOERROR;
}

void CCrystalPlayer::Clear()
{
	if(m_lpData)
	{
		delete[] m_lpData;
		m_lpData = NULL;
	}
}

MMRESULT CCrystalPlayer::SetVolume(DWORD dwVolume)
{
	if(g_hWaveOut)
		return waveOutSetVolume(g_hWaveOut, dwVolume);
	return MMSYSERR_NOERROR;
}

MMRESULT CCrystalPlayer::GetVolume(LPDWORD pdwVolume)
{
	if(g_hWaveOut)
		return waveOutGetVolume(g_hWaveOut, pdwVolume);
	return MMSYSERR_NOERROR;
}

MMRESULT CCrystalPlayer::PrepareHeader(LPSTR lpData, UINT nSize, BOOL bAutoFree)
{
	if(g_hWaveOut)
	{
		Clear();
		if(bAutoFree)
			m_lpData = lpData;

		m_wh.lpData = lpData;
		m_wh.dwBufferLength = nSize;

		g_dwMilliSeconds = nSize / m_wfx.nBlockAlign / m_wfx.nSamplesPerSec / 1000;

		return waveOutPrepareHeader(g_hWaveOut, &m_wh, sizeof(WAVEHDR));
	}
	return MMSYSERR_NOERROR;
}

MMRESULT CCrystalPlayer::UnprepareHeader()
{
	if(g_hWaveOut)
	{
		Clear();
		return waveOutUnprepareHeader(g_hWaveOut, &m_wh, sizeof(WAVEHDR));
	}
	return MMSYSERR_NOERROR;
}

MMRESULT CCrystalPlayer::Write(LPSTR lpData, UINT nSize, BOOL bAutoFree)
{
	if(g_hWaveOut)
	{
		PrepareHeader(lpData, nSize, bAutoFree);
		return waveOutWrite(g_hWaveOut, &m_wh, sizeof(WAVEHDR));
	}
	return MMSYSERR_NOERROR;
}

MMRESULT CCrystalPlayer::Reset()
{
	if(g_hWaveOut)
		return waveOutReset(g_hWaveOut);
	return MMSYSERR_NOERROR;
}

MMRESULT CCrystalPlayer::Pause()
{
	if(g_hWaveOut)
	{
		return waveOutPause(g_hWaveOut);
		// return waveOutRestart(g_hWaveOut);
	}
	return MMSYSERR_NOERROR;
}

MMRESULT CCrystalPlayer::Message(UINT uMsg, DWORD_PTR dw1, DWORD_PTR dw2)
{
	if(g_hWaveOut)
		return waveOutMessage(g_hWaveOut, uMsg, dw1, dw2);
	return MMSYSERR_NOERROR;
}

CString CCrystalPlayer::GetErrorText(MMRESULT err)
{
	waveOutGetErrorText(err, g_szBuffer, _MAX_PATH);
	return g_szBuffer;
}

void CCrystalPlayer::Play()
{
	MMRESULT mmr;

	CFileDialog fd(TRUE);
	CFile f;
	if( fd.DoModal()==IDOK &&
		f.Open(fd.GetPathName(), CFile::modeRead|CFile::typeBinary)  )
	{
		WAVEFILE wf;
		f.Read(&wf, sizeof(wf));
		static WAVEFORMATEX wfx;
		f.Read(&wfx, sizeof(wfx));
		WAVEDATA wd;
		f.Read(&wd, sizeof(wd));
		UINT nSize = (UINT)(f.GetLength()-f.GetPosition());
		if(nSize>wd.nWaveSize)
			nSize = wd.nWaveSize;
		BYTE *pSound = new BYTE[nSize];
		f.Read(pSound, nSize);
		f.Close();

		mmr = Open(&wfx);
		if(mmr==MMSYSERR_NOERROR)
		{
			mmr = SetVolume(-1);

			//m_wh.dwBufferLength = nSize;
			//m_wh.lpData = (LPSTR)pSound;//(LPSTR)((BYTE*)pwd+sizeof(WAVEDATA));

			mmr = Write((LPSTR)pSound, nSize, TRUE);

			//mmr = Close();
		}
		//delete[] pSound;
	}
}

void CCrystalPlayer::Play(LPSTR lpData, UINT nSize)
{
	// m_wfx.nChannels		= 2 / 1;
	// m_wfx.nSamplesPerSec	= 44100 / 22050;
	// m_wfx.wBitsPerSample	= 16 / 8;
	MMRESULT mmr = Open(&m_wfx);
	if(mmr==MMSYSERR_NOERROR)
	{
		//m_wh.dwBufferLength = nSize;
		//m_wh.lpData = (LPSTR)pData;
		mmr = Write(lpData, nSize, FALSE);
	}else
	{
		waveOutGetErrorText(mmr, g_szBuffer, _MAX_PATH);
		Hint(g_szBuffer, MB_OK|MB_ICONERROR);
	}
}

#endif