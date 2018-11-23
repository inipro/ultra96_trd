set_property IOSTANDARD LVCMOS18 [get_ports BT*]

#BT_HCI_RTS on FPGA /  emio_uart0_ctsn connect to 
set_property PACKAGE_PIN B7 [get_ports BT_ctsn]
#BT_HCI_CTS on FPGA / emio_uart0_rtsn
set_property PACKAGE_PIN B5 [get_ports BT_rtsn]

set_property IOSTANDARD LVCMOS18 [get_ports UART*]

#HD_GPIO_3 on FPGA / Connector pin 9 / UART0_rxd
set_property PACKAGE_PIN G7 [get_ports UART0_rxd]
#HD_GPIO_1 on FPGA / Connector pin 5 / UART0_txd
set_property PACKAGE_PIN F8 [get_ports UART0_txd]

