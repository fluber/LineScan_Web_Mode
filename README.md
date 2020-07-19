# LineScan_Web_Mode
Use Basler Camera Link Line Scan Camera + Euresys GrabLink Full Card


This souce cdoe demo Use Basler Camera Link Camera + Euresys GrabLink Full Card (AcquistionMode: Web).

#Reference SDK: 

1.Euresys MultiCam 6.18 SDK, download https://www.euresys.com/en/Support/Download-area

2.Balser pylon 6.1.1 SDK and Balser clBBProtocolLib SDK, download https://www.baslerweb.com/en/sales-support/downloads/software-downloads/

#Detail Description:

QT       += core gui network

INCLUDEPATH += "C:\Program Files (x86)\Euresys\MultiCam\Include"
INCLUDEPATH += "C:\Program Files (x86)\Basler\clBBProtocolLib\include"

LIBS += -L"C:\Program Files (x86)\Euresys\MultiCam\Lib" -lMultiCam
LIBS += -L"C:\Program Files (x86)\Basler\clBBProtocolLib\lib" -lClBBProtocolLib -lclallserial

#Function List:

1.Modify Balser Camera parameters using clBBProtocolLib SDK
2.Configure Euresys GrabLink Full Card using MultiCamSDK
3.Remote Recieve command to Gab Image.

#Import Hardware Configuration:
1: Use encoder signal OA/invert OA, OB/ invert OB, the four lines.
2: Euresys GrabLink Full Card External I/O Connector Pin Assignments:
   Pin 2: (DIN2+)   ==>  OB
   Pin 11: (DIN2-)  ==>  invert OB
   Pin 19: (DIN1-)  ==>  invert OA
   Pin 20: (DIN1+)  ==>  OA

#Import Software Configuration:
1.Use Balser Pylon Viewer to set TapGeometry 1X8
2.Use MultiCam to create camera file for raL12288-66km_L12288RG
3.Calculator RateDivisionFactor = optical accuracy by 1 pixel / moving distance by 1 pulse. ex: optical accuracy by 1 pixel: 60um, moving distance by 1 pulse: 10um
RateDivisionFactor = 60um / 10um = 6

