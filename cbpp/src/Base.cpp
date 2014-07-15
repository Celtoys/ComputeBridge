
#include "Base.h"


File::File()
	: fp(0)
{
}


File::~File()
{
	if (fp != 0)
		fclose(fp);
}


bool Open(File& file, const char* filename, const char* mode)
{
	// Close any old file before opening a new one
	if (file.fp)
		fclose(file.fp);
	file.fp = fopen(filename, mode);
	return file.fp != 0;
}


size_t Size(const File& file)
{
	if (file.fp == 0)
		return 0;

	// Seek to EOF and get position
	if (fseek(file.fp, 0, SEEK_END) != 0)
		return 0;
	size_t size = ftell(file.fp);
	if (fseek(file.fp, 0, SEEK_SET) != 0)
		return 0;

	return size;
}


bool Read(const File& file, void* dest, size_t size)
{
	if (file.fp == 0)
		return false;
	return fread(dest, 1, size, file.fp) == size;
}


bool LoadFileData(const char* filename, std::vector<char>& file_data)
{
	File file;
	if (!Open(file, filename, "rb"))
		return false;

	file_data.resize(Size(file));
	return Read(file, file_data.data(), file_data.size());
}