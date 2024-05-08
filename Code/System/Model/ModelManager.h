#pragma once

#include <string>

class Model;
struct ModelData;
struct MaterialData;
class ModelManager {
public:
	void Create(Model *model, const std::string &directoryPath, const std::string &filename);
private:
	ModelData LoadObjFile(const std::string &directoryPath, const std::string &filename);
	MaterialData LoadMtlFile(const std::string &directoryPath, const std::string &filename);
};