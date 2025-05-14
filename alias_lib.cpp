#include "llvm/IR/Function.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include <iostream>
#include <map>
#include <sstream>
using namespace llvm;
using namespace std;

// #define pfel(i) errs() << i << "\n"
// #define pf(i) errs() << i
#define pfel(i) errs() << "" << ""
#define pf(i) errs() << ""
#define pfvar(i) errs() << "%" << i
#define getnamefo(Si) Si->getPointerOperand()->getName().str()

// macro for print the Value" type of data we get from getoperand() function
#define pfop(operand)                                                          \
  operand->print(llvm::outs());                                                \
  llvm::outs();
#define pfopel(operand)                                                        \
  operand->print(llvm::outs());                                                \
  llvm::outs() << "\n";

namespace {
std::string stripmod(std::string var) {
  if (var[0] == '%') {
    std::string var2(var.begin() + 1, var.end());
    return var2;
  } else
    return var;
}

std::string addmod(std::string var) {
  if (var.at(0) == '%')
    return var;
  std::string s = "%";
  return s.append(var);
}
void printSet(const std::set<std::string>& mySet) {
    for (const auto& elem : mySet) {
        // std::cout << elem << std::endl;
        pfel(elem);
    }
}

void iterateME(Instruction *i) {
  unsigned numOperands = i->getNumOperands();

  for (unsigned ii = 0; ii < numOperands; ++ii) {
    Value *operand = i->getOperand(ii);
    // pfel(operand->print());
    pf("\nindex :");
    pfel(ii);
    pf("operand type:");
    pfopel(operand->getType());
    pf("Plain operand :");
    pfopel(operand);
    pfel("----------------------");
  }
}
bool isInteger(const std::string &str) {
  if (str.empty() ||
      ((!isdigit(str[0])) && (str[0] != '-') && (str[0] != '+'))) {
    return false;
  }

  char *p;
  strtol(str.c_str(), &p, 10);

  return (*p == 0);
}
std::string getTypeString(Type *t) {
  std::string s;
  raw_string_ostream os(s);
  t->print(os, false); // llvm::outs() << "\n";
  // i->printAsOperand(os, false);
  os.flush();
  std::string instructionSubstr = os.str();
  return instructionSubstr;
}
void printMap2(const std::map<std::string, std::vector<std::string>>& myMap) {
    // Define an iterator to iterate over the map
    for (auto it = myMap.begin(); it != myMap.end(); ++it) {
        // Print the key
        // std::cout << "Key: " << it->first << std::endl;
        pfel("Key: " << it->first);

        // Print the values associated with the key
        // std::cout << "Values: ";
        pf("Values: ");
        for (const auto& value : it->second) {
            // std::cout << value << " ";
            pf(value<<" ");
        }
        pfel("");
        // std::cout << std::endl;
    }
}
std::string getresultOperand(Instruction *i) {
  std::string s;
  raw_string_ostream os(s);
  i->printAsOperand(os, false);
  os.flush();
  std::string instructionSubstr = os.str();
  return instructionSubstr;
}

int stringToInt(const std::string &str) {
  if (!isInteger(str)) {
    // Handle error or return a default value
    // std::cerr << "Invalid integer string: " << str << std::endl;
    return 0; // Default value
  }

  int intValue;
  std::istringstream iss(str);
  iss >> intValue;
  return intValue;
}
void iterateBasicBlocks2(vector<BasicBlock *> &BBs) {

  for (BasicBlock *BB : BBs) {
    // Do something with each BasicBlock pointer 'BB'
    // For example, you can print its name
    outs() << "BasicBlock: " << BB->getName();
  }
  pfel("\n");
}
std::string getVariableNamefromoperand(Value *i) {
  if (llvm::ConstantInt *constantint = llvm::dyn_cast<llvm::ConstantInt>(i)) {

    // pfel("value is constant");
    int value = constantint->getSExtValue();
    return std::to_string(value);
    ;
    pf(value);
    pf("\n");
  } else {
    // pfel("value is variable!!!");

    // Check if the stored value is an instruction
    if (Instruction *storedInstruction = dyn_cast<Instruction>(i)) {

      // Get the name of the variable from the instruction
      std::string variableName = storedInstruction->getName().str();

      // Print the name of the variable
      return getresultOperand(storedInstruction);
      pfel("HMMMMMMMM" << getresultOperand(storedInstruction));
      pfvar(variableName);
      pf("\n");
      return variableName;
      // outs() << "Variable name: " << variableName << "\n";
    } else {
      // The stored value is not an instruction
      outs() << "Stored value is not an instruction.\n";
    }
  }
}

void iterateBasicBlocksVector(const vector<BasicBlock *> &BBs) {
  for (size_t i = 0; i < BBs.size(); ++i) {
    BasicBlock *BB = BBs[i];
    // Do something with each BasicBlock pointer 'BB'
    // For example, you can print its name
    outs() << "\n\tBasicBlock " << i << ": " << BB->getName() << "\n";
    BB->print(llvm::outs());
  }
}
bool isAllocaInstruction(Value *operand) {
    if (AllocaInst *AI = dyn_cast<AllocaInst>(operand)) {
        // Operand is an AllocaInst
        return true;
    }
    return false;
}
void printMap(const std::map<std::string, std::string> &myMap) {
  for (auto it = myMap.begin(); it != myMap.end(); ++it) {
    pfel("Key: " << it->first << ", Value: " << it->second);
  }
}
// string typediff(string vartyp2,string optype)
// {

// }
std::string typediff(const std::string &s1, const std::string &s2) {
  std::string difference;
  size_t minLength = std::min(s1.length(), s2.length());

  for (size_t i = 0; i < minLength; ++i) {
    if (s1[i] != s2[i]) {
      difference += s1[i];
    }
  }

  // Add remaining characters of longer string
  if (s1.length() > minLength) {
    difference += s1.substr(minLength);
  } else if (s2.length() > minLength) {
    difference += s2.substr(minLength);
  }

  return difference;
}

// Function to iterate over pointsTo map
void iteratePointsTo(
    const std::map<Instruction *,
                   std::map<std::string, std::vector<std::string>>> &pointsTo) {
  pfel("\n\nPOINTS TO INFO");
  // Iterate over the outer map
  for (auto it = pointsTo.begin(); it != pointsTo.end(); ++it) {
    Instruction *instr = it->first;
    pfel("Instruction: " << *instr);

    // Iterate over the inner map
    for (auto inner_it = it->second.begin(); inner_it != it->second.end();
         ++inner_it) {
      std::string pointer = inner_it->first;
      const std::vector<std::string> &values =
          inner_it->second; // Change to const reference

      pfel("  Pointer: " << pointer);

      // Iterate over the vector of strings
      for (const std::string &value : values) {
        pfel("    \t\tValue: " << value);
      }
    }
  }
}

void iterateVector(const std::vector<Instruction *> &instructions) {
  for (llvm::Instruction *inst : instructions) {
    // std::cout << inst << std::endl;
    pfel("|"<<*inst);
  }
}

std::vector<Instruction *> findSuccessors(Instruction *Inst) {
  std::vector<Instruction *> successorList;
  // if (!I)
  //   return;
  BasicBlock *BB = Inst->getParent();
  for (BasicBlock *Successor : successors(BB)) {
    // Do something with the successor
    // BasicBlock *Succ = Inst.getSuccessor(i);
    successorList.push_back(dyn_cast<Instruction>(Successor->begin()));
  }

  // pf(*Inst << " sizeeeeeeeeeeEE:::::::::::" << successorList.size());
  return successorList;
}
vector<pair<string, string>> cross(const set<string> &s1,
                                   const set<string> &s2) {
  pfel("IN cross");
  vector<pair<string, string>> result;
  if(s1.size()==0)
  {
    return result;
  }

  if(s2.size()==0)
  {
    for(auto &a:s1)result.push_back(make_pair(a,"?"));

  }
  
  for(auto &a:s1)pf(a<<"-");
  for ( auto &elem2 : s2) {
    pf(elem2<<"+");
    }

  // Iterate over each element in s1
  for (auto &elem1 : s1) {
    // Iterate over each element in s2 and form pairs with elements in s1
    for ( auto &elem2 : s2) {
      result.push_back(make_pair(elem1, elem2));
    }
  }
  pfel("out of cross");

  return result;
}
// bool ValueChanged(vector<pair<string, string>> a,
//                   vector<pair<string, string>> b) {
//   //
//   return false;
// }
bool hasDifference(const std::vector<std::pair<std::string, std::string>>& a,
                   const std::vector<std::pair<std::string, std::string>>& b) {
    // Check if sizes are different
    if (a.size() != b.size()) {
        return true;
    }

    // Iterate over pairs in 'a' and check if they exist in 'b'
    for (const auto& pair : a) {
        if (std::find(b.begin(), b.end(), pair) == b.end()) {
            // Pair not found in 'b'
            return true;
        }
    }

    // Iterate over pairs in 'b' and check if they exist in 'a'
    for (const auto& pair : b) {
        if (std::find(a.begin(), a.end(), pair) == a.end()) {
            // Pair not found in 'a'
            return true;
        }
    }

    // No difference found
    return false;
}
bool ValueChanged(const std::vector<std::pair<std::string, std::string>>& a,
                     const std::vector<std::pair<std::string, std::string>>& b) {
    // Check if the sizes of the vectors are equal
    if (a.size() != b.size()) {
        return false;
    }

    // Iterate over the elements of both vectors
    for (size_t i = 0; i < a.size(); ++i) {
        // Check if the pairs at the same index are equal
        if (a[i] != b[i]) {
            return false;
        }
    }

    // Vectors are equal
    return true;
}
void printPairs(const vector<pair<string, string>> &vec) {
  pf("(");
  for (const auto &pair : vec) {
    pf( "(" << pair.first
              << ", " << pair.second<<")" );
    // std::cout << "First value: " << pair.first
    //           << ", Second value: " << pair.second << std::endl;
  }pf(")\n");
}
std::vector<std::pair<std::string, std::string>> Uni(
    const std::vector<std::pair<std::string, std::string>> &vec1,
    const std::vector<std::pair<std::string, std::string>> &vec2) {
  
  std::vector<std::pair<std::string, std::string>> result = vec1;

  for (const auto &pair : vec2) {
    if (std::find(vec1.begin(), vec1.end(), pair) == vec1.end()) {
      result.push_back(pair);
    }
  }

  return result;
}
// check if same first var
// vector<pair<string, string>>
// Uni(const std::vector<std::pair<std::string, std::string>> &vec1,
//     const std::vector<std::pair<std::string, std::string>> &vec2) {

//   std::unordered_set<std::string> uniqueKeys;
//   std::vector<std::pair<std::string, std::string>> result;

//   // Add elements from vec1 to result vector
//   for (const auto &pair : vec1) {
//     if (uniqueKeys.insert(pair.first).second) {
//       result.push_back(pair);
//     }
//   }

//   // Add elements from vec2 to result vector
//   for (const auto &pair : vec2) {
//     if (uniqueKeys.insert(pair.first).second) {
//       result.push_back(pair);
//     }
//   }

//   return result;
// }
// set of definitions in an instructions
set<string> definition(vector<pair<string, string>> in, int pointertype,
                       StoreInst *Si) {
  pfel("IN DEF");
  set<string> def;
  switch (pointertype) {
  
  //alloca load
  case 2:
  case 4:
  {
LoadInst *li= dyn_cast<LoadInst>(Si->getPointerOperand());
    int depth=1;//          Ai->getOperand(0);
    string result = getVariableNamefromoperand(li->getPointerOperand());
    // pfel("88"<<getVariableNamefromoperand(li->getPointerOperand()));
    while(!isAllocaInstruction(li->getPointerOperand()))
    {
      result = getVariableNamefromoperand(li->getPointerOperand());
      pfel("--"<<result);
      depth++;
          // if (LoadInst *LI = dyn_cast<LoadInst>(operand)) {
      li=dyn_cast<LoadInst>(li->getPointerOperand());
      
    }
    pfel("type 2\n depth is "<<depth<<" result operands is"<<result);
    
    set<string> targetvar;
    //variablename,depth
    queue<pair<string,int> >q;
    
    q.push(make_pair(result,depth));
    while(!q.empty())
    {
      pfel("in queu");
      pair<string,int> pr;
      pr=q.front();q.pop();
      string temp=pr.first;
      int tempdepth=pr.second-1;
      
      
      for (auto it = in.begin(); it != in.end(); ++it)
      {
        pfel("infor");
        if(it->first==result)
        {
          if(it->second=="?")continue;
          temp=it->second;
          pfel("remp"<<temp);
          
          
          if(tempdepth==0)
          {
            targetvar.insert(it->second);
            pf("<"<<it->second<<">");
            pair<string,int> t;
            while(!q.empty())
            {
              t=q.front();
              q.pop();
              targetvar.insert(t.first);

            }
            // return targetvar;

          }else
          {
            q.push(make_pair(it->second,tempdepth));
          }
          
        }
        // pfel("Key: " << it->first << ", Value: " << it->second);
      }

    }
    // pfel("points to before killing");
    // pointsTo[Si];
    // printMap2(pointsTo[Si]);
    // for (const auto& elem : targetvar) {
    //     // std::cout << elem << std::endl;
    //     // pfel(elem);
    //     deleteVectorForKey(pointsTo[Si],elem);
    // }
    
    return targetvar;
    break;

    // LoadInst *li= dyn_cast<LoadInst>(Si->getPointerOperand());
    // pfel("load"<<*li);
    // int depth=1;//          Ai->getOperand(0);
    // string result = getVariableNamefromoperand(li->getOperand(0));
    // pfel("ll"<<result);
    // pfopel(li->getPointerOperand());
    // while(!isAllocaInstruction(li->getPointerOperand()))
    // {
    //   pfel("ww");
    //   result = getresultOperand(li);
    //   pfel("res"<<result);
    //   depth++;
    //       // if (LoadInst *LI = dyn_cast<LoadInst>(operand)) {
    //   li=dyn_cast<LoadInst>(li->getPointerOperand());
      
    // }
    // pfel("type 2\n depth is "<<depth<<" result operands is"<<result);
    def.insert(result);
    return def;
  }
  case 1:
  case 3: {
    string op1 = getVariableNamefromoperand(Si->getPointerOperand());
    pfel("def is of "<<op1);  
    def.insert(op1);
    return def;
  }
  
  }
  // Instructions type
  // x=&a;
  // x=y
  // x=*y
  //*x=y
}
void removeValuesForKey(const std::string& s, std::map<std::string, std::vector<std::string>>& curmap) {
    // Check if the key exists in the map
    auto it = curmap.find(s);
    if (it != curmap.end()) {
        // Erase the vector associated with the key
        curmap.erase(it);
    }
}
void deleteVectorForKey(map<string, vector<string>>& myMap, const string& key) {
    auto it = myMap.find(key); // Find the key in the map
    if (it != myMap.end()) {   // If the key is found
        myMap.erase(it);        // Erase the entry for the key
    }
}
// set of variables tnat are killed by the instruction
set<string> kill(vector<pair<string, string>> in , int pointertype, StoreInst *Si,
                 map<Instruction *, map<string, vector<string>>> pointsTo) {
                  pfel("IN kill");
  switch (pointertype) {
  
  //destination is  load
  case 4:
  case 2:
  {
    pfel("ddd"<<getVariableNamefromoperand(Si->getPointerOperand()));
    LoadInst *li= dyn_cast<LoadInst>(Si->getPointerOperand());
    int depth=1;//          Ai->getOperand(0);
    pfel("before reuslt"<<li);
    string result = getVariableNamefromoperand(li->getPointerOperand());
    pfel("88"<<getVariableNamefromoperand(li->getPointerOperand()));
    while(!isAllocaInstruction(li->getPointerOperand()))
    {
      result = getVariableNamefromoperand(li->getPointerOperand());
      pfel("--"<<result);
      depth++;
          // if (LoadInst *LI = dyn_cast<LoadInst>(operand)) {
      li=dyn_cast<LoadInst>(li->getPointerOperand());
      
    }
    pfel("type 2\n depth is "<<depth<<" result operands is"<<result);
    
    set<string> targetvar;
    //variablename,depth
    queue<pair<string,int> >q;
    
    q.push(make_pair(result,depth));
    while(!q.empty())
    {
      pfel("in queu");
      pair<string,int> pr;
      pr=q.front();q.pop();
      string temp=pr.first;
      int tempdepth=pr.second-1;
      
      
      for (auto it = in.begin(); it != in.end(); ++it)
      {
        pfel("infor");
        if(it->first==result)
        {
          if(it->second=="?")continue;
          temp=it->second;
          pfel("remp"<<temp);
          
          
          if(tempdepth==0)
          {
            targetvar.insert(it->second);
            pf("<"<<it->second<<">");
            pair<string,int> t;
            while(!q.empty())
            {
              t=q.front();
              q.pop();
              targetvar.insert(t.first);

            }
            // return targetvar;

          }else
          {
            q.push(make_pair(it->second,tempdepth));
          }
          
        }
        // pfel("Key: " << it->first << ", Value: " << it->second);
      }

    }
    // pfel("points to before killing");
    // // pointsTo[Si];
    // printMap2(pointsTo[Si]);
    for (const auto& elem : targetvar) {
        // std::cout << elem << std::endl;
        // pfel(elem);
        deleteVectorForKey(pointsTo[Si],elem);
    }
    
    return targetvar;
    break;
  }
  //destination of alloca
  case 1:
  case 3: {
    string op1 = getVariableNamefromoperand(Si->getPointerOperand());
    pfel("kill set"<<op1);
    pfel("points to before killing");
    // pointsTo[Si];
    // printMap2(pointsTo[Si]);
    deleteVectorForKey(pointsTo[Si],op1);
    pfel("points to before killing");
    set<string> s1;
    s1.insert(op1);
    return s1;


    // map<string, vector<string>> curmap=pointsTo[dyn_cast<Instruction>(Si)];
    // removeValuesForKey(op1,`curmap);
    break;
  }
  
  }
  // Instructions type
  // x=&a;
  // x=y
  // x=*y
  //*x=y
}

std::set<std::string> createSetFromPairs(
    const std::vector<std::pair<std::string, std::string>> &pairs,
    const std::string &str) {
  std::set<std::string> resultSet;

  for (const auto &pair : pairs) {
    if (pair.first == str) {
      resultSet.insert(pair.second);
    }
  }

  return resultSet;
}



// returns pointee set of the given variable
set<string> pointee(vector<pair<string, string>> in, int pointertype,
                    StoreInst *Si) {
  pfel("In pointee");
  set<string> pointeeInfo;
  switch (pointertype) {
  // p = &a;
  //  store i32* %a, i32** %p, align 8
  //source is alloca
  case 1:
  case 2: {
        

    string op1 ;
    // if(Si->getValueOperand()->getName().str()=="")op1="NULL";
    if(isa<ConstantPointerNull>(Si->getOperand(0)))op1="NULL";
    else
    op1= getVariableNamefromoperand(Si->getValueOperand());
    pfel("pointeee\t\t{"<<op1<<"}");
    set<string> s;
    s.insert(op1);
    return s;
    // return createSetFromPairs(input, op1);

    break;
  }

  //source is load
  case 3: 
  case 4: {
    pfel("var is load");
    string result;
    int depth=1;
    set<string> targetvar;
    if(GetElementPtrInst *GEP = dyn_cast<GetElementPtrInst>(Si->getValueOperand()))
    {
      // GEP->getOperand(0);
      result = getVariableNamefromoperand(GEP->getOperand(0));
      pfel("op1"<<result);
      targetvar.insert(result);
      return targetvar;

    }else
    {
      LoadInst *li= dyn_cast<LoadInst>(Si->getValueOperand());
    //          Ai->getOperand(0);
    result = getVariableNamefromoperand(li->getPointerOperand());
    // pfel("88"<<getVariableNamefromoperand(li->getPointerOperand()));
    while(!isAllocaInstruction(li->getPointerOperand()))
    {
      result = getVariableNamefromoperand(li->getPointerOperand());
      pfel("--"<<result);
      depth++;
          // if (LoadInst *LI = dyn_cast<LoadInst>(operand)) {
      li=dyn_cast<LoadInst>(li->getPointerOperand());
      
    }

    }
    
    pfel("type 2\n depth is "<<depth<<" result operands is"<<result);
    
    
    //variablename,depth
    queue<pair<string,int> >q;
    
    q.push(make_pair(result,depth));
    while(!q.empty())
    {
      pfel("in queu");
      pair<string,int> pr;
      pr=q.front();q.pop();
      string temp=pr.first;
      int tempdepth=pr.second-1;
      
      
      for (auto it = in.begin(); it != in.end(); ++it)
      {
        pfel("infor");
        if(it->first==result)
        {
          if(it->second=="?")continue;
          temp=it->second;
          pfel("remp"<<temp);
          
          
          if(tempdepth==0)
          {
            targetvar.insert(it->second);
            pf("<"<<it->second<<">");
            pair<string,int> t;
            while(!q.empty())
            {
              t=q.front();
              q.pop();
              targetvar.insert(t.first);

            }
            // return targetvar;

          }else
          {
            q.push(make_pair(it->second,tempdepth));
          }
          
        }
        // pfel("Key: " << it->first << ", Value: " << it->second);
      }

    }
    // pfel("points to before killing");
    // // pointsTo[Si];
    // printMap2(pointsTo[Si]);
    // for (const auto& elem : targetvar) {
    //     // std::cout << elem << std::endl;
    //     // pfel(elem);
    //     deleteVectorForKey(pointsTo[Si],elem);
    // }
    
    return targetvar;
    break;
  }
  }
  // Instructions type
  // x=&a;
  // x=y
  // x=*y
  //*x=y
}

vector<pair<string, string>> minus(const vector<pair<string, string>> &op1,
                                   const vector<pair<string, string>> &op2) {
  vector<pair<string, string>> result;
  pfel("In minus");

  printPairs(op1);

  // Iterate over op1 and check if each pair is present in op2
  for (const auto &pair1 : op1) {
    // pfel("in ");
    bool found = false;
    for (const auto &pair2 : op2) {
      // pfel("inin");
      if (pair1 == pair2) {
        // pfel("checking?");
        found = true;
        break;
      }
    }
    // If the pair is not found in op2, add it to the result
    if (!found) {
      result.push_back(pair1);
    }
  }

  return result;
}


bool isLoadInstruction(Value *operand) {
    if (LoadInst *LI = dyn_cast<LoadInst>(operand)) {
        // Operand is a LoadInst
        return true;
    }
    return false;
}

int getTypeofPointerInstruction(StoreInst *Si, map<string, string> varType) {
  if (Si->getValueOperand()->getType()->isPointerTy()) {
    // iterateME(Si);
    pfel(*Si);
    if(isa<ConstantPointerNull>(Si->getOperand(0)))
    {
    errs() << "Pointer value is null\n";
    return 1;
    }
    string op1 = getVariableNamefromoperand(Si->getPointerOperand());
    // Si->getPointerOperandType()->print(errs());
    // pfel(Si->getPointerOperandType()->print(errs()));
    string op2 = getVariableNamefromoperand(Si->getValueOperand());
    string ty1, ty2;
    ty1 = getTypeString(Si->getPointerOperand()->getType());
    ty2 = getTypeString(Si->getValueOperand()->getType());
    // pfel("op1 " << op1 << "type of op1 " << ty1);
    // pfel("op2 " << op2 << "type of op2 " << ty2);

    string getfromvarType1, getfromvarType2;
    if (varType.find(op1) != varType.end() &&
        varType.find(op2) != varType.end()) {

      getfromvarType1 = varType[op1];
      getfromvarType2 = varType[op2];
      // pfel("fromm vartype" << getfromvarType1 << " " << getfromvarType2);

      // store i32* %a, i32** %p, align 8 || p=&a p-op1 a-op2
      if (typediff(ty1, getfromvarType1) == "*" &&
          typediff(ty2, getfromvarType2) == "*") {
        // map<Instruction *, map<string, vector<string>>> pointsTo;
        return 1;
      }

    }//source alloca destination load
    if( isAllocaInstruction(Si->getValueOperand()) && isLoadInstruction(Si->getPointerOperand()))
    {
      return 2;
    }
    //source load destination alloca
    else if(isAllocaInstruction(Si->getPointerOperand()) && isLoadInstruction(Si->getValueOperand()))
    {
      return 3;
    }
    //both loads
    else
    {
      if ((dyn_cast<GetElementPtrInst>(Si->getPointerOperand()))|| ( dyn_cast<GetElementPtrInst>(Si->getValueOperand())))
      {
        return 3;
      }
      return 4;

    }
    //  else {
    //   pfel("error: Type not found in the varType");
    //   return -1;
    // }
  }
  return -1;
}

vector<pair<string, string>>
transferFunc(Instruction *I, vector<pair<string, string>> in,
             map<Instruction *, map<string, vector<string>>> pointsTo,
             map<string, string> varType) {
  pfel("Current INstruction\t\t "<<*I);
  vector<pair<string, string>> newout;
  set<string> variables;
  for (auto it = varType.begin(); it != varType.end(); ++it) {
    // pfel("Key: " << it->first << ", Value: " << it->second);
    variables.insert(it->first);
  }

  // 4 types of pointer instruction to be handled
  //
  int pointertype = -1;


  if (StoreInst *Si = dyn_cast<StoreInst>(I)) {

    pointertype = getTypeofPointerInstruction(Si, varType);

    if(pointertype==-1)return in;

    pfel("TRansfer function Pointer Type"<<pointertype);
    set<string > tempkillset=kill(in, pointertype, Si, pointsTo);
    if(tempkillset.size()>1)
    tempkillset.clear();
    pfel("KILL SET IS");
    pf("{");
    printSet(tempkillset);
    pf("}");
    // pfel("transferfunction new points to");
    // printMap2(pointsTo[Si]);

    vector<pair<string, string>> cross_data=cross(tempkillset, variables);
    pfel("after 1st cross");
    set<string> definition_data=definition(in, pointertype, Si);
    pfel("\t\tdefinition set");
    printSet(definition_data);
    set<string> pointee_out =pointee(in, pointertype, Si);
    pfel("\t\tpointee set");
    printSet(pointee_out);
    vector<pair<string, string>> cross_data2=cross(definition_data,pointee_out);
    pfel("after 2nd cross");
    pfel("\t\tInput:");
    printPairs(in);
    pfel("");

    pfel("\t\tkillsetXVarible:");
    printPairs(cross_data);
    pfel("");

    pfel("\t\tdefintions X pointee");
    printPairs(cross_data2);
    pfel("");

    vector<pair<string, string>> minus_out=minus(in, cross_data);

    pfel("\t\tMinus output Data\n");
    printPairs(minus_out);


    newout = Uni(
        minus_out,cross_data2);

    pfel("OUTSET:");
    printPairs(newout);
    
    // pfel("NEWOUTS");
    // printPairs(newout);

    return newout;
    // a=&b;
    // a=*p two loads
    // a=p one load
    //*a=&p one load

  }
  else 
    {
      pfel("Not store instruction");
      return in;
    }

  return newout;
}

bool hasCommonString(const vector<string>& a, const vector<string>& b) {
    // Create a set to store unique strings from vector a
    std::unordered_set<std::string> setA(a.begin(), a.end());

    // Iterate through vector b and check if any string is present in setA
    for (const std::string& str : b) {
      // if(str=="?")
        if (setA.find(str) != setA.end() && str!="?" && str!="NULL") {
            return true; // Common string found
        }
    }

    return false; // No common string found
}

std::map<std::string, std::vector<std::string>> createMap(const vector<pair<string,string>>& vec) {
    std::map<std::string, std::vector<std::string>> m1;
    for (const auto& pair : vec) {
      if(pair.second!="?")
        m1[pair.first].push_back(pair.second);
    }
    return m1;
}

// vector<pair<string,string>>
std::map<std::string, std::vector<std::string>> getAlias(const std::vector<std::pair<std::string, std::string>>& OUT)
{
  std::map<std::string, std::vector<std::string>> outmap=createMap(OUT);
  map<string, vector<string>> alias;

  for (const auto& pair : outmap) {
    for (const auto& pair2 : outmap) {
      // pfel(pair.first<<"[]"<<pair2.first);
      if(pair.first!=pair2.first)
      {
        if(hasCommonString(pair.second,pair2.second))
        {
          alias[pair.first].push_back(pair2.first);

        }
      }
    }
  }
    return alias;


}
void printAlias(std::map<std::string, std::vector<std::string>> myMap)
{
  for (auto it = myMap.begin(); it != myMap.end(); ++it) {
        // Print the key
        // pf(stripmod(it->first)<<" -> " );
        errs()<<stripmod(it->first)<<" -> ";

        // Print the values associated with the key
        // pf("{ ");
        errs()<<"{ ";
        for (const auto& value : it->second) {
            // pf(stripmod(value)<<" ") ;
            errs()<<stripmod(value)<<" ";
        }
        errs()<<" }\n";
        // pfel(" }");
    }

} 



struct alias_c : public FunctionPass {
  static char ID;
  alias_c() : FunctionPass(ID) {}

  void kildalls(Function &F) {
    std::queue<Instruction *> worklist;
    std::map<Instruction *, std::vector<Instruction *>> successor;

    map<string, int> storeMap;
    map<string, vector<string>> loads; // loads
    // map<string, vector<string>> pointsTO;

    // each instructinos
    map<Instruction *, map<string, vector<string>>> pointsTo;
    map<string, string> varType;
    map<string, vector<string>> pt;

    bool flag = 1;
    for (BasicBlock &BB : F) {
      for (Instruction &I : BB) {

        if (AllocaInst *Ai = dyn_cast<AllocaInst>(&I)) {
          // if (Ai->getPointe)
          // iterateME(Ai);
          // pfel("shoot"
          //      << ":" << I);
          
          string result = getresultOperand(Ai);
          string optype = getTypeString(Ai->getAllocatedType());
          if (ArrayType *ArrayTy = dyn_cast<ArrayType>(Ai->getAllocatedType())) {
            // The allocated type is an array type
            // Do something
            pfel("someshit"<<result);
            varType[result] = result;
          }else
          {
            if (varType.find(result) == varType.end())
            varType[result] = optype;
          else {
            pfel("errror: same variable name found overwriting original");
            varType[result] = optype;
          }

          }
          

          // getresultOperand();
        } else if (StoreInst *Si = dyn_cast<StoreInst>(&I)) {
          // iterateME(Si);
          Value *storedValue = Si->getValueOperand();

          if (Si->getValueOperand()->getType()->isPointerTy()) {
            // iterateME(Si);
            // pfel(*Si);
            string op1 = getVariableNamefromoperand(Si->getPointerOperand());
            // Si->getPointerOperandType()->print(errs());
            // pfel(Si->getPointerOperandType()->print(errs()));
            string op2 = getVariableNamefromoperand(Si->getValueOperand());
            string ty1, ty2;
            ty1 = getTypeString(Si->getPointerOperand()->getType());
            ty2 = getTypeString(Si->getValueOperand()->getType());
            // pfel("op1 " << op1 << "type of op1 " << ty1);
            // pfel("op2 " << op2 << "type of op2 " << ty2);

            string getfromvarType1, getfromvarType2;
            if (varType.find(op1) != varType.end() &&
                varType.find(op2) != varType.end()) {

              getfromvarType1 = varType[op1];
              getfromvarType2 = varType[op2];
              // pfel("fromm vartype" << getfromvarType1 << " "
              //                      << getfromvarType2);

              // store i32* %a, i32** %p, align 8 || p=&a p-op1 a-op2
              if (typediff(ty1, getfromvarType1) == "*" &&
                  typediff(ty2, getfromvarType2) == "*") {
                // map<Instruction *, map<string, vector<string>>> pointsTo;

                // already variables is present we have to add to its list
                if (pt.find(op1) != pt.end()) {
                  pt[op1].push_back(op2);
                } else {
                  vector<string> pointeeInfo;
                  pointeeInfo.push_back(op2);
                  pt[op1] = pointeeInfo;
                }

                pointsTo[dyn_cast<Instruction>(&I)] = pt;

                // pfel(op1 << "->");
                // iterateVector(pointeeInfo);
              }

            } else {
              pfel("error: Type not found in the varType");
            }

            // op2->op1;
          }

          // errs() << " " << I << " ";

          // unsigned numOperands = Si->getNumOperands();
          // pf("\\\\");
          // pfop(Si->getPointerOperand());
          // pfop(Si->getValueOperand());
          // string op1, op2;
          // op1 = getVariableNamefromoperand(Si->getPointerOperand());
          // op2 = getVariableNamefromoperand(Si->getValueOperand());
          // pf("op1" << op1);
          // pf("op2" << op2);
          // pf("///");
          // // storeMap[op1] = op2;

          // std::string variableName =
          //     getVariableNamefromoperand(Si->getPointerOperand());
          // // Si->getPointerOperand()->getName().str(); // 1getOperand(1)
          // Value *storedValue = Si->getValueOperand(); // 0

          // pfel("1st operand/variable name :");
          // pf("-->");
          // // variableName = addmod(variableName);

          // // pf(variableName << " = ");
          // string destinationOpernad =
          // getVariableNamefromoperand(storedValue);
          // pfel(destinationOpernad);
          // // val is int assign it to variable
          // if (isInteger(destinationOpernad)) {
          //   storeMap[variableName] = stringToInt(destinationOpernad);

          // } else {
          //   if (storeMap.find(destinationOpernad) != storeMap.end())
          //     storeMap[variableName] = storeMap[destinationOpernad];
          //   // newMap[variableName] =
          //   //     meetValue(curMap[variableName], destinationOpernad);
          //   // newMap[variableName] =
          //   // curMap[getVariableNamefromoperand(storedValue)];
          // }
          // pf("PPPPPPPP" << storeMap[variableName]);
        } else if (LoadInst *Li = dyn_cast<LoadInst>(&I)) {
          if (Li->getPointerOperandType()->isPointerTy()) {
            pfel("In pointer load" << *Li);
            // std::string s;
            // // iterateME(Li);

            // raw_string_ostream os(s);
            // Li->printAsOperand(os, false);
            // os.flush();
            // std::string resultoperand = os.str();
            // // pf(*Li);
            // std::string variableName =
            // Li->getPointerOperand()->getName().str(); unsigned numOperands
            // = Li->getNumOperands(); pfel("-->" << resultoperand << " = " <<
            // variableName);

            string result = getresultOperand(Li);
            string optype = getTypeString(Li->getOperand(0)->getType());
            optype = optype.substr(0, optype.size() - 1);

            // pfel("result" << result << "type" << optype);
            // pfel("alloca result " << result << " |" << optype);
            // pushhing %p into map for storing its value
            // if (varType.find(result) == varType.end())
            //   varType[result] = optype;

            //  outFile << "-->" << addmod(instructionSubstr) << "
            // = "`
            //         << valueTuple[getStringFromInstruction(
            //                dyn_cast<Instruction>(i))][addmod(variableName)];
            // outFile << "\t" << addmod(variableName) << " = "
            //         << valueTuple[getStringFromInstruction(
            //  dyn_cast<Instruction>(i))][addmod(variableName)];
          }
        }
      }
    }
    printMap(varType);
    // iteratePointsTo(pointsTo);

    map<Instruction *, vector<pair<string, string>>> IN;
    map<Instruction *, vector<pair<string, string>>> OUT;
    Instruction *lastinst;

    for (Function::iterator bb = F.begin(), e = F.end(); bb != e; bb++) {
      for (BasicBlock::iterator i = bb->begin(), i2 = bb->end(); i != i2; i++) {
        
        if (flag) {
          worklist.push(dyn_cast<Instruction>(i));

          vector<pair<string, string>> startin, startout;
          for (auto it = varType.begin(); it != varType.end(); ++it) {
            // pfel("Key: " << it->first << ", Value: " << it->second);
            if(it->first==it->second)
            startin.push_back(make_pair(it->first, it->first));
            else
            startin.push_back(make_pair(it->first, "?"));
          }
          IN[dyn_cast<Instruction>(i)] = startin;
          OUT[dyn_cast<Instruction>(i)] = startout;
          flag = 0;
        }
        if (dyn_cast<Instruction>(i) == bb->getTerminator()) {
          lastinst=dyn_cast<Instruction>(i);
          pf(*i<< "last node of block successors are:\n");
          successor[dyn_cast<Instruction>(i)] =
              findSuccessors(dyn_cast<Instruction>(i));
          iterateVector(successor[dyn_cast<Instruction>(i)]);
          pf("---\n");

        } else {
          // consecutive instruction in the basic block have next instruction
          // as successor
          std::vector<Instruction *> v;
          v.push_back(dyn_cast<Instruction>(i->getNextNode()));
          successor[dyn_cast<Instruction>(i)] = v;
          // pfel(*i);
          // iterateVector(successor[dyn_cast<Instruction>(i)]);
          
        }
      }
    }
    // Inset & outset for each instruction
    std::map<Instruction*, bool> inQ;
    int lol=100;

    while (!worklist.empty()) {
      // if(!lol--)break;
      Instruction *currInst = worklist.front();
      worklist.pop();
      inQ[currInst] = false ;//we are processing it not in the queue anymore
      vector<pair<string, string>> oldIn, oldOut;
      if (IN.find(currInst) == IN.end()) {
        pfel("error: old in value not found");
      } else {
        oldIn = IN[currInst];
      }
      if (OUT.find(currInst) == OUT.end()) {
        // OUT[currInst] = NULL;
        pfel("error: old out value not found");
      } else {
        oldOut = OUT[currInst];
      }

      // pfel("old points too");
      // iteratePointsTo(pointsTo);

      // transfer function
      // effects of transfer functions are same?
      vector<pair<string, string>> newpairs =
          transferFunc(currInst, IN[currInst], pointsTo, varType);
      //    map<Instruction *, map<string, vector<string>>> pointsTo;
      // pointsTo[currInst]=newpairs;

      

      OUT[currInst]=newpairs;

      // pfel("new pairs");
      // printPairs(newpairs);

      vector<Instruction *> curr_successors = successor[currInst];

      // if (ValueChanged(IN[currInst], oldIn) ||
      //     ValueChanged(OUT[currInst], oldOut)) {
      
      if (hasDifference(IN[currInst], oldIn) ||
          hasDifference(OUT[currInst], oldOut)) {
            // hasDifference(a, b)
        pfel("pairs are changed for"<<*currInst);
        printPairs(oldIn);
        printPairs(IN[currInst]);
        
        printPairs(oldOut);
        printPairs(OUT[currInst]);
        

        // printPairs(newpairs);
        // Changed = true;
        for (llvm::Instruction *inst : curr_successors) {
          // std::cout << inst << std::endl;
          
          // union meet operation on the successors in set
          IN[inst] = Uni(IN[inst], OUT[currInst]);
          // pfel("out of union is ");
          // printPairs(IN[inst]);
          // worklist.push(inst);

          if (!inQ[inst]) {
            pfel("successors added to queue");
            pf(*inst<<"-"); 
            worklist.push(inst);
            inQ[inst] = true;
      }
        }

      } else {
        pfel("NOT Changed");
      }

      // pf("OLD values of successor");
      // printPairs(oldOut);

      // pf("\nNEW values of successor");
      // printPairs(OUT[currInst]);

      // valueTuple[curr_successors[i]] = newSuccessorMap;

      
    }
    pfel("last instruction is "<<lastinst<<" "<<*lastinst);
    // printMap2(pointsTo[lastinst]);
    printPairs(OUT[lastinst]);
    // iteratePointsTo(pointsTo);
    pfel("Alias is");
    std::map<std::string, std::vector<std::string>> final;
    final=getAlias(OUT[lastinst]);
    if(final.empty())
    {
      for(auto i:varType)
      {
        if(i.second.back()=='*')
        {
          errs()<<i.first<<" -> {}\n";
          }
      }
    }
    printAlias(final);
  }

  void temp(Function &F) {

    map<string, int> storeMap;
    map<string, vector<string>> loads; // loads
    // map<string, vector<string>> pointsTO;

    // each instructinos
    map<Instruction *, map<string, vector<string>>> pointsTo;
    map<string, string> varType;
    map<string, vector<string>> pt;

    for (BasicBlock &BB : F) {
      for (Instruction &I : BB) {

        if (AllocaInst *Ai = dyn_cast<AllocaInst>(&I)) {
          // if (Ai->getPointe)
          // iterateME(Ai);
          // pfel("shoot"
          //      << ":" << I);
          string result = getresultOperand(Ai);
          string optype = getTypeString(Ai->getAllocatedType());
          pfel("alloca result " << result << " |" << optype);
          // pushhing %p into map for storing its value
          if (varType.find(result) == varType.end())
            varType[result] = optype;
          else {
            pfel("errror: same variable name found overwriting original");
            varType[result] = optype;
          }
          if (Ai->getAllocatedType()->isPointerTy()) {
            //
          }

          // getresultOperand();
        } else if (StoreInst *Si = dyn_cast<StoreInst>(&I)) {
          // iterateME(Si);
          if (Si->getValueOperand()->getType()->isPointerTy()) {
            // iterateME(Si);
            pfel(*Si);
            string op1 = getVariableNamefromoperand(Si->getPointerOperand());
            // Si->getPointerOperandType()->print(errs());
            // pfel(Si->getPointerOperandType()->print(errs()));
            string op2 = getVariableNamefromoperand(Si->getValueOperand());
            string ty1, ty2;
            ty1 = getTypeString(Si->getPointerOperand()->getType());
            ty2 = getTypeString(Si->getValueOperand()->getType());
            pfel("op1 " << op1 << "type of op1 " << ty1);
            pfel("op2 " << op2 << "type of op2 " << ty2);

            string getfromvarType1, getfromvarType2;
            if (varType.find(op1) != varType.end() &&
                varType.find(op2) != varType.end()) {

              getfromvarType1 = varType[op1];
              getfromvarType2 = varType[op2];
              pfel("fromm vartype" << getfromvarType1 << " "
                                   << getfromvarType2);

              // store i32* %a, i32** %p, align 8 || p=&a p-op1 a-op2
              if (typediff(ty1, getfromvarType1) == "*" &&
                  typediff(ty2, getfromvarType2) == "*") {
                // map<Instruction *, map<string, vector<string>>> pointsTo;

                // already variables is present we have to add to its list
                if (pt.find(op1) != pt.end()) {
                  pt[op1].push_back(op2);
                } else {
                  vector<string> pointeeInfo;
                  pointeeInfo.push_back(op2);
                  pt[op1] = pointeeInfo;
                }

                pointsTo[dyn_cast<Instruction>(&I)] = pt;

                // pfel(op1 << "->");
                // iterateVector(pointeeInfo);
              }

            } else {
              pfel("error: Type not found in the varType");
            }

            // op2->op1;
          }

          // errs() << " " << I << " ";

          // unsigned numOperands = Si->getNumOperands();
          // pf("\\\\");
          // pfop(Si->getPointerOperand());
          // pfop(Si->getValueOperand());
          // string op1, op2;
          // op1 = getVariableNamefromoperand(Si->getPointerOperand());
          // op2 = getVariableNamefromoperand(Si->getValueOperand());
          // pf("op1" << op1);
          // pf("op2" << op2);
          // pf("///");
          // // storeMap[op1] = op2;

          // std::string variableName =
          //     getVariableNamefromoperand(Si->getPointerOperand());
          // // Si->getPointerOperand()->getName().str(); // 1getOperand(1)
          // Value *storedValue = Si->getValueOperand(); // 0

          // pfel("1st operand/variable name :");
          // pf("-->");
          // // variableName = addmod(variableName);

          // // pf(variableName << " = ");
          // string destinationOpernad =
          // getVariableNamefromoperand(storedValue);
          // pfel(destinationOpernad);
          // // val is int assign it to variable
          // if (isInteger(destinationOpernad)) {
          //   storeMap[variableName] = stringToInt(destinationOpernad);

          // } else {
          //   if (storeMap.find(destinationOpernad) != storeMap.end())
          //     storeMap[variableName] = storeMap[destinationOpernad];
          //   // newMap[variableName] =
          //   //     meetValue(curMap[variableName], destinationOpernad);
          //   // newMap[variableName] =
          //   // curMap[getVariableNamefromoperand(storedValue)];
          // }
          // pf("PPPPPPPP" << storeMap[variableName]);
        } else if (LoadInst *Li = dyn_cast<LoadInst>(&I)) {
          if (Li->getPointerOperandType()->isPointerTy()) {
            pfel("In pointer load" << *Li);
          }
        }
      }
    }

    printMap(varType);
    iteratePointsTo(pointsTo);
  }

  bool runOnFunction(Function &F) override {
    // errs() << "ALIASSSSSSSSSSSSHello: ";
    // errs().write_escaped(F.getName()) << '\n';

    // int linenumber = 0;

    // std::map<std::string, std::map<std::string, std::string>> valueTuple;

    // std::map<std::string, std::vector<std::string>> successor;
    // std::map<std::string, Instruction *> getInstructionFromString;
    // temp(F);
      
    kildalls(F);
  

    // for (Function::iterator bb = F.begin(), e = F.end(); bb != e; bb++) {
    //   for (BasicBlock::iterator i = bb->begin(), i2 = bb->end(); i != i2;
    //   i++) {

    //   }
    // }
    // write your code here
    return false;
  }
}; // end of struct alias_c
} // end of anonymous namespace

char alias_c::ID = 0;
static RegisterPass<alias_c> X("alias_lib_given",
                               "Alias Analysis Pass for Assignment",
                               false /* Only looks at CFG */,
                               false /* Analysis Pass */);
