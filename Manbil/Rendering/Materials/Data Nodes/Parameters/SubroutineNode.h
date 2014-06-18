#pragma once

#include "../DataNode.h"


//Takes in an arbitrary number of DataLines representing different subroutine functions.
class SubroutineNode : public DataNode
{
public:

    //A parallel vector to "SubroutineDeclaration.PossibleValues". Provides the full definition of each subroutine option.
    std::vector<std::string> SubroutineImplementations;
    UniformSubroutineValue SubroutineDeclaration;


    SubroutineNode(std::vector<std::string> subroutineImplementations,
                   std::vector<DataLine> subroutineInputs,
                   UniformSubroutineValue subroutineDeclaration = UniformSubroutineValue())
                   : DataNode(subroutineInputs, MakeVector(subroutineDeclaration.ReturnValueSize)),
                     SubroutineDeclaration(subroutineDeclaration)
    {
        for (unsigned int inputIndex = 0; inputIndex < subroutineInputs.size(); ++inputIndex)
        {
            Assert(subroutineInputs[inputIndex].GetDataLineSize() == subroutineDeclaration.Parameters[inputIndex].ParamSize,
                   "Parameter index " + std::to_string(inputIndex) + " should be size " + std::to_string(subroutineDeclaration.Parameters[inputIndex].ParamSize) +
                   ", but is size " + std::to_string(subroutineInputs[inputIndex].GetDataLineSize()));
        }
    }


    virtual std::string GetName(void) const override { return "subroutineNode"; }
    virtual std::string GetOutputName(unsigned int index) const override
    {
        Assert(index == 0, "Output index must be 0 but it was " + std::to_string(index));
        return GetName() + std::to_string(GetUniqueID()) + "_returnValue";
    }


protected:

    virtual void GetMyParameterDeclarations(UniformDictionary & outUniforms) const override
    {
        outUniforms.SubroutineUniforms[SubroutineDeclaration.UniformName] = SubroutineDeclaration;
    }
    virtual void GetMyFunctionDeclarations(std::vector<std::string> & outDecls) const override
    {
        outDecls.insert(outDecls.end(), SubroutineImplementations.begin(), SubroutineImplementations.end());
    }
    virtual void WriteMyOutputs(std::string & outCode) const override
    {
        outCode += "\t" + VectorF(GetOutputs()[0]).GetGLSLType() + " " + GetOutputName(0) + " = " + SubroutineDeclaration.UniformName + "(";
        for (unsigned int i = 0; i < GetInputs().size(); ++i)
        {
            if (i > 0) outCode += ", ";
            outCode += GetInputs()[i].GetValue();
        }
        outCode += ");\n";
    }
};