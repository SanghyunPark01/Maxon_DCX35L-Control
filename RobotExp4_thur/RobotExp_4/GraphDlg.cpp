// GraphDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "RobotExp_4.h"
#include "GraphDlg.h"
#include "afxdialogex.h"

#define RED RGB(127,0,0)
#define GREEN RGB(0,127,0)
#define BLUE RGB(0,0,127)

#define BLACK RGB(0,0,0)
#define WHITE RGB(255,255,255)

#define MAX_POS 180
#define MAX_Vel 180
#define MAX_Torque 0.15
// CGraphDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CGraphDlg, CDialogEx)

CGraphDlg::CGraphDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_GRAPH_DIALOG, pParent)
{

}

CGraphDlg::~CGraphDlg()
{
}

void CGraphDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_NTGRAPH_POS, m_ntgPOS);
	DDX_Control(pDX, IDC_NTGRAPH_VEL, m_ntgVel);
	DDX_Control(pDX, IDC_NTGRAPH_TORQ, m_ntgTorque);
}


BEGIN_MESSAGE_MAP(CGraphDlg, CDialogEx)
	ON_WM_TIMER()
END_MESSAGE_MAP()

//추가
BOOL CGraphDlg::OnInitDialog() {
	CDialogEx::OnInitDialog();

	InitNTGraph();

	return TRUE;
}
// CGraphDlg 메시지 처리기입니다.
void CGraphDlg::InitNTGraph() {
	m_ntgPOS.ClearGraph();
	m_ntgVel.ClearGraph();
	m_ntgTorque.ClearGraph();

	m_ntgPOS.SetFrameStyle(0);
	m_ntgVel.SetFrameStyle(0);
	m_ntgTorque.SetFrameStyle(0);

	m_ntgPOS.SetPlotAreaColor(WHITE);
	m_ntgVel.SetPlotAreaColor(WHITE);
	m_ntgTorque.SetPlotAreaColor(WHITE);

	m_ntgPOS.SetShowGrid(TRUE);
	m_ntgVel.SetShowGrid(TRUE);
	m_ntgTorque.SetShowGrid(TRUE);

	m_ntgPOS.SetFormatAxisBottom(_T("%.2f"));
	m_ntgVel.SetFormatAxisBottom(_T("%.2f"));
	m_ntgTorque.SetFormatAxisBottom(_T("%.2f"));

	m_ntgPOS.SetCaption(_T("위치"));
	m_ntgVel.SetCaption(_T("속도"));
	m_ntgTorque.SetCaption(_T("토크"));

	m_ntgPOS.SetXLabel(_T("Time[s]"));
	m_ntgVel.SetXLabel(_T("Time[s]"));
	m_ntgTorque.SetXLabel(_T("Time[s]"));

	m_ntgPOS.SetYLabel(_T("Degree[deg]"));
	m_ntgVel.SetYLabel(_T("Velocity[deg/s]"));
	m_ntgTorque.SetYLabel(_T("Torque[Nm]"));

	m_ntgPOS.AddElement();
	m_ntgPOS.SetElementWidth(3);
	m_ntgPOS.SetElementLineColor(RED);
	m_ntgPOS.AddElement();
	m_ntgPOS.SetElementWidth(3);
	m_ntgPOS.SetElementLineColor(BLUE);

	m_ntgPOS.SetRange(0.0, 10.0, -MAX_POS, MAX_POS);
	m_ntgPOS.SetYGridNumber(4);

	m_ntgVel.AddElement();
	m_ntgVel.SetElementWidth(4);
	m_ntgVel.SetElementLineColor(RED);
	m_ntgVel.AddElement();
	m_ntgVel.SetElementWidth(3);
	m_ntgVel.SetElementLineColor(BLUE);

	m_ntgVel.SetRange(0.0, 10.0, -MAX_Vel, MAX_Vel);
	m_ntgVel.SetYGridNumber(4);

	m_ntgTorque.AddElement();
	m_ntgTorque.SetElementWidth(4);
	m_ntgTorque.SetElementLineColor(RED);
	m_ntgTorque.AddElement();
	m_ntgTorque.SetElementWidth(3);
	m_ntgTorque.SetElementLineColor(BLUE);

	m_ntgTorque.SetRange(0.0, 10.0, -MAX_Torque, MAX_Torque);
	m_ntgTorque.SetYGridNumber(4);

	SetTimer(1, 100, NULL);
}

void CGraphDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_dCnt += 0.1;
	DataType_t jointData;
	ControlData_t motor_data;
	ControlData_t motor_data_target;
	GET_SYSTEM_MEMORY("JointData", jointData);
	GET_SYSTEM_MEMORY("Comm1Work_Controller_Current", motor_data);
	GET_SYSTEM_MEMORY("Comm1Work_Controller_Target", motor_data_target);
	if (m_dCnt >= 10.0) {
		m_ntgPOS.SetRange(m_dCnt - 10.0, m_dCnt, -MAX_POS, MAX_POS);
		m_ntgVel.SetRange(m_dCnt - 10.0, m_dCnt, -MAX_Vel, MAX_Vel);
		m_ntgTorque.SetRange(m_dCnt - 10.0, m_dCnt, -MAX_Torque, MAX_Torque);
	}

	//위치
	//if not open
	m_ntgPOS.PlotXY(m_dCnt, motor_data_target.position * RAD2DEG, 1);
	m_ntgPOS.PlotXY(m_dCnt, motor_data.position * RAD2DEG, 2);
	//if open
	//m_ntgPOS.PlotXY(m_dCnt, motor_data.position * RAD2DEG, 1);

	//속도
	m_ntgVel.PlotXY(m_dCnt, motor_data_target.velocity * RAD2DEG, 1);
	m_ntgVel.PlotXY(m_dCnt, motor_data.velocity * RAD2DEG, 2);
	

	//토크
	m_ntgTorque.PlotXY(m_dCnt, motor_data_target.current * 0.0683, 1);
	m_ntgTorque.PlotXY(m_dCnt, motor_data.current * 0.0683, 2);

	CDialogEx::OnTimer(nIDEvent);
}
