#pragma once

#include "../DataNode.h"


//Handles an identical shader input across different kinds of shaders.
class ShaderInNode : public DataNode
{
public:

    bool HasVertexInput(void) const { return vInputIndex >= 0; }
    bool HasGeometryInput(void) const { return gInputIndex >= 0; }
    bool HasFragmentInput(void) const { return fInputIndex >= 0; }


    virtual unsigned int GetOutputSize(unsigned int index) const override;
    virtual std::string GetOutputName(unsigned int outputIndex) const override;


    ShaderInNode(unsigned int size, std::string name = "", int vertexInputIndex = -1, int fragmentInputIndex = -1,
                 int geometryInputIndex = -1, unsigned int geometryInputArrayIndex = 0);

protected:

    virtual void WriteMyOutputs(std::string & outCode) const;

    virtual void WriteExtraData(DataWriter* writer) const override;
    virtual void ReadExtraData(DataReader* reader) override;


private:

    int vInputIndex, gInputIndex, fInputIndex;
    unsigned int gInputArrayIndex;

    unsigned int outSize;


    ADD_NODE_REFLECTION_DATA_H(ShaderInNode)
};