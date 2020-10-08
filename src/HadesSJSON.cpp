//
//  main.cpp
//  HadesCSV
//
//  Copyright © 2020 Ravbug. All rights reserved.
//

#include <iostream>
#include <sstream>
#include "OperationBase.h"
#include <unordered_map>
#include <list>
#include <regex>

using namespace std;

typedef unordered_map<string, string> strdict;

class HadesSJSONbase{
protected:
	/**
	 Split a string by a regex pattern
	 @param in_pattern the pattern to use to split with
	 @param content the string to split
	 @return a vector containing the split strings
	 */
	vector<string> split(const string& in_pattern, const string& content){
		vector<string> split_content;
		
		regex pattern(in_pattern);
		copy( sregex_token_iterator(content.begin(), content.end(), pattern, -1),
			 sregex_token_iterator(),back_inserter(split_content));
		return split_content;
	}
	
	
	/**
	 Parses a SJSON entry into a dictionary
	 @param str the string to parse
	 @return the string dictionary parse result
	 */
	strdict parse_sjsonstr(const string& str){
		strdict parsed;
		
		//split string by lines
		auto lines = split("¬", str);
		for (const auto& line : lines){
			if(line.size() < 3) continue;
			//split each line by '='
			auto keyval = split("=",line);
			
			if (keyval.size() != 2){
				continue;
			}
			
			auto key = trim(keyval[0]);
			auto val = trim(keyval[1]);
			
			
			SplitterBase::replace_all(key, "\"", "");
			SplitterBase::replace_all(val, "\"", "");
			
			parsed[key] = val;
		}
		return parsed;
	}
	
	/**
	 Parse a Hades file
	 @param instr the input stream to the file
	 @param symbols the symbol table to populate
	 @return a dictionary of key-value mappings
	 */
	list<strdict> parse_hadesfile(std::istream& instr, unordered_set<string>& symbols){
		list<strdict> texts;
		//read in entire file
		stringstream buffer;
		buffer << instr.rdbuf();
		auto str = buffer.str();
		//replace all newlines in the file with a marker character: ¬
		SplitterBase::replace_all(str, "\n", " ¬ ");
		
		//use this as the stream
		istringstream data(str);
		
		//parse the SJSON to the extent that it is required
		{
			//read until [ to get correct starting location
			std::string next;
			while (data >> next && next != "["){}
			
			//repeat until ]
			while (data >> next && next != "]"){
				//read until {
				while (data >> next && next != "{"){}
				//assemble string until }
				string temp;
				while (data >> next && next != "}"){
					temp += " " + next;
				}
				
				//now the string has been assembled, convert it to keys and values
				texts.push_back(parse_sjsonstr(temp));
			}
		}
		
		//find all the special formatting symbols, which are encased in {}
		for (const auto& entry : texts){
			for (const auto& e : entry){
				istringstream str(e.second);
				
				//read to the first {
				char c;
				while (str.get(c) && c != '{');
				int balance = 1;
				string symbol = "{";
				while(str.get(c)){
					if (c == '{'){
						++balance;
					}
					if (c == '}'){
						--balance;
					}
					symbol += c;
					if (balance == 0){
						symbol = trim(symbol);
						if (symbol.size() > 2 && symbol.find('{') != string::npos && symbol.find('}') != string::npos){
							symbols.insert(symbol);
						}
						symbol = "";
					}
				}
			}
		}
		
		//add additional symbols
		symbols.insert("\\[");
		symbols.insert("\\]");
		symbols.insert("\\n");
		symbols.insert("%s");
		
		return texts;
	}
};

class HadesSJSONsplitter : public SplitterBase, public HadesSJSONbase{
	
	
	valuetableptr split_file(std::ifstream& instr) override{
		auto tableptr = std::make_shared<valuetable>();
		
		auto texts = parse_hadesfile(instr, symbols);
		
		//do not translate data that is meant for the game
		unordered_set<string> do_not_include = {"OverwriteLocalization","InheritFrom"};
		
		//convert into tablepointer
		for (const auto& entry : texts){
			for (const auto& e : entry){
				if (e.first == "Id"){
					continue;
				}
				
				//do not include
				if (do_not_include.find(e.first) != do_not_include.end()){
					continue;
				}
				
				string id = entry.at("Id") + "." + e.first;
				tableptr->push_back({id,e.second, strip_symbols(e.second)});
			}
		}
		
		return tableptr;
	}
};

class HadesSJSONjoiner : public JoinerBase, public HadesSJSONbase{
	
	virtual void write_combined(valuetableptr table){
	
		//parse the original file
		auto inputstream = ifstream(orig_path);
		auto texts = parse_hadesfile(inputstream, symbols);
		
		//add the symbols back
		insert_symbols(table);
		
		//merge the valuetable with the texts
		for (const auto& row : *table){
			//deconstruct name
			auto parts = split("[.]+", row[0]);
			
			//overwrite the correct part of the table
			bool stop = false;
			for (auto& r : texts){
				for (auto& p : r){
					//replace
					if (p.second == parts[0]){
						r[parts[1]] = row[2];
						stop = true;
						break;
					}
				}
				if (stop){
					break;
				}
			}
		}
		
		list<string> strs;
		//convert each item into a SJSON string
		for (const auto& r : texts){
			string temp = "\t{\n";
			for (const auto& pair : r){
				temp += "\t\t" + pair.first + " = \"" + pair.second + "\"\n";
			}
			strs.push_back(temp + "\t}\n");
		}
		
		//write to output
		
		//first read in file and find location of "["
		inputstream = ifstream(orig_path);
		
		ostringstream outputbuffer;
		
		//.get(v) reads whitespace
		char t;
		while (inputstream.get(t) && t != '['){
			outputbuffer << t;
		}
		//once [ is found, write the replacement
		outputbuffer << "[\n";
		for (const auto& str : strs){
			outputbuffer << str;
		}
		
//		//advance the input to "]"
//		while (inputstream.get(t) && t != ']'){}
		
		//add the end sbracket
		outputbuffer << "\n]\n}";
		
//		//write the remainder of the file from the original
//		while (inputstream.get(t)){
//			outputbuffer << t;
//		}
	
		
		//multipass replace double spaces in output
		auto str = outputbuffer.str();
		for (int i = 0; i < 10; ++i){
			SplitterBase::replace_all(str, "  ", " ");
		}
		
		//fix OverwriteLocalization = "true" --> OverwriteLocalization = true
		SplitterBase::replace_all(str, "OverwriteLocalization = \"true\"", "OverwriteLocalization = true");
		
		output << str;
	}
public:
	string orig_path;
	/**
	 Set the path to the original Hades file. This is required because the program will inline the translated body into the original file's data, as well as build the symbol table
	 @param path the path to the file
	 */
	void set_orig_file(const std::string& path){
		orig_path = path;
	}
};

/**
 Check the arguments
 @param args the argument dictionary
 */
bool check_args(const unordered_map<string, string>& args, bool splitting){
	if (splitting){
		return (args.find("i") != args.end() && args.find("o") != args.end());
	}
	else{
		return (args.find("i") != args.end() && args.find("o") != args.end()) && args.find("t") != args.end();
	}
}

void print_usage(char* program){
	cout << "	Usage:  " << string(program) << " --s --i path/to/original.sjson --o table.tsv" << endl
	<< "		" << string(program) << " --j --i path/to/table.tsv --t path/to/original.sjson --o translate.json" << endl;
}

int main(int argc, char** argv){
	
	auto opts = ParseArgs(argc, argv);
	
	//validate args
	if (opts.find("s") == opts.end() && opts.find("j") == opts.end()){
		cout << "	Must have flag: --s (split JSON), or --j (join TSV)" << endl;
		return 2;
	}
	
	if (opts.find("s") != opts.end() && opts.find("j") == opts.end()){
		//run splitter
		if (check_args(opts,true)){
			HadesSJSONsplitter hss;
			hss.SetIO(opts["i"],opts["o"]);
			hss.Run();
		}
		else{
			print_usage(argv[0]);
			return 1;
		}
	}
	else if (opts.find("s") == opts.end() && opts.find("j") != opts.end()){
		if (check_args(opts,false)){
			HadesSJSONjoiner hsj;
			hsj.SetIO(opts["i"],opts["o"]);
			hsj.set_orig_file(opts["t"]);
			hsj.Run();
		}
		else{
			print_usage(argv[0]);
			return 1;
		}
	}
	else{
		print_usage(argv[0]);
	}
	
	return 0;
}
