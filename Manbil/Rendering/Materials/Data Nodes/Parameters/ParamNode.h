#pragma once

#include "../DataNode.h"


//Represents a DataNode that outputs some user-defined
//     vector that can be changed at run-time.
class ParamNode : public DataNode
{
public:

    const std::string & GetParamName(void) const { return pName; }


    virtual unsigned int GetOutputSize(unsigned int index) const override;
    virtual std::string GetOutputName(unsigned int index) const override;

    ParamNode(unsigned int vectorSize, std::string paramName, std::string name = "");


protected:

    virtual void GetMyParameterDeclarations(UniformDictionary & outUniforms) const override;

    virtual void WriteMyOutputs(std::string & outCode) const override;

    virtual bool WriteExtraData(DataWriter * writer, std::string & outError) const override;
    virtual bool ReadExtraData(DataReader * reader, std::string & outError) override;


private:

    std::string pName;
    unsigned int vSize;

    MAKE_NODE_READABLE_H(ParamNode)
};