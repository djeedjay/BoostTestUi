//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

// Derived from atlgdix.h:
// Written by Bjarke Viksoe (bjarke@viksoe.dk)
// Copyright (c) 2001-2002 Bjarke Viksoe.
// Thanks to Daniel Bowen for COffscreenDrawRect.

#ifndef BOOST_TESTUI_OFFSCREENDRAW_H
#define BOOST_TESTUI_OFFSCREENDRAW_H

#pragma once

namespace WTL {

class CMemDC : public CDC
{
public:
	CMemDC(HDC hDC, LPRECT pRect = nullptr)
	{
		ATLASSERT(hDC);
		m_dc = hDC;
		if (pRect)
			m_rc = *pRect;
		else
			m_dc.GetClipBox(&m_rc);

		CreateCompatibleDC(m_dc);
		::LPtoDP(m_dc, (LPPOINT) &m_rc, sizeof(RECT) / sizeof(POINT));
		m_bitmap.CreateCompatibleBitmap(m_dc, m_rc.right - m_rc.left, m_rc.bottom - m_rc.top);
		m_hOldBitmap = SelectBitmap(m_bitmap);
		::DPtoLP(m_dc, (LPPOINT) &m_rc, sizeof(RECT) / sizeof(POINT));
		SetWindowOrg(m_rc.left, m_rc.top);
	}

	~CMemDC()
	{
		// Copy the offscreen bitmap onto the screen.
		m_dc.BitBlt(m_rc.left, m_rc.top, m_rc.right - m_rc.left, m_rc.bottom - m_rc.top,
					m_hDC, m_rc.left, m_rc.top, SRCCOPY);
		// Swap back the original bitmap.
		SelectBitmap(m_hOldBitmap);
	}

	CDCHandle     m_dc;          // Owner DC
	CBitmap       m_bitmap;      // Offscreen bitmap
	CBitmapHandle m_hOldBitmap;  // Originally selected bitmap
	RECT          m_rc;          // Rectangle of drawing area
};


/////////////////////////////////////////////////////////////////////////////
// COffscreenDraw

template <typename T>
class COffscreenDraw
{
public:
	BEGIN_MSG_MAP(COffscreenDraw)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_PRINTCLIENT, OnPaint)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
	END_MSG_MAP()

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		if (wParam)
		{
			CMemDC memdc(reinterpret_cast<HDC>(wParam), nullptr);
			pT->DoPaint(memdc.m_hDC);
		}
		else
		{
			RECT rc;
			pT->GetClientRect(&rc);
			CPaintDC dc(pT->m_hWnd);
			CMemDC memdc(dc.m_hDC, &rc);
			pT->DoPaint(memdc.m_hDC);
		}
		return 0;
	}

	LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return 1; // handled; no need to erase background; do it in DoPaint();
	}

	void DoPaint(CDCHandle dc)
	{
		ATLASSERT(false); // must override this
	}
};

template <typename T>
class COffscreenDrawRect
{
public:
	BEGIN_MSG_MAP(COffscreenDrawRect)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_PRINTCLIENT, OnPaint)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
	END_MSG_MAP()

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		if (wParam)
		{
			CMemDC memdc(reinterpret_cast<HDC>(wParam), nullptr);
			pT->DoPaint(memdc.m_hDC, memdc.m_rc);
		}
		else
		{
			CPaintDC dc(pT->m_hWnd);
			CMemDC memdc(dc.m_hDC, &dc.m_ps.rcPaint);
			pT->DoPaint(memdc.m_hDC, dc.m_ps.rcPaint);
		}
		return 0;
	}

	LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return 1; // handled; no need to erase background; do it in DoPaint();
	}

	void DoPaint(CDCHandle dc, const RECT& rcClip)
	{
		ATLASSERT(false); // must override this
	}
};

} // namespace WTL

#endif // BOOST_TESTUI_OFFSCREENDRAW_H
