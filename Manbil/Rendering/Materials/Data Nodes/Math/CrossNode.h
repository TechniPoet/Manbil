#pragma once

#include "../DataNode.h"


//A node that outputs the cross product of two vec3's.
class CrossNode : public DataNode
{
public:

    virtual std::string GetName(void) const override { return "crossProductNode"; }
    
    CrossNode(const DataLine & firstL, const DataLine & secondL)
        : DataNode(MakeVector(firstL, secondL), MakeVector(3))
    {

    }

protected:

    virtual void WriteMyOutputs(std::string & outCode, Shaders shaderType) const override
    {
        outCode += "\tvec3 " + GetOutputName(0, shaderType) + " = cross(" + GetInputs()[0].GetValue(shaderType) + ", " + GetInputs()[1].GetValue(shaderType) + ");\n";
    }
};