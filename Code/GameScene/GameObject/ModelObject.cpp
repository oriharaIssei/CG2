#include "ModelObject.h"

void ModelObject::Init(const std::string &objectName){
	name_ = objectName;
}

void ModelObject::Updata(){
#ifdef _DEBUG
	IGameObject::Updata();



#endif // _DEBUG
}

void ModelObject::Draw(const ViewProjection &viewProj){
}
