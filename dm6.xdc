set_property IOSTANDARD LVCMOS18 [get_ports BT*]

#BT_HCI_RTS on FPGA /  emio_uart0_ctsn connect to 
set_property PACKAGE_PIN B7 [get_ports BT_ctsn]
#BT_HCI_CTS on FPGA / emio_uart0_rtsn
set_property PACKAGE_PIN B5 [get_ports BT_rtsn]

set_property IOSTANDARD LVCMOS18 [get_ports UART*]

#HD_GPIO_9 on FPGA / Connector pin 16 / UART0_rxd
set_property PACKAGE_PIN E6 [get_ports UART0_rxd]
#HD_GPIO_11 on FPGA / Connector pin 20 / UART0_txd
set_property PACKAGE_PIN D6 [get_ports UART0_txd]

