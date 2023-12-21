#include "optimizer.h"

bool changed = true;

void Optimizer(NodeList *funcdecls) {
     while(ConstantFolding(funcdecls) || ConstProp(funcdecls) || DeadAssign(funcdecls)) {
          ;
     }
}
