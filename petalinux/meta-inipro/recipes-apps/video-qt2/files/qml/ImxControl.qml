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
 * This file defines video QT application CSI-IMX274 Controls Tab option custom component.
 */
import QtQuick 2.7
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick 2.6
import QtQuick.Controls  1.5
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.3
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.4
Rectangle{
	width: parent.width-6
	height: parent.height-110
	color: "transparent"
	Rectangle{
		id:imx274rect
		width: parent.width
		height: 30
		color: "transparent"
		//DroDown
		Text {
			id: testPatternTitle
			anchors.top: parent.top
			anchors.topMargin: 45
			anchors.left: parent.left
			anchors.leftMargin: 49
			font.pixelSize: 13
			text: qsTr("Test Pattern:")
			font.family: fontFamily.name
			font.bold: true
		}
		DropDownBtn{
			id: testPatternBox
			anchors.top: parent.top
			anchors.topMargin: 43
			anchors.left: testPatternTitle.right
			anchors.leftMargin: 65
			selectedText: testPatterns[testPatternSelected]
			dropDnBtn.onClicked: {
				testComboSelected = !testComboSelected
			}
		}
		Text {
			id: exposuretext
			anchors.top: testPatternTitle.bottom
			anchors.topMargin: 50
			anchors.left: parent.left
			anchors.leftMargin: 50
			font.pixelSize: 13
			text: qsTr("Exposure:")
			font.family: fontFamily.name
			font.bold: true
		}
		Text{
			id:exposurelabel
			width:30
			text: exposureslider.value
			anchors.top: testPatternTitle.bottom
			anchors.topMargin: 50
			anchors.left: exposuretext.right
			anchors.leftMargin: 83
			font.pixelSize: 13
		}
		Slider {
			id: exposureslider
			anchors.top: testPatternTitle.bottom
			anchors.topMargin: 48
			anchors.left: exposurelabel.right
			anchors.leftMargin: 30
			maximumValue: imxexposuremaxvalue
			minimumValue: imxexposureminvalue
			style: sliderstyle
			stepSize: 1.0
			updateValueWhileDragging: true
			value: 16636
			onValueChanged:{
				csiexposure(value)
			}
		}
		Text {
			id: gaintext
			anchors.top: exposuretext.bottom
			anchors.topMargin: 35
			anchors.left: parent.left
			anchors.leftMargin: 50
			font.pixelSize: 13
			text: qsTr("Gain:")
			font.family: fontFamily.name
			font.bold: true
		}
		Text{
			id:gainlabel
			width:30
			text: gainslider.value
			anchors.top: exposurelabel.bottom
			anchors.topMargin: 36
			anchors.left: gaintext.right
			anchors.leftMargin: 109
			font.pixelSize: 13
		}
		Slider {
			id: gainslider
			anchors.top: exposureslider.bottom
			anchors.topMargin: 30
			anchors.left: gainlabel.right
			anchors.leftMargin: 30
			maximumValue: imxgainmaxvalue
			minimumValue: imxgainminvalue
			style: sliderstyle
			stepSize: 1.0
			updateValueWhileDragging: true
			value: 5120
			onValueChanged:{
				csiimxgain(value)
			}
		}
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
					implicitWidth: 180
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
		//checkbox
		Text {
			id: vfliptext
			anchors.top: gaintext.bottom
			anchors.topMargin: 38
			anchors.left: parent.left
			anchors.leftMargin: 50
			font.pixelSize: 13
			text: qsTr("Vertical Flip:")
			font.family: fontFamily.name
			font.bold: true
		}
		FocusScope {
			id: checkbox
			anchors.top: gaintext.bottom
			anchors.topMargin: 38
			anchors.left: vfliptext.right
			anchors.leftMargin: 70
			Accessible.role: Accessible.CheckBox
			property string text: "CheckBox"
			property bool checked: false
			property bool funcheck: false
			width: 20
			height: 20
			Row {
				spacing: 1
				Rectangle {
					width: 19
					height: 19
					border.width: checkbox.focus ? 2 : 1
					border.color: "black"
					Text {
						Rectangle {
							implicitWidth: 19
							implicitHeight: 19
							border.color: checkbox.activeFocus ? "green" : "black"
							border.width: 1
							Rectangle {
								visible: checkbox.checked
								color: "green"
								border.color: "gray"
								anchors.margins: 4
								anchors.fill: parent
							}
						}
					}
				}
			}
			MouseArea {
				cursorShape: Qt.PointingHandCursor
				anchors.fill: parent
				onClicked: {
					if(checkbox.checked){
						setVerticalFlip(false)
					}
					else{
						setVerticalFlip(true)
					}
					checkbox.checked = !checkbox.checked
				}
				Keys.onSpacePressed: checkbox.checked = !checkbox.checked
			}
		}

		TansparantLayer{
			visible: testComboSelected
			tlayer.onClicked: {
				testComboSelected = false
			}
		}

		DropDownScrollVu{
			id:testPatternScrl
			width: testPatternBox.width
			anchors.top: testPatternBox.bottom
			anchors.left: testPatternBox.left
			visible: testComboSelected
			listModel.model: testPatterns
			selecteItem: testPatternSelected
			delgate: this
			function clicked(indexval){
				testPatternSelected = indexval
				setTestPattern(indexval);
				testComboSelected = false;
			}
		}
	}
}
