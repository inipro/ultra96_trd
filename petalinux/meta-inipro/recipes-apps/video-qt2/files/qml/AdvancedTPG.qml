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
 * This file defines video QT application TPG panel setting option custom component.
 */

import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.0
import QtQuick.Layouts 1.0

Rectangle{
	anchors.fill: parent
	color: "transparent"
	Component {
		id: sliderstyle
		SliderStyle {
			handle: Rectangle {
				anchors.centerIn: parent
				color: "gray"
				border.color: "gray"
				antialiasing: true
				border.width: 2
				implicitWidth: 10
				implicitHeight: 20
			}
			groove: Item {
				implicitHeight: 3
				implicitWidth: 194
				Rectangle {
					height: 3
					width: parent.width
					color: "gray"
					Rectangle {
						antialiasing: true
						radius: 1
						color: "blue"
						height: parent.height
						width: parent.width * control.value / control.maximumValue
					}
				}
			}
		}
	}

	Text {
		id: tpgPatternTitle
		anchors.top: parent.top
		anchors.topMargin: 20
		anchors.left: parent.left
		anchors.leftMargin: 50
		font.pixelSize: 13
		text: qsTr("TPG Pattern:")
		font.family: fontFamily.name
		font.bold: true
	}
	DropDownBtn{
		id: tpgBox
		anchors.top: parent.top
		anchors.topMargin: 20
		anchors.left: tpgPatternTitle.right
		anchors.leftMargin: 60
		selectedText: tpgPatterns[tpgPatternSelected]
		dropDnBtn.onClicked: {
			tpgComboSelected = !tpgComboSelected
		}
	}

	Text {
		id: zoneHPlateTitle
		anchors.top: tpgPatternTitle.bottom
		anchors.topMargin: 20
		anchors.left: parent.left
		anchors.leftMargin: 50
		font.pixelSize: 13
		text: qsTr("Horizontal Zone Plate:")
		font.family: fontFamily.name
		font.bold: true
		visible: tpgPatternSelected == 9
	}
	Text {
		id:zoneHPlatelabel
		width:50
		text: zoneHPlateslider.value
		anchors.top: tpgPatternTitle.bottom
		anchors.topMargin: 20
		anchors.left: zoneHPlateTitle.right
		anchors.leftMargin: 10
		font.pixelSize: 13
		visible: tpgPatternSelected == 9
	}
	Slider {
		id: zoneHPlateslider
		anchors.top: tpgBox.bottom
		anchors.topMargin: 15
		anchors.left: zoneHPlatelabel.right
		anchors.leftMargin: 2
		visible: tpgPatternSelected == 9
		maximumValue: maxZoneHPlateValue
		style: sliderstyle
		stepSize: 1.0
		updateValueWhileDragging: true
		value: 30
		onValueChanged:{
			setZoneH(value)
		}
	}

	Text {
		id: zoneHPlateDeltaTitle
		anchors.top: zoneHPlateTitle.bottom
		anchors.topMargin: 20
		anchors.left: parent.left
		anchors.leftMargin: 50
		font.pixelSize: 13
		text: qsTr("Horizontal Zone Delta:")
		font.family: fontFamily.name
		font.bold: true
		visible: tpgPatternSelected == 9
	}
	Text {
		id:zoneHPlateDeltalabel
		width:50
		text: zoneHPlateDeltaslider.value
		anchors.top: zoneHPlatelabel.bottom
		anchors.topMargin: 20
		anchors.left: zoneHPlateDeltaTitle.right
		anchors.leftMargin: 11
		font.pixelSize: 13
		visible: tpgPatternSelected == 9
	}
	Slider {
		id: zoneHPlateDeltaslider
		anchors.top: zoneHPlateTitle.bottom
		anchors.topMargin: 15
		anchors.left: zoneHPlateDeltalabel.right
		anchors.leftMargin: 1
		visible: tpgPatternSelected == 9
		maximumValue: maxZoneHPlateDeltaValue
		style: sliderstyle
		stepSize: 1.0
		updateValueWhileDragging: true
		value: 0
		onValueChanged:{
			setZoneHDelta(value)
		}
	}

	Text {
		id: zoneVPlateTitle
		anchors.top: zoneHPlateDeltaTitle.bottom
		anchors.topMargin: 20
		anchors.left: parent.left
		anchors.leftMargin: 50
		font.pixelSize: 13
		text: qsTr("Vertical Zone Plate:")
		font.family: fontFamily.name
		font.bold: true
		visible: tpgPatternSelected == 9
	}
	Text {
		id:zoneVPlatelabel
		width:50
		text: zoneVPlateslider.value
		anchors.top: zoneHPlateDeltalabel.bottom
		anchors.topMargin: 20
		anchors.left: zoneVPlateTitle.right
		anchors.leftMargin: 27
		font.pixelSize: 13
		visible: tpgPatternSelected == 9
	}
	Slider {
		id: zoneVPlateslider
		anchors.top: zoneHPlateDeltaTitle.bottom
		anchors.topMargin: 15
		anchors.left: zoneVPlatelabel.right
		anchors.leftMargin: 1
		visible: tpgPatternSelected == 9
		maximumValue: maxZoneVPlateValue
		style: sliderstyle
		stepSize: 1.0
		updateValueWhileDragging: true
		value: 1
		onValueChanged:{
			setZoneV(value)
		}
	}

	Text {
		id: zoneVPlateDeltaTitle
		anchors.top: zoneVPlateTitle.bottom
		anchors.topMargin: 20
		anchors.left: parent.left
		anchors.leftMargin: 50
		font.pixelSize: 13
		text: qsTr("Vertical Zone Delta:")
		font.family: fontFamily.name
		font.bold: true
		visible: tpgPatternSelected == 9
	}
	Text{
		id:zoneVPlateDeltalabel
		width:50
		text: zoneVPlateDeltaslider.value
		anchors.top: zoneVPlatelabel.bottom
		anchors.topMargin: 20
		anchors.left: zoneVPlateDeltaTitle.right
		anchors.leftMargin: 27
		font.pixelSize: 13
		visible: tpgPatternSelected == 9
	}
	Slider {
		id: zoneVPlateDeltaslider
		anchors.top: zoneVPlateTitle.bottom
		anchors.topMargin: 15
		anchors.left: zoneVPlateDeltalabel.right
		anchors.leftMargin: 0
		visible: tpgPatternSelected == 9
		maximumValue: maxZoneVPlateDeltaValue
		style: sliderstyle
		stepSize: 1.0
		updateValueWhileDragging: true
		value: 0
		onValueChanged:{
			setZoneVDelta(value)
		}
	}

	Text {
		id: boxSpeedTxt
		anchors.top: zoneVPlateDeltaTitle.visible  ? zoneVPlateDeltaTitle.bottom : tpgBox.bottom
		anchors.topMargin: 20
		anchors.left: parent.left
		anchors.leftMargin: 50
		font.pixelSize: 13
		text: qsTr("Motion Speed:")
		font.family: fontFamily.name
		font.bold: true
		visible: true

	}
	Text {
		id:boxSpeedlabel
		width:50
		text: sliderBoxSpeed.value
		anchors.top: zoneVPlateDeltalabel.visible ? zoneVPlateDeltaTitle.bottom : tpgBox.bottom
		anchors.topMargin: 20
		anchors.left: boxSpeedTxt.right
		anchors.leftMargin: 57
		font.pixelSize: 13
	}
	Slider {
		id: sliderBoxSpeed
		anchors.top: zoneVPlateDeltaTitle.visible  ? zoneVPlateDeltaTitle.bottom : tpgBox.bottom
		anchors.topMargin: 15
		anchors.left: tpgBox.left
		anchors.leftMargin: 56
		maximumValue: maxBoxSpeedValue
		style: sliderstyle
		stepSize: 1.0
		updateValueWhileDragging: true
		value: 4
		onValueChanged:{
			setBoxSpeed(value)
		}
	}

	Text {
		id: foreGroundTitle
		anchors.top: sliderBoxSpeed.bottom
		anchors.topMargin: 20
		anchors.left: parent.left
		anchors.leftMargin: 50
		font.pixelSize: 13
		text: qsTr("Foreground Overlay:")
		font.family: fontFamily.name
		font.bold: true
	}
	DropDownBtn{
		id: fgBox
		anchors.top: sliderBoxSpeed.bottom
		anchors.topMargin: 15
		anchors.left: tpgPatternTitle.right
		anchors.leftMargin: 60
		selectedText: foreGroundList[foreGroundSelectOpt]
		dropDnBtn.onClicked: {
			fgComboSelected = !fgComboSelected
		}
	}
	Text {
		id: boxColorText
		anchors.top: fgBox.bottom
		anchors.topMargin: 20
		anchors.left: parent.left
		anchors.leftMargin: 50
		font.pixelSize: 13
		text: qsTr("Box Color:")
		font.family: fontFamily.name
		font.bold: true
		visible: foreGroundSelectOpt == 1

	}
	Rectangle{
		id: boxColorCombo
		width: 250
		height: 20
		anchors.top: fgBox.bottom
		anchors.topMargin: 15
		anchors.left:  tpgPatternTitle.right
		visible: foreGroundSelectOpt == 1
		anchors.leftMargin: 60
		color:"#00000000"
		GridLayout{
			columns: 8
			rowSpacing: 0
			columnSpacing: 0
			Repeater{
				id: boxColorRepeater
				model: boxColorList
				Rectangle{
					Text {
						font.family: fontFamily.name
						font.pointSize: 13
						anchors.horizontalCenter: parent.horizontalCenter
					}
					height: 20
					width: 20
					color: modelData
					border.color: boxColorbSelect == index ? "#0C95CD" : barColors
					border.width: 3
					MouseArea{
						anchors.fill: parent
						hoverEnabled: true
						cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor
						onClicked:{
							boxColorbSelect = index
							setBoxColor(boxYUVColors[boxColorbSelect])
							boxColorSelected= false;
						}
					}
				}
			}
		}
	}

	Text {
		id: boxSize
		anchors.top: boxColorCombo.bottom
		anchors.topMargin: 20
		anchors.left: parent.left
		anchors.leftMargin: 50
		font.pixelSize: 13
		text: qsTr("Box Size:")
		font.family: fontFamily.name
		font.bold: true
		visible: foreGroundSelectOpt == 1

	}
	Text {
		id:boxSizelabel
		width:50
		text: sliderBoxSize.value
		anchors.top: boxColorCombo.bottom
		anchors.topMargin: 20
		anchors.left: boxSize.right
		anchors.leftMargin: 87
		font.pixelSize: 13
		visible: foreGroundSelectOpt == 1
	}
	Slider {
		id: sliderBoxSize
		anchors.top: boxColorCombo.bottom
		anchors.topMargin: 15
		anchors.left: boxSizelabel.right
		anchors.leftMargin: 1
		maximumValue: maxBoxSizeValue
		visible: foreGroundSelectOpt == 1
		style: sliderstyle
		stepSize: 1.0
		updateValueWhileDragging: true
		value: 150
		onValueChanged:{
			setBoxSize(value)
		}
	}

	// Cross hairs options

	Text {
		id: crossRowsText
		anchors.top: foreGroundTitle.bottom
		anchors.topMargin: 20
		anchors.left: parent.left
		anchors.leftMargin: 50
		font.pixelSize: 13
		text: qsTr("Cross Hairs Row:")
		font.family: fontFamily.name
		font.bold: true
		visible: foreGroundSelectOpt == 2

	}
	Text {
		id:crossRowslabel
		width:50
		text: crossRows.value
		anchors.top: foreGroundTitle.bottom
		anchors.topMargin: 20
		anchors.left: crossRowsText.right
		anchors.leftMargin: 42
		font.pixelSize: 13
		visible: foreGroundSelectOpt == 2
	}
	Slider {
		id: crossRows
		anchors.top: fgBox.bottom
		anchors.topMargin: 15
		anchors.left: crossRowslabel.right
		anchors.leftMargin: 0
		maximumValue: maxCrossRowsValue
		visible: foreGroundSelectOpt == 2
		style: sliderstyle
		stepSize: 1.0
		updateValueWhileDragging: true
		value: 100
		onValueChanged:{
			setCrossRows(value)
		}
	}

	Text {
		id: crossColumnsText
		anchors.top: crossRowsText.bottom
		anchors.topMargin: 20
		anchors.left: parent.left
		anchors.leftMargin: 50
		font.pixelSize: 13
		text: qsTr("Cross Hairs Column:")
		font.family: fontFamily.name
		font.bold: true
		visible: foreGroundSelectOpt == 2

	}
	Text {
		id:crossColumnslabel
		width:50
		text: crossColumnsslider.value
		anchors.top: crossRowslabel.bottom
		anchors.topMargin: 20
		anchors.left: crossColumnsText.right
		anchors.leftMargin: 23
		font.pixelSize: 13
		visible: foreGroundSelectOpt == 2
	}
	Slider {
		id: crossColumnsslider
		anchors.top: crossRows.bottom
		anchors.topMargin: 15
		anchors.left: crossColumnslabel.right
		anchors.leftMargin: 0
		maximumValue: maxCrossColumnsValue
		visible: foreGroundSelectOpt == 2
		style: sliderstyle
		stepSize: 1.0
		updateValueWhileDragging: true
		value: 100
		onValueChanged:{
			setCrossColumns(value)
		}
	}

	/*A transparent layer on clicked will dismiss the open pop over combobox optinos*/
	TansparantLayer{
		visible: tpgComboSelected || fgComboSelected || boxColorSelected
		tlayer.onClicked: {
			tpgComboSelected = false
			fgComboSelected = false
			boxColorSelected = false
		}
	}

	// dropdown area for TPG
	DropDownScrollVu{
		id:tpgScrl
		width: tpgBox.width
		anchors.top: tpgBox.bottom
		anchors.left: tpgBox.left
		visible: tpgComboSelected
		listModel.model: tpgPatterns
		selecteItem: tpgPatternSelected
		delgate: this
		function clicked(indexval){
			tpgPatternSelected = indexval
			setTPGPattern(indexval);
			tpgComboSelected = false;
		}
	}

	DropDownScrollVu{
		id:fgBxScrl
		width: fgBox.width
		anchors.top: fgBox.bottom
		anchors.left: fgBox.left
		visible: fgComboSelected
		listModel.model: foreGroundList
		selecteItem: foreGroundSelectOpt
		delgate: this
		function clicked(indexval){
			foreGroundSelectOpt = indexval
			setFgOverlay(indexval)
			fgComboSelected = false;
		}
	}
}
