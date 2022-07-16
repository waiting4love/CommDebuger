
#pragma once

template <class T>
class CColorCtrl
	: public CWindowImpl<CColorCtrl<T>,T>
{
public:
    BEGIN_MSG_MAP_EX(CColorCtrl<T>)
        MSG_OCM_CTLCOLORSTATIC( SetStaticColor )
		MSG_OCM_CTLCOLOREDIT( SetStaticColor )
        DEFAULT_REFLECTION_HANDLER()
    END_MSG_MAP()

	CColorCtrl() : m_dwTextColor(0),m_dwBkColor(0xffffff),m_iBkMode(TRANSPARENT),m_Brush((HBRUSH)GetStockObject
(NULL_BRUSH)){;}

	void SetTextColor(COLORREF Color){
		if(m_dwTextColor == Color) return;
		m_dwTextColor = Color;
		Invalidate();
	}

	COLORREF GetTextColor(){
		return m_dwTextColor;
	}

	void SetBkColor(COLORREF Color){
		if(m_dwBkColor == Color) return;
		m_dwBkColor = Color;
		Invalidate();
	}

	COLORREF GetBkColor(){
		return m_dwBkColor;
	}

	void SetBkMode(int BkMode){
		if(m_iBkMode == BkMode) return;

		m_iBkMode = BkMode;
		Invalidate();
	}

	int GetBkMode(){
		return m_iBkMode;
	}

	HBRUSH SetBrush(HBRUSH Brush){
		HBRUSH OldBrush = m_Brush;
		m_Brush = Brush;
		Invalidate();
		return OldBrush;
	}

	HBRUSH GetBrush(){
		return m_Brush;
	}

	void SetTheme(COLORREF TextColor, COLORREF BkColor = 0xffffff, int BkMode = TRANSPARENT, HBRUSH Brush = GetStockObject
(NULL_BRUSH))
	{
		if(TextColor == m_dwTextColor && BkColor == m_dwBkColor
			&& BkMode == m_iBkMode && Brush == m_Brush) return;
		
		m_dwTextColor = TextColor;
		m_dwBkColor = BkColor;
		m_iBkMode = BkMode;
		m_Brush = Brush;

		Invalidate();
	}
private:

	LRESULT SetStaticColor(HDC dc,HWND)
	{
		::SetTextColor(dc,m_dwTextColor);

		::SetBkMode(dc,m_iBkMode);
		if(m_iBkMode != TRANSPARENT)  ::SetBkColor(dc,m_dwBkColor);

		return (LRESULT) m_Brush;
	}
	
	COLORREF m_dwTextColor,m_dwBkColor;
	int m_iBkMode;
	HBRUSH m_Brush;
};

typedef CColorCtrl<CStatic> CColorStatic;
typedef CColorCtrl<CEdit> CColorEdit;