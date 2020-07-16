//
//  SplitterBase.h
//  mac
//
//  Copyright © 2020 Ravbug. All rights reserved.
//

#pragma once
#include <fstream>
#include <iostream>
#include <array>
#include <vector>
#include <unordered_map>
#include <regex>
#include <unordered_set>

typedef std::array<std::string, 3> csvrow;
typedef std::vector<csvrow> valuetable;
typedef std::shared_ptr<valuetable> valuetableptr;

class IOBase{
protected:
	std::ofstream output;						//the input file stream
	std::ifstream input;						//the output file stream
	std::unordered_set<std::string> symbols;			//stores symbols for formatting
public:
	/**
	 Set the input and output paths for this class
	 @param inpath the file to read from
	 @param outpath the path to write the CSV to
	 */
	virtual void SetIO(const std::string& inpath, const std::string& outpath){
		input = std::ifstream(inpath);
		output = std::ofstream(outpath);
	}
};

class SplitterBase : public IOBase{
protected:
	/**
	 Split an input stream into a value table
	 @param inputstream the stream to the file
	 @return a shared pointer to a valuetable
	 */
	virtual valuetableptr split_file(std::ifstream& inputstream) = 0;
	
	/**
	 Write a table to a CSV file
	 @param tableptr the shared pointer to the table in memory
	 */
	void write_table(valuetableptr tableptr);
	
	/**
	 Remove all of the formatting symols defined in symbols
	 @param str the string to clean
	 @return a new string containing str without the formatting symbols
	 */
	std::string strip_symbols(const std::string& str);
	
public:
	/**
	 Replace all occurances of one string inside another string. Modifies the original.
	 @param orig the string to replace
	 @param to_replace the string to be replaced
	 @param replace_with the string to replace with
	 */
	static void replace_all(std::string& orig, const std::string& to_replace, const std::string& replace_with);
	
	
	/**
	 Split a string using regular expressions
	 @param str the string to split
	 @param pattern the regex pattern to split the string with
	 */
	static std::vector<std::string> string_split(const std::string& str, const std::string& pattern);
	
	/**
	 Execute this SplitterBase. Splits the input file and outputs the CSV.
	 */
	void Run(){
		//split file into value table
		auto table = split_file(input);
		//write value table as CSV
		write_table(table);
	}
};

class JoinerBase : public IOBase{
protected:
	/**
	 Imports the CSV file as a value table
	 @return a valuetable shared pointer containing the parsed CSV
	 */
	valuetableptr import_csv();
	
	/**
	 Re-insert the formatting symbols into each row of the table
	 @param table the table to insert into
	 */
	void insert_symbols(valuetableptr table);
	
	/**
	 Write the combined file to disk
	 */
	virtual void write_combined(valuetableptr table) = 0;

public:
	
	/**
	 Execute this JoinerBase
	 */
	void Run(){
		write_combined(import_csv());
	}
};

/**
 Templated value remapper. Remap a value from one range to another.
 @param value the value to remap
 @param low1 the low bound of the orignal range
 @param high1 the high bound of the original range
 @param low2 the low bound of the new range
 @param high2 the high bound of the new range
 @return value remapped to the scale [low2,high2]
 */
template<typename T, typename U, typename V, typename W, typename X>
inline T range_remap(T value, U low1, V high1, W low2, X high2){
	return low2 + (value - low1) * (high2 - low2) / (high1 - low1);
}

/**
 Trim whitespace from a string
 @param str the string to trim
 @return the trimmed string
 */
inline std::string trim(const std::string& str)
{
	size_t first = str.find_first_not_of(' ');
	if (std::string::npos == first)
	{
		return str;
	}
	size_t last = str.find_last_not_of(' ');
	return str.substr(first, (last - first + 1));
}


std::unordered_map<std::string,std::string> ParseArgs(int argc, char** argv);
