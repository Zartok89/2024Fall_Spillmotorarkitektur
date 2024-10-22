#pragma once
#include <string>
#include <vector>

#include "graphical/Mesh.h"

class ReadWriteFiles
{
public:

	ReadWriteFiles();
	void WriteToFile(std::string Filepath, std::vector<Vertex>& VertexVector);
	void AppendToFile(std::string Filepath);
	void ReadFromFile(std::string FileToRead);
	static void ReadFromFileWriteIntoNewFile(std::string FileToRead, std::string NewDataFile);
	static void FromDataToVertexVector(std::string DataFileToVertexVector, std::vector<Vertex>& VerticesVector);
	static void RemovingUnwantedChars(std::string& Line);
};
