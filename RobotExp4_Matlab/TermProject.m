clc
clear all
close all
%% Parameter Setting

% motor parameter
La          = 0.000658;
Ra          = 1.76;
Kt          = 68.3*(10^-3);
Ke          = Kt;
t           = 0.00376;
J           = 95.5*(10^-7);
b           = J/t;

% Gear
N1          = 1;
N2          = 81;
alpha       = 0.72;
JG          = 5*(10^-7);

% Load
m_bar       = 0.175;                                                                               % [kg]
L_bar       = 0.3;                                                                                 % [m]
H_bar       = 0.025;                                                                               % [m]
m_circle    = 0.340;                                                                               % [kg]
R_circle    = 0.05;                                                                                % [m]
J_load      = m_bar*(L_bar^2 + 0.25*H_bar^2)/3 + 0.5*m_circle*R_circle^2 + m_circle*L_bar^2;       % [kgm^2]
B_load      = J_load/t;                                                                            % Load's Friction Coefficient(J/t)
k           = (1/alpha)*((N1/N2)^2);

% Total
Jeq         = J+k*JG+k*J_load;
Beq         = Jeq/t;

% 라디안을 각도로 변환
radtodeg    = 180/pi;
degtorad    = pi/180;

%% Current Control
fcc         = 200;        % MCU Control Period / 10
wcc         = 2*pi*fcc;   % 전류제어 교차각 주파수
Kp_c        = wcc*La;     % 전류제어 P게인
Ki_c        = wcc*Ra;     % 전류제어 I게인
Ka_c        = 1/Kp_c;     % 전류제어 ANTI게인
%% Speed Control
wcs         = wcc/10;     % 속도제어 교차각 주파수
Kp_s        = Jeq*wcs/Kt; % 속도제어 P게인
Ki_s        = Beq*wcs/Kt; % 속도제어 I게인
Ka_s        = 1/Kp_s;     % 속도제어 ANTI게인
%% Position Control
wcp         = wcs/10;     % 위치제어 교차각 주파수
Kp_p        = wcp;        % 위치제어 P게인
Kd_p        = wcp/wcs;    % 위치제어 D게인




