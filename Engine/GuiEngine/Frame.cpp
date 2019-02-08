#include "Frame.hpp"

#include "Placeholders/PlaceholderOverlayEntity.hpp"


namespace inl::gui {


Frame::Frame() {}


void Frame::SetSize(const Vec2& size) {
	m_background.SetSize(size);
	if (m_layout) {
		m_layout->SetSize(size);
	}
}


Vec2 Frame::GetSize() const {
	return m_background.GetSize();
}

Vec2 Frame::GetMinimumSize() const {
	return m_layout ? m_layout->GetMinimumSize() : Vec2{ 0.f, 0.f };
}

Vec2 Frame::GetPreferredSize() const {
	return m_layout ? m_layout->GetPreferredSize() : Vec2{ 0.f, 0.f };
}


void Frame::SetPosition(const Vec2& position) {
	m_background.SetPosition(position);
	if (m_layout) {
		m_layout->SetPosition(position);
	}
}


Vec2 Frame::GetPosition() const {
	return m_background.GetPosition();
}


void Frame::SetLayout(std::shared_ptr<Layout> layout) {
	if (m_layout) {
		RemoveChild(m_layout.get());
		m_layout.reset();
	}

	AddChild(layout);
	m_layout = layout;
	if (m_layout) {
		m_layout->SetSize(GetSize());
		m_layout->SetPosition(GetPosition());
		m_layout->SetVisible(GetVisible());
	}
}


std::shared_ptr<Layout> Frame::GetLayout() const {
	return m_layout;
}


float Frame::SetDepth(float depth) {
	m_background.SetDepth(depth);
	float span = 1.0f;
	if (m_layout) {
		span += m_layout->SetDepth(depth + 1.0f);
	}
	return span;
}

float Frame::GetDepth() const {
	return m_background.GetDepth();
}


void Frame::SetVisible(bool visible) {
	throw NotImplementedException();
}


bool Frame::GetVisible() const {
	throw NotImplementedException();
}


bool Frame::IsShown() const {
	throw true;
}



} // namespace inl::gui