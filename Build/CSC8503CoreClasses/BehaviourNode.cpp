#include "H:\CSC8503\Project\Build\CSC8503CoreClasses\CMakeFiles\CSC8503CoreClasses.dir\Debug\cmake_pch.hxx"
#include "BehaviourNode.h"

BehaviourNode::BehaviourNode(const std::string& nodeName) {
	currentState = Initialise;
	name = nodeName;
}

BehaviourNode::~BehaviourNode() {}

void BehaviourNode::Reset() { 
	currentState = Initialise; 
}