#ifndef _TYPE_H
#define _TYPE_H

#if (LLVM_VERSION >= 33)
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Support/SourceMgr.h>
#else
#include <llvm/DerivedTypes.h>
#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Support/IRBuilder.h>

#endif

#include <iostream>
#include <assert.h>


typedef llvm::Value Fvalue;
typedef std::string Fstring;
extern llvm::LLVMContext* llvm_context;

namespace lc{
class StorType;
class QualType;
class ExtQualType;
class ExtTypeBase;
class ASTType;

class BuiltinType;

enum {
	TypeAlignmentInBits = 4,
	TypeAlignment = 1 << 4,
};
}
namespace llvm {
  template <typename T>
  class PointerLikeTypeTraits;
  template<>
  class PointerLikeTypeTraits< ::lc::ASTType*> {
  public:
    static inline void *getAsVoidPointer(::lc::ASTType *P) { return P; } 
    static inline ::lc::ASTType *getFromVoidPointer(void *P) {
      return static_cast< ::lc::ASTType*>(P);
    }    
    enum { NumLowBitsAvailable = lc::TypeAlignmentInBits};
  };
  template<>
  class PointerLikeTypeTraits< ::lc::ExtQualType*> {
  public:
    static inline void *getAsVoidPointer(::lc::ExtQualType *P) { return P; } 
    static inline ::lc::ExtQualType *getFromVoidPointer(void *P) {
      return static_cast< ::lc::ExtQualType*>(P);
    }    
    enum { NumLowBitsAvailable = lc::TypeAlignmentInBits };
  };

  template <>
  struct isPodLike<lc::QualType> { static const bool value = true; };
}
namespace lc{
class NodeType{
public:
  void set_name(const char* str){name = str;}
  Fstring get_name(void){return name;}
public:
    
    enum TypeID {
      VoidID = 0,
      FloatID,
      DoubleID,
      LableID,
      // Derived types
      //
      IntegerID,
      FunctionID,
      StructID,
      ArrayID,
      PointerID,
      VectorID,

      NumTypeID,
      LastPrimitiveID = LableID,
      FirstDerivedID = IntegerID
    };
    Fstring    name;
private:
	// Note: TypeID : low 8 bit; SubclassData : high 24 bit.
	uint32_t IDAndSub;
protected:
	explicit NodeType(TypeID tid):IDAndSub(0){
		setTypeID(tid);
	}
	void setTypeID(TypeID tid){
		IDAndSub = (tid & 0xFF) | (IDAndSub & 0xFFFFFF00);
		assert(getTypeID() == tid && "TypeID too big to fit");
	}


	void setSubclass(unsigned int val){
		IDAndSub = (IDAndSub & 0xFF) | (val << 8);
		assert(getSubclass() == val && "SubclassData too big");
	}
	unsigned int getSubclass() const {return IDAndSub >> 8;}

	// A type may contain many types
	// NumContained may be 0 
	unsigned int NumContained;
	NodeType* const * Contained;
public:
	TypeID getTypeID() const {return (TypeID)(IDAndSub & 0xFF);}

	bool isVoid()	const{return getTypeID() == VoidID;}
	bool isFloat()	const{return getTypeID() == FloatID;}
	bool isDouble()	const{return getTypeID() == DoubleID;}
	bool isFloatingPoint() const {
		return getTypeID() == FloatID ||
				getTypeID() == DoubleID;
	}
	const llvm::fltSemantics &getFltSemantics() const {
		switch(getTypeID()){
			case FloatID: return llvm::APFloat::IEEEsingle;
			case DoubleID:return llvm::APFloat::IEEEdouble;

			default: llvm_unreachable("invalid floating type");
		}
	}

	bool isLabel()    const {return getTypeID() == LableID;		}
	bool isInteger()  const {return getTypeID() == IntegerID;	}
	bool isFunction() const {return getTypeID() == FunctionID;	}
	bool isStruct()   const {return getTypeID() == StructID;	}
	bool isArray()	  const {return getTypeID() == ArrayID;		}
	bool isPointer()  const {return getTypeID() == PointerID;	}

	bool isPrimitiveType() const {
		return getTypeID() <= LastPrimitiveID;
	}
	bool isDerivedType() const {
		return getTypeID() >= FirstDerivedID;
	}
	// FirstClassType - Function is not first class
	bool isFirstClassType() const {
		return getTypeID() !=  FunctionID && getTypeID() != VoidID;
	}
	// SingleValueType - the type is valid for a single register in codegen
	// include all first-class type except struct and array types
	bool isSingleValueType() const {
		return (getTypeID() != VoidID && isPrimitiveType()) ||
				getTypeID() == IntegerID || getTypeID() == PointerID ||
				getTypeID() == VectorID;
	}

	// AggregateType - Return true if the type
	// include struct and array , but dose't include vector
	bool isAggregateType() const {
		return getTypeID() == StructID || getTypeID() == ArrayID;
	}

	// isSized - return true if it make sense to take the size fo type
	// get Actual-Size for a particular target , need DataLayout subsystem to do this
	bool isSized() const {
		// if it's a primitive, it is always sized.
		if (getTypeID() == IntegerID || isFloatingPoint() ||
			getTypeID() == PointerID)
			return true;

		// a function or label, something that can not have a size
		if (getTypeID() != StructID && getTypeID() != ArrayID &&
			getTypeID() !=VectorID)
			return false;
		// try harder in DerivedType
		return isSizedDerivedType();
	}

	bool isSizedDerivedType()const;
	unsigned int getPrimitiveSizeInBits() const;

};
class TypeImplement{
public:
	TypeImplement(){}
	NodeType *VoidType, *IntegerType, *FloatType, *DoubleType;

	NodeType* getVoid(){  }
};
class TypeContext{
public:
	TypeContext():instance(new TypeImplement()){

	}
	TypeImplement* instance;
};

class StorType {
	uint32_t ID;
public:
	enum Type_Storage {
		Storage_None = 0,
		Storage_Typedef,
		Storage_Static,
		Storage_Extern,
		Storage_Auto,
		Storage_Regisiter,
	};
//    void accept(Visitor *v){v->visit(this);}
	void addStorage(Type_Storage tid){
		ID = ((1 << tid) & 0xFF) | (ID & 0xFFFFFF00);
	}

};
class QualType {
public:
	uint32_t ID;
	union{
	lc::ASTType* type;
	lc::ExtQualType* etype;
	};
	typedef llvm::PointerIntPair<
		llvm::PointerUnion<const ASTType*, const ExtQualType*>, 3> PIe;
public:
	enum Type_Qualifier{
		Qualifier_None = 0,
		Qualifier_Const= 0x1,
		Qualifier_Restrict = 0x2,
		Qualifier_Volatile = 0x4,
	};
public:
	explicit QualType(unsigned tid):type(NULL){addQualifier(tid);}
	QualType(ASTType *t, unsigned i):type(t){addQualifier(i);}
	QualType(ExtQualType *t, unsigned i):etype(t){addQualifier(i);}

	void addQualifier(unsigned tid) {
		ID = ID | (1<<tid) ;
	}
	void removeQualifier(unsigned tid){
		ID = (~(1<<tid) & 0xFF) & (ID & 0xFFFFFF00);
	}

	void addVolatile()	{ addQualifier(Qualifier_Volatile); }
	void addConst()		{ addQualifier(Qualifier_Const); }
	void addRestrict() 	{ addQualifier(Qualifier_Restrict); }

	bool hasConst()		{return ID & (1<<Qualifier_Const);}
	bool hasVolatile()	{return ID & (1<<Qualifier_Volatile);}
	bool hasRestrict()	{return ID & (1<<Qualifier_Restrict);}

	QualType* add(QualType* qt){
			assert(this->type == NULL || qt->type == NULL);
			this->ID |= qt->ID;
			if(this->type == NULL)
				this->type = qt->type;
			delete qt;
			return this;
	}
  //  void accept(Visitor *v){v->visit(this);}
};
class ExtTypeBase{

	ExtTypeBase(const ASTType *base, QualType cannon):
		BaseType(base), CannonicalType(cannon){}

	const ASTType const *BaseType;
	QualType CannonicalType;

	friend class ASTType;
};
// do not need ExtQuals just for C
class ExtQualType	: public ExtTypeBase, public llvm::FoldingSetNode{};
/*
	The real something
*/
class ASTType	: public ExtTypeBase {

public:
	ASTType():ExtTypeBase(this, QualType(this,0)){}
	ASTType(QualType canon):
		ExtTypeBase(this, canon){

	}
	/*
	void setPrimitiveType(TypeBuiltin tid){
		switch(tid){
			case Type_Integer: 	ast_type = llvm::Type::getInt32Ty(*llvm_context);
			case Type_Void	: 	ast_type = llvm::Type::getVoidTy(*llvm_context);
			case Type_Label	: 	ast_type = llvm::Type::getLabelTy(*llvm_context);
			case Type_Float	: 	ast_type = llvm::Type::getFloatTy(*llvm_context);
			case Type_Double: 	ast_type = llvm::Type::getDoubleTy(*llvm_context);
		}
	}
	*/
	llvm::Type *ast_type;


	class ArrayTypeBits{};
	class BuiltinTypeBits{unsigned kind;};
	class FunctionTypeBits{unsigned qual:3;};
	class ReferenceTypeBits{unsigned lvalue:1; unsigned inner:1;};

	class AttributedTypeBits{unsigned attrkind;};

	union{
		ArrayTypeBits ArryBits;
		BuiltinTypeBits BuiltinBits;
		AttributedTypeBits AttributeBits;
		FunctionTypeBits FunctionBits;
		ReferenceTypeBits ReferenceBits;
	};



	bool isInteger()const{
		//if (const BuiltinType *bt = llvm::dyn_cast<BuiltinType>(CannonicalType)){
	//		return bt->getKind() == BuiltinType::Integer;
	//	}
	}

};
class BuiltinType : public ASTType{
public:
	unsigned tid;
	BuiltinType(){}
	explicit BuiltinType(unsigned t):tid(t){}
	enum{
		Bool,
		Char,
		Integer,
		Doubel,
	};
	unsigned getKind(){
		return tid;
	}
};
class FunctionType: public ASTType{

};
class FunctionProtoType: public FunctionType{

};
class ParentType	: public ASTType{

};
class ASTContext{
private:
	llvm::BumpPtrAllocator BumpAlloc;

public:
	ASTContext(){

	}
	void* Allocate(unsigned size, unsigned align = 8){
		return BumpAlloc.Allocate(size, align);
	}
};


}
#endif