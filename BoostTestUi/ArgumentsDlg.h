// (C) Copyright Gert-Jan de Vos 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)

// See http://boosttestui.wordpress.com/ for the boosttestui home page.

#ifndef BOOST_TESTUI_ARGUMENTSDLG_H
#define BOOST_TESTUI_ARGUMENTSDLG_H

#pragma once

#include "AtlWinExt.h"
#include "Resource.h"

namespace gj {

class CArgumentsDlg :
	public CDialogImpl<CArgumentsDlg>,
	public CDialogResize<CArgumentsDlg>,
	public ExceptionHandler<CArgumentsDlg, std::exception>
{
public:
	explicit CArgumentsDlg(const std::wstring& arguments);
	std::wstring GetArguments() const;

	static const int IDD = IDD_ARGUMENTS;

	DECLARE_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(CArgumentsDlg)
		DLGRESIZE_CONTROL(IDC_ARGUMENTS, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDOK, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDCANCEL, DLSZ_MOVE_X)
	END_DLGRESIZE_MAP()

private:
	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnGetMinMaxInfo(MINMAXINFO* pInfo);
	void OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);
	void OnOk(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);
	void OnException();
	void OnException(const std::exception& ex);

	std::wstring m_arguments;
};

} // namespace gj

#endif // BOOST_TESTUI_ARGUMENTSDLG_H
