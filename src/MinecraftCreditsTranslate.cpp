#include "OperationBase.h"
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <iostream>
#include <filesystem>
#include <deque>

using namespace std;
using namespace rapidjson;

void print_usage(char* program){
    cout << "    Usage:  " << program << " --s --i path/to/credits.json --o table.txt" << endl
         << "        " << program << " --j --i path/to/table.txt --v path/to/credits.json --o newcredits.json" << endl;
}

bool check_args(const unordered_map<string, string>& args, bool join = false){
    return (args.find("i") != args.end() && args.find("o") != args.end());
}

void SplitCredits(const filesystem::path& input, const filesystem::path& output){
    ofstream out(output);
    ifstream in(input);
    Document doc;
    {
        //read in the whole file at once (bad idea)
        string data((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
        
        doc.Parse(data.c_str());
    }
    for(const auto& m : doc.GetArray()){
        auto entry = m.GetObject();
        out << entry["section"].GetString() << "\n";
        for(const auto& section : m["titles"].GetArray()){
            out << section.GetObject()["title"].GetString() << "\n";
        }
    }
}

void JoinCredits(const filesystem::path& input, const filesystem::path& original, const filesystem::path& output){
    ifstream in(input);
    ifstream orig(original);
    
    Document doc;
    {
        //read in the whole file at once (bad idea)
        string data((std::istreambuf_iterator<char>(orig)), std::istreambuf_iterator<char>());
        
        doc.Parse(data.c_str());
    }
    std::deque<string> strs;
    for(auto& m : doc.GetArray()){
        auto entry = m.GetObject();
        strs.emplace_back();
        getline(in, strs.back());
        entry["section"].SetString(strs.back().c_str(), strs.back().size());
        for(auto& section : m["titles"].GetArray()){
            strs.emplace_back();
            getline(in, strs.back());
            section.GetObject()["title"].SetString(strs.back().c_str(), strs.back().size());
        }
    }
    ofstream out(output);
    
    //stringify and write
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    doc.Accept(writer);
    out << buffer.GetString();

}

int main(int argc, char** argv){
    auto opts = ParseArgs(argc, argv);
    
    if (opts.find("s") == opts.end() && opts.find("j") == opts.end()){
        cout << "   Must have --s (split) or --j (join)" << endl;
        return 1;
    }
    
    if (opts.find("s") != opts.end()){
        if (check_args(opts)){
            SplitCredits(opts.at("i"),opts.at("o"));
        }
        else{
            print_usage(argv[0]);
        }
    }
    else if (opts.find("j") != opts.end()){
        if (check_args(opts) && opts.find("v") != opts.end()){
            JoinCredits(opts.at("i"),opts.at("v"),opts.at("o"));
        }
        else{
            print_usage(argv[0]);
        }
    }
}
