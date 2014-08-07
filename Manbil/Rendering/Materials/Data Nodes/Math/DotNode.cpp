#include "DotNode.h"


MAKE_NODE_READABLE_CPP(DotNode, Vector3f(), Vector3f())


unsigned int DotNode::GetOutputSize(unsigned int index) const
{
    return GetInputs()[0].GetSize();
}
std::string DotNode::GetOutputName(unsigned int index) const
{
    return GetName() + "_dotted";
}

DotNode::DotNode(const DataLine & first, const DataLine & second, std::string name)
    : DataNode(MakeVector(first, second), name)
{

}

void DotNode::WriteMyOutputs(std::string & outCode) const
{
    std::string vecType = VectorF(GetInputs()[0].GetSize()).GetGLSLType();

    outCode += "\t" + vecType + " " + GetOutputName(0) +
                " = dot(" + GetInputs()[0].GetValue() + ", " +
                GetInputs()[1].GetValue() + ");\n";
}

void DotNode::AssertMyInputsValid(void) const
{
    Assert(GetInputs()[0].GetSize() == GetInputs()[1].GetSize(), "The two inputs must be the same size!");
}