#============================================================================
# Run: 
#    xtclsh xpacne00_edge_detect_ise.tcl  - creates XILINX ISE project file
#    ise xpacne00_edge_detect_project.ise - opens the project
#============================================================================
source "../../../../../base/xilinxise.tcl"

project_new "xpacne00_edge_detect_project"
project_set_props
puts "Adding source files"
xfile add "../../../../../fpga/units/clkgen/clkgen_config.vhd"
xfile add "xpacne00_edge_detect_config.vhd"
xfile add "../../../../../fpga/units/clkgen/clkgen.vhd"
xfile add "../../../../../fpga/chips/fpga_xc3s50.vhd"
xfile add "../../fpga/top.vhd"
project_set_top "fpga"
project_close
