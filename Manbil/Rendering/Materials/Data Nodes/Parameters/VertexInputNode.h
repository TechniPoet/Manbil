#pragma once

#include "../DataNode.h"
#include "../../MaterialData.h"
#include "../../../../Vertices.h"



//Represents the value of vertex inputs.
//This node is a singleton.
//Each output index corresponds to that vertex input.
//NOTE: This node is only usable in the vertex shader!
class VertexInputNode : public DataNode
{
public:

    static std::string GetInstanceName(void) { return "vertexInputs"; }
    static std::shared_ptr<DataNode> GetInstance(void) { return instance; }


    virtual unsigned int GetNumbOutputs(void) const override { return VertexIns.GetNumbAttributes(); }

    virtual unsigned int GetOutputSize(unsigned int index) const override;
    virtual std::string GetOutputName(unsigned int index) const override;


protected:

    virtual void WriteMyOutputs(std::string & outCode) const override;


private:

    static std::shared_ptr<DataNode> instance;

    VertexInputNode(void);


    MAKE_NODE_READABLE_H(VertexInputNode)
};