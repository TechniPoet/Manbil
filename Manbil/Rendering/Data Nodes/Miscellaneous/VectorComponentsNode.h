#pragma once

#include "../DataNode.h"


//Splits a vector into its components.
//The outputs are the 0-based indices of the input vector (0 = x, 1 = y, etc.).
class VectorComponentsNode : public DataNode
{
public:

    virtual unsigned int GetNumbOutputs(void) const override { return GetInputs()[0].GetSize(); }

    virtual unsigned int GetOutputSize(unsigned int index) const override;
    virtual std::string GetOutputName(unsigned int index) const override;


    VectorComponentsNode(const DataLine & inData, std::string name = "");


protected:

    virtual void WriteMyOutputs(std::string & outCode) const override;

    virtual std::string GetInputDescription(unsigned int index) const override;

    ADD_NODE_REFLECTION_DATA_H(VectorComponentsNode)
};