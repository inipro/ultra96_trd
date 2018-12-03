/******************************************************************************
 * (c) Copyright 2017 Xilinx, Inc. All rights reserved.
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
 * This file defines video QT application File Browser custom component.
 */

import QtQuick 2.7
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.0
import QtQuick 2.4
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.2
import QtQuick.Controls 1.3

Rectangle{
	id:fileList
	property alias fileListModel: listView.model
	anchors.fill: parent
	color: "transparent"
	MouseArea{
		anchors.fill: parent
	}
	Rectangle{
		anchors.fill: parent
		property var fileName: ""
		property var selectedDir: ""
		property var filenamepath :""
		onVisibleChanged: {
			if(visible){
				var lst = dirOPS.changeFolder("/media/card/")
				updateTable(lst)
			}
		}
		id: browseVU
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.verticalCenter
		color: "transparent"
		radius: 2
		border.color: "gray"
		Button{
			id: prevDirBtn
			text: "ok"
			anchors{
				left: parent.left
				leftMargin: 10
				top: parent.top
				topMargin: 10
			}
			width: 25
			height: 25
			Image {
				anchors.centerIn: parent
				source: "qrc:///imgs/images/backFolder@"+imageResolution+".png"
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
				onExited: {
				}
				onClicked:{
					dirOPS.applyTypeFilter("*")
					var lst = dirOPS.previousClick()
					updateTable(lst)
					browseVU.fileName = ""
				}
			}
		}
		// rectangle vertical line
		Rectangle{
			id:previousBtnVertLine
			anchors{
				left: prevDirBtn.right
				leftMargin:13
				top: parent.top
				topMargin: 2
			}
			height: 35
			width: 1
			color: "black"
		}
		Rectangle{
			anchors{
				left: previousBtnVertLine.right
				leftMargin: 10
				right: parent.right
				rightMargin: 10
				top: parent.top
				topMargin: 10
			}
			height: 25
			color: "white"
			Text{
				height: parent.height
				width: parent.width
				color: "black"
				id: filePathLbl
				verticalAlignment: Text.AlignVCenter
			}
		}
		Rectangle{
			id:linerect
			anchors{
				left: parent.left
				right: parent.right
				top: prevDirBtn.bottom
				topMargin: 3
			}
			height: 1
			color: "black"
		}
		TableView {
			backgroundVisible: false
			id: listView
			clip: true
			anchors{
				left: parent.left
				leftMargin: 5
				right: parent.right
				rightMargin: 5
				top: prevDirBtn.bottom
				topMargin: 12
				bottom: openBtn.top
				bottomMargin:10
			}
			TableViewColumn {
				role: "icon"
				title: ""
				width: 20
				delegate: Image {
					fillMode: Image.PreserveAspectFit
					height:20
					cache : true;
					asynchronous: true;
					source:  styleData.value
				}
				resizable: false
				movable: false
			}
			TableViewColumn{
				role: "itemName"
				title: ""
				width: 200
				movable: false
				delegate: Item {
					Text {
						anchors.verticalCenter: parent.verticalCenter
						anchors.left:parent.left
						anchors.leftMargin: 12
						property variant value: styleData.value
						text: styleData.value
						font.family: fontFamily.name
					}
				}
			}
			model: libraryModel
			style: TableViewStyle {
				headerDelegate: Rectangle {
					height: 0
					width: textItem.implicitWidth
					Text {
						id: textItem
						anchors.fill: parent
						verticalAlignment: Text.AlignVCenter
						horizontalAlignment: styleData.textAlignment
						anchors.leftMargin: 12
						text: styleData.value
						elide: Text.ElideLeft
						color: textColor
					}
					Rectangle {
						anchors.right: parent.right
						anchors.top: parent.top
						anchors.bottom: parent.bottom
						anchors.bottomMargin: 1
						anchors.topMargin: 1
						width: 1
						color: "transparent"
					}
				}
			}
			Component {
				id: folderImageDelegate
				Item {
					Image {
						anchors.verticalCenter: parent.verticalCenter
						anchors.horizontalCenter: parent.horizontalCenter
						fillMode: Image.PreserveAspectFit
						height:20
						cache : true;
						asynchronous: true;
						source:  "qrc:///imgs/images/folder@"+imageResolution+".png"
					}
				}
			}
			Component {
				id: fileImageDelegate
				Item {
					Image {
						anchors.verticalCenter: parent.verticalCenter
						anchors.horizontalCenter: parent.horizontalCenter
						fillMode: Image.PreserveAspectFit
						height:18
						cache : true;
						asynchronous: true;
						source:  "qrc:///imgs/images/textFile@"+imageResolution+".png"
					}
				}
			}
			onClicked: {
				if(!libraryModel.get(row).itemType){
					browseVU.fileName = libraryModel.get(row).itemName
					browseVU.selectedDir = ""
				}
				else{
					browseVU.fileName = ""
					browseVU.selectedDir = libraryModel.get(row).itemName
				}
			}
			Text{
				ListModel{
					id: libraryModel
				}
			}
			onDoubleClicked: {
				if(libraryModel.get(row).itemType){
					var lst = dirOPS.changeFolder(libraryModel.get(row).itemName)
					updateTable(lst)
				}
				else{
					browseVU.filenamepath = filePathLbl.text + "/" + libraryModel.get(row).itemName
					fileSelect(browseVU.filenamepath,play)
					popupVisible = 0
				}
			}
		}
		Button{
			id: cancelBtn
			anchors{
				right: openBtn.left
				rightMargin: 20
				bottom: parent.bottom
				bottomMargin: 10
			}
			width: 100
			height: 30
			Text{
				text: "Cancel"
				height: parent.height
				width: parent.width
				verticalAlignment: Text.AlignVCenter
				horizontalAlignment: Text.AlignHCenter
				font.family: fontFamily.name
			}
			style: ButtonStyle{
				background: Rectangle{
					border.color: "gray"
					border.width: 1
				}
			}
			MouseArea{
				anchors.fill: parent
				hoverEnabled: true
				cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor
				onClicked:{
					browseVU.fileName=""
					dirOPS.applyTypeFilter("*")
					popupVisible = 0
				}
			}
		}
		Button{
			id: openBtn
				anchors{
				right: parent.right
				rightMargin: 10
				bottom: parent.bottom
				bottomMargin: 10
			}
			width: 100
			height: 30
			Text{
				text: "Open"
				height: parent.height
				width: parent.width
				verticalAlignment: Text.AlignVCenter
				horizontalAlignment: Text.AlignHCenter
				font.family: fontFamily.name
			}
			style: ButtonStyle{
				background: Rectangle{
					border.color: "gray"
					border.width: 1
				}
			}
			MouseArea{
				anchors.fill: parent
				hoverEnabled: true
				cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor
				onExited: {
				}
				onClicked:{
					if(browseVU.fileName.length){
						browseVU.filenamepath = filePathLbl.text + "/" + browseVU.fileName
						fileSelect(browseVU.filenamepath,play)
						popupVisible = 0
					}
					else{
						if(browseVU.selectedDir.length){
							var lst = dirOPS.changeFolder(browseVU.selectedDir)
							updateTable(lst)
							browseVU.selectedDir = ""
						}
					}
				}
			}
		}
		TextField{
			id: filterType
			visible: false
			anchors{
				left: parent.left
				leftMargin: 10
				right: cancelBtn.left
				rightMargin: 10
				bottom: parent.bottom
				bottomMargin: 10
			}
			MouseArea{
				anchors.fill: parent
				onClicked: {
					dirOPS.applyTypeFilter("")
				}
			}
		}

	}
	function setFilePath(){
		filePathLbl.text = dirOPS.getFilePath()
	}
	function updateTable(lst){
		libraryModel.clear()
		for(var i = 0; i <lst.length; i++){
			libraryModel.append({"itemName": lst[i].itemName, "itemType": lst[i].itemType,"icon": (lst[i].itemType ? "qrc:///imgs/images/folder@"+imageResolution+".png" : "qrc:///imgs/images/textFile@"+imageResolution+".png")});
		}
		setFilePath()
	}
}

