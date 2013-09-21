#include "type.h"

bool NodeType::isSizedDerivedType() const {
  if(this->isInteger())
    return true;

}
/*
NodeType* NodeType::getPrimitiveType(TypeID tid){
  switch(tid){
    case VoidID   :return getVoid();
  }
}

NodeType* NodeType::getVoid(){
    return NULL;
}
*/


