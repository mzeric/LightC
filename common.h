#ifndef _LLVM_COMMON_H
#define _LLVM_COMMON_H


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


#include <iostream>
#include <string>
#include <list>










extern llvm::LLVMContext* llvm_context;
extern struct llvm::Module *TheModule;
extern llvm::IRBuilder<> Builder;

typedef llvm::Value Fvalue;
typedef std::string Fstring;

extern "C" int yyparse(void);
extern "C" void check(const char*str,...);
void A_fatal(const char *str,int e = 1);
#define debug_visit(x) std::cout << "visit " << x << std::endl;
#define printf printf
//static std::map<std::string, Value*> NamedValues;

struct IRContext{
  llvm::LLVMContext *context;
  struct llvm::Module *TheModule;
  llvm::IRBuilder<>  *Builder;
};
extern struct IRContext ir_context;



















#endif
