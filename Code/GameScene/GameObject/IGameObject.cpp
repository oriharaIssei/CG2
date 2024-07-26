#include "IGameObject.h"

#include <imgui.h>

#include "System.h"

std::list<std::pair<std::string,std::string>> IGameObject::textureList_;

void IGameObject::Init(const std::string &directryPath,const std::string &objectName){
	name_ = objectName;
	materialManager_ = System::getInstance()->getMaterialManager();
}