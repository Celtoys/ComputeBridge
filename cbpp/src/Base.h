
#ifndef INCLUDED_BASE_H
#define INCLUDED_BASE_H


#include <vector>


//
// RAII file structure with public data members to encourage use of existing cstd functions
// that haven't been wrapped.
//
struct File
{
	File();
	~File();

	FILE* fp;
};


// Wrapped file operations
bool Open(File& file, const char* filename, const char* mode);
size_t Size(const File& file);
bool Read(const File& file, void* dest, size_t size);
bool LoadFileData(const char* filename, std::vector<char>& file_data);



//
// Searchable command-line arguments that can be shared between transforms.
// Doesn't require any form of option configuration so trades slower parsing for flexibility.
//
class Arguments
{
public:
	Arguments(int argc, const char* argv[]);

	size_t GetIndexOf(const std::string& arg, int occurrence = 0) const;
	bool Have(const std::string& arg) const;
	std::string GetProperty(const std::string& arg, int occurrence = 0) const;
	size_t Count() const;

	const std::string& operator [] (int index) const;

private:
	std::vector<std::string> m_Arguments;
};


#endif