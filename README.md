# RockSim 10's RKT format specification reader
A small command-line utility for .RKT files to a more readable C-style file. Known issues:
- some names use '-' as separator which makes the output not really a C header
- some data types are not specified (`Q3ValueVector<int>`, `CEngine`...)

Feel free to open an issue if you have any problems or suggestions.

# Usage
0) Compile the program using CMake -- tested on Windows 11 (MSVC) & with file from RockSim 10.3.1.4f
1) Go to the file in `<install_dir>\\RockSim 10\\RockSim_Xml_Doc.txt`
2) (optional) Copy the file to `<your_dir>\\rkt_format\\build\\bin\\`
3) Run the program from command line (CMD or PowerShell):  
	`.\parse.exe <path_to_input_file> <path_to_output_file>`  
if you followed the 2nd step:
	`.\parse.exe RockSim_Xml_Doc.txt rocksim.h`
4) (optional) fix the naming problems with '-' characters in some names

# License
MIT ([license file](LICENSE.MD)). 
