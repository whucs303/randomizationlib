#include <iostream>
#include <fstream>
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/Transforms/Utils/ValueMapper.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/Twine.h"
#include "llvm-c/Core.h"
#include "llvm/ADT/APInt.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
using namespace llvm;
Function *down=NULL;
Function* rando=NULL;
void rando_func(Module & module)
{
	Module &M=module;
	Module::iterator Mite= M.end();
	Module::iterator Mit=M.begin();
	int numb=0;
	for (;Mit!=Mite;Mit++)
		{numb++;}
	int numb2=0;
	Function* func[numb];
	int i=0;
	numb=0;
	for (Function &I : M)
	{
		numb++;
		//errs()<<"find Function:"<<I.getName()<<"\n";
		func[i]=&I;
		//errs()<<"save Function:"<<func[i]->getName()<<"\n";
		i++;
	}
	srand((unsigned) time(NULL));
	int k=numb;
	for(int l=0;l<numb-1;l++)
	{
        int j=rand()%k;  //产生0 ~ k-1的随机数j
        Function* t=func[j];func[j]=func[k-1];func[k-1]=t;	//将a[j]与“最后一个元素”对换
        k--;
	}
	for (numb2=0;numb2!=numb;numb2++)
	{
		i--;
		//numb2++;
		//errs()<<"numb2="<<numb2<<"\n";
		//errs()<<"numb="<<numb<<"\n";
		//errs()<<"begin change Function:"<<func[i]->getName()<<"\n";
		// if (numb2==numb)
		// 	break;
		if(!func[i]->empty())
		{
		//errs()<<"not empyty Function:"<<func[i]->getName()<<"\n";
		std::string NAME=func[i]->getName().str()+"_text";
		std::string NAME_OLD=func[i]->getName().str();
		std::vector<Type*> ArgTypes;
		ValueToValueMapTy VMap;
		Function *new_F=llvm::CloneFunction(func[i],VMap,NULL);
			new_F->setName(NAME);
			//new_F->dump();
			func[i]->replaceAllUsesWith(new_F);
			//errs()<<"ok"<<"\n";
		func[i]->eraseFromParent();
		new_F->setName(NAME_OLD);
		//new_F->dump();
		}
	}
}


Function* create_down_function(Module* module, LLVMContext& context)
{
	 // 函数基本信息
		  Constant* c = module->getOrInsertFunction("check_fail", // 函数名
		    // 返回值
		    Type::getVoidTy(context),                                             
		    // 函数参数
		    //IntegerType::get(context, 32), IntegerType::get(context, 32), 
		    NULL);
		  Function* check_fail = cast<Function> (c);
		  // 调用约定
		  check_fail->setCallingConv(CallingConv::C);                            

		  // 参数名
		  // 指令块
		  BasicBlock* block = BasicBlock::Create(context, "entry", check_fail);
		  IRBuilder<> builder2(block);
		  Constant *StackChkFail =module->getOrInsertFunction("exit", Type::getVoidTy(context),
                               nullptr);
		  errs()<<"ok2"<<"\n";
    	  	  builder2.CreateCall(StackChkFail, {});
		  errs()<<"ok3"<<"\n";
		  builder2.CreateRetVoid();
		  return check_fail;
}
void add_guard(Module & module)
{

	Module &M=module;
	Module::iterator Mite= M.end();
	Module::iterator Mit=M.begin();
	int numb=0;
	for (;Mit!=Mite;Mit++)
		{numb++;}
	int numb2=0;
	Function* func[numb];
	int i=0;
	numb=0;
	int a=0;
	short int guard=0;
	int set=0;
	for (Function &I : M)
	{
		set=0;
		a=0;
		if(!I.empty()&&I.getName()!="rando"&&I.getName()!="check_fail"){
			LoadInst *Load=NULL;
			Value* tmp=NULL;
			Function *lladd=NULL;
			Value* addre2=NULL;
			Value *Loadadd2=NULL;
			Value *llframe;
			Value *	addreframe;
			Value *	Loadaddframe;
			Value* XORnew=NULL;
			BasicBlock* bb2=NULL;
			std::string name=I.getName();
			for (auto &bb : I)
			{
				//bb.dump();
				
				if(set<0){set++;continue;}
				set=(set+1)%3;
				if(set==1){
				
				if(bb.size()<1)
				{set=0;continue;}
				
				auto it=bb.getTerminator();
				if (a==0)//函数的第一个基本块插入插桩指令
				{
					
					guard++;
					a=1;
					Instruction* it_bbstart=bb.getFirstNonPHI();
					IRBuilder<> builder(bb.getFirstNonPHI());
					
					lladd = Intrinsic::getDeclaration(I.getParent(), Intrinsic::returnaddress);
					addre2 = builder.CreateCall(lladd, builder.getInt32(0));
					Loadadd2=builder.CreatePtrToInt(addre2,Type::getInt16Ty(M.getContext ()));

					llframe=Intrinsic::getDeclaration(I.getParent(), Intrinsic::frameaddress);
					addreframe = builder.CreateCall(llframe, builder.getInt32(0));
					Loadaddframe=builder.CreatePtrToInt(addreframe,Type::getInt16Ty(M.getContext ()));

					XORnew = builder.CreateXor(Loadadd2,Loadaddframe);
					ReturnInst *RI = dyn_cast<ReturnInst>(bb.getTerminator());
					IndirectBrInst *IDI = dyn_cast<IndirectBrInst>(bb.getTerminator());
					SwitchInst *SI = dyn_cast<SwitchInst>(bb.getTerminator());
    				if ((!RI) && (!IDI) && (!SI))
    				{
    					set=0;
      					continue;
    				}

      				set=-4;
					TerminatorInst *ThenTerm;
					Value* cmp = builder.CreateICmpNE(XORnew,XORnew);
					ThenTerm=SplitBlockAndInsertIfThen(cmp,it_bbstart,true);
					IRBuilder<> builder2(ThenTerm);
					Constant *StackChkFail =module.getOrInsertFunction("exit", Type::getVoidTy(M.getContext ()),
                               nullptr);
		
    	  	  			builder2.CreateCall(StackChkFail, {});
					//builder2.CreateCall(down, {});

					//it=it_bbstart->getParent()->getTerminator();
				}
					//it--;
					//cast<Instruction>(it)->dump();
					ReturnInst *RI = dyn_cast<ReturnInst>(it);
					
					IndirectBrInst *IDI = dyn_cast<IndirectBrInst>(it);
					
					SwitchInst *SI = dyn_cast<SwitchInst>(it);
	
    					if ((!RI) && (!IDI) && (!SI))
    					{
    						set=0;
      						continue;
    					}
					
					//cast<Instruction>(it)->dump();
					if(cast<Instruction>(it)->getOpcode()==Instruction::Br)
					{
						if(cast<BranchInst>(it)->isConditional()){
						it--;}}
					
					
					IRBuilder<> builder(cast<Instruction>(it)/*->getTerminator()*/);
					
					Instruction *fortext=cast<Instruction>(it);

					
					//I.dump();
					//获取返回地址
					Value *addre = builder.CreateCall(lladd, builder.getInt32(0));

					Value* Loadadd=builder.CreatePtrToInt(addre,Type::getInt16Ty(M.getContext ()));
					Value *addreframe2 = builder.CreateCall(llframe, builder.getInt32(0));

					Value* Loadaddframe2=builder.CreatePtrToInt(addreframe,Type::getInt16Ty(M.getContext ()));
					Value* XOR = builder.CreateXor(Loadadd,Loadaddframe2);
					Value* cmp = builder.CreateICmpNE(XOR,XORnew);
					
					
					TerminatorInst *ThenTerm;

					

					//cast<Instruction>(it)->dump();
					BasicBlock* BB=cast<Instruction>(it)->getParent();

					
					ThenTerm=SplitBlockAndInsertIfThen(cmp,fortext,true);
					IRBuilder<> builder2(ThenTerm);
					Constant *StackChkFail =module.getOrInsertFunction("exit", Type::getVoidTy(M.getContext ()),
                               nullptr);
		
    	  	  			builder2.CreateCall(StackChkFail, {});
					//Value *addre2 = builder2.CreateCall(down, {});

					//BB->dump();
				}
				
			}
			a=0;
			numb++;
			//errs()<<"find Function:"<<I.getName()<<"\n";
			//I.dump();
			func[i]=&I;
			//errs()<<"save Function:"<<func[i]->getName()<<"\n";
			i++;
			
		}
	}
	//fout.close();
}
namespace
{
	struct FRProtector : public ModulePass{
		static char ID;
		FRProtector() : ModulePass(ID){}
		virtual bool runOnModule(Module &M)
		{
			
			add_guard(M);
			rando_func(M);
			
			return false;
		}
	};
}

	char FRProtector::ID = 0;

	static RegisterPass<FRProtector> X("FRProtector","function-level randomization and control flow protection");
