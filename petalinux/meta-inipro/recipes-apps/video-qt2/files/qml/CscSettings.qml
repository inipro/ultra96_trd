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
 * This file defines video QT application CSi-CSC Settings Tab option custom component.
 */
import QtQuick 2.7
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

Rectangle{
	width: parent.width-6
	height: parent.height-110
	color: "transparent"
	Rectangle{
		id:cscsetrect
		width: parent.width
		height: 30
		color: "transparent"
		Text {
			id: brightnesstext
			anchors.top: parent.top
			anchors.topMargin: 25
			anchors.left: parent.left
			anchors.leftMargin: 50
			font.pixelSize: 13
			text: qsTr("Brightness:")
			font.family: fontFamily.name
			font.bold: true
		}
		Text{
			id:brightnesslabel
			width:30
			text: brightnessslider.value
			anchors.top: parent.top
			anchors.topMargin: 25
			anchors.left: brightnesstext.right
			anchors.leftMargin: 39
			font.pixelSize: 13
		}
		Slider {
			id: brightnessslider
			anchors.top: parent.top
			anchors.topMargin: 25
			anchors.left: brightnesslabel.right
			anchors.leftMargin: 30
			maximumValue: csisetingsmaxvalue
			style: sliderstyle
			stepSize: 1.0
			updateValueWhileDragging: true
			value: 50
			onValueChanged:{
				csibrightness(value)
			}
		}
		Text {
			id: contrasttext
			anchors.top: brightnesstext.bottom
			anchors.topMargin: 30
			anchors.left: parent.left
			anchors.leftMargin: 50
			font.pixelSize: 13
			text: qsTr("Contrast:")
			font.family: fontFamily.name
			font.bold: true
		}
		Text{
			id:contrastlabel
			width:30
			text: contrastslider.value
			anchors.top: brightnesslabel.bottom
			anchors.topMargin: 30
			anchors.left: contrasttext.right
			anchors.leftMargin: 52
			font.pixelSize: 13
		}
		Slider {
			id: contrastslider
			anchors.top: brightnessslider.bottom
			anchors.topMargin: 28
			anchors.left: contrastlabel.right
			anchors.leftMargin: 30
			maximumValue: csisetingsmaxvalue
			style: sliderstyle
			stepSize: 1.0
			updateValueWhileDragging: true
			value: 50
			onValueChanged:{
				csicontrast(value)
			}
		}
		Text {
			id: redgaintext
			anchors.top: contrasttext.bottom
			anchors.topMargin: 30
			anchors.left: parent.left
			anchors.leftMargin: 50
			font.pixelSize: 13
			text: qsTr("Red Gain:")
			font.family: fontFamily.name
			font.bold: true
		}
		Text{
			id:redgainlabel
			width:30
			text: redgainslider.value
			anchors.top: contrastlabel.bottom
			anchors.topMargin: 30
			anchors.left: redgaintext.right
			anchors.leftMargin: 47
			font.pixelSize: 13
		}
		Slider {
			id: redgainslider
			anchors.top: contrastslider.bottom
			anchors.topMargin: 25
			anchors.left: redgainlabel.right
			anchors.leftMargin: 31
			maximumValue: csisetingsmaxvalue
			style: sliderstyle
			stepSize: 1.0
			updateValueWhileDragging: true
			value: 50
			onValueChanged:{
				csiredgain(value)
			}
		}
		Text {
			id: greengaintext
			anchors.top: redgaintext.bottom
			anchors.topMargin: 30
			anchors.left: parent.left
			anchors.leftMargin: 50
			font.pixelSize: 13
			text: qsTr("Green Gain:")
			font.family: fontFamily.name
			font.bold: true
		}
		Text{
			id:greengainlabel
			width:30
			text: greengainslider.value
			anchors.top: redgainlabel.bottom
			anchors.topMargin: 30
			anchors.left: greengaintext.right
			anchors.leftMargin: 35
			font.pixelSize: 13
		}
		Slider {
			id: greengainslider
			anchors.top: redgainslider.bottom
			anchors.topMargin: 25
			anchors.left: greengainlabel.right
			anchors.leftMargin: 30
			maximumValue: csisetingsmaxvalue
			style: sliderstyle
			stepSize: 1.0
			updateValueWhileDragging: true
			value: 50
			onValueChanged:{
				csigreengain(value)
			}
		}
		Text {
			id: bluegaintext
			anchors.top: greengaintext.bottom
			anchors.topMargin: 30
			anchors.left: parent.left
			anchors.leftMargin: 50
			font.pixelSize: 13
			text: qsTr("Blue Gain:")
			font.family: fontFamily.name
			font.bold: true
		}
		Text{
			id:bluegainlabel
			width:30
			text: bluegainslider.value
			anchors.top: greengainlabel.bottom
			anchors.topMargin: 30
			anchors.left: bluegaintext.right
			anchors.leftMargin: 45
			font.pixelSize: 13
		}
		Slider {
			id: bluegainslider
			anchors.top: greengainslider.bottom
			anchors.topMargin: 25
			anchors.left: bluegainlabel.right
			anchors.leftMargin: 30
			maximumValue: csisetingsmaxvalue
			style: sliderstyle
			stepSize: 1.0
			updateValueWhileDragging: true
			value: 50
			onValueChanged:{
				csibluegain(value)
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
					implicitWidth: 190
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
	}
}
