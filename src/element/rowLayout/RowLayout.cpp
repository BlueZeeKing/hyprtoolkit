#include "RowLayout.hpp"
#include <cmath>

#include "../../layout/Positioner.hpp"
#include "../../renderer/Renderer.hpp"
#include "../../core/InternalBackend.hpp"
#include "../../window/ToolkitWindow.hpp"

#include "../Element.hpp"
#include "../LinearLayout.hpp"

using namespace Hyprtoolkit;

SP<CRowLayoutElement> CRowLayoutElement::create(const SRowLayoutData& data) {
    auto p          = SP<CRowLayoutElement>(new CRowLayoutElement(data));
    p->impl->self   = p;
    p->m_impl->self = p;
    return p;
}

CRowLayoutElement::CRowLayoutElement(const SRowLayoutData& data) : IElement(), m_impl(makeUnique<SRowLayoutImpl>()) {
    m_impl->data = data;
}

void CRowLayoutElement::paint() {
    ; // no-op
}

void CRowLayoutElement::replaceData(const SRowLayoutData& data) {
    m_impl->data = data;

    if (impl->window)
        impl->window->scheduleReposition(impl->self);
}

void CRowLayoutElement::reposition(const Hyprutils::Math::CBox& sbox, const Hyprutils::Math::Vector2D& maxSize) {
    IElement::reposition(sbox);
    LinearLayout::reposition<true>(this, impl->position, impl->children, m_impl->data.gap, [this](SP<IElement> c) { return childSize(c); });
}

Hyprutils::Math::Vector2D CRowLayoutElement::childSize(Hyprutils::Memory::CSharedPointer<IElement> child) {
    auto pref = child->preferredSize(impl->position.size());
    if (pref)
        return *pref;

    auto min = child->minimumSize(impl->position.size());
    if (min)
        return *min;

    return {-1, -1};
}

Hyprutils::Math::Vector2D CRowLayoutElement::size() {
    return impl->position.size();
}

std::optional<Hyprutils::Math::Vector2D> CRowLayoutElement::preferredSize(const Hyprutils::Math::Vector2D& parent, bool grow) {
    auto calc = m_impl->data.size.calculate(grow ? parent : Vector2D{0, 0});

    if (calc.x != -1 && calc.y != -1)
        return calc;

    Vector2D parentForChild = parent;
    parentForChild.x -= impl->margin * 2;
    parentForChild.y -= impl->margin * 2;

    if (calc.x != -1) 
        parentForChild.x = calc.x;
    if (calc.y != -1)
        parentForChild.y = calc.y;

    Vector2D max;
    for (const auto& child : impl->children) {
        Vector2D childPref = child->preferredSize(parentForChild, false).value_or({ 0, 0 });
        max.x += childPref.x + m_impl->data.gap;
        max.y = std::max(max.y, childPref.y);
    }

    if (!impl->children.empty())
        max.x -= m_impl->data.gap;

    max.x += impl->margin * 2;
    max.y += impl->margin * 2;

    max.x = std::ceil(max.x);
    max.y = std::ceil(max.y);

    if (calc.x == -1)
        calc.x = max.x;
    if (calc.y == -1)
        calc.y = max.y;

    return calc;
}

std::optional<Hyprutils::Math::Vector2D> CRowLayoutElement::minimumSize(const Hyprutils::Math::Vector2D& parent) {
    Vector2D min;
    for (const auto& child : impl->children) {
        min.y = std::max(min.y, childSize(child).y);
    }

    return min;
}

bool CRowLayoutElement::positioningDependsOnChild() {
    return m_impl->data.size.hasAuto();
}
