struct Visitor; struct Ast { virtual void can_dynamic_cast() {} virtual void accept(const std::string&,Visitor&)=0; };
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
    this->id->accept("id",visitor);
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
    this->name->accept("name",visitor);
    this->type->accept("type",visitor);
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
    this->name->accept("name",visitor);
    visitor.collectionPre();
    for (auto& item : attributes) {
      item->accept("attributes",visitor);
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
      item->accept("nodes",visitor);
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


// Pretty Print Visitor
struct PrettyPrintVisitor : public Visitor {
  std::stack<bool> doIndent;
  void applyIndent(int64_t mod=0) { if (doIndent.top()) { std::cerr << std::string((doIndent.size()+mod)*2,' '); } }
  void applyNl() { if (doIndent.top()) { std::cerr << std::endl; } }
  PrettyPrintVisitor() { doIndent.push(false); }
  void collectionPre() { applyIndent(); std::cerr << "[" << std::endl; doIndent.push(true); }
  void collectionPost() { applyIndent(-1); doIndent.pop(); std::cerr << "]" << std::endl; }
  virtual void visitPre(const std::string& name,const Id& n) { applyIndent(); std::cerr << "(" << name << ":Id "; doIndent.push(false); }
  virtual void visitPost(const std::string& name,const Id& n) { doIndent.pop(); applyIndent(); std::cerr << ")"; applyNl(); }
  virtual void visitPre(const std::string& name,const Type& n) { applyIndent(); std::cerr << "(" << name << ":Type "; doIndent.push(false); }
  virtual void visitPost(const std::string& name,const Type& n) { doIndent.pop(); applyIndent(); std::cerr << ")"; applyNl(); }
  virtual void visitPre(const std::string& name,const Attribute& n) { applyIndent(); std::cerr << "(" << name << ":Attribute "; doIndent.push(false); }
  virtual void visitPost(const std::string& name,const Attribute& n) { doIndent.pop(); applyIndent(); std::cerr << ")"; applyNl(); }
  virtual void visitPre(const std::string& name,const Node& n) { applyIndent(); std::cerr << "(" << name << ":Node "; doIndent.push(false); }
  virtual void visitPost(const std::string& name,const Node& n) { doIndent.pop(); applyIndent(); std::cerr << ")"; applyNl(); }
  virtual void visitPre(const std::string& name,const Nodes& n) { applyIndent(); std::cerr << "(" << name << ":Nodes "; doIndent.push(false); }
  virtual void visitPost(const std::string& name,const Nodes& n) { doIndent.pop(); applyIndent(); std::cerr << ")"; applyNl(); }
};

