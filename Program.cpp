#include <iostream>
#include <fstream>
#include <cstdint>
#include <sstream>
#include <string>
#include <vector>


struct Vector2 { float x, y; };
struct Vector3 { float x, y, z; };

struct Vertex
{
	Vector3 Position;
	Vector3 Normal;
	Vector2 UV;
};

std::vector<Vertex>  g_Vertices;
std::vector<Vector3> g_Normals;
std::vector<Vector2> g_UVs;
std::vector<std::uint32_t> g_VertexIndices;
std::vector<std::uint32_t> g_NormalIndices;
std::vector<std::uint32_t> g_UVIndices;

bool readObj(const std::string& path)
{
	std::ifstream in(path, std::ios::in);
	if (!in) {
		std::cerr << "\nNão foi possível abrir o arquivo: " << path << std::endl;
		return false;
	}

	std::string line;
	while (getline(in, line))
	{
		if (line.substr(0, 2) == "v ")
		{
			std::istringstream s(line.substr(2));
			Vertex vertex;
			s >> vertex.Position.x;
			s >> vertex.Position.y;
			s >> vertex.Position.z;
			g_Vertices.push_back(vertex);
		}
		else if (line.substr(0, 3) == "vn ")
		{
			std::istringstream s(line.substr(2));
			Vector3 normal;
			s >> normal.x; s >> normal.y; s >> normal.z;
			g_Normals.push_back(normal);
		}
		else if (line.substr(0, 3) == "vt ")
		{
			std::istringstream s(line.substr(2));
			Vector2 uv;
			s >> uv.x; s >> uv.y;
			g_UVs.push_back(uv);
		}
		else if (line.substr(0, 2) == "f ")
		{
			std::istringstream s(line.substr(2));
			std::uint32_t vIndex1, vIndex2, vIndex3;
			std::uint32_t nIndex1, nIndex2, nIndex3;
			std::uint32_t tIndex1, tIndex2, tIndex3;
			char temp;

			s >> vIndex1; s >> temp; s >> tIndex1; s >> temp; s >> nIndex1;
			s >> vIndex2; s >> temp; s >> tIndex2; s >> temp; s >> nIndex2;
			s >> vIndex3; s >> temp; s >> tIndex3; s >> temp; s >> nIndex3;

			g_VertexIndices.push_back(--vIndex1);
			g_VertexIndices.push_back(--vIndex2);
			g_VertexIndices.push_back(--vIndex3);
			g_NormalIndices.push_back(--nIndex1);
			g_NormalIndices.push_back(--nIndex2);
			g_NormalIndices.push_back(--nIndex3);
			g_UVIndices.push_back(--tIndex1);
			g_UVIndices.push_back(--tIndex2);
			g_UVIndices.push_back(--tIndex3);
		}
	}

	for (int i = 0; i < g_VertexIndices.size(); i++)
	{
		std::uint32_t vIndex = g_VertexIndices[i];
		std::uint32_t nIndex = g_NormalIndices[i];
		std::uint32_t tIndex = g_UVIndices[i];
		g_Vertices[vIndex].Normal = g_Normals[nIndex];
		g_Vertices[vIndex].UV     = g_UVs[tIndex];
	}

	in.close();

	if (g_Vertices.size() > 0 && g_VertexIndices.size() > 0)
	{
		std::cout << "\nFile loaded successfully!" << std::endl;
	}

	return true;
}

bool writeAsset(const std::string& path)
{
	std::ofstream out(path, std::ios::out);
	if (!out) {
		std::cerr << "\nNão foi possível abrir o arquivo: " << path << std::endl;
		return false;
	}

	std::uint32_t vertexCount = g_Vertices.size();
	std::uint32_t triangleCount = g_VertexIndices.size() / 3;

	out << "VertexCount: " << vertexCount << std::endl;
	out << "TriangleCount: " << triangleCount << std::endl;
	out << "VertexList (pos, normal, uv)\n{" << std::endl;

	for (std::uint32_t i = 0; i < vertexCount; i++)
	{
		Vector3& Pos    = g_Vertices[i].Position;
		Vector3& Normal = g_Vertices[i].Normal;
		Vector2& UV     = g_Vertices[i].UV;

		out.precision(7);
		out << "\t" << Pos.x << " " << Pos.y << " " << (Pos.z * -1.0f) << " ";
		out << Normal.x << " " << Normal.y << " " << (Normal.z * -1.0f) << " ";
		out << UV.x << " " << (1.0f - UV.y) << std::endl;
	}

	out << "}" << std::endl;
	out << "TriangleList\n{" << std::endl;

	std::uint32_t index = 0;
	for (int i = 0; i < triangleCount; i++)
	{
		out << "\t" << g_VertexIndices[index + 2] << " "
			        << g_VertexIndices[index + 1] << " "
			        << g_VertexIndices[index + 0] << std::endl;
		index += 3;
	}

	out << "}";

	out.close();

	return true;
}

int main(int argc, char** argv)
{
	int temp;

	if (argc <= 1)
	{
		std::cout << "Missing the .obj file path! Usage:\n\n"
			      << "ObjToAsset.exe \"path to the .obj file\" \"optional output path\"" << std::endl;
		std::cin >> temp;
		return -1;
	}

	if (!readObj(argv[1]))
	{
		std::cin >> temp;
		return -1;
	}

	std::string outputPath = (argv[2] != nullptr) ? argv[2] : "output.asset";
	if (!writeAsset(outputPath))
	{
		std::cin >> temp;
		return -1;
	}

	return 0;
}