/**
 * @file
 */

#pragma once

#include "core/CommandlineApp.h"
#include "Types.h"
#include <simplecpp.h>
#include <vector>

/**
 * @brief This tool validates the compute shaders and generates c++ code for them.
 *
 * @li contains a C preprocessor (simplecpp/cppcheck).
 * @li detects the needed dimensions of the compute shader and generate worksizes with
 *  proper types to call the kernels.
 * @li converts OpenCL types into glm and stl types (basically just vector).
 * @li handles alignment and padding of types according to the OpenCL specification.
 * @li detects buffer flags like use-the-host-pointer(-luke) according to the alignment
 *  and size.
 * @li hides all the buffer creation/deletion mambo-jambo from the caller.
 * @li parses OpenCL structs and generate proper aligned C++ struct for them.
 *
 * @ingroup Tools
 * @ingroup Compute
 */
class ComputeShaderTool: public core::CommandlineApp {
private:
	using Super = core::CommandlineApp;
protected:
	core::String _namespaceSrc;
	core::String _sourceDirectory;
	core::String _postfix;
	core::String _shaderDirectory;
	core::String _computeFilename;
	core::String _shaderTemplateFile;
	core::String _name;
	std::vector<computeshadertool::Kernel> _kernels;
	std::vector<computeshadertool::Struct> _structs;
	std::map<core::String, core::String> _constants;
	std::vector<core::String> _includeDirs;

	std::pair<core::String, bool> getSource(const core::String& file) const;
	bool parse(const core::String& src);
public:
	ComputeShaderTool(const metric::MetricPtr& metric, const io::FilesystemPtr& filesystem, const core::EventBusPtr& eventBus, const core::TimeProviderPtr& timeProvider);
	~ComputeShaderTool();

	core::AppState onConstruct() override;
	core::AppState onRunning() override;
};
