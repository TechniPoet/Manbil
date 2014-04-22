#pragma once

#include "../DataNode.h"


//Splits a vector into its components.
class VectorComponentsNode : public DataNode
{
public:

    virtual std::string GetName(void) const { return "vectorComponentsNode"; }

    const DataLine & GetVectorInput(void) const { return GetInputs()[0]; }


    VectorComponentsNode(const DataLine & inData) : DataNode(MakeVector(inData), buildOutputs(inData)) { }


    virtual std::string GetOutputName(unsigned int outputIndex) const override;


protected:

    virtual void WriteMyOutputs(std::string & outCode) const override
    {
        //No need to write anything; we're just pulling out components of another vector.
    }

private:

    //Generates the outputs for this node given the input.
    static std::vector<unsigned int> buildOutputs(const DataLine & inData);
};