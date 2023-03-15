#pragma once

// 머신에서 Microsoft Visual C++를 사용하여 생성한 IDispatch 래퍼 클래스입니다.

// 참고: 이 파일의 콘텐츠를 수정하지 마세요. Microsoft Visual C++를 통해 이 클래스가 다시 생성될 경우 
// 수정 내용을 덮어씁니다.

/////////////////////////////////////////////////////////////////////////////

#include "afxwin.h"

class CNTGRAPH_TORQ : public CWnd
{
protected:
	DECLARE_DYNCREATE(CNTGRAPH_TORQ)
public:
	CLSID const& GetClsid()
	{
		static CLSID const clsid
			= {0xc9fe01c2,0x2746,0x479b,{0x96,0xab,0xe0,0xbe,0x99,0x31,0xb0,0x18}};
		return clsid;
	}
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle,
						const RECT& rect, CWnd* pParentWnd, UINT nID, 
						CCreateContext* pContext = nullptr)
	{ 
		return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID);
	}

	BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd,
				UINT nID, CFile* pPersist = nullptr, BOOL bStorage = FALSE,
				BSTR bstrLicKey = nullptr)
	{ 
		return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID,
		pPersist, bStorage, bstrLicKey); 
	}

// 특성
public:
enum
{
	Solid = 0,
	Dash = 1,
	Dot = 2,
	DashDot = 3,
	DashDotDot = 4,
	Null = 5,
	XYStep = 6,
	YXStep = 7,
	Bars = 8,
	Stick = 9
}LineType;

enum
{
	Nosym = 0,
	Dots = 1,
	Rectangles = 2,
	Diamonds = 3,
	Asterisk = 4,
	DownTriangles = 5,
	UpTriangles = 6,
	LeftTriangles = 7,
	RightTriangles = 8
}SymbolType;

enum
{
	Flat = 0,
	Scope = 1,
	Bitmap = 2
}FrameStyle;

enum
{
	None = 0,
	Track = 1,
	Zoom = 2,
	PanXY = 3,
	PanX = 4,
	PanY = 5
}TrackModeState;



// 작업
public:
// _DNTGraph

// 함수
//

	void SetRange(double xmin, double xmax, double ymin, double ymax)
	{
		static BYTE parms[] = VTS_R8 VTS_R8 VTS_R8 VTS_R8;
		InvokeHelper(0x35, DISPATCH_METHOD, VT_EMPTY, nullptr, parms, xmin, xmax, ymin, ymax);
	}
	void AutoRange()
	{
		InvokeHelper(0x36, DISPATCH_METHOD, VT_EMPTY, nullptr, nullptr);
	}
	void CopyToClipboard()
	{
		InvokeHelper(0x37, DISPATCH_METHOD, VT_EMPTY, nullptr, nullptr);
	}
	void PrintGraph()
	{
		InvokeHelper(0x38, DISPATCH_METHOD, VT_EMPTY, nullptr, nullptr);
	}
	void AddElement()
	{
		InvokeHelper(0x39, DISPATCH_METHOD, VT_EMPTY, nullptr, nullptr);
	}
	void DeleteElement(short ElementID)
	{
		static BYTE parms[] = VTS_I2;
		InvokeHelper(0x3A, DISPATCH_METHOD, VT_EMPTY, nullptr, parms, ElementID);
	}
	void ClearGraph()
	{
		InvokeHelper(0x3B, DISPATCH_METHOD, VT_EMPTY, nullptr, nullptr);
	}
	double get_ElementXValue(long index, short ElementID)
	{
		double result;
		static BYTE parms[] = VTS_I4 VTS_I2;
		InvokeHelper(0x44, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, parms, index, ElementID);
		return result;
	}
	void put_ElementXValue(long index, short ElementID, double newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_I2 VTS_R8;
		InvokeHelper(0x44, DISPATCH_PROPERTYPUT, VT_EMPTY, nullptr, parms, ElementID, newValue);
	}
	double get_ElementYValue(long index, short ElementID)
	{
		double result;
		static BYTE parms[] = VTS_I4 VTS_I2;
		InvokeHelper(0x45, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, parms, index, ElementID);
		return result;
	}
	void put_ElementYValue(long index, short ElementID, double newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_I2 VTS_R8;
		InvokeHelper(0x45, DISPATCH_PROPERTYPUT, VT_EMPTY, nullptr, parms, ElementID, newValue);
	}
	void PlotXY(double X, double Y, short ElementID)
	{
		static BYTE parms[] = VTS_R8 VTS_R8 VTS_I2;
		InvokeHelper(0x3C, DISPATCH_METHOD, VT_EMPTY, nullptr, parms, X, Y, ElementID);
	}
	void PlotY(double Y, short ElementID)
	{
		static BYTE parms[] = VTS_R8 VTS_I2;
		InvokeHelper(0x3D, DISPATCH_METHOD, VT_EMPTY, nullptr, parms, Y, ElementID);
	}
	void ShowProperties()
	{
		InvokeHelper(0x3E, DISPATCH_METHOD, VT_EMPTY, nullptr, nullptr);
	}
	void SaveAs(LPCTSTR szFilename)
	{
		static BYTE parms[] = VTS_BSTR;
		InvokeHelper(0x3F, DISPATCH_METHOD, VT_EMPTY, nullptr, parms, szFilename);
	}
	void AddAnnotation()
	{
		InvokeHelper(0x40, DISPATCH_METHOD, VT_EMPTY, nullptr, nullptr);
	}
	void DeleteAnnotation(short annoID)
	{
		static BYTE parms[] = VTS_I2;
		InvokeHelper(0x41, DISPATCH_METHOD, VT_EMPTY, nullptr, parms, annoID);
	}
	void AddCursor()
	{
		InvokeHelper(0x42, DISPATCH_METHOD, VT_EMPTY, nullptr, nullptr);
	}
	void DeleteCursor(short cursorID)
	{
		static BYTE parms[] = VTS_I2;
		InvokeHelper(0x43, DISPATCH_METHOD, VT_EMPTY, nullptr, parms, cursorID);
	}
	void AboutBox()
	{
		InvokeHelper(0xFFFFFDD8, DISPATCH_METHOD, VT_EMPTY, nullptr, nullptr);
	}

// 속성
//

// _DNTGraphEvents

// 함수
//

	void CursorPosition(double X, double Y)
	{
		static BYTE parms[] = VTS_R8 VTS_R8;
		InvokeHelper(0x1, DISPATCH_METHOD, VT_EMPTY, nullptr, parms, X, Y);
	}
	void TrackModeChanged(long lNewState)
	{
		static BYTE parms[] = VTS_I4;
		InvokeHelper(0x2, DISPATCH_METHOD, VT_EMPTY, nullptr, parms, lNewState);
	}
	void Click()
	{
		InvokeHelper(0xFFFFFDA8, DISPATCH_METHOD, VT_EMPTY, nullptr, nullptr);
	}

// 속성
//



};
