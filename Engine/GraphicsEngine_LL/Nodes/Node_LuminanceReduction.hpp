#pragma once

#include "../GraphicsNode.hpp"

#include "../Scene.hpp"
#include "../PerspectiveCamera.hpp"
#include "../Mesh.hpp"
#include "../ConstBufferHeap.hpp"
#include "../PipelineTypes.hpp"
#include "GraphicsApi_LL/IPipelineState.hpp"
#include "GraphicsApi_LL/IGxapiManager.hpp"

#include <optional>

namespace inl::gxeng::nodes {


class LuminanceReduction :
	virtual public GraphicsNode,
	virtual public GraphicsTask,
	virtual public InputPortConfig<Texture2D>,
	virtual public OutputPortConfig<Texture2D>
{
public:
	static const char* Info_GetName() { return "LuminanceReduction"; }
	LuminanceReduction();

	void Update() override {}
	void Notify(InputPortBase* sender) override {}
	void Initialize(EngineContext& context) override;
	void Reset() override;
	void Setup(SetupContext& context) override;
	void Execute(RenderContext& context) override;

protected:
	TextureView2D m_luminanceView;

	uint64_t m_width;
	uint32_t m_height;

	gxeng::RWTextureView2D m_uav;
	gxeng::TextureView2D m_srv;

protected:
	std::optional<Binder> m_binder;
	BindParameter m_luminanceBindParam;
	BindParameter m_outputBindParam;
	ShaderProgram m_shader;
	std::unique_ptr<gxapi::IPipelineState> m_CSO;

private:
	void InitRenderTarget(SetupContext& context);
};


} // namespace inl::gxeng::nodes
