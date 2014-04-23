#pragma once

#include "ShaderInNodes.h"


//Different calculated values based on shader vertex inputs.


//The screen-space position of the vertex/fragment.
//Outputs two values:
//0: the screen position as a vec3.
//1: the screen position before the homogenous component division, as a vec4.
class ObjectPosToScreenPosCalcNode : public DataNode
{
public:

    static unsigned int GetPosOutputIndex(void) { return 0; }
    static unsigned int GetHomogenousPosOutputIndex(void) { return 1; }


    virtual std::string GetName(void) const override { return "objToScreenPosNode"; }
    virtual std::string GetOutputName(unsigned int index) const override
    {
        Assert(index <= 1, std::string() + "Invalid output index " + ToString(index));

        return GetName() + ToString(GetUniqueID()) +
                  (index == 0 ? "_pos" : "_homogenousPos");
    }

    ObjectPosToScreenPosCalcNode(DataLine objectPos = DataLine(DataNodePtr(new ObjectPosNode()), 0))
        : DataNode(MakeVector(objectPos), MakeVector(3, 4)) { }


protected:

    virtual void SetMyFlags(MaterialUsageFlags & flags, unsigned int outputIndex) const override
    {
        Assert(outputIndex <= 1, std::string() + "Invalid output index " + ToString(outputIndex));
        flags.EnableFlag(MaterialUsageFlags::Flags::DNF_USES_WVP_MAT);
    }
    virtual void WriteMyOutputs(std::string & outCode) const override;
};
//The screen-space vertex/fragment normal.
class ObjectNormalToScreenNormalCalcNode : public DataNode
{
public:

    virtual std::string GetName(void) const override { return "screenNormalNode"; }

    ObjectNormalToScreenNormalCalcNode(DataLine objectNormal = DataLine(DataNodePtr(new ObjectNormalNode()), 0))
        : DataNode(MakeVector(objectNormal), MakeVector(3)) { }


protected:

    virtual void SetMyFlags(MaterialUsageFlags & flags, unsigned int outputIndex) const override
    {
        Assert(outputIndex == 0, std::string() + "Invalid output index " + ToString(outputIndex));
        flags.EnableFlag(MaterialUsageFlags::Flags::DNF_USES_WVP_MAT);
    }
    virtual void WriteMyOutputs(std::string & outCode) const override;
};



//The world-space position of the vertex/fragment.
class ObjectPosToWorldPosCalcNode : public DataNode
{
public:

    virtual std::string GetName(void) const override { return "objPosToWorldNode"; }
    virtual std::string GetOutputName(unsigned int index) const override
    {
        Assert(index == 0, std::string() + "Invalid output index " + std::to_string(index));

        return GetName() + ToString(GetUniqueID()) + "_pos";
    }

    ObjectPosToWorldPosCalcNode(DataLine objectPos = DataLine(DataNodePtr(new ObjectPosNode()), 0))
        : DataNode(MakeVector(objectPos), MakeVector(3)) { }


protected:

    virtual void SetMyFlags(MaterialUsageFlags & flags, unsigned int outputIndex) const override
    {
        Assert(outputIndex <= 1, std::string() + "Invalid output index " + ToString(outputIndex));
        flags.EnableFlag(MaterialUsageFlags::Flags::DNF_USES_WORLD_MAT);
    }
    virtual void WriteMyOutputs(std::string & outCode) const override;
};
//The world-space vertex/fragment normal.
class ObjectNormalToWorldNormalCalcNode : public DataNode
{
public:

    virtual std::string GetName(void) const override { return "objNormalToWorldNode"; }

    ObjectNormalToWorldNormalCalcNode(DataLine objNormal = DataLine(DataNodePtr(new ObjectNormalNode()), 0))
        : DataNode(MakeVector(objNormal), MakeVector(3)) { }


protected:

    virtual void SetMyFlags(MaterialUsageFlags & flags, unsigned int outputIndex) const override
    {
        Assert(outputIndex == 0, std::string() + "Invalid output index " + ToString(outputIndex));
        flags.EnableFlag(MaterialUsageFlags::Flags::DNF_USES_WORLD_MAT);
    }
    virtual void WriteMyOutputs(std::string & outCode) const override;
};