#include "stdafx.h"
#include "ODE.h"
#include "SystemMemory.h"
#include "DataType.h"
#include "RobotExp_4Dlg.h"

#ifndef DRAWSTUFF_TEXTURE_PATH
#define DRAWSTUFF_TEXTURE_PATH "./../ode-0.13/drawstuff/textures"
#endif

#define GRAVITY 9.81
#define MAX_JOINT_NUM 2

#define DEG2RAD 0.0174533
#define RAD2DEG 57.2958
#define PI 3.141592653589793238


//ODE Global Variable
dsFunctions g_Fn;

static dWorldID g_World;
static dSpaceID g_Space;
static dJointGroupID g_Contactgroup;
static dGeomID g_Ground;

//My Robot Global Variable
Object g_oObj[MAX_JOINT_NUM + 1];
static dJointID g_oJoint[MAX_JOINT_NUM + 1];

double g_tar_q[MAX_JOINT_NUM] = { 0.0, 0.0 };
double g_cur_q[MAX_JOINT_NUM] = { 0.0, 0.0 };

dReal dErrorP_Q[MAX_JOINT_NUM] = { 0, };

void InitDrawStuff() {

    g_Fn.version = DS_VERSION;
    g_Fn.start = &StartDrawStuff;
    g_Fn.step = &SimLoopDrawStuff;
    g_Fn.command = &CommandDrawStuff;
    g_Fn.stop = StopDrawStuff;
    g_Fn.path_to_textures = DRAWSTUFF_TEXTURE_PATH;
}


void InitODE() {

    //TO DO
    //setDrawStuff();
    dInitODE();
    g_World = dWorldCreate();
    g_Space = dHashSpaceCreate(0);
    g_Contactgroup = dJointGroupCreate(0);
    g_Ground = dCreatePlane(g_Space, 0, 0, 1, 0);
    dWorldSetGravity(g_World, 0, 0, -9.8);
    dWorldSetERP(g_World, 1.0);
    dWorldSetCFM(g_World, 1e-5);
}



void RunODE(size_t width, size_t height) {

    //TO DO
    InitDrawStuff();
    InitODE();

    InitRobot();

    //run simulation
    dsSimulationLoop(0, 0, width, height, &g_Fn);
}



void ReleaseODE() {

    dJointGroupDestroy(g_Contactgroup);
    dSpaceDestroy(g_Space);
    dWorldDestroy(g_World);
    dCloseODE();
}



void StartDrawStuff() {

    //TO DO
    // 카메라 뷰 설정
    float dPos[3] = { 0.0,0.0,5.0 };   // X, Y, Z 좌표
    float dRot[3] = { 0.0,-90.0,90.0 };   // X축 Y축 Z축 기준 회전
    //float dPos[3] = { 3.0,3.0,5.0 };   // X, Y, Z 좌표
    //float dRot[3] = { -135,-45.0,0.0 };   // X축 Y축 Z축 기준 회전
    dsSetViewpoint(dPos, dRot);
}


void SimLoopDrawStuff(int pause)
{

    //TO DO
    DataType_t jointData;
    ControlData_t motor_data;
    GET_SYSTEM_MEMORY("JointData", jointData);
    GET_SYSTEM_MEMORY("Comm1Work_Controller_Current", motor_data);
    if (CheckODEFlag()) {
        g_tar_q[0] = jointData.Q_tar[0];
        g_tar_q[1] = jointData.Q_tar[1];
    }
    else {
        g_tar_q[0] = motor_data.position;
        g_tar_q[1] = 0;
    }
    

    jointData.Q_cur[0] = g_cur_q[0];
    jointData.Q_cur[1] = g_cur_q[1];

    SET_SYSTEM_MEMORY("JointData", jointData);
    g_tar_q[0] = fmod(g_tar_q[0], 360.0 * DEG2RAD);
    g_tar_q[1] = fmod(g_tar_q[1], 360.0 * DEG2RAD);
    if (g_tar_q[0] >= 360.0 * DEG2RAD) g_tar_q[0] -= 360.0 * DEG2RAD;
    if (g_tar_q[0] <= -360.0 * DEG2RAD) g_tar_q[0] += 360.0 * DEG2RAD;
    if (g_tar_q[1] >= 360.0 * DEG2RAD) g_tar_q[0] -= 360.0 * DEG2RAD;
    if (g_tar_q[1] <= -360.0 * DEG2RAD) g_tar_q[0] += 360.0 * DEG2RAD;
    // 
    //Robot Control
    PControl();

    //Close Loop Setting
    double dt = 0.01; //사실과 같아지지만 연산량이 많아짐, dt만큼 코드를 한스텝 돌려라
    dWorldStep(g_World, dt);

    //Draw Robot
    dsSetColor(1., 0., 0.);
    dsDrawCapsuleD(dBodyGetPosition(g_oObj[0].body),
        dBodyGetRotation(g_oObj[0].body),
        0.5, 0.125);
    dsSetColor(0., 1., 0.);
    dsDrawCapsuleD(dBodyGetPosition(g_oObj[1].body),
        dBodyGetRotation(g_oObj[1].body),
        1.0, 0.125);
    dsSetColor(0., 0., 1.);
    dsDrawCapsuleD(dBodyGetPosition(g_oObj[2].body),
        dBodyGetRotation(g_oObj[2].body),
        0.5, 0.125);

}




void CommandDrawStuff(int cmd) {

    //TO DO

}



void StopDrawStuff() {

    //TO DO

}


void InitRobot()
{

    //TO DO
    //Body 설정
    dMass mass;
    dMatrix3 R;

    //질량중심점
    dReal x[MAX_JOINT_NUM + 1] = { 0.00, 0.50, 1.25 };
    dReal y[MAX_JOINT_NUM + 1] = { 0.0, 0.00, 0.00 };
    dReal z[MAX_JOINT_NUM + 1] = { 0.25, 0.5, 0.5 };

    //링크 자세
    dReal ori_x[MAX_JOINT_NUM + 1] = { 0.0, 0.0, 0.0 };
    dReal ori_y[MAX_JOINT_NUM + 1] = { 0.0, 1.0, 1.0 };
    dReal ori_z[MAX_JOINT_NUM + 1] = { 0.0, 0, 0.0 };
    dReal ori_q[MAX_JOINT_NUM + 1] = { 0.0, 90*DEG2RAD, 90 * DEG2RAD };

    //림 한덩이 링크 길이
    dReal length[MAX_JOINT_NUM + 1] = { 0.5, 1.0, 0.5 };

    //한덩이 무게
    dReal weight[MAX_JOINT_NUM + 1] = { 1.0,1.0, 1.0 };

    //캡슐의 반지름
    dReal r[MAX_JOINT_NUM + 1];
    for (int i = 0; i < MAX_JOINT_NUM + 1; i++)
        r[i] = 0.125;

    //Body 생성
    for (int i = 0; i < MAX_JOINT_NUM + 1; i++)
    {
        g_oObj[i].body = dBodyCreate(g_World);
        dBodySetPosition(g_oObj[i].body, x[i], y[i], z[i]);
        dMassSetZero(&mass);
        dMassSetCapsuleTotal(&mass, weight[i], 1, r[i], length[i]);
        dBodySetMass(g_oObj[i].body, &mass);
        g_oObj[i].geom = dCreateCapsule(g_Space, r[i], length[i]);
        dGeomSetBody(g_oObj[i].geom, g_oObj[i].body);
        dRFromAxisAndAngle(R, ori_x[i], ori_y[i], ori_z[i], ori_q[i]);
        dBodySetRotation(g_oObj[i].body, R);
    }
    ///////////////////////////////////////////////////////////
    //각 조인트의 회전축의 위치
    dReal c_x[MAX_JOINT_NUM + 1] = { 0.0, 0.0, 1.0 };
    dReal c_y[MAX_JOINT_NUM + 1] = { 0.0, 0.0, 0.0 };
    dReal c_z[MAX_JOINT_NUM + 1] = { 0.0, 0.5, 0.5 };

    //회전축의 로테이션 방향
    dReal axis_x[MAX_JOINT_NUM + 1] = { 0.0, 0.0, 0.0 };
    dReal axis_y[MAX_JOINT_NUM + 1] = { 0.0, 0.0, 0.0 };
    dReal axis_z[MAX_JOINT_NUM + 1] = { 1.0, -1.0, -1.0 };

    //고정 축 설정
    g_oJoint[0] = dJointCreateFixed(g_World, 0);
    dJointAttach(g_oJoint[0], 0, g_oObj[0].body);
    dJointSetFixed(g_oJoint[0]);
    //조인트 설정
    for (int i = 1; i < MAX_JOINT_NUM + 1; i++)
    {
        g_oJoint[i] = dJointCreateHinge(g_World, 0);
        dJointAttach(g_oJoint[i], g_oObj[i].body, g_oObj[i - 1].body);
        dJointSetHingeAnchor(g_oJoint[i], c_x[i], c_y[i], c_z[i]);
        dJointSetHingeAxis(g_oJoint[i], axis_x[i], axis_y[i], axis_z[i]);
    }
}

void PControl()
{

    //TO DO
    dReal dKp = 0.1, dMax = 100.0;
    dReal dKd = 0.01;
    dReal dError_Q[MAX_JOINT_NUM];
    bool flag = CheckODEFlag();
    if (!flag)dKp = 10;

    for (int i = 1; i < MAX_JOINT_NUM + 1; i++)
    {
        g_cur_q[i - 1] = dJointGetHingeAngle(g_oJoint[i]);
        if (g_tar_q[i - 1] - g_cur_q[i - 1] > 180.0 * DEG2RAD)
        {
                g_cur_q[i - 1] += 359.9 * DEG2RAD;
        }

        if (g_tar_q[i - 1] - g_cur_q[i - 1] < -180.0 * DEG2RAD)
        {
                g_cur_q[i - 1] -= 359.9 * DEG2RAD;
        }

        dError_Q[i - 1] = g_tar_q[i - 1] - g_cur_q[i - 1];

        //dJointSetHingeParam(g_oJoint[i], dParamVel, dKp * dError_Q[i - 1]); //P Control
        dJointSetHingeParam(g_oJoint[i], dParamVel, dKp * dError_Q[i - 1] + dKd * (dError_Q[i - 1] - dErrorP_Q[i - 1]) * 0.01); // PD Control
        dJointSetHingeParam(g_oJoint[i], dParamFMax, dMax);
        //Update Past Error
        dErrorP_Q[i - 1] = dError_Q[i - 1];
    }
}