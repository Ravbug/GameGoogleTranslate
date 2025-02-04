//
//  MinecraftTranslate.cpp
//  mac
//
//  Copyright © 2020 Ravbug. All rights reserved.
//

#include "MinecraftTranslate.h"
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

using namespace std;
using namespace rapidjson;

valuetableptr MinecraftSplitter::split_file(ifstream& input){
	auto tableptr = std::make_shared<valuetable>();
	
	//parse JSON
	Document doc;
	{
		//read in the whole file at once (bad idea)
		string data((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
		
		doc.Parse(data.c_str());
	}
	
	//table header
	tableptr->push_back({"Key","Value","NoFormatting"});
	
	//write into tableptr
	for(const auto& m : doc.GetObject()){
		csvrow row;
		row[0] = m.name.GetString();
		row[1] = m.value.GetString();
		row[2] = strip_symbols(row[1]);
        
        // get rid of newlines because they break parsers
        replace_all(row[1],"\n",NEWLINE_MARKER);
        replace_all(row[1],"\n",NEWLINE_MARKER);
        replace_all(row[1],"\n",NEWLINE_MARKER);
		
		tableptr->push_back(row);
	}
	
	return tableptr;
}


void MinecraftJoiner::write_combined(valuetableptr table){
	Document d;
	d.Parse("{}");
	
	//re-add formatting
	insert_symbols(table);
	
	//convert to JSON datastructure
	for(auto& row : *table){
		Value key(row[0].c_str(),d.GetAllocator());
		Value value(row[2].c_str(),d.GetAllocator());
		d.AddMember(key,value,d.GetAllocator());
	}
	
	//stringify and write
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	d.Accept(writer);
	output << buffer.GetString();
}

//launching the program

using namespace std;
#include <unordered_map>

/**
 Check the arguments
 @param args the argument dictionary
 */
bool check_args(const unordered_map<string, string>& args){
	return (args.find("i") != args.end() && args.find("o") != args.end());
}

void print_usage(char* program){
	cout << "	Usage:  " << string(program) << " --s --i path/to/en_us.json --o table.tsv" << endl
		 << "		" << string(program) << " --j --i path/to/table.tsv --o translate.json" << endl;
}

/*
 Usage: --s --i input.json --o output.csv
 Usage: --j --i input.csv --o lang.json
 */
int main(int argc, char** argv){
	
	auto opts = ParseArgs(argc, argv);
	
	//validate args
	if (opts.find("s") == opts.end() && opts.find("j") == opts.end()){
		cout << "	Must have flag: --s (split JSON), or --j (join TSV)" << endl;
		return 2;
	}
	
	if (opts.find("s") != opts.end() && opts.find("j") == opts.end()){
		//run splitter
		if (check_args(opts)){
			MinecraftSplitter ms;
			ms.SetIO(opts["i"],opts["o"]);
			ms.Run();
		}
		else{
			print_usage(argv[0]);
			return 1;
		}
	}
	
	if (opts.find("s") == opts.end() && opts.find("j") != opts.end()){
		if (check_args(opts)){
			MinecraftJoiner mj;
			mj.SetIO(opts["i"],opts["o"]);
			mj.Run();
		}
		else{
			print_usage(argv[0]);
			return 1;
		}
	}
	
	return 0;
}
