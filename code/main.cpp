/* 
	Possible values:
		0 -- standard behavior;  
		1 -- assume we have input = "rkt_format/rocksim.txt", output = "rkt_format/rocksim.h" 
*/
#define USE_HARDCODED_VALUES 0

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <format>
#include <filesystem>

// $ELEMENT, can span across multiple lines
struct Field_t {
	std::string name;
	std::string type;
	std::string desc; /* if next line is not a "$", it is the continuation of description */
};

// $CLASS, always a single line entry
struct Structure_t {
	std::string name;
	std::string desc;
	std::vector<Field_t> fields;
};

// Global program context
struct Context_t {
	std::vector<std::string> lines; /* lines without leading whitespace. lines beginning with '*' are not here */
	size_t current_line_idx;
	std::vector<Structure_t> structures;
	std::string input_file_name;
	std::string output_file_name;
} static Context;


[[noreturn]] static void fail(std::string reason); /* print error & exit */
static void print_usage();
static void load_input_file(); 
static void parse_input_data();
static void parse_structure(Structure_t& structure); /* used by parse_input_data */
[[nodiscard]] static bool parse_field(Field_t& field); /* used by parse_input_data, returns true if next line is also a Field_t */
static void write_output();
static void ate(std::string& str); /* trims every character from the beginning that passes std::isspace test */


int main(int argc, char** argv) {
#if USE_HARDCODED_VALUES == 1
	(void) argc; (void) argv;
	Context.input_file_name  = std::string{"..\\..\\rocksim.txt"};
	Context.output_file_name = std::string{"..\\..\\rocksim.h"};
#else
	if (argc != 3) {
		print_usage();
		return EXIT_SUCCESS;
	} else {
		Context.input_file_name  = std::string{argv[1]};
		Context.output_file_name = std::string{argv[2]};
	}
#endif

	load_input_file();	
	parse_input_data();
	write_output();

	return EXIT_SUCCESS;
}

[[noreturn]] static void fail(std::string reason) {
	std::cerr << "ERROR: " << reason << std::endl;
	exit(EXIT_FAILURE);
}

static void print_usage() {
	std::cout << "RockSim parser v1 -- utility for parsing documentation ";
	std::cout << "of an RKT format. The result is a C-style definition of structures.\n Usage: " << std::endl;
	std::cout << "\t.\\parse.exe <input file> <output file>" << std::endl;
	std::cout << " <output file> can be left as '-'. Then output will be written to stdout." << std::endl;
	std::cout << " Example: " << std::endl;
	std::cout << "\t.\\parse.exe rocksim.txt result.h" << std::endl;
	std::cout << "\n\n";
	std::cout << "NOTE: the output is not a *real* code -- for some reason, a few names include '-' which is not valid for a name in C." << std::endl;
}

static void load_input_file() {
	if (!std::filesystem::exists(Context.input_file_name)) {
		fail("input file does not exist");
	}

	std::ifstream file(Context.input_file_name);
	if (!file.is_open()) {
		fail("failed to open the input file");
	}

	std::stringstream buffer;
	buffer << file.rdbuf();

	std::string line;
	while(std::getline(buffer, line)) {
		ate(line);
		if (line.empty() || line.front() == '*') continue;
		Context.lines.push_back(line);
	}
}

static void parse_structure(Structure_t& structure) {
	std::string& current_line = Context.lines[Context.current_line_idx];
	// Assume we start with "$Element:". Copy the content via iterators.
	std::string const class_name{current_line.begin() + ::strlen("$CLASS:"), 
								 current_line.begin() +  current_line.find_first_of(',')};
	// Seek for "$" from "$DESC:", then copy everything until the end of the buffer
	std::string const desc{current_line.begin() + current_line.find_first_of("$", 1) + ::strlen("$DESC:"), 
						   current_line.end()};

	structure.name = class_name;
	structure.desc = desc;
	Context.current_line_idx++;
}

[[nodiscard]] static bool parse_field(Field_t& field) {
	std::string& current_line = Context.lines[Context.current_line_idx];
	std::string const field_name{current_line.begin() + ::strlen("$ELEMENT:"), 
								 current_line.begin() +  current_line.find_first_of(',')};
	std::string const data_type{current_line.begin() + current_line.find_first_of("$", 1) + ::strlen("$DATATYPE:"),
								current_line.begin() + current_line.find_first_of(",", current_line.find_first_of(",") + 1)};
	std::string desc{current_line.begin() + current_line.find_first_of("$", current_line.find_first_of("$", 1) + 1) + ::strlen("$DESC:"),
					 current_line.end()};

	field.name = field_name;
	field.type = data_type;

	Context.current_line_idx++;
	while (Context.current_line_idx < Context.lines.size() && 
		   !Context.lines[Context.current_line_idx].empty() &&
		   Context.lines[Context.current_line_idx].front() != '$') {

		desc = desc + " | " + Context.lines[Context.current_line_idx];
		Context.current_line_idx++;
	}

	field.desc = desc;
	return Context.current_line_idx < Context.lines.size() && 
		   Context.lines[Context.current_line_idx].at(1) == 'E'; /* starts with $ELEMENT */
}

static void parse_input_data() {
	do {
		Structure_t& s = Context.structures.emplace_back();
		parse_structure(s);
		while (parse_field(s.fields.emplace_back()));
	} while(Context.current_line_idx < Context.lines.size());
}

static void write_output() {
	std::string buffer;
	for (auto& s : Context.structures) {
		buffer += "\n/*\n\t" + s.desc + "\n*/\n";
		buffer += "struct " + s.name + " {\n";
		for (auto f : s.fields) {
			buffer += '\t' + f.type + "\t" + f.name + ";\t\t//<! " + f.desc + "\n";
		}
		buffer += "};\n\n";
	}

	if (Context.output_file_name == "-") {
		std::cout << buffer << std::endl;
	} else {
		std::ofstream file(Context.output_file_name);
		if (!file.is_open()) {
			fail("unable to open/create the file for writing");
		}
		if (!file.write(buffer.data(), buffer.size())) {
			fail("failed to write to the file");
		}
	}
}

static void ate(std::string& str) {
	size_t i = 0;
	while(i < str.size() && std::isspace((int)str[i]) != 0) {
		i++;
	}

	str = str.substr(i);
}