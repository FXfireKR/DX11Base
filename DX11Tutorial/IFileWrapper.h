#pragma once

class IFileStreamWrapper
{
public:
	static bool ReadAllStream(IN const string& strPath, OUT stringstream& buffer
		, ios_base::openmode mode = std::ios::binary) 
	{
		buffer.clear();
		ifstream file(strPath.c_str(), mode);
		if (true == file.is_open()) {
			buffer << file.rdbuf();
			file.close();
			return true;
		}
		return false;
	}
};