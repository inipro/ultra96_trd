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
 * This file defines video QT application spinner custom component.
 */

import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.0
import QtQuick.Layouts 1.0
Rectangle{
	width: (widthCoeffsBox + 20 )
	color: "#00000000"
	height: 20
	property alias filterValue: filterVal.text
	property alias plusButton: plusButtonMA
	property alias minusButton: minusButtonMA
	property bool minusButtonVisible: true
	property bool plusButtonVisible: true
	property bool showButtons: false

	Rectangle{
		width: widthCoeffsBox
		height: 20
		anchors.top: parent.top
		anchors.topMargin: 0
		anchors.left: parent.left
		anchors.leftMargin: 0
		Text {
			id: filterVal
			color: "black"
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.fill: parent
			horizontalAlignment: Text.AlignHCenter
			font.pixelSize: 13
			font.family: fontFamily.name

		}
	}
	Rectangle{
		id: plsBtn
		width: 20
		height: 10
		anchors.top: parent.top
		anchors.topMargin: 0
		anchors.right: parent.right
		anchors.rightMargin: 0
		visible: showButtons
		color: barTitleColors
		MouseArea{
			id: plusButtonMA
			anchors.fill: parent
			hoverEnabled: true
			cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor
		}
		Image {
			anchors.centerIn: parent
			anchors.fill: parent
			source: "qrc:///imgs/images/plus@"+imageResolution+".png"
		}

	}
	Rectangle{
		width: 20
		height: 10
		anchors.top: plsBtn.bottom
		anchors.topMargin: 0
		anchors.right:  parent.right
		anchors.rightMargin: 0
		visible: showButtons
		color: barTitleColors
		MouseArea{
			id: minusButtonMA
			anchors.fill: parent
			hoverEnabled: true
			cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor
		}

		Image {
			anchors.centerIn: parent
			anchors.fill: parent
			source: "qrc:///imgs/images/minus@"+imageResolution+".png"
		}
	}
}
