#include "souffle/CompiledSouffle.h"

namespace souffle {
using namespace ram;
class Sf_scalaEx : public SouffleProgram {
private:
static bool regex_wrapper(const char *pattern, const char *text) {
   bool result = false; 
   try { result = std::regex_match(text, std::regex(pattern)); } catch(...) { 
     std::cerr << "warning: wrong pattern provided for match(\"" << pattern << "\",\"" << text << "\")\n";
}
   return result;
}
public:
SymbolTable symTable;
// -- Table: edge
ram::Relation<2,ram::index<0>> rel_edge;
souffle::RelationWrapper<0,ram::Relation<2,ram::index<0>>,Tuple<RamDomain,2>,2,true,false> wrapper_edge;
// -- Table: path
ram::Relation<2,ram::index<0,1>> rel_path;
souffle::RelationWrapper<1,ram::Relation<2,ram::index<0,1>>,Tuple<RamDomain,2>,2,false,true> wrapper_path;
// -- Table: _temp1_path
ram::Relation<2> rel__temp1_path;
// -- Table: _temp2_path
ram::Relation<2> rel__temp2_path;
public:
Sf_scalaEx() : 
wrapper_edge(rel_edge,symTable,"edge",std::array<const char *,2>{"s:Node","s:Node"},std::array<const char *,2>{"node1","node2"}),
wrapper_path(rel_path,symTable,"path",std::array<const char *,2>{"s:Node","s:Node"},std::array<const char *,2>{"node1","node2"}){
addRelation("edge",&wrapper_edge,1,0);
addRelation("path",&wrapper_path,0,1);
}
void run() {
// -- initialize counter --
std::atomic<RamDomain> ctr(0);

#if defined(__EMBEDDED_SOUFFLE__) && defined(_OPENMP)
omp_set_num_threads(1);
#endif

// -- query evaluation --
if (!rel_edge.empty()) [&](){
auto part = rel_edge.partition();
PARALLEL_START;
CREATE_OP_CONTEXT(rel_edge_op_ctxt,rel_edge.createContext());
CREATE_OP_CONTEXT(rel_path_op_ctxt,rel_path.createContext());
pfor(auto it = part.begin(); it<part.end(); ++it) 
for(const auto& env0 : *it) {
Tuple<RamDomain,2> tuple({env0[0],env0[1]});
rel_path.insert(tuple,READ_OP_CONTEXT(rel_path_op_ctxt));
}
PARALLEL_END;
}
();rel__temp1_path.insertAll(rel_path);
for(;;) {
if (!rel__temp1_path.empty()&&!rel_edge.empty()) [&](){
auto part = rel__temp1_path.partition();
PARALLEL_START;
CREATE_OP_CONTEXT(rel__temp1_path_op_ctxt,rel__temp1_path.createContext());
CREATE_OP_CONTEXT(rel__temp2_path_op_ctxt,rel__temp2_path.createContext());
CREATE_OP_CONTEXT(rel_edge_op_ctxt,rel_edge.createContext());
CREATE_OP_CONTEXT(rel_path_op_ctxt,rel_path.createContext());
pfor(auto it = part.begin(); it<part.end(); ++it) 
for(const auto& env0 : *it) {
const Tuple<RamDomain,2> key({env0[1],0});
auto range = rel_edge.equalRange<0>(key,READ_OP_CONTEXT(rel_edge_op_ctxt));
for(const auto& env1 : range) {
if( !rel_path.contains(Tuple<RamDomain,2>({env0[0],env1[1]}),READ_OP_CONTEXT(rel_path_op_ctxt))) {
Tuple<RamDomain,2> tuple({env0[0],env1[1]});
rel__temp2_path.insert(tuple,READ_OP_CONTEXT(rel__temp2_path_op_ctxt));
}
}
}
PARALLEL_END;
}
();if(rel__temp2_path.empty()) break;
rel_path.insertAll(rel__temp2_path);
rel__temp1_path.purge();
if (!rel__temp2_path.empty()&&!rel_edge.empty()) [&](){
auto part = rel__temp2_path.partition();
PARALLEL_START;
CREATE_OP_CONTEXT(rel__temp1_path_op_ctxt,rel__temp1_path.createContext());
CREATE_OP_CONTEXT(rel__temp2_path_op_ctxt,rel__temp2_path.createContext());
CREATE_OP_CONTEXT(rel_edge_op_ctxt,rel_edge.createContext());
CREATE_OP_CONTEXT(rel_path_op_ctxt,rel_path.createContext());
pfor(auto it = part.begin(); it<part.end(); ++it) 
for(const auto& env0 : *it) {
const Tuple<RamDomain,2> key({env0[1],0});
auto range = rel_edge.equalRange<0>(key,READ_OP_CONTEXT(rel_edge_op_ctxt));
for(const auto& env1 : range) {
if( !rel_path.contains(Tuple<RamDomain,2>({env0[0],env1[1]}),READ_OP_CONTEXT(rel_path_op_ctxt))) {
Tuple<RamDomain,2> tuple({env0[0],env1[1]});
rel__temp1_path.insert(tuple,READ_OP_CONTEXT(rel__temp1_path_op_ctxt));
}
}
}
PARALLEL_END;
}
();if(rel__temp1_path.empty()) break;
rel_path.insertAll(rel__temp1_path);
rel__temp2_path.purge();
}
rel__temp1_path.purge();
rel__temp2_path.purge();
rel_edge.purge();
}
public:
void printAll(std::string dirname="./") {
rel_path.printCSV(dirname + "/path.csv",symTable,1,1);
}
public:
void loadAll(std::string dirname="./") {
rel_edge.loadCSV(dirname + "/edge.facts",symTable,1,1);
}
public:
void dumpInputs(std::ostream& out = std::cout) {
out << "---------------\nedge\n===============\n";
rel_edge.printCSV(out,symTable,1,1);
out << "===============\n";
}
public:
void dumpOutputs(std::ostream& out = std::cout) {
out << "---------------\npath\n===============\n";
rel_path.printCSV(out,symTable,1,1);
out << "===============\n";
}
public:
const SymbolTable &getSymbolTable() const {
return symTable;
}
};
SouffleProgram *newInstance_scalaEx(){return new Sf_scalaEx;}
SymbolTable *getST_scalaEx(SouffleProgram *p){return &reinterpret_cast<Sf_scalaEx*>(p)->symTable;}
#ifdef __EMBEDDED_SOUFFLE__
class factory_Sf_scalaEx: public souffle::ProgramFactory {
SouffleProgram *newInstance() {
return new Sf_scalaEx();
};
public:
factory_Sf_scalaEx() : ProgramFactory("scalaEx"){}
};
static factory_Sf_scalaEx __factory_Sf_scalaEx_instance;
}
#else
}
int main(int argc, char** argv)
{
souffle::CmdOptions opt(R"(-unknown-)",
R"(./)",
R"(./)",
false,
R"()",
1,
R"(false)");
if (!opt.parse(argc,argv)) return 1;
#if defined(_OPENMP) 
omp_set_nested(true);
#endif
souffle::Sf_scalaEx obj;
obj.loadAll(opt.getInputFileDir());
obj.run();
if (!opt.getOutputFileDir().empty()) obj.printAll(opt.getOutputFileDir());
return 0;
}
#endif
