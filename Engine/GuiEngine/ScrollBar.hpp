#pragma once

#include "StandardControl.hpp"

#include <GraphicsEngine/Scene/ITextEntity.hpp>


namespace inl::gui {



class ScrollBar : public StandardControl {
public:
	enum eDirection {
		VERTICAL,
		HORIZONTAL,
	};

public:
	ScrollBar(eDirection direction = VERTICAL);

	void SetSize(Vec2 size) override;
	Vec2 GetSize() const override;
	Vec2 GetMinimumSize() const override;
	Vec2 GetPreferredSize() const override;

	void SetPosition(Vec2 position) override;
	Vec2 GetPosition() const override;

	void Update(float elapsed = 0.0f) override;

	float SetDepth(float depth) override;
	float GetDepth() const override;

	// ScrollBar specific properties
	void SetDirection(eDirection direction);
	void SetInverted(bool inverted);
	void SetTotalLength(float length);
	void SetVisibleLength(float length);
	void SetVisiblePosition(float begin);
	void SetHandleMinimumSize(float size);

protected:
	std::vector<std::reference_wrapper<std::unique_ptr<gxeng::ITextEntity>>> GetTextEntities() override;
	std::vector<std::reference_wrapper<std::unique_ptr<gxeng::IOverlayEntity>>> GetOverlayEntities() override;

private:
	void UpdateHandlePosition();
	void UpdateColor();
	void SetScripts();

private:
	std::unique_ptr<gxeng::IOverlayEntity> m_background;
	std::unique_ptr<gxeng::IOverlayEntity> m_handle;
	eDirection m_direction;
	bool m_inverted = false;
	float m_totalLength = 100.f;
	float m_visibleLength = 33.0f;
	float m_visiblePosition = 33.0f;
	float m_minHandleSize = 10.f;
};


} // namespace inl::gui