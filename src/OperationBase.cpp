//
//  OperationBase.cpp
//  mac
//
//  Copyright © 2020 Ravbug. All rights reserved.
//

#include <stdio.h>
#include <regex>
#include "OperationBase.h"
#include "MinecraftTranslate.h"

using namespace std;

unordered_map<string,string> ParseArgs(int argc, char **argv){
	unordered_map<string, string> opts;
	{
		//combine
		string args;
		for(int i = 1; i < argc; i++){
			args += string(argv[i]) + " ";
		}
		//split
		auto split = SplitterBase::string_split(args, "--");
		for(const auto& arg : split){
			auto firstspace = arg.find(" ");
			string argname(arg.substr(0,firstspace)), value;
			if (firstspace == std::string::npos){
				value = "";
			}
			else{
				value = arg.substr(firstspace,arg.size());
			}
			opts[trim(argname)] = trim(value);
		}
	}
	return opts;
}

void SplitterBase::write_table(valuetableptr tableptr){
	
	for(auto& row : *tableptr){
		//replace newlines inside record with cr
        //replace tabs with four spaces
		for (auto& str : row){
            //replace_all(str, "\n", "\r");
            //replace_all(str, "\t", "    ");
		}
		
		//write each line surrounded in quotes
		output << '"' << row[0] << "\"\t\"" << row[1] << "\"\t\"" << row[2] << '"' << "\r\n";
	}
	output.close();
}

string SplitterBase::strip_symbols(const string& str){
	string temp = str;
	for(const auto& symbol : symbols){
		//replace the symbol with the blanking string
		replace_all(temp, symbol, " ");
	}
	return temp;
}

void SplitterBase::replace_all(std::string& orig, const std::string& to_replace, const std::string& replace_with){
	// Get the first occurrence
	auto pos = orig.find(to_replace);
	
	while( pos != std::string::npos)
	{
		// Replace current occurrence
		orig.replace(pos, to_replace.size(), replace_with);
		
		// Get the next occurrence from the current position
		pos = orig.find(to_replace, pos + replace_with.size());
	}
}

vector<string> SplitterBase::string_split(const std::string &str, const std::string &pattern){
	regex re(pattern);
	sregex_token_iterator begin(str.begin(), str.end(), re, -1), end;
	vector<string> parts;
	copy(begin, end, std::back_inserter(parts));
	return parts;
}

void JoinerBase::insert_symbols(valuetableptr table){
	for(auto& row : *table){
		//re-add the formatting symbols
        auto insertSymbol = [&](auto&& search, auto&& symbol){
            // Get the first occurrence
            auto pos = row[1].find(search);
            while( pos != std::string::npos)
            {
                // insert the symbol into the third cell
                auto insertion = range_remap(pos, 0, row[1].size(), 0, row[2].size());
                
                //find the next space after this spot
                for( ; insertion < row[2].size(); ++insertion){
                    if (isspace(row[2][insertion])){
                        break;
                    }
                }
                
                row[2] = row[2].substr(0,min(insertion,row[2].size())) + " " + symbol + " " +  row[2].substr(min(insertion,row[2].size()),row[2].size());
                
                // Get the next occurrence from the current position
                pos = row[1].find(symbol, pos + search.size());
            }
        };
        
		for(const auto& symbol : symbols){
            if (symbol != "\n"){
                insertSymbol(symbol,symbol);
            }
		}
        insertSymbol(NEWLINE_MARKER,"\n");
	}
}

valuetableptr JoinerBase::import_csv(){
	valuetableptr tableptr = make_shared<valuetable>();
	
	regex re("\t");
	
	vector<string> lines;
	{
		//read in whole file at once (necessary for line splitting)
		string data((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
		
		lines = SplitterBase::string_split(data, "\n");
	}
	
	for(const auto& line : lines){
		vector<string> data = SplitterBase::string_split(line, "\t");
		
		for(auto i = data.size(); i < 3; i++){
			data.push_back("");
		}
		
		csvrow row;
		for (int i = 0; i < row.size(); i++){
			//does the string start with quotations and is not empty?
			if (data[i].size() > 1 && data[i][0] == '"'){
				row[i] = data[i].substr(1,data[i].size()-2);
			}
			else{
				row[i] = data[i];
			}
		}
		tableptr->push_back(row);
	}
	return tableptr;
}
