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
 * This file defines video QT application CSI panel setting option custom component.
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
Rectangle{
	id: settingsPanel
	width: 500
	height: 400
	anchors.fill: parent
	color: "transparent"
	MouseArea{
		anchors.fill: parent
	}
	Rectangle {
		anchors{
			horizontalCenter: parent.horizontalCenter
			verticalCenter: parent.verticalCenter
		}
		width: 500
		height: 370
		color: "transparent"
		border.color: "gray"
		border.width: 1
		Row{
			id: tabRow
			anchors{
				left: parent.left
				leftMargin: 2
				right: parent.right
				rightMargin: 2
				topMargin: 2
			}
			width: parent.width-5
			height: 25
			Rectangle{
				id: tab3
				height: parent.height
				width: parent.width/3
				color: "transparent"
				border.color: "black"
				border.width: resoluteFrac < 1 ? 1.5 : 1
				Text{
					anchors.fill: parent
					text: "IMX274"
					font.family: fontFamily.name
					verticalAlignment: Text.AlignVCenter
					horizontalAlignment: Text.AlignHCenter
				}
				Rectangle{
					id: tab3UL
					anchors.left: parent.left
					anchors.bottom: parent.bottom
					anchors.bottomMargin: 1
					height: 1
					width: parent.width
					color: "black"
					visible: false
				}
				MouseArea{
					anchors.fill: parent
					onClicked: {
						tab1UL.visible = true
						tab2UL.visible = true
						tab3UL.visible = false
						tab1.color = "lightgray"
						tab2.color = "lightgray"
						tab3.color = "transparent"
						imxcontrolstab.visible = true
						cscsettingstab.visible = false
						gammasettingstab.visible = false
					}
				}
			}
			Rectangle{
				id: tab1
				height: parent.height
				width: parent.width/3
				color: "lightgray"
				border.color: "black"
				border.width: resoluteFrac < 1 ? 1.5 : 1
				Text{
					anchors.fill: parent
					text:  qsTr("Gamma")
					font.family: fontFamily.name
					verticalAlignment: Text.AlignVCenter
					horizontalAlignment: Text.AlignHCenter
				}
				Rectangle{
					id: tab1UL
					anchors.left: parent.left
					anchors.bottom: parent.bottom
					anchors.bottomMargin: 1
					height: 1
					width: parent.width
					color: "black"
					visible: true
				}
				MouseArea{
					anchors.fill: parent
					onClicked: {
						tab1UL.visible = false
						tab2UL.visible = true
						tab3UL.visible = true
						tab1.color = "transparent"
						tab2.color = "lightgray"
						tab3.color = "lightgray"
						cscsettingstab.visible = false
						gammasettingstab.visible = true
						imxcontrolstab.visible = false
					}
				}
			}
			Rectangle{
				id: tab2
				height: parent.height
				width: parent.width/3
				color: "lightgray"
				border.color: "black"
				border.width: resoluteFrac < 1 ? 1.5 : 1
				Text{
					anchors.fill: parent
					text:  qsTr("CSC")
					font.family: fontFamily.name
					verticalAlignment: Text.AlignVCenter
					horizontalAlignment: Text.AlignHCenter
				}
				Rectangle{
					id: tab2UL
					anchors.left: parent.left
					anchors.bottom: parent.bottom
					anchors.bottomMargin: 1
					height: 1
					width: parent.width
					color: "black"
					visible: true
				}
				MouseArea{
					anchors.fill: parent
					onClicked: {
						tab1UL.visible = true
						tab3UL.visible = true
						tab2UL.visible = false
						tab1.color = "lightgray"
						tab3.color = "lightgray"
						tab2.color = "transparent"
						cscsettingstab.visible = true
						gammasettingstab.visible = false
						imxcontrolstab.visible = false
					}
				}
			}
		}

		GammaSet{
			id: gammasettingstab
			visible: false
			color: "transparent"
			anchors{
				left: parent.left
				leftMargin: 3
				top: tabRow.bottom
				topMargin: -1
			}
		}
		CscSettings{
			id: cscsettingstab
			visible: false
			color: "transparent"
			anchors{
				left: parent.left
				leftMargin: 3
				top: tabRow.bottom
				topMargin: -1
			}
		}
		ImxControl{
			id: imxcontrolstab
			visible: true
			color: "transparent"
			anchors{
				left: parent.left
				leftMargin: 3
				top: tabRow.bottom
				topMargin: -1
			}
		}
	}
}
