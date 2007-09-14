//==- CFGVarDeclVisitor - Generic visitor of VarDecls in a CFG --*- C++ --*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file was developed by Ted Kremenek and is distributed under
// the University of Illinois Open Source License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the template class CFGVarDeclVisitor, which provides
// a generic way to visit all the VarDecl's in a CFG.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_ANALYSIS_CFG_VARDECL_VISITOR_H
#define LLVM_CLANG_ANALYSIS_CFG_VARDECL_VISITOR_H

#include "clang/AST/StmtVisitor.h"
#include "clang/AST/Decl.h"
#include "clang/AST/Stmt.h"
#include "clang/AST/CFG.h"

namespace clang {

template <typename ImplClass>
class CFGVarDeclVisitor : public StmtVisitor<ImplClass> {
  const CFG& cfg;
public:  
  CFGVarDeclVisitor(const CFG& c) : cfg(c) {}
  
  void VisitStmt(Stmt* S) {
    for (Stmt::child_iterator I=S->child_begin(), E=S->child_end(); I!=E; ++I)
      static_cast<ImplClass*>(this)->Visit(*I);
  }
  
  void VisitDeclRefExpr(DeclRefExpr* DR) {
    static_cast<ImplClass*>(this)->VisitDeclChain(DR->getDecl());
  }
  
  void VisitDeclStmt(DeclStmt* DS) {
    static_cast<ImplClass*>(this)->VisitDeclChain(DS->getDecl());
  }
  
  void VisitDeclChain(ScopedDecl* D) {  
    for (; D != NULL ; D = D->getNextDeclarator())
      static_cast<ImplClass*>(this)->VisitScopedDecl(D);
  }
  
  void VisitScopedDecl(ScopedDecl* D) {
    if (VarDecl* V = dyn_cast<VarDecl>(D))
      static_cast<ImplClass*>(this)->VisitVarDecl(V);
  }
  
  void VisitVarDecl(VarDecl* D) {}
  
  void VisitAllDecls() {
    for (CFG::const_iterator BI = cfg.begin(), BE = cfg.end(); BI != BE; ++BI)
      for (CFGBlock::const_iterator SI=BI->begin(),SE = BI->end();SI != SE;++SI)
        static_cast<ImplClass*>(this)->Visit(const_cast<Stmt*>(*SI));    
  }
};

} // end namespace clang

#endif
