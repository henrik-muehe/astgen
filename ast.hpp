struct Ast { virtual void can_dynamic_cast() {} };
std::ostream& operator<< (std::ostream& out,const Ast& node) { out << "(Ast)"; }
using std::string;

struct Id : public Ast {
  string id;

  Id(const string& id) {
    this->id=id;
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
    }
  }
};

std::ostream& operator<< (std::ostream& out,const Node& node) {
  out << "(Node: ";
  out << *node.name;
  out << "[";
  for (auto& item : node.attributes) {
  out << *item;  }
  out << "]";
  out << ")";
}


