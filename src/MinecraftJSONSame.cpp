#include <iostream>
#include <fstream>
#include <filesystem>

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

using namespace std;
using namespace rapidjson;

void DiffFiles(const filesystem::path& path_a, const filesystem::path& path_b, const filesystem::path& output_path) {

	auto parseDoc = [](auto& path) {
		Document doc;
		ifstream input(path);
		string data((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());

		doc.Parse(data.c_str());
		return doc;
	};

	auto file_a = parseDoc(path_a);
	auto file_b = parseDoc(path_b);

	Document output;
	output.Parse("{}");
	auto& output_allocator = output.GetAllocator();

	for (const auto& item : file_b.GetObject()) {
		// is key not unique?
		if (file_a.HasMember(item.name)) {
			Value key(item.name, output_allocator);
			Value value(item.value, output_allocator);
			output.AddMember(key, value, output_allocator);
		}
	}

	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	output.Accept(writer);

	ofstream outstream(output_path, std::ios::binary | std::ios::out);
	outstream.write(buffer.GetString(), buffer.GetSize());
}

int main(int argc, char** argv) {

	if (argc != 4) {
		cerr << "Usage: " << argv[0] << " A.json B.json out.json; outputs key-values not-unique to B.json to out.json" << endl;
		return 1;
	}

	DiffFiles(argv[1], argv[2], argv[3]);
	return 0;
}