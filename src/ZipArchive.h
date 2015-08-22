#ifndef ZIPARCHIVE_H
#define ZIPARCHIVE_H

#include "dojostring.h"
#include <string>
#include <vector>
#include <zzip/zzip.h>

namespace Dojo {

	class ZipArchive;
	class ZipFile;

	class ZipFile {
	public:

		friend class ZipArchive;

		explicit ZipFile(ZZIP_FILE* file);
		~ZipFile();
		//close file
		void close();
		//read from file
		size_t read(void* ptr, size_t size, size_t count);
		//seek from file
		int seek(int64_t offset, int origin);
		//tell from file
		int64_t tell();
		//get file size
		int64_t size();
		//return a uchar cast in int
		int getc();
		//rewind from file
		void rewind();
		//write to file
		size_t write(const void* ptr, size_t size, size_t count);

	private:

		ZZIP_FILE* file;
	};

	class ZipArchive {
	public:

		ZipArchive();
		explicit ZipArchive(const utf::string& path);
		~ZipArchive();
		//open zip file
		bool open(const utf::string& path);
		//close zip file
		void close();
		//open file
		Unique<ZipFile> openFile(const utf::string& path, const utf::string& mode);
		//paths and files in zip
		void getList(const utf::string& path, std::vector<utf::string>& out);
		void getListFiles(const utf::string& path, std::vector<utf::string>& out);
		void getListSubDirectories(const utf::string& path, std::vector<utf::string>& out);

		void getListAll(const utf::string& path, std::vector<utf::string>& out);
		void getListAllFiles(const utf::string& path, std::vector<utf::string>& out);
		void getListAllSubDirectories(const utf::string& path, std::vector<utf::string>& out);

	private:

		utf::string makeValidPath(const utf::string& path);
		ZZIP_DIR* zip_file;

	};


};


#endif
