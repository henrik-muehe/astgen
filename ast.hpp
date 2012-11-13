struct Visitor; struct Ast { int64_t line; int64_t col; virtual void can_dynamic_cast() {} virtual void accept(const std::string&,Visitor&)=0; };
std::ostream& operator<< (std::ostream& out,const Ast& node) { out << "(Ast)"; }
using std::string;

struct Collection : Ast {
  void accept(const string&, Visitor&) {};
  std::vector<std::unique_ptr<Ast>> items; 
  void push_back(std::unique_ptr<Ast>&& item) { items.push_back(std::move(item)); } 
  std::vector<std::unique_ptr<Ast>>& get() { return items; }
};

template<class T,class S>
T tryCast(S s) {
  if (!s) return 0;
  T t=dynamic_cast<T>(s);
  if (!t) {
    std::cerr << "AST type mismatch." << std::endl;
    throw;
  }
  return t;
}

// Forward declarations
struct Id;
struct Type;
struct Attribute;
struct Node;
struct Nodes;

// Visitor base class
struct Visitor {
  virtual void visitPre(const std::string& name,const Ast&) {}
  virtual void visitPost(const std::string& name,const Ast&) {}
  virtual void visitPre(const std::string& name,const Collection&) {}
  virtual void visitPost(const std::string& name,const Collection&) {}
  virtual void visit(const std::string& name,const int64_t&) {}
  virtual void visit(const std::string& name,const std::string&) {}
  virtual void collectionPre() {}
  virtual void collectionPost() {}
  virtual void visitPre(const std::string& name,const Id&) {}
  virtual void visitPost(const std::string& name,const Id&) {}
  virtual void visitPre(const std::string& name,const Type&) {}
  virtual void visitPost(const std::string& name,const Type&) {}
  virtual void visitPre(const std::string& name,const Attribute&) {}
  virtual void visitPost(const std::string& name,const Attribute&) {}
  virtual void visitPre(const std::string& name,const Node&) {}
  virtual void visitPost(const std::string& name,const Node&) {}
  virtual void visitPre(const std::string& name,const Nodes&) {}
  virtual void visitPost(const std::string& name,const Nodes&) {}
};

struct Id : public Ast {
  string id;

  Id(const string& id) {
    this->id=id;
  }

  void accept(const std::string& name,Visitor& visitor) {
    visitor.visitPre(name,*this);
    visitor.visit("id",this->id);
    visitor.visitPost(name,*this);
  }
};

std::ostream& operator<< (std::ostream& out,const Id& node) {
  out << "(Id: ";
  out << node.id;
  out << ")";
}


struct Type : public Ast {
  std::unique_ptr<Id> id;
  bool collection;

  Type(std::unique_ptr<Ast>&& id,const bool& collection) {
    this->id=std::unique_ptr<Id>(tryCast<Id*>(id.get()));
    id.release();

    this->collection=collection;
  }

  void accept(const std::string& name,Visitor& visitor) {
    visitor.visitPre(name,*this);
    if (this->id.get()) this->id->accept("id",visitor);
    visitor.visit("collection",this->collection);
    visitor.visitPost(name,*this);
  }
};

std::ostream& operator<< (std::ostream& out,const Type& node) {
  out << "(Type: ";
  out << *node.id;
  out << node.collection;
  out << ")";
}


struct Attribute : public Ast {
  std::unique_ptr<Id> name;
  std::unique_ptr<Type> type;

  Attribute(std::unique_ptr<Ast>&& name,std::unique_ptr<Ast>&& type) {
    this->name=std::unique_ptr<Id>(tryCast<Id*>(name.get()));
    name.release();

    this->type=std::unique_ptr<Type>(tryCast<Type*>(type.get()));
    type.release();

  }

  void accept(const std::string& name,Visitor& visitor) {
    visitor.visitPre(name,*this);
    if (this->name.get()) this->name->accept("name",visitor);
    if (this->type.get()) this->type->accept("type",visitor);
    visitor.visitPost(name,*this);
  }
};

std::ostream& operator<< (std::ostream& out,const Attribute& node) {
  out << "(Attribute: ";
  out << *node.name;
  out << *node.type;
  out << ")";
}


struct Node : public Ast {
  std::unique_ptr<Id> name;
  std::vector<std::unique_ptr<Attribute>> attributes;

  Node(std::unique_ptr<Ast>&& name,std::unique_ptr<Ast>&& attributes) {
    this->name=std::unique_ptr<Id>(tryCast<Id*>(name.get()));
    name.release();

    if (attributes.get())
    for (auto& item : tryCast<Collection*>(attributes.get())->get()) {
      this->attributes.push_back(std::unique_ptr<Attribute>(tryCast<Attribute*>(item.get())));
      item.release();
    }
  }

  void accept(const std::string& name,Visitor& visitor) {
    visitor.visitPre(name,*this);
    if (this->name.get()) this->name->accept("name",visitor);
    visitor.collectionPre();
    for (auto& item : attributes) {
      if (item.get()) item->accept("attributes",visitor);
    }
    visitor.collectionPost();
    visitor.visitPost(name,*this);
  }
};

std::ostream& operator<< (std::ostream& out,const Node& node) {
  out << "(Node: ";
  out << *node.name;
  out << "[";
  for (auto& item : node.attributes) {
    out << *item;
  }
  out << "]";
  out << ")";
}


struct Nodes : public Ast {
  std::vector<std::unique_ptr<Node>> nodes;

  Nodes(std::unique_ptr<Ast>&& nodes) {
    if (nodes.get())
    for (auto& item : tryCast<Collection*>(nodes.get())->get()) {
      this->nodes.push_back(std::unique_ptr<Node>(tryCast<Node*>(item.get())));
      item.release();
    }
  }

  void accept(const std::string& name,Visitor& visitor) {
    visitor.visitPre(name,*this);
    visitor.collectionPre();
    for (auto& item : nodes) {
      if (item.get()) item->accept("nodes",visitor);
    }
    visitor.collectionPost();
    visitor.visitPost(name,*this);
  }
};

std::ostream& operator<< (std::ostream& out,const Nodes& node) {
  out << "(Nodes: ";
  out << "[";
  for (auto& item : node.nodes) {
    out << *item;
  }
  out << "]";
  out << ")";
}



struct PrettyPrintVisitor : public Visitor {
  std::stack<bool> indentScopes;
  uint64_t indentDepth;
  
  void pushScope(bool indent=false) { indentScopes.push(indent); indentDepth+=indent; }
  void popScope() { indentDepth-=indentScopes.top(); indentScopes.pop(); }
  
  void applyIndent(int64_t mod=0) { 
    if (indentScopes.top()) { 
      std::cerr << std::string((indentDepth+mod)*2,' '); 
    } 
  }
  
  void applyNl() { 
    if (indentScopes.top()) 
      std::cerr << std::endl;
  }
  
  PrettyPrintVisitor() : indentDepth(0) { pushScope(); }
  
  void collectionPre() { 
    applyIndent();
    std::cerr << "[" << std::endl; 
    pushScope(true);
  }
  
  void collectionPost() { 
    applyIndent(-1); 
    popScope();
    std::cerr << "]";// << std::endl; 
  }  
  
  
  virtual void visitPre(const std::string& name,const Id& n) { 
    applyIndent(); 
    std::cerr << "(" << "Id " << name << "="; 
    pushScope();
  }
  
  virtual void visitPost(const std::string& name,const Id& n) { 
    applyIndent();
    popScope();
    std::cerr << ")"; 
    applyNl(); 
  }  
  
  virtual void visitPre(const std::string& name,const Type& n) { 
    applyIndent(); 
    std::cerr << "(" << "Type " << name << "="; 
    pushScope();
  }
  
  virtual void visitPost(const std::string& name,const Type& n) { 
    applyIndent();
    popScope();
    std::cerr << ")"; 
    applyNl(); 
  }  
  
  virtual void visitPre(const std::string& name,const Attribute& n) { 
    applyIndent(); 
    std::cerr << "(" << "Attribute " << name << "="; 
    pushScope();
  }
  
  virtual void visitPost(const std::string& name,const Attribute& n) { 
    applyIndent();
    popScope();
    std::cerr << ")"; 
    applyNl(); 
  }  
  
  virtual void visitPre(const std::string& name,const Node& n) { 
    applyIndent(); 
    std::cerr << "(" << "Node " << name << "="; 
    pushScope();
  }
  
  virtual void visitPost(const std::string& name,const Node& n) { 
    applyIndent();
    popScope();
    std::cerr << ")"; 
    applyNl(); 
  }  
  
  virtual void visitPre(const std::string& name,const Nodes& n) { 
    applyIndent(); 
    std::cerr << "(" << "Nodes " << name << "="; 
    pushScope();
  }
  
  virtual void visitPost(const std::string& name,const Nodes& n) { 
    applyIndent();
    popScope();
    std::cerr << ")"; 
    applyNl(); 
  }  
  
  
  virtual void visit(const std::string& name,const int64_t& v) { std::cerr << "(" << name << "=" << "\"" << v << "\")"; }
  virtual void visit(const std::string& name,const std::string& v) { std::cerr << "(" << name << "=" << "\"" << v << "\")"; }
};


struct RubyAstVisitor : public Visitor {
	bool doComma;
   
   RubyAstVisitor() : doComma(false) {}
	
	std::string getDefinition() const {
		return R"(
			class Id < RenderStruct.new(:id); end
class Type < RenderStruct.new(:id,:collection); end
class Attribute < RenderStruct.new(:name,:type); end
class Node < RenderStruct.new(:name,:attributes); end
class Nodes < RenderStruct.new(:nodes); end

		)";
	}
	
	void tryComma() {
		if (doComma) {
			std::cerr << "," << std::endl;
			doComma=false;
		}
	}
	
  void collectionPre() { 
		tryComma();
    std::cerr << "[";
		doComma=false;
  }
  
  void collectionPost() { 
    std::cerr << "]";
		doComma=true;
  }  
  
  
  virtual void visitPre(const std::string& name,const Id& n) { 
		tryComma();
    std::cerr << "Id.new(";
  }
  
  virtual void visitPost(const std::string& name,const Id& n) { 
    std::cerr << ")";
		doComma=true;
  }  
  
  virtual void visitPre(const std::string& name,const Type& n) { 
		tryComma();
    std::cerr << "Type.new(";
  }
  
  virtual void visitPost(const std::string& name,const Type& n) { 
    std::cerr << ")";
		doComma=true;
  }  
  
  virtual void visitPre(const std::string& name,const Attribute& n) { 
		tryComma();
    std::cerr << "Attribute.new(";
  }
  
  virtual void visitPost(const std::string& name,const Attribute& n) { 
    std::cerr << ")";
		doComma=true;
  }  
  
  virtual void visitPre(const std::string& name,const Node& n) { 
		tryComma();
    std::cerr << "Node.new(";
  }
  
  virtual void visitPost(const std::string& name,const Node& n) { 
    std::cerr << ")";
		doComma=true;
  }  
  
  virtual void visitPre(const std::string& name,const Nodes& n) { 
		tryComma();
    std::cerr << "Nodes.new(";
  }
  
  virtual void visitPost(const std::string& name,const Nodes& n) { 
    std::cerr << ")";
		doComma=true;
  }  
  
  
  virtual void visit(const std::string& name,const int64_t& v) { tryComma(); std::cerr << v; }
  virtual void visit(const std::string& name,const std::string& v) { tryComma(); std::cerr << "\"" << v << "\""; }
};

