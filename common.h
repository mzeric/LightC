#ifndef _LLVM_COMMON_H
#define _LLVM_COMMON_H


#if (LLVM_VERSION >= 33)
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/TypeBuilder.h>

#include <llvm/IRReader/IRReader.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Support/Allocator.h>
#include <llvm/ADT/PointerIntPair.h>
#include <llvm/ADT/PointerUnion.h>

#else
#include <llvm/DerivedTypes.h>
#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Support/IRBuilder.h>
#endif

#include <iostream>
#include <string>
#include <list>

#include "type.h"

using namespace llvm;
using namespace lc;





extern LLVMContext* llvm_context;
extern struct Module *TheModule;
extern IRBuilder<> Builder;

typedef llvm::Value Fvalue;
typedef std::string Fstring;

extern "C" int yyparse(void);
extern "C" void check(const char*str,...);
void A_fatal(const char *str,int e = 1);
#define debug_visit(x) std::cout << "visit " << x << std::endl;
#define printf printf
//static std::map<std::string, Value*> NamedValues;

struct IRContext{
  LLVMContext *context;
  struct Module *TheModule;
  IRBuilder<>  *Builder;
};
extern struct IRContext ir_context;



















#endif
