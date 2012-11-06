ASTGEN
======

Simple generator for C++ ASTs, requires henrik-muehe/greg-cpp fork of greg to compile. 
Generates AST nodes containing basic types, other AST nodes or collections of other
AST nodes.

    Id(id:string)
    Type(id:Id,collection:bool)
    Attribute(name:Id,type:Type)
    Node(name:Id,attributes:[Attribute])
    Nodes(nodes:[Node])

Defines all AST nodes required for ASTGEN itself. Ideally used with my greg fork as I
will soon illustrate somewhere in greg's README. 