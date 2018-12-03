/******************************************************************************
 * (c) Copyright 2012-2016 Xilinx, Inc. All rights reserved.
 *
 * This file contains confidential and proprietary information of Xilinx, Inc.
 * and is protected under U.S. and international copyright and other
 * intellectual property laws.
 *
 * DISCLAIMER
 * This disclaimer is not a license and does not grant any rights to the
 * materials distributed herewith. Except as otherwise provided in a valid
 * license issued to you by Xilinx, and to the maximum extent permitted by
 * applicable law: (1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND WITH ALL
 * FAULTS, AND XILINX HEREBY DISCLAIMS ALL WARRANTIES AND CONDITIONS, EXPRESS,
 * IMPLIED, OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NON-INFRINGEMENT, OR FITNESS FOR ANY PARTICULAR PURPOSE;
 * and (2) Xilinx shall not be liable (whether in contract or tort, including
 * negligence, or under any other theory of liability) for any loss or damage
 * of any kind or nature related to, arising under or in connection with these
 * materials, including for any direct, or any indirect, special, incidental,
 * or consequential loss or damage (including loss of data, profits, goodwill,
 * or any type of loss or damage suffered as a result of any action brought by
 * a third party) even if such damage or loss was reasonably foreseeable or
 * Xilinx had been advised of the possibility of the same.
 *
 * CRITICAL APPLICATIONS
 * Xilinx products are not designed or intended to be fail-safe, or for use in
 * any application requiring fail-safe performance, such as life-support or
 * safety devices or systems, Class III medical devices, nuclear facilities,
 * applications related to the deployment of airbags, or any other applications
 * that could lead to death, personal injury, or severe property or
 * environmental damage (individually and collectively, "Critical
 * Applications"). Customer assumes the sole risk and liability of any use of
 * Xilinx products in Critical Applications, subject only to applicable laws
 * and regulations governing limitations on product liability.
 *
 * THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS PART OF THIS FILE
 * AT ALL TIMES.
 *******************************************************************************/

/*
 * This file defines video QT application user interface.
 */

import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.0
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.1
import QtCharts 2.1

import "qrc:///custom/qml"
Rectangle {
	visible: true
	id: root

	property int sobleToggle :0
	property bool play: false
	property int videoInput: 0
	property int filterTypeInput: 0
	property int playmodeInput:0
	property int previousVideoInput: -1
	property int filterinput: 0
	property int tpgPatternSelected: 13
	property int testPatternSelected: 0
	property bool statsDisplay: true
	property bool plotDisplay: true
	property bool invertEnabled: false
	property bool hideControlBar: false
	property bool auto_start: false
	property bool appRunning: true

	property var toolBarHeight: 58
	property var errorMessageText: ""
	property var errorNameText: ""
	property var headerNames:["TPG Settings","2D Filter Settings"]
	property int resolution: 720
	property var barColors: "#F0F7F7F7"
	property var barTitleColors: "#F0AAAAAA"
	property var cellColor: "#FFEEEEEE"
	property var cellHighlightColor: "#FFAAAAAA"
	property var borderColors: "#F0AAAAAA"
	property int boarderWidths: 1
	property var foreGroundList: ["None","Moving Box","Cross Hairs"]
	property int foreGroundSelectOpt: 1
	property int boxSelectSize: 3

	property var boxColorSelected: false
	property var boxColorList: ["white","yellow","cyan","green","magenta","red","blue","black"]
	property var boxYUVColors: [0xFF8080,0xE10094,0xB2AA00,0X952B15,0X69D4EA,0X4C55FF,0x1DFF6B,0x008080]
	property var boxColorLabelList: ["White","Yellow","Cyan","Green","Magenta","Red","Blue","Black"];
	property int boxColorbSelect:3
	property int boxSpeedSelOption: 1
	property int boxSpeedValue: 4
	property int maxBoxSpeedValue: 255
	property int boxSizeValue: 150
	property int maxBoxSizeValue: 4095
	property int crossRowsValue: 100
	property int maxCrossRowsValue: imageResolutionHeight - 1 //4095
	property int crossColumnsValue: 100
	property int maxCrossColumnsValue: imageResolutionWidth - 1 //4095
	property int zoneHPlateValue: 30
	property int maxZoneHPlateValue: 65536
	property int zoneVPlateValue: 1
	property int maxZoneVPlateValue: 65536
	property int zoneHPlateDeltaValue: 0
	property int maxZoneHPlateDeltaValue: 65536
	property int zoneVPlateDeltaValue: 0
	property int maxZoneVPlateDeltaValue: 65536

	property int crossRowsSelOption: 2
	property int crossColumnsSelOption: 2
	property int zoneHSelOption: 1
	property int zoneVSelOption: 1

	property var filterCoeffList: [-9,-8,-7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7,8,9]
	property var filter00Coeff: 10
	property var filter01Coeff: 9
	property var filter02Coeff: 9
	property var filter10Coeff: 9
	property var filter11Coeff: 10
	property var filter12Coeff: 9
	property var filter20Coeff: 9
	property var filter21Coeff: 9
	property var filter22Coeff: 10

	property var widthCoeffsBox: 50
	property var tooltipWidth: 0

	property int gammasetingsmaxvalue: 40
	property int csisetingsmaxvalue: 100
	property int imxexposuremaxvalue:16666
	property int  imxexposureminvalue: 14
	property int imxgainmaxvalue: 46088
	property int imxgainminvalue: 256

	FontLoader { id: fontFamily; source: "/font/font/luxisr.ttf" }

	property var popupVisible: 0
	property var popupTitle: "Settings"
	property var tpgComboSelected: false
	property var testComboSelected: false
	property var fgComboSelected: false

	property var presetComboSelected: false
	property int previousePatterSelVal: 0
	property int presetPatternSelected: 1  //-1 for none. Default is set to edge (index 1 starting with 0)

	property var filterComboSelected: false
	property var filterComboSelOption : 0

	property var showMemoryThroughput: false

	property alias numericinputlabelText: numericInputLabel.text
	property alias numericfilterlabelText: numericFilterLabel.text
	property alias numericFilterTypeLabelText: numericFilterTypeLabel.text

	property alias fpsValueText: framesValueLabel.text
//	property alias videoOptSel: videoInput
//	property alias filterOptSel: filterinput

	property bool errorFound: false

	property alias cp1Lbl: cpu1Destails.text
	property alias cp2Lbl: cpu2Destails.text
	property alias cp3Lbl: cpu3Destails.text
	property alias cp4Lbl: cpu4Destails.text

	property alias truPutVideoSrcLbl: videoSrcLbl.text
	property alias truPutFilterLbl: filterLbl.text
	property alias truPutDisplayPortLbl: displayPortLbl.text

	property var demoSequence: [{"source":0, "filterType":0, "filterMode":0},
					{"source":0, "filterType":0, "filterMode":0},
					{"source":0, "filterType":0, "filterMode":0},
					{"source":0, "filterType":0, "filterMode":0},
					{"source":0, "filterType":0, "filterMode":0},
					{"source":0, "filterType":0, "filterMode":0},
					{"source":0, "filterType":0, "filterMode":0},
					{"source":0, "filterType":0, "filterMode":0},
					{"source":0, "filterType":0, "filterMode":0}]

	signal playvideo(int val)
	signal setFilter(int val,int flag)
	signal setFilterType(int val,int flag)
	signal setFilterMode(int val)
	signal setInputType(int val, int flag)
//	signal setInvert(int val)
	signal setTPGPattern(int val)
	signal setpreset (int val)
//	signal setSensitivity(int max, int min)
	signal demoMode(bool start)
	signal setFgOverlay(int overlay)
	signal setBoxSize(int size)
	signal setBoxColor(int color)
	signal setBoxSpeed(int speed)
	signal setCrossRows(int rows)
	signal setCrossColumns(int columns)
	signal setZoneH(int H)
	signal setZoneHDelta(int H)
	signal setZoneV(int v)
	signal setZoneVDelta(int v)
	signal csiredgamma(int redg)
	signal csibluegamma(int blueg)
	signal csigreengamma(int greeng)
	signal csicontrast(int contrast)
	signal csibrightness(int brightness)
	signal csiredgain(int redgain)
	signal csigreengain(int greengain)
	signal csibluegain(int bluegain)
	signal csiexposure(int imxgain)
	signal csiimxgain(int exposure)
	signal setTestPattern(int testpattern)
	signal setVerticalFlip(int flip)
	signal filterCoeff(int c00,int c01,int c02,int c10,int c11,int c12,int c20 ,int c21,int c22)
	signal fileSelect(string filename, int flag)
	Timer {
		id:timerABCD
		repeat: true
		interval: 5000
		onTriggered: {
			if(hideControlBar && appRunning){
				popupVisible = 0
				showPlayarea(false)
				showDemoarea(false)
				showInputarea(false)
				showFilterarea(false)
				showFilterTypeArea(false)
				toolBarItem.visible = false
			}
		}
	}
	function resetTimer(val){
		if(appRunning){
			timerABCD.restart()
			toolBarItem.visible = true
		}
	}
        /*Tool tip functions*/

	function toolTipOnEnter(x, y, message){
		toolTipRect.toolTipOnEnter(x, y, message)
	}
	function toolTipOnExit(){
		toolTipRect.toolTipOnExit()

	}
	function updatePlayState(state){
		play = state
		playbuttimage.source = ((!play && (playBackModeLabel.text === "Manual")) ? "qrc:///imgs/images/play@"+imageResolution+".png" : "qrc:///imgs/images/pause@"+imageResolution+".png")
	}
	function updateInput(status) {
		if(status){
			previousVideoInput = videoInput
		}
		if(previousVideoInput != -1){
			inputPopSettingsButton.enabled = videoSourceList[previousVideoInput].hasPanel
			videoInput = previousVideoInput
			videoSrcOptionsSV.resetSource(videoInput)
		}else if(!status){
			play = false
		}
		numericInputLabel.text = videoSourceList[videoInput].shortName
		return ""
	}
	color: play || auto_start? "#00000000" : "#FF000000"
	// Numeric panel
	Rectangle{
		id: numericPanel
		width: 340
		height:  (120 + (!auto_start ? 25 : 0) + (acceleratorRect.visible ? 25 :0) + (aceeleratorModeRec.visible ? 25 : 0) + (presetRec.visible ? 25 : 0) + (playbackmodeRec.visible ? 25 : 0)) //numColumnLayout.childrenRect.height
		border.color: borderColors
		border.width: boarderWidths
		color: barColors
		visible: statsDisplay
		anchors{
			left: parent.left
			top: parent.top
			leftMargin: 30
			topMargin: 30
		}
		ColumnLayout{
			id: numColumnLayout
			width: 340
			spacing: 1
			Rectangle{
				width: parent.width
				height: 35
				color: barTitleColors
				anchors.top: parent.top
				anchors.topMargin: 0
				RowLayout{
					anchors.fill: parent
					Text{
						text: "  Video Info"
						anchors.leftMargin: 10
						anchors.left: parent.left
						font.bold: true
						font.pointSize: 15
						font.family: fontFamily.name
					}
				}
			}
			Rectangle{
				width: parent.width - 30
				height: 25
				anchors.leftMargin: 15
				anchors.left: parent.left
				color: "transparent"
				RowLayout{
					anchors.fill: parent
					Text{
						text: "Display Device:"
						anchors.leftMargin: 10
						anchors.left: parent.left
						font.pointSize: 13
						font.family: fontFamily.name
					}
					Text{
						text: displayDeviceType
						font.bold: true
						anchors.right: parent.right
						anchors.rightMargin: 10
						font.pointSize: 13
						font.family: fontFamily.name
					}
				}
			}
			Rectangle{
				width: parent.width - 30
				height: 25
				anchors.leftMargin: 15
				anchors.left: parent.left
				color: "transparent"
				RowLayout{
					anchors.fill: parent
					Text{
						text: "Display Resolution:"
						anchors.leftMargin: 10
						anchors.left: parent.left
						font.pointSize: 13
						font.family: fontFamily.name
					}
					Text{
						text: resolution+"p"
						font.bold: true
						anchors.right: parent.right
						anchors.rightMargin: 10
						font.pointSize: 13
						font.family: fontFamily.name
					}
				}

			}
			Rectangle{
				width: parent.width - 30
				height: 25
				anchors.leftMargin: 15
				anchors.left: parent.left
				color: "transparent"
				visible: (!auto_start)
				RowLayout{
					anchors.fill: parent
					Text{
						text: "Frame Rate:"
						anchors.leftMargin: 10
						anchors.left: parent.left
						font.pointSize: 13
						font.family: fontFamily.name
					}
					Text{
						id: framesValueLabel
						text: "0 fps"
						font.bold: true
						anchors.right: parent.right
						anchors.rightMargin: 10
						font.pointSize: 13
						font.family: fontFamily.name
					}
				}
			}
			Rectangle{
				width: parent.width - 30
				height: 25
				anchors.leftMargin: 15
				anchors.left: parent.left
				color: "transparent"
				RowLayout{
					anchors.fill: parent
					Text{
						text: "Video Source:"
						anchors.leftMargin: 10
						anchors.left: parent.left
						font.pointSize: 13
						font.family: fontFamily.name
					}
					Text{
						id: numericInputLabel
						text: videoSourceList.length ? videoSourceList[0].shortName : "No Video Sources Available"
						font.bold: true
						anchors.right: parent.right
						anchors.rightMargin: 10
						font.pointSize: 13
						font.family: fontFamily.name
					}
				}

			}
			Rectangle{
				id:acceleratorRect
				width: parent.width - 30
				height: 25
				anchors.leftMargin: 15
				anchors.left: parent.left
				color: "transparent"
				RowLayout{
					anchors.fill: parent
					Text{
						text: "Accelerator:"
						anchors.leftMargin: 10
						anchors.left: parent.left
						font.pointSize: 13
						font.family: fontFamily.name
					}
					Text{
						id:numericFilterTypeLabel
						objectName: "numericFilterTypeLabel"
						text: filterTypeSourceList.length ? filterTypeSourceList[filterTypeInput].displayName : ""
						font.bold: true
						anchors.right: parent.right
						anchors.rightMargin: 10
						font.pointSize: 13
						font.family: fontFamily.name
					}
				}
			}
			Rectangle{
				id: aceeleratorModeRec
				width: parent.width - 30
				height: 25
				anchors.leftMargin: 15
				anchors.left: parent.left
				color: "transparent"
				visible:((filterTypeSourceList[filterTypeInput].displayName === "2D Filter")||
					(filterTypeSourceList[filterTypeInput].displayName === "Optical Flow"))
				RowLayout{
					anchors.fill: parent
					Text{
						text: "Accelerator Mode:"
						anchors.leftMargin: 10
						anchors.left: parent.left
						font.pointSize: 13
						font.family: fontFamily.name
					}
					Text{
						id:numericFilterLabel
						objectName: "numericFilterLabel"
						text: (filterTypeSourceList.length &&
							filterTypeSourceList[filterTypeInput].modesList.length) ?
							filterTypeSourceList[filterTypeInput].modesList[filterinput].displayName : ""
						font.bold: true
						anchors.right: parent.right
						anchors.rightMargin: 10
						font.pointSize: 13
						font.family: fontFamily.name
					}
				}

			}
			Rectangle{
				id: presetRec
				width: parent.width - 30
				height:25
				anchors.leftMargin: 15
				anchors.left: parent.left
				color: "transparent"
				visible: filter2DEnabled &&
					((filterTypeSourceList.length && filterTypeSourceList[filterTypeInput].
					modesList.length)) ? (filterTypeSourceList[filterTypeInput].displayName === "2D Filter") : false
				RowLayout{
					anchors.fill: parent
					Text{
						text: "2D Filter Preset:"
						anchors.leftMargin: 10
						anchors.left: parent.left
						font.pointSize: 13
						font.family: fontFamily.name
					}
					Text{
						text: presetList.length ? presetList[presetPatternSelected] : "NA"
						font.bold: true
						anchors.right: parent.right
						anchors.rightMargin: 10
						font.pointSize: 13
						font.family: fontFamily.name
					}
				}
			}
			Rectangle{
				id:playbackmodeRec
				width: parent.width - 30
				height: 25
				anchors.leftMargin: 15
				anchors.left: parent.left
				color: "transparent"
				RowLayout{
					anchors.fill: parent
					Text{
						text: "Playback Mode:"
						anchors.leftMargin: 10
						anchors.left: parent.left
						font.pointSize: 13
						font.family: fontFamily.name
					}
					Text{
						id:playBackModeLabel
						text:"Manual"
						font.bold: true
						anchors.right: parent.right
						anchors.rightMargin: 10
						font.pointSize: 13
						font.family: fontFamily.name
					}
				}
			}
		}
	}

	Flipable{
		id: flipable
		property int txtVuWidth: 240
		property int txtVuHeight: 140
                width: side == Flipable.Front ? 450  : flipable.txtVuWidth
                height: side == Flipable.Front ? 300  : flipable.txtVuHeight
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.rightMargin: 30
                anchors.topMargin: 30
                property bool flipped: false
                back: Rectangle{
			visible: plotDisplay
			width: flipable.txtVuWidth ; height: flipable.txtVuHeight ; anchors.right: parent.right; color: barColors
			ColumnLayout{
				anchors.fill: parent
				spacing: 1
				Rectangle{
					anchors.top: parent.top
					anchors.topMargin: 0
					width: parent.width
					height: 35
					color: barTitleColors
					RowLayout{
						anchors.fill: parent
						Text{
							text: "CPU Utilization"
							anchors.leftMargin: 10
							anchors.left: parent.left
							font.bold: true
							font.pointSize: 15
							font.family: fontFamily.name
						}
					}

				}
				Rectangle{
					width: parent.width
					height: 25
					color: "transparent"
					RowLayout{
						anchors.fill: parent
						Text{
							text: "CPU 1:"
							anchors.leftMargin: 10
							anchors.left: parent.left
							font.pointSize: 13
							font.family: fontFamily.name
						}
						Text{
							id: cpu1Destails
							font.bold: true
							anchors.right: parent.right
							anchors.rightMargin: 10
							font.pointSize: 13
							font.family: fontFamily.name
						}
					}

				}
				Rectangle{
					width: parent.width
					height: 25
					color: "transparent"
					RowLayout{
						anchors.fill: parent
						Text{
							text: "CPU 2:"
							anchors.leftMargin: 10
							anchors.left: parent.left
							font.pointSize: 13
							font.family: fontFamily.name
						}
						Text{
							id: cpu2Destails
							font.bold: true
							anchors.right: parent.right
							anchors.rightMargin: 10
							font.pointSize: 13
							font.family: fontFamily.name
						}
					}

				}Rectangle{
					width: parent.width
					height: 25
					color: "transparent"
					RowLayout{
						anchors.fill: parent
						Text{
							text: "CPU 3:"
							anchors.leftMargin: 10
							anchors.left: parent.left
							font.pointSize: 13
							font.family: fontFamily.name
						}
						Text{
							id: cpu3Destails
							font.bold: true
							anchors.right: parent.right
							anchors.rightMargin: 10
							font.pointSize: 13
							font.family: fontFamily.name
						}
					}
				}Rectangle{
					width: parent.width
					height: 25
					color: "transparent"
					RowLayout{
						anchors.fill: parent
						Text{
							text: "CPU 4:"
							anchors.leftMargin: 10
							anchors.left: parent.left
							font.pointSize: 13
							font.family: fontFamily.name
						}
						Text{
							id: cpu4Destails
							font.bold: true
							anchors.right: parent.right
							anchors.rightMargin: 10
							font.pointSize: 13
							font.family: fontFamily.name
						}
					}
				}
			}
			border.color: borderColors
			border.width: boarderWidths
                }
                front:	Rectangle {
			id: cpuUtilizationPlot
			width: 450
			height: 300
			visible: plotDisplay
			color: barColors
			border.color: borderColors
			border.width: boarderWidths
			ChartView {
				id: chart_line_CPU
				width: parent.width
				height : parent.height
				animationOptions: ChartView.NoAnimation
				property bool openGL: true
				backgroundRoundness: 0
				legend.alignment: Qt.AlignBottom
				legend.font.pixelSize: resoluteFrac < 1 ? 8 : 12
				titleFont.pixelSize: 13
				legend.visible: true
				margins.bottom: resoluteFrac < 1 ? 90 : 0
				margins.top: 45
				margins.left: 0
				margins.right: resoluteFrac < 1 ? 150 : 0
				plotAreaColor: "transparent"
				backgroundColor: "transparent"
				Rectangle{
					width: parent.width
					height: 35
					color: barTitleColors
					anchors.top: parent.top
					anchors.topMargin: 0
					anchors.left: parent.left
					anchors.leftMargin: 0
					Text{
						text: "CPU Utilization (%)"
						anchors.leftMargin: 20
						anchors.left: parent.left
						height: parent.height
						verticalAlignment: Text.AlignVCenter
						font.bold: true
						font.pointSize: 15
						font.family: fontFamily.name
					}
				}
				ValueAxis {
					id: axisYcpu
					min: 0
					max: 100
					labelFormat: "%d"
					labelsFont.pointSize: 10
					labelsColor: "black"
					gridLineColor: "black"
					lineVisible: false
                                }
                                ValueAxis {
					id: axisXcpu
					min: 0
					max: 60
					labelsVisible: false
					gridLineColor: "black"
					lineVisible: false
					labelsFont.pointSize: 1
                                }
                                SplineSeries {
					name: "CPU 1"
					axisX: axisXcpu
					axisY: axisYcpu
					useOpenGL: chart_line_CPU.openGL
                                }
                                SplineSeries {
					name: "CPU 2"
					axisX: axisXcpu
					axisY: axisYcpu
					useOpenGL: chart_line_CPU.openGL
                                }
                                SplineSeries {
					name: "CPU 3"
					axisX: axisXcpu
					axisY: axisYcpu
					useOpenGL: chart_line_CPU.openGL
                                }
                                SplineSeries {
					name: "CPU 4"
					axisX: axisXcpu
					axisY: axisYcpu
					useOpenGL: chart_line_CPU.openGL
					color: "red"
                                }
			}
                }
                transform: Rotation {
			id: rotation
			origin.x: flipable.width/2
			origin.y: flipable.height/2
			axis.x: 0; axis.y: 1; axis.z: 0     // set axis.y to 1 to rotate around y-axis
			angle: 0    // the default angle
                }

                states: State {
			name: "back"
			PropertyChanges { target: rotation; angle: 180 }
			when: flipable.flipped
                }

                transitions: Transition {
			NumberAnimation { target: rotation; property: "angle"; duration: 200 }
                }
                MouseArea {
			anchors.fill: parent
			hoverEnabled: true
			cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor
			onClicked: flipable.flipped = !flipable.flipped
			onEntered: {
				toolTipOnEnter(parent.x + parent.width/3 + 10,parent.y  + parent.height/2 + 10, "Click to Toggle View")
			}
			onExited: {
				toolTipOnExit()
			}
                }
                Behavior on height{
			NumberAnimation {
				duration: 500
				easing.type: Easing.OutSine
			}
		}
	}

	Flipable{
		id: flipableMT
		property int txtVuWidthMT: 240
		property int txtVuHeightMT: 120
                width: side == Flipable.Front ? 450  : flipableMT.txtVuWidthMT
                height: side == Flipable.Front ? 300  : flipableMT.txtVuHeightMT
                anchors.right: parent.right
                anchors.rightMargin: 30
                anchors.topMargin: 30
                anchors.top: flipable.bottom
                property bool flipped: false
                visible: showMemoryThroughput && plotDisplay
                back: Rectangle{
			width: flipableMT.txtVuWidthMT ; height: flipableMT.txtVuHeightMT ; anchors.right: parent.right; color: barColors
			border.color: borderColors
			border.width: boarderWidths

			ColumnLayout{
				anchors.fill: parent
				spacing: 1
				Rectangle{
					anchors.top: parent.top
					anchors.topMargin: 0
					width: parent.width
					height: 35
					color: barTitleColors
					RowLayout{
						anchors.fill: parent
						Text{
							text: "Memory Throughput"
							anchors.leftMargin: 10
							anchors.left: parent.left
							font.bold: true
							font.pointSize: 15
							font.family: fontFamily.name
						}
					}
				}
				Rectangle{
					width: parent.width
					height: 25
					color: "transparent"
					RowLayout{
						anchors.fill: parent
						Text{
							text: "Video Source:"
							anchors.leftMargin: 10
							anchors.left: parent.left
							font.pointSize: 13
							font.family: fontFamily.name
						}
						Text{
							id: videoSrcLbl
							font.bold: true
							anchors.right: parent.right
							anchors.rightMargin: 10
							font.pointSize: 13
							font.family: fontFamily.name
						}
					}
				}
				Rectangle{
					width: parent.width
					height: 25
					color: "transparent"
					RowLayout{
						anchors.fill: parent
						Text{
							text: "Accelerator:"
							anchors.leftMargin: 10
							anchors.left: parent.left
							font.pointSize: 13
							font.family: fontFamily.name
						}
						Text{
							id: filterLbl
							font.bold: true
							anchors.right: parent.right
							anchors.rightMargin: 10
							font.pointSize: 13
							font.family: fontFamily.name
						}
					}

				}Rectangle{
					width: parent.width
					height: 25
					color: "transparent"
					RowLayout{
						anchors.fill: parent
						Text{
							text: "Display:"
							anchors.leftMargin: 10
							anchors.left: parent.left
							font.pointSize: 13
							font.family: fontFamily.name
						}
						Text{
							id: displayPortLbl
							font.bold: true
							anchors.right: parent.right
							anchors.rightMargin: 10
							font.pointSize: 13
							font.family: fontFamily.name
						}
					}
				}
			}

                }
                front:	Rectangle {
			id: performanceRect
			width: 450
			height: 300
			visible: plotDisplay
			border.color: borderColors
			border.width: boarderWidths
			color: barColors
			ChartView {
				id: chart_line_throughput
				anchors.fill: parent
				animationOptions: ChartView.NoAnimation
				property bool openGL: true
				backgroundRoundness: 0
				legend.alignment: Qt.AlignBottom
				legend.font.pixelSize: resoluteFrac < 1 ? 8 : 12
				titleFont.pixelSize: 13
				legend.visible: true
				margins.bottom: resoluteFrac < 1 ? 90 :0
				margins.top: 45
				margins.left: 10
				margins.right: resoluteFrac < 1 ? 170 :0
				plotAreaColor: "transparent"
				backgroundColor: "transparent"
				Rectangle{
					width: parent.width
					height: 35
					color: barTitleColors
					anchors.top: parent.top
					anchors.topMargin: 0
					anchors.left: parent.left
					anchors.leftMargin: 0
					Text{
						text: "Memory Throughput (Gbps)"
						anchors.leftMargin: 20
						anchors.left: parent.left
						height: parent.height
						verticalAlignment: Text.AlignVCenter
						font.bold: true
						font.pointSize: 15
						font.family: fontFamily.name
					}
				}
				ValueAxis {
					id: axisYtp
					min: 0
					max: 40
					labelFormat: "%d"
					labelsFont.pointSize: 10
					labelsColor: "black"
					gridLineColor: "black"
					lineVisible: false
                                }
                                ValueAxis {
					id: axisXtp
					min: 0
					max: 60
					labelsVisible: false
					gridLineColor: "black"
					lineVisible: false
					labelsFont.pointSize: 1
                                }
                                SplineSeries {
					name: "Video Source (00)"
					axisX: axisXtp
					axisY: axisYtp
					useOpenGL: chart_line_throughput.openGL
                                }
                                SplineSeries {
					name: "Accelerator (00)"
					axisX: axisXtp
					axisY: axisYtp
					useOpenGL: chart_line_throughput.openGL
                                }
                                SplineSeries {
					name: "Display (00)"
					axisX: axisXtp
					axisY: axisYtp
					useOpenGL: chart_line_throughput.openGL
                                }
			}
		}
		transform: Rotation {
			id: rotationMT
			origin.x: flipableMT.width/2
			origin.y: flipableMT.height/2
			axis.x: 0; axis.y: 1; axis.z: 0     // set axis.y to 1 to rotate around y-axis
			angle: 0    // the default angle
		}
		states: State {
			name: "back"
			PropertyChanges { target: rotationMT; angle: 180 }
			when: flipableMT.flipped
                }

                transitions: Transition {
			NumberAnimation { target: rotationMT; property: "angle"; duration: 200 }
                }
                MouseArea {
			anchors.fill: parent
			hoverEnabled: true
			cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor
			onClicked: flipableMT.flipped = !flipableMT.flipped

			onEntered:  {
				toolTipOnEnter(parent.x + parent.width/3,parent.y  +parent.height/2 + 10, "Click to Toggle View")
			}
			onExited: {
				toolTipOnExit()
			}
		}
                Behavior on height{
			NumberAnimation {
				duration: 500
				easing.type: Easing.OutSine
			}
		}
	}
	Rectangle{
		width: parent.width
		height: 100
		anchors.bottom: toolBarItem.top
		anchors.bottomMargin: 100
		color: "transparent"
		MouseArea{
			hoverEnabled: true
			anchors.fill: parent
			onEntered: {
				showPlayarea(false)
				showDemoarea(false)
				showInputarea(false)
				showFilterarea(false)
				showFilterTypeArea(false)
			}
		}
	}
	Timer {
		id: refreshTimer
		interval: 1000
		running: true
		repeat: true
		onTriggered: {
			dataSource.updatecpu(chart_line_CPU.series(0),chart_line_CPU.series(1),chart_line_CPU.series(2),chart_line_CPU.series(3))
			if(showMemoryThroughput)
				dataSource.updateThroughput(chart_line_throughput.series(0),chart_line_throughput.series(1),chart_line_throughput.series(2))
			if(root.play && !root.errorFound){
				dataSource.pollEvent()
			}
		}
	}
	/* Source options display view */
	Rectangle{
		id: playmodeRectangle
		width: 140
		anchors.leftMargin: 5     // icon width + margin
		anchors.left: parent.left
		height: 0
		border.color: borderColors
		border.width: boarderWidths
		clip: true
		color: barColors
		anchors.bottom: toolBarItem.top
		anchors.bottomMargin: 1

		OptionsScrollVu{
			id: playmodeSV
			anchors.fill: parent    // icon width + margin
			listModel.model: playSourceList
			selectedItem: playmodeInput
			delgate: this
			width: parent.width
			function clicked(indexval){
				popupVisible = 0
				playmodeInput = indexval
				setpreset(presetPatternSelected)
				if(playmodeInput==0){
					playmanualmode()
					playBackModeLabel.text = "Manual"
					demoSettingsButtonImage.source = "qrc:///imgs/images/gearsDisabled@"+imageResolution+".png"
					playbuttimage.source = ((!play && (playBackModeLabel.text === "Manual" )) ? "qrc:///imgs/images/play@"+imageResolution+".png" : "qrc:///imgs/images/pause@"+imageResolution+".png")
				}
				if(playmodeInput==1){
					playdemomode()
					playBackModeLabel.text = "Demo"
					demoSettingsButtonImage.source = "qrc:///imgs/images/gears@"+imageResolution+".png"
					playbuttimage.source = (((auto_start || playSourceList.displayName === "Demo")) ? "qrc:///imgs/images/pause@"+imageResolution+".png" : "qrc:///imgs/images/play@"+imageResolution+".png")
				}
			}
		}
	}
	Rectangle{
		id: inputRectangle
		width: 140
		anchors.leftMargin: (25 + 40)     // icon width + margin
		anchors.left: parent.left
		height: 0
		border.color: borderColors
		border.width: boarderWidths
		clip: true
		color: barColors
		anchors.bottom: toolBarItem.top
		anchors.bottomMargin: 1

		OptionsScrollVu{
			id: videoSrcOptionsSV
			anchors.fill: parent    // icon width + margin
			listModel.model: videoSourceList
			selectedItem: videoInput
			delgate: this
			width: parent.width
			function clicked(indexval){
				popupVisible = 0
				videoInput = indexval
				setInputType(videoSourceList[indexval].sourceIndex,play)
				setpreset(presetPatternSelected)
			}
		}
	}
	Rectangle{
		id: filterTypeRectangle
		width: 140
		anchors.leftMargin: (25 + 37 + 37 + 25)     // icon width + margin
		anchors.left: parent.left
		height: 0
		border.color: borderColors
		border.width: boarderWidths
		clip: true
		color: barColors
		anchors.bottom: toolBarItem.top
		anchors.bottomMargin: 1

		OptionsScrollVu{
			id: filterTypeOptionsSV
			anchors.fill: parent    // icon width + margin
			listModel.model: filterTypeSourceList
			selectedItem: filterTypeInput
			delgate: this
			width: 140
			function clicked(indexval){
				popupVisible = 0
				filterTypeInput = indexval
				if(play)
					setFilterType(filterTypeSourceList[indexval].sourceIndex,play)
					setpreset(presetPatternSelected)
					filterinput=0
					numericFilterTypeLabel.text = filterTypeSourceList.length ? filterTypeSourceList[filterTypeInput].displayName : ""
					numericFilterLabel.text= (filterTypeSourceList.length && filterTypeSourceList[filterTypeInput].modesList.length) ? filterTypeSourceList[filterTypeInput].modesList[filterinput].displayName : ""

			}
		}
	}
	Rectangle{
		id: sobleRectangle
		width: 140
		anchors.leftMargin: (25 + 37 + 37 + 57 + 30)
		anchors.left: parent.left
		height: 0
		border.color: borderColors
		border.width: boarderWidths
		clip: true
		color: barColors
		anchors.bottom: toolBarItem.top
		anchors.bottomMargin: 1

		OptionsScrollVu{
			id: filterModeOptionsSV
			anchors.fill: parent    // icon width + margin
			listModel.model: filterTypeSourceList[filterTypeInput].modesList
			selectedItem: filterinput
			delgate: this
			width: 140
			function clicked(indexval){
				popupVisible = 0
				filterinput = indexval
				if(play)
				    setFilter(filterTypeSourceList[filterTypeInput].modesList[indexval].sourceIndex,play)
				setpreset(presetPatternSelected)
				numericFilterLabel.text = filterTypeSourceList.length  ? filterTypeSourceList[filterTypeInput].modesList[filterinput].displayName : ""
			}
		}
	}
	Rectangle{
		id: hideRectangle
		width: root.width - (290 )
		height: 100
		anchors.leftMargin: 10
		anchors.left: sobleRectangle.right
		Behavior on height{
			NumberAnimation {
				duration: 200
				easing.type: Easing.OutBounce
			}
		}
		clip: true
		anchors.bottom: toolBarItem.top
		anchors.bottomMargin: 1
		color: "transparent"
		MouseArea{
			hoverEnabled: true
			anchors.fill: parent
			onEntered: {
				showPlayarea(false)
				showDemoarea(false)
				showInputarea(false)
				showFilterarea(false)
				showFilterTypeArea(false)
			}
		}
	}

	// bottom tool bar
	Rectangle{
		id: toolBarItem
		width: parent.width
		height: toolBarHeight
		anchors{
			bottom: parent.bottom
			bottomMargin: 20
		}
		color: barColors
		Behavior on y{
			NumberAnimation {
				duration: 200
				easing.type: Easing.OutBounce
			}
		}
		RowLayout{

			Rectangle{
				height: childrenRect.height
				clip: true
				anchors.topMargin: 0
				anchors.leftMargin: 0
				anchors.top: parent.top
				anchors.left: parent.left
				color: "transparent"
				Behavior on width{
					NumberAnimation {
						duration: 200
					}
				}
				id: playBtnArea
				width: playButton.width
					Button{
						id: playButton
						width: toolBarHeight
						height: toolBarHeight
						anchors.topMargin: 0
						anchors.leftMargin: 0
						anchors.top: parent.top
						anchors.left: parent.left
						Image {
							id:playbuttimage
							anchors.fill: playButton
							source: (!play  ? "qrc:///imgs/images/play@"+imageResolution+".png" : "qrc:///imgs/images/pause@"+imageResolution+".png")
						}
						style: ButtonStyle{
							background: Rectangle{
								color: "transparent"
							}
						}
						MouseArea{
							anchors.fill: parent
							hoverEnabled: true
							cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor
							onEntered: {
								//show other buttons
								showPlayarea(true)
								playmodeRectangle.height = 22 * playSourceCount
							}
							onPositionChanged: {
								if(!auto_start)
									toolTipOnEnter(toolBarItem.x +parent.parent.x + parent.x + (125 ),toolBarItem.y +parent.parent.y + parent.y  - (20 ), "Select PlayBack Mode")
							}
							onExited: {
								toolTipOnExit()
							}
							onClicked: {
								if(playmodeInput==1){
									playdemomode()
									demoSettingsButtonImage.source = "qrc:///imgs/images/gears@"+imageResolution+".png"
									playbuttimage.source = (((auto_start || playSourceList.displayName === "Demo")) ? "qrc:///imgs/images/pause@"+imageResolution+".png" : "qrc:///imgs/images/play@"+imageResolution+".png")
								}
								if(playmodeInput==0){
									playmanualmode()
									demoSettingsButtonImage.source = "qrc:///imgs/images/gearsDisabled@"+imageResolution+".png"
									playbuttimage.source = ((!play && (playBackModeLabel.text === "Manual")) ? "qrc:///imgs/images/play@"+imageResolution+".png" : "qrc:///imgs/images/pause@"+imageResolution+".png")
								}
							}
						}
					}
					Button{
						Image {
							id: demoSettingsButtonImage
							anchors.fill: parent
							source:(auto_start || playSourceList.displayName === "Demo") ? "qrc:///imgs/images/gears@"+imageResolution+".png" : "qrc:///imgs/images/gearsDisabled@"+imageResolution+".png"
						}
						id: demoPopSettingsButton
						width: toolBarHeight
						height: toolBarHeight
						anchors.left: playButton.right
						enabled: true
						style: ButtonStyle{
							background: Rectangle{
								color: "transparent"
							}
						}
						MouseArea{
							anchors.fill: parent
							hoverEnabled: true
							cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor
							onPositionChanged: {
								toolTipOnEnter(toolBarItem.x +parent.parent.x + parent.x + 60,toolBarItem.y +parent.parent.y + parent.y  - (20 ), "Show Demo Mode Settings")
							}
							onExited: {
								toolTipOnExit()
							}
							onClicked: {
								if(playmodeInput==1){
									popupVisible = "Demo Mode"
									popupTitle = "Demo Mode Settings"
								}
							}
						}
					}
				}
				// rectangle vertical line
				Rectangle{
					id: playAreaSeperator
					width: 2
					anchors{
						left:playBtnArea.right
					}
					color: "transparent"
					Image {
						anchors.fill: parent
						source: "qrc:///imgs/images/icon-divider@"+imageResolution+".png"
					}
					height: toolBarHeight
				}
				Rectangle{
					height: childrenRect.height
					clip: true
					color: "transparent"

					Behavior on width{
						NumberAnimation {
							duration: 200
						}
					}
					id: inputBtnArea
					width: inputButton.width
					anchors{
						left: playAreaSeperator.right
					}
					Button{
						id: inputButton
						width: toolBarHeight
						height: toolBarHeight
						Image {
							anchors.fill: parent
							source: auto_start ? "qrc:///imgs/images/configurationDisabled@"+imageResolution+".png" : "qrc:///imgs/images/configuration@"+imageResolution+".png"
						}
						style: ButtonStyle{
							background: Rectangle{
								color: "transparent"
							}
						}

						MouseArea{
							anchors.fill: parent
							hoverEnabled: true
							cursorShape: containsMouse ? (auto_start ? Qt.ArrowCursor :Qt.PointingHandCursor) : Qt.ArrowCursor
							onEntered: {
								if(!auto_start){
									showInputarea(true)
									inputRectangle.height = 22 * videoSourcesCount
								}
							}
							onPositionChanged: {
								if(!auto_start)
									toolTipOnEnter(toolBarItem.x +parent.parent.x + parent.x + (115 ),toolBarItem.y +parent.parent.y + parent.y  - (20 ), "Select Video Source")
							}
							onExited: {
								toolTipOnExit()
							}
						}
					}
					Button{
						Image {
							id: sourceSettingsButtonImage
							anchors.fill: parent
							source: (videoSourceList[videoInput].hasPanel) ? "qrc:///imgs/images/gears@"+imageResolution+".png" : "qrc:///imgs/images/gearsDisabled@"+imageResolution+".png"
						}
						id: inputPopSettingsButton
						width: toolBarHeight
						height: toolBarHeight
						anchors.left: inputButton.right
						enabled: true
						style: ButtonStyle{
							background: Rectangle{
								color: "transparent"
							}
						}
						MouseArea{
							anchors.fill: parent
							hoverEnabled: true
							cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor
							onPositionChanged: {
								toolTipOnEnter(toolBarItem.x +parent.parent.x + parent.x  + (65 ),toolBarItem.y +parent.parent.y + parent.y  - (20 ), "Show Video Source Settings")
							}
							onExited: {
								toolTipOnExit()
							}
							onClicked: {
								if(videoSourceList[videoInput].hasPanel && !auto_start){
									popupVisible = videoSourceList[videoInput].settingsPanelName
									popupTitle = videoSourceList[videoInput].displayName + " Settings"
								}
								if( videoSourceList[videoInput].shortName=="FILE" && !auto_start){
									popupVisible=videoSourceList[videoInput].settingsPanelName
									popupTitle = videoSourceList[videoInput].displayName +"  " +"Browser"
								}
							}
						}
					}
				}
				// rectangle vertical line
				Rectangle{
					id: inputButnAreaDivider
					width: 2
					anchors{
						left: inputBtnArea.right
					}
					color: "transparent"
					height: toolBarHeight
					Image {
						anchors.fill: parent
						source: "qrc:///imgs/images/icon-divider@"+imageResolution+".png"
					}
				}
				Rectangle{
					height: childrenRect.height
					clip: true
					anchors{
					}
					color: "transparent"

					Behavior on width{
						NumberAnimation {
							duration: 200
						}
					}
					id: filterTypeArea
					width: inputButton.width
					anchors{
						left: inputButnAreaDivider.right
					}

					Button{
						width: toolBarHeight
						height: toolBarHeight
						id: filterTypeButton
						Image {
							anchors.fill: parent
							source: auto_start ? "qrc:///imgs/images/filterTypeDisabled@"+imageResolution+".png" :"qrc:///imgs/images/filterType@"+imageResolution+".png"
						}
						style: ButtonStyle{
							background: Rectangle{
								color: "transparent"
							}
						}
						MouseArea{
							anchors.fill: parent
							hoverEnabled: true
							cursorShape: containsMouse ? (auto_start ? Qt.ArrowCursor : Qt.PointingHandCursor) : Qt.ArrowCursor
							onPositionChanged: {
								if(!auto_start)
									toolTipOnEnter(toolBarItem.x + parent.parent.x + parent.x + (145 ), toolBarItem.y + parent.parent.y + parent.y - (20 ), "Select Accelerator Type")
							}
							onExited: {
								toolTipOnExit()
							}
							onEntered: {
								if(!auto_start){
									showFilterTypeArea(true)
									filterTypeRectangle.height = 22 * filterTypeSourcesCount
								}
							}
						}
					}
					Button{
						Image {
							id: filterTypeSettingsImage
							anchors.fill: parent
							source: (filterTypeSourcesCount && filterTypeSourceList[filterTypeInput].hasPanel) ?  "qrc:///imgs/images/gears@"+imageResolution+".png" : "qrc:///imgs/images/gearsDisabled@"+imageResolution+".png"
						}
						width: toolBarHeight
						height: toolBarHeight
						anchors.left: filterTypeButton.right
						id:sobelpopUpSettingsButton
						style: ButtonStyle{
							background: Rectangle{
							color: "transparent"
							}
						}
						MouseArea{
							anchors.fill: parent
							hoverEnabled: true
							cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor
							onPositionChanged: {
								toolTipOnEnter(toolBarItem.x + parent.parent.x + parent.x + (90 ),toolBarItem.y + parent.parent.y + parent.y  - (20 ), "Show Accelerator Settings")
							}
							onExited: {
								toolTipOnExit()
							}
							onClicked: {
								if((filterTypeSourcesCount && filterTypeSourceList[filterTypeInput].hasPanel) && !auto_start){
									popupVisible = filterTypeSourceList[filterTypeInput].settingsPanelName
									popupTitle = filterTypeSourceList[filterTypeInput].displayName + " Settings"
								}
							}
						}
					}
				}
				// rectangle vertical line
				Rectangle{
					width: 2
					id:filterTypeBtnVertLine
					anchors{
						left: filterTypeArea.right
					}
					color: "transparent"
					height: toolBarHeight
					Image {
						anchors.fill: parent
						source: "qrc:///imgs/images/icon-divider@"+imageResolution+".png"
					}
				}
				Rectangle{
					height: childrenRect.height
					clip: true
					color: "transparent"

					Behavior on width{
						NumberAnimation {
						duration: 200
						}
					}
					id: filterBtnArea
					width: inputButton.width
					anchors{
						left: filterTypeBtnVertLine.right
					}

					Button{
						width: toolBarHeight
						height: toolBarHeight
						id: filterButton

						Image {
							id:filterModeSettingsImage
							anchors.fill: parent
							source: (auto_start || filterTypeSourceList[filterTypeInput].displayName === "Pass Through") ? "qrc:///imgs/images/filterModeDisabled@"+imageResolution+".png" :"qrc:///imgs/images/filterMode@"+imageResolution+".png"
						}
						style: ButtonStyle{
							background: Rectangle{
								color: "transparent"
							}
						}

						MouseArea{
							anchors.fill: parent
							hoverEnabled: true
							cursorShape: containsMouse ? (auto_start ? Qt.ArrowCursor :Qt.PointingHandCursor) : Qt.ArrowCursor
							onPositionChanged: {
								if(!auto_start)
									toolTipOnEnter(toolBarItem.x + parent.parent.x + parent.x + (60 ),toolBarItem.y + parent.parent.y + parent.y  - (20 ), "Select Accelerator Mode")
								}
								onExited: {
									toolTipOnExit()
								}
								onEntered: {
									if(play){
										showFilterarea(true)
										sobleRectangle.height = 22 * filterTypeSourceList[filterTypeInput].modesList.length
									}
								}
						}
					}
				}
				// rectangle vertical line
				Rectangle{
					width: 2
					id:filterBtnVertLine
					anchors{
						left: filterBtnArea.right
					}
					color: "transparent"
					height: toolBarHeight
					Image {
						anchors.fill: parent
						source: "qrc:///imgs/images/icon-divider@"+imageResolution+".png"
					}
				}
				Rectangle{
					width: 100
					anchors{
						left: filterBtnVertLine.right
						leftMargin: 8
					}
					color: "transparent"
					height: toolBarHeight
					MouseArea{
						hoverEnabled: true
						anchors.fill: parent
						onEntered: {
							showPlayarea(false)
							showDemoarea(false)
							showInputarea(false)
							showFilterarea(false)
							showFilterTypeArea(false)
						}
					}
				}
			}
			// close settings and other buttons
			Rectangle{
				id: hidePanel
				width: 100
				height: parent.height
				color: "transparent"
				anchors.right: displayOptionsPanel.left
				MouseArea{
					hoverEnabled: true
					anchors.fill: parent
					onEntered: {
					}
				}
			}

			Rectangle{
				Behavior on width{
					NumberAnimation {
						duration: 200
					}
				}
				clip: true
				id: displayOptionsPanel
				color: "transparent"
				height: toolBarHeight
				width: (2 * toolBarHeight) + (4 )
				anchors.right: controlBarPin.left
				Rectangle{
					width: 2
					anchors{
						right: displayNumPanelCheckBx.left
					}
					color: "transparent"
					height: toolBarHeight
					Image {
						anchors.fill: parent
						source: "qrc:///imgs/images/icon-divider@"+imageResolution+".png"
					}
				}
				Rectangle{
					Image {
						id: numericPanelCheckImage
						source: statsDisplay ? "qrc:///imgs/images/numStatsOn@"+imageResolution+".png" : "qrc:///imgs/images/numStats@"+imageResolution+".png"
						anchors.fill: parent
					}
					id: displayNumPanelCheckBx
					anchors.right: numstatsSepLine.left
					width: toolBarHeight
					height: toolBarHeight
					color: "transparent"
					MouseArea{
						anchors.fill: parent
						hoverEnabled: true
						cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor
						onPositionChanged: {
							toolTipOnEnter(toolBarItem.x +parent.parent.x + parent.x - (tooltipWidth),toolBarItem.y +parent.parent.y + parent.y  - (20 ), "Show/Hide Video Info")
						}
						onExited: {
							toolTipOnExit()
						}
						onClicked:{
							statsDisplay = !statsDisplay
						}
					}

				}
				Rectangle{
					id: numstatsSepLine
					width: 2
					anchors{
						right: displayPlotsPanelCheckBx.left
					}
					color: "transparent"
					height: toolBarHeight
					Image {
						anchors.fill: parent
						source: "qrc:///imgs/images/icon-divider@"+imageResolution+".png"
					}
				}
				Rectangle{
					Image {
						id: plotsPanelCheckImage
						source: plotDisplay ? "qrc:///imgs/images/statOn@"+imageResolution+".png" : "qrc:///imgs/images/stat@"+imageResolution+".png"
						anchors.fill: parent
					}
					id: displayPlotsPanelCheckBx
					anchors.right: parent.right
					width: toolBarHeight
					height: toolBarHeight
					color: "transparent"
					MouseArea{
						anchors.fill: parent
						hoverEnabled: true
						cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor
						onPositionChanged: {
							toolTipOnEnter(toolBarItem.x +parent.parent.x + parent.x - (tooltipWidth),toolBarItem.y +parent.parent.y + parent.y  - (20 ), "Show/Hide Performance Monitor")
						}
						onExited: {
							toolTipOnExit()
						}
						onClicked:{
							plotDisplay = !plotDisplay
						}
					}

				}
		}
		Rectangle{
			id: controlBarPin
			color: "transparent"
			height: toolBarHeight
			width: (toolBarHeight)+(2 )
			anchors.right: closeButton.left
			Rectangle{
			width: 2
				anchors{
					right: pinIcon.left
				}
				color: "transparent"
				height: toolBarHeight
				Image {
					anchors.centerIn: parent
					source: "qrc:///imgs/images/icon-divider@"+imageResolution+".png"
					anchors.fill: parent
				}

			}
			Button{
				id: pinIcon
				height: toolBarHeight
				width: toolBarHeight
				anchors.right: parent.right
				Image {
					anchors.centerIn: parent
					source: hideControlBar ? "qrc:///imgs/images/pin@"+imageResolution+".png" : "qrc:///imgs/images/unpin@"+imageResolution+".png"
					anchors.fill: parent
				}
				style: ButtonStyle{
					background: Rectangle{
						color: "transparent"
					}
				}
				MouseArea{
					anchors.fill: parent
					hoverEnabled: true
					cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor
					onPositionChanged: {
						toolTipOnEnter(toolBarItem.x +parent.parent.x + parent.x  - (tooltipWidth),toolBarItem.y +parent.parent.y + parent.y  - (20 ), "Pin/Unpin Control Bar")
					}
					onExited: {
						toolTipOnExit()
					}
					onClicked:{
						hideControlBar = !hideControlBar

					}
				}
			}
		}

		Rectangle{
			Rectangle{
				width: 2
				anchors{
					left: closeButton.left
				}
				color: "transparent"
				height: toolBarHeight
				Image {
					anchors.fill: parent
					source: "qrc:///imgs/images/icon-divider@"+imageResolution+".png"
				}
			}
			color: "transparent"
			width: parent.height + (2 )
			height: parent.height
			anchors.right: parent.right
			id: closeButton
			Timer{
				id: quitTimer
				repeat: false
				interval: 5
				running: false
				onTriggered: {
					Qt.quit()
				}
			}
			Button{
				height: toolBarHeight
				width: toolBarHeight
				anchors.right: parent.right
				Image {
					anchors.fill: parent
					source: "qrc:///imgs/images/close@"+imageResolution+".png"
				}
				style: ButtonStyle{
					background: Rectangle{
						color: "transparent"
					}
				}
				MouseArea{
					id: exitClickedMA
					anchors.fill: parent
					hoverEnabled: true
					cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor
					onPositionChanged: {
						toolTipOnEnter(toolBarItem.x +parent.parent.x + parent.x  - (tooltipWidth),toolBarItem.y + parent.y - (20 ), "Exit Application")
					}
					onExited: {
						toolTipOnExit()
					}
					onClicked:{
						errorMessageText = ""
						errorNameText = ""
						exitClickedMA.cursorShape = Qt.BlankCursor
						refreshTimer.stop()
						// removing other components
						appRunning = false
						popupVisible = 0
						toolBarItem.visible = false
						numericPanel.visible = false
						cpuUtilizationPlot.visible = false
						flipableMT.visible = false
						flipable.visible = false
						quitTimer.start()
					}
				}
			}
		}
	}

	/********common pop up********/
	// Common Popup holder
	PopupHolder{
		id: popup
	}
	/*************************/
	/*Error message*/
	ErrorMessage{
		width: parent.width
		height: parent.height
		id: errorMessage
		messageText: errorMessageText
		errorName: errorNameText
		message.onClicked:{
			errorMessageText = ""
			errorNameText = ""
		}
	}
	/*************************/
	/*Tool tip rectangl*/
	Tooltip {
		id: toolTipRect
		delegate: this
		function tooltipWdth(wdth){
			tooltipWidth = wdth;
		}
	}
	// Supported functions

	function showPlayarea(val){
		if(val == true){
			// hide other open menus here
			playBtnArea.width = playBtnArea.childrenRect.width
			showInputarea(false)
			showFilterarea(false)
			showFilterTypeArea(false)
			inputRectangle.height = 0
			sobleRectangle.height = 0
			filterTypeRectangle.height = 0
		}else{
			playBtnArea.width = playButton.width
		}
	}
	function showDemoarea(val){
		if(val == true){
			// hide other open menus here
			playBtnArea.width = playBtnArea.childrenRect.width
			showPlayarea(false)
			showInputarea(false)
			showFilterarea(false)
			showFilterTypeArea(false)
			inputRectangle.height = 0
			sobleRectangle.height = 0
			filterTypeRectangle.height = 0
		}else{
			playBtnArea.width = playButton.width 
		}
	}
	function showInputarea(val){
		if(val == true){
			showPlayarea(false)
			showDemoarea(false)
			showFilterarea(false)
			showFilterTypeArea(false)
			inputBtnArea.width = inputBtnArea.childrenRect.width
		}else{
			inputBtnArea.width = inputButton.width
		}
		sobleRectangle.height = 0
		playmodeRectangle.height = 0
	}
	function showFilterarea(val){
		if(val == true){
			showPlayarea(false)
			showInputarea(false)
			showDemoarea(false)
			showFilterTypeArea(false)
			filterBtnArea.width = filterBtnArea.childrenRect.width
		}else{
			filterBtnArea.width = filterButton.width
		}
		inputRectangle.height = 0
		playmodeRectangle.height = 0
	}
	function showFilterTypeArea(val){
		if(val == true){
			showPlayarea(false)
			showInputarea(false)
			showDemoarea(false)
			showFilterarea(false)
			filterTypeArea.width = filterTypeArea.childrenRect.width
		}else{
			filterTypeArea.width = filterTypeButton.width
		}
		filterTypeRectangle.height = 0
		playmodeRectangle.height = 0
	}
	function applyCoeff(){
		filterCoeff(filterCoeffList[filter00Coeff],filterCoeffList[filter01Coeff],filterCoeffList[filter02Coeff],
			filterCoeffList[filter10Coeff],filterCoeffList[filter11Coeff],filterCoeffList[filter12Coeff],
			filterCoeffList[filter20Coeff],filterCoeffList[filter21Coeff],filterCoeffList[filter22Coeff])
	}
	function abortDemoMode(val){
		filterinput = 0
		videoInput = 0
		filterTypeInput = 0
		setFilter(0,play)
		numericInputLabel.text = videoSourceList.length?videoSourceList[videoInput].shortName:"No Video Sources Available"
		numericFilterTypeLabel.text = filterTypeSourceList.length ? filterTypeSourceList[filterTypeInput].displayName : ""
		numericFilterLabel.text = filterModes[filterinput]
		videoSrcOptionsSV.resetSource(videoInput)
		filterTypeOptionsSV.resetSource(filterTypeInput)
		presetPatternSelected = 1
	}
	function playmanualmode(){
		if(auto_start){
			auto_start = !auto_start
			filterinput = 0
			videoInput = 0
			filterTypeInput = 0
			setFilter(0,play)
			numericInputLabel.text = videoSourceList.length?videoSourceList[videoInput].shortName:"No Video Sources Available"
			numericFilterTypeLabel.text = filterTypeSourceList.length ? filterTypeSourceList[filterTypeInput].displayName : ""
			numericFilterLabel.text= (filterTypeSourceList.length && filterTypeSourceList[filterTypeInput].modesList.length) ? filterTypeSourceList[filterTypeInput].modesList[filterinput].displayName : ""
			videoSrcOptionsSV.resetSource(videoInput)
			filterTypeOptionsSV.resetSource(filterTypeInput)
			demoMode(auto_start)
			presetPatternSelected = 1
		}
		if(play){
			play = false
			root.playvideo(0)
		}else{
			setInputType(videoSourceList[videoInput].sourceIndex, play)
			if(filterTypeSourcesCount)
				setFilterType(filterTypeSourceList[filterTypeInput].sourceIndex,play)
			numericFilterLabel.text= (filterTypeSourceList.length && filterTypeSourceList[filterTypeInput].modesList.length) ? filterTypeSourceList[filterTypeInput].modesList[filterinput].displayName : ""
			play = true
			root.playvideo(play)
			root.errorFound = false
		}
	}
	function playdemomode(){
		auto_start = !auto_start
		if(auto_start){
			// stop play if it is playing
			play = false
			if(play)
				root.playvideo(0)
				if(popupVisible != 0 && popupTitle != "Demo Mode Settings"){
					popupVisible = 0
				}
		}else{
			filterinput = 0
			videoInput = 0
			filterTypeInput = 0
			setFilter(0,play)
			numericInputLabel.text = videoSourceList.length?videoSourceList[videoInput].shortName:"No Video Sources Available"
			numericFilterTypeLabel.text = filterTypeSourceList.length ? filterTypeSourceList[filterTypeInput].displayName : ""
			numericFilterLabel.text= (filterTypeSourceList.length && filterTypeSourceList[filterTypeInput].modesList.length) ? filterTypeSourceList[filterTypeInput].modesList[filterinput].displayName : ""
			videoSrcOptionsSV.resetSource(videoInput)
			filterTypeOptionsSV.resetSource(filterTypeInput)
		}
		demoMode(auto_start)
		presetPatternSelected = 1
	}
}
