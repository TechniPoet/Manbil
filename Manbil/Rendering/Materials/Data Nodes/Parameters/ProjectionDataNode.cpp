#include "ProjectionDataNode.h"

#include "../../MaterialData.h"


DataNodePtr ProjectionDataNode::instance = DataNodePtr(new ProjectionDataNode());


std::string ProjectionDataNode::GetOutputName(unsigned int index) const
{
    switch (index)
    {
        case 0: return MaterialConstants::CameraWidthName;
        case 1: return MaterialConstants::CameraHeightName;
        case 2: return MaterialConstants::CameraZNearName;
        case 3: return MaterialConstants::CameraZFarName;
        case 4: return MaterialConstants::CameraFovName;

        default:
            Assert(false, std::string() + "Invalid output index " + ToString(index));
            return "INVALID_PROJECTION_DATA_NAME";
    }
}
unsigned int ProjectionDataNode::GetOutputSize(unsigned int index) const
{
    switch (index)
    {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
            return 1;

        default:
            Assert(false, std::string() + "Invalid output index " + ToString(index));
            return 0;
    }
}

void ProjectionDataNode::SetMyFlags(MaterialUsageFlags & flags, unsigned int outputIndex) const
{
    switch (outputIndex)
    {
        case 0:
            flags.EnableFlag(MaterialUsageFlags::Flags::DNF_USES_WIDTH);
            break;
        case 1:
            flags.EnableFlag(MaterialUsageFlags::Flags::DNF_USES_HEIGHT);
            break;
        case 2:
            flags.EnableFlag(MaterialUsageFlags::Flags::DNF_USES_ZNEAR);
            break;
        case 3:
            flags.EnableFlag(MaterialUsageFlags::Flags::DNF_USES_ZFAR);
            break;
        case 4:
            flags.EnableFlag(MaterialUsageFlags::Flags::DNF_USES_FOV);
            break;

        default: Assert(false, std::string() + "Invalid output index " + ToString(outputIndex));
    }
}


#pragma warning(disable: 4100)
void ProjectionDataNode::WriteMyOutputs(std::string & outCode) const
{
    //No output writing needed.
}
#pragma warning(default: 4100)


ProjectionDataNode::ProjectionDataNode(void) : DataNode(std::vector<DataLine>(), []() { return instance; }, "projectionUniforms") { }