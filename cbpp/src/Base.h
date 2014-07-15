
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



#endif