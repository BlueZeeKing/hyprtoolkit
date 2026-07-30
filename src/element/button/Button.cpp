#include "Button.hpp"

#include <hyprtoolkit/palette/Palette.hpp>

#include "../../core/InternalBackend.hpp"
#include "../../layout/Positioner.hpp"
#include "../../renderer/Renderer.hpp"
#include "../../window/ToolkitWindow.hpp"
#include "../../core/AnimationManager.hpp"
#include "../Element.hpp"

using namespace Hyprtoolkit;
using namespace Hyprgraphics;

constexpr double   BUTTON_PAD = 5;

SP<CButtonElement> CButtonElement::create(const SButtonData& data) {
    auto p          = SP<CButtonElement>(new CButtonElement(data));
    p->impl->self   = p;
    p->m_impl->self = p;
    return p;
}

CButtonElement::CButtonElement(const SButtonData& data) : IElement(), m_impl(makeUnique<SButtonImpl>()) {
    m_impl->data = data;

    auto calc_size = data.size.calculate({0, 0});

    m_impl->background = CRectangleBuilder::begin()
                             ->color([acc = m_impl->data.accent, nobg = m_impl->data.noBg] {
                                 if (acc)
                                     return g_palette->m_colors.accent;
                                 if (nobg)
                                     return CHyprColor{g_palette->m_colors.base.asRGB(), 0.F};
                                 return g_palette->m_colors.base;
                             })
                             ->rounding(g_palette->m_vars.smallRounding)
                             ->borderColor([acc = m_impl->data.accent] {
                                 if (acc)
                                     return g_palette->m_colors.accent;
                                 return g_palette->m_colors.alternateBase;
                             })
                             ->borderThickness(data.noBorder ? 0 : 1)
                             ->size(CDynamicSize{
                                     calc_size.x == -1 ? CDynamicSize::HT_SIZE_AUTO : CDynamicSize::HT_SIZE_PERCENT,
                                     calc_size.y == -1 ? CDynamicSize::HT_SIZE_AUTO : CDynamicSize::HT_SIZE_PERCENT,
                                     {1.F, 1.F}})
                             ->commence();

    m_impl->label = CTextBuilder::begin()
                        ->text(std::string{data.label})
                        ->fontSize(CFontSize{data.fontSize})
                        ->fontFamily(std::string{data.fontFamily})
                        ->color([impl = m_impl.get(), acc = m_impl->data.accent] {
                            auto c = g_palette->m_colors.text;
                            if (acc)
                                c = g_palette->m_colors.accent.asOkLab().l > 0.5 ? g_palette->m_colors.background : g_palette->m_colors.brightText;
                            if (!impl->data.enabled)
                                c.a *= 0.5F;
                            return c;
                        })
                        ->size(CDynamicSize{CDynamicSize::HT_SIZE_AUTO, CDynamicSize::HT_SIZE_AUTO, {1.F, 1.F}})
                        ->align(m_impl->data.alignText)
                        ->callback([this] {
                            m_impl->labelChanged = true;
                            if (impl->window)
                                impl->window->scheduleReposition(impl->self);
                        })
                        ->noEllipsize(!m_impl->data.ellipsize)
                        ->commence();

    m_impl->label->setPositionMode(HT_POSITION_ABSOLUTE);
    m_impl->label->setPositionFlag(
        m_impl->data.alignText == HT_FONT_ALIGN_CENTER ? HT_POSITION_FLAG_CENTER : (m_impl->data.alignText == HT_FONT_ALIGN_RIGHT ? HT_POSITION_FLAG_RIGHT : HT_POSITION_FLAG_LEFT),
        true);
    m_impl->label->setPositionFlag(HT_POSITION_FLAG_VCENTER, true);

    addChild(m_impl->background);
    m_impl->background->addChild(m_impl->label);
    m_impl->label->setMargin(BUTTON_PAD);

    impl->m_externalEvents.mouseEnter.listenStatic([this](const Vector2D& pos) {
        if (!m_impl->data.enabled)
            return;
        m_impl->background
            ->rebuild() //
            ->color([acc = m_impl->data.accent, nb = m_impl->data.noBorder, nobg = m_impl->data.noBg] {
                if (acc)
                    return g_palette->m_colors.accent.brighten(0.1F);
                if (nobg)
                    return g_palette->m_colors.base.brighten(0.05F);
                return g_palette->m_colors.base.brighten(nb ? 0.3F : 0.11F);
            })
            ->borderColor([] { return g_palette->m_colors.accent; })
            ->commence();
    });

    impl->m_externalEvents.mouseLeave.listenStatic([this]() {
        m_impl->background
            ->rebuild() //
            ->color([acc = m_impl->data.accent, nobg = m_impl->data.noBg] {
                if (acc)
                    return g_palette->m_colors.accent;
                if (nobg)
                    return CHyprColor{g_palette->m_colors.base.asRGB(), 0.F};
                return g_palette->m_colors.base;
            })
            ->borderColor([acc = m_impl->data.accent] {
                if (acc)
                    return g_palette->m_colors.accent;
                return g_palette->m_colors.alternateBase;
            })
            ->commence();
    });

    impl->m_externalEvents.mouseButton.listenStatic([this](const Input::eMouseButton button, bool down) {
        if (!down || !m_impl->data.enabled)
            return;

        if (button == Input::MOUSE_BUTTON_RIGHT) {
            if (m_impl->data.onRightClick)
                m_impl->data.onRightClick(m_impl->self.lock());
        } else if (button == Input::MOUSE_BUTTON_LEFT) {
            if (m_impl->data.onMainClick)
                m_impl->data.onMainClick(m_impl->self.lock());
        }
    });

    impl->grouped = true;
}

void CButtonElement::paint() {
    ;
}

void CButtonElement::reposition(const Hyprutils::Math::CBox& box, const Hyprutils::Math::Vector2D& maxSize) {
    IElement::reposition(box);

    g_positioner->positionChildren(impl->self.lock());
}

void CButtonElement::setLabel(std::string label) {
    if (label == m_impl->data.label)
        return;

    m_impl->data.label = std::move(label);
    m_impl->label->setText(m_impl->data.label);
}

void CButtonElement::setEnabled(bool enabled) {
    if (enabled == m_impl->data.enabled)
        return;

    m_impl->data.enabled = enabled;
    m_impl->label->recheckColor();

    if (impl->window)
        impl->window->scheduleReposition(impl->self);
}

SP<CButtonBuilder> CButtonElement::rebuild() {
    auto p       = SP<CButtonBuilder>(new CButtonBuilder());
    p->m_self    = p;
    p->m_data    = makeUnique<SButtonData>(m_impl->data);
    p->m_element = m_impl->self;
    return p;
}

void CButtonElement::replaceData(const SButtonData& data) {
    m_impl->data = data;

    m_impl->label->rebuild()->text(std::string{data.label})->commence();
    m_impl->label->recheckColor();

    m_impl->label->setPositionFlag(HT_POSITION_FLAG_ALL, false);
    m_impl->label->setPositionFlag(
        m_impl->data.alignText == HT_FONT_ALIGN_CENTER ? HT_POSITION_FLAG_CENTER : (m_impl->data.alignText == HT_FONT_ALIGN_RIGHT ? HT_POSITION_FLAG_RIGHT : HT_POSITION_FLAG_LEFT),
        true);

    if (impl->window)
        impl->window->scheduleReposition(impl->self);
}

Hyprutils::Math::Vector2D CButtonElement::size() {
    return impl->position.size();
}

std::optional<Vector2D> CButtonElement::preferredSize(const Hyprutils::Math::Vector2D& parent, bool grow) {
    return impl->getPreferredSizeGeneric(m_impl->data.size, parent, grow);
}

std::optional<Vector2D> CButtonElement::minimumSize(const Hyprutils::Math::Vector2D& parent) {
    return Vector2D{0, 0}; // TODO: implement
}

std::optional<Vector2D> CButtonElement::maximumSize(const Hyprutils::Math::Vector2D& parent) {
    return std::nullopt; // TODO: implement
}

bool CButtonElement::acceptsMouseInput() {
    return true;
}

ePointerShape CButtonElement::pointerShape() {
    return HT_POINTER_POINTER;
}

bool CButtonElement::positioningDependsOnChild() {
    return m_impl->data.size.hasAuto();
}
