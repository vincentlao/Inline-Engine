#pragma once


#include "SharedPtrLess.hpp"
#include "BlankShared.hpp"

#include <BaseLibrary/Event.hpp>
#include <BaseLibrary/Exception/Exception.hpp>
#include <BaseLibrary/Platform/Input.hpp>

#include <InlineMath.hpp>
#include <any>
#include <memory>
#include <set>

namespace inl::gui {



class Control {
public:
	// Hierarchy
	void AddChild(std::shared_ptr<Control> child);
	void RemoveChild(const Control* child);
	void ClearChildren();

	const Control* GetParent() const;
	std::set<const Control*> GetChildren() const;

	// Sizing
	virtual void SetSize(const Vec2& size) = 0;
	virtual Vec2 GetSize() const = 0;
	virtual Vec2 GetPreferredSize() const = 0;
	virtual Vec2 GetMinimumSize() const = 0;

	// Position
	virtual void SetPosition(const Vec2& position) = 0;
	virtual Vec2 GetPosition() const = 0;
	virtual float SetDepth(float depth) = 0;
	virtual float GetDepth() const = 0;

	// Visibility
	virtual void SetVisible(bool visible) = 0;
	virtual bool GetVisible() const = 0;
	virtual bool IsShown() const = 0;

	virtual void Update(float elapsed = 0.0f) {}


	// Events
	Event<Control*, const Control*> OnChildAdded; // subject, child
	Event<Control*, const Control*> OnChildRemoved; // subject, child

	Event<Control*> OnEnterArea; // subject
	Event<Control*, Vec2> OnHover; // subject, where absolute
	Event<Control*> OnLeaveArea; // subject

	Event<Control*, Vec2, eMouseButton> OnMouseDown; // subject, where absolute, which button
	Event<Control*, Vec2, eMouseButton> OnMouseUp; // subject, where absolute, which button
	Event<Control*, Vec2, eMouseButton> OnClick; // subject, where absolute, which button
	Event<Control*, Vec2, eMouseButton> OnDoubleClick; // subject, where absolute, which button
	Event<Control*, Vec2> OnDragBegin; // subject, dragOrigin
	Event<Control*, Vec2> OnDrag; // subject, dragPosition
	Event<Control*, Vec2, Control*> OnDragEnd; // subject, dragPosition, dragTarget

	Event<Control*, eKey> OnKeydown;
	Event<Control*, eKey> OnKeyup;
	Event<Control*, char32_t> OnCharacter;

	Event<Control*> OnGainFocus;
	Event<Control*> OnLoseFocus;

protected:
	virtual void ChildAddedHandler(Control& child) {}
	virtual void ChildRemovedHandler(Control& child) {}
	virtual void AttachedHandler(Control& parent) {}
	virtual void DetachedHandler() {}

	template <class T>
	static void SetLayoutPosition(Control& control, T data);

	template <class T>
	static T& GetLayoutPosition(const Control& control);

	template <class EventT, class... Args>
	void CallEventUpstream(EventT event, const Args&... args) const;

private:
	const Control* m_parent = nullptr;
	std::set<std::shared_ptr<Control>, SharedPtrLess<Control>> m_children;
	mutable std::any m_layoutPosition;
};



template <class T>
void Control::SetLayoutPosition(Control& control, T data) {
	control.m_layoutPosition = std::any(data);
}


template <class T>
T& Control::GetLayoutPosition(const Control& control) {
	T* data = std::any_cast<T>(&control.m_layoutPosition);
	if (data) {
		return *data;
	}
	throw InvalidCastException("Layout position data stored in control has type different to the requested.");
}


template <class EventT, class... Args>
void Control::CallEventUpstream(EventT event, const Args&... args) const {
	(this->*event)(args...);
	const Control* parent = GetParent();
	if (parent) {
		parent->CallEventUpstream(event, args...);
	}
}



} // namespace inl::gui