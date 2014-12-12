#pragma once

#include "Math/AddNode.h"
#include "Math/ClampNode.h"
#include "Math/CrossNode.h"
#include "Math/DistanceNode.h"
#include "Math/DivideNode.h"
#include "Math/DotNode.h"
#include "Math/GetLerpComponentNode.h"
#include "Math/InterpolateNode.h"
#include "Math/LogNode.h"
#include "Math/MaxMinNodes.h"
#include "Math/ModuloNode.h"
#include "Math/MultiplyNode.h"
#include "Math/PowNode.h"
#include "Math/ReflectNode.h"
#include "Math/RefractNode.h"
#include "Math/RemapNode.h"
#include "Math/RotateAroundAxisNode.h"
#include "Math/SubtractNode.h"

#include "Miscellaneous/TangentSpaceNormalsNode.h"
#include "Miscellaneous/CombineVectorNode.h"
#include "Miscellaneous/CustomExpressionNode.h"
#include "Miscellaneous/LightingNode.h"
#include "Miscellaneous/SimpleFunctionNodes.h"
#include "Miscellaneous/SwizzleNode.h"
#include "Miscellaneous/VectorComponentsNode.h"
#include "Miscellaneous/WhiteNoiseNode.h"

#include "Parameters/CameraDataNode.h"
#include "Parameters/FragmentInputNode.h"
#include "Parameters/GeometryInputNode.h"
#include "Parameters/LinearizeDepthSampleNode.h"
#include "Parameters/ParamNode.h"
#include "Parameters/ProjectionDataNode.h"
#include "Parameters/ShaderInNode.h"
#include "Parameters/SpaceConverterNode.h"
#include "Parameters/SubroutineNode.h"
#include "Parameters/TextureSample2DNode.h"
#include "Parameters/TextureSample3DNode.h"
#include "Parameters/TextureSampleCubemapNode.h"
#include "Parameters/TimeNode.h"
#include "Parameters/VertexInputNode.h"


//Sets up all basic nodes (i.e. the ones inside this folder and its subfolder) to be readable from a file.
extern void PrepareBasicNodesToBeRead(void);