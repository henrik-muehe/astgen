struct Visitor; struct Ast { virtual void can_dynamic_cast() {} virtual void accept(Visitor&)=0; };
std::ostream& operator<< (std::ostream& out,const Ast& node) { out << "(Ast)"; }
using std::string;

// Forward declarations
struct Id;
struct Type;
struct Attribute;
struct Node;
struct Nodes;

// Visitor base class
struct Visitor {
  virtual void visitPre(const Ast&) {}
  virtual void visitPost(const Ast&) {}
  virtual void visitPre(const Id&) {}
  virtual void visitPost(const Id&) {}
  virtual void visitPre(const Type&) {}
  virtual void visitPost(const Type&) {}
  virtual void visitPre(const Attribute&) {}
  virtual void visitPost(const Attribute&) {}
  virtual void visitPre(const Node&) {}
  virtual void visitPost(const Node&) {}
  virtual void visitPre(const Nodes&) {}
  virtual void visitPost(const Nodes&) {}
};

struct Id : public Ast {
  string id;

  Id(const string& id) {
    this->id=id;
  }

  void accept(Visitor& visitor) {
    visitor.visitPre(*this);
    visitor.visitPost(*this);
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
    this->id=std::unique_ptr<Id>(dynamic_cast<Id*>(id.get()));
    if (!this->id) { std::cerr << "AST type failure for Type attribute id" << std::endl; throw; }
    id.release();

    this->collection=collection;
  }

  void accept(Visitor& visitor) {
    visitor.visitPre(*this);
    this->id->accept(visitor);
    visitor.visitPost(*this);
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
    this->name=std::unique_ptr<Id>(dynamic_cast<Id*>(name.get()));
    if (!this->name) { std::cerr << "AST type failure for Attribute attribute name" << std::endl; throw; }
    name.release();

    this->type=std::unique_ptr<Type>(dynamic_cast<Type*>(type.get()));
    if (!this->type) { std::cerr << "AST type failure for Attribute attribute type" << std::endl; throw; }
    type.release();

  }

  void accept(Visitor& visitor) {
    visitor.visitPre(*this);
    this->name->accept(visitor);
    this->type->accept(visitor);
    visitor.visitPost(*this);
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

  Node(std::unique_ptr<Ast>&& name,std::vector<std::unique_ptr<Ast>>&& attributes) {
    this->name=std::unique_ptr<Id>(dynamic_cast<Id*>(name.get()));
    if (!this->name) { std::cerr << "AST type failure for Node attribute name" << std::endl; throw; }
    name.release();

    for (auto& item : attributes) {
      this->attributes.push_back(std::unique_ptr<Attribute>(dynamic_cast<Attribute*>(item.get())));
      item.release();
    }
  }

  void accept(Visitor& visitor) {
    visitor.visitPre(*this);
    this->name->accept(visitor);
    for (auto& item : attributes) {
      item->accept(visitor);
    }
    visitor.visitPost(*this);
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

  Nodes(std::vector<std::unique_ptr<Ast>>&& nodes) {
    for (auto& item : nodes) {
      this->nodes.push_back(std::unique_ptr<Node>(dynamic_cast<Node*>(item.get())));
      item.release();
    }
  }

  void accept(Visitor& visitor) {
    visitor.visitPre(*this);
    for (auto& item : nodes) {
      item->accept(visitor);
    }
    visitor.visitPost(*this);
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


