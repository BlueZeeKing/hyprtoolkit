#include "hyprtoolkit/types/SizeType.hpp"
#include <hyprtoolkit/core/Backend.hpp>
#include <hyprtoolkit/window/Window.hpp>
#include <hyprtoolkit/system/Icons.hpp>
#include <hyprtoolkit/element/Rectangle.hpp>
#include <hyprtoolkit/element/RowLayout.hpp>
#include <hyprtoolkit/element/ColumnLayout.hpp>
#include <hyprtoolkit/element/Text.hpp>
#include <hyprtoolkit/element/Image.hpp>
#include <hyprtoolkit/element/Button.hpp>
#include <hyprtoolkit/element/Null.hpp>
#include <hyprtoolkit/element/Checkbox.hpp>
#include <hyprtoolkit/element/RadioGroup.hpp>
#include <hyprtoolkit/element/Spinbox.hpp>
#include <hyprtoolkit/element/ProgressBar.hpp>
#include <hyprtoolkit/element/Slider.hpp>
#include <hyprtoolkit/element/ScrollArea.hpp>
#include <hyprtoolkit/element/Combobox.hpp>
#include <hyprtoolkit/element/Textbox.hpp>

#include <hyprutils/memory/SharedPtr.hpp>
#include <hyprutils/memory/UniquePtr.hpp>

#include <print>

#include "unit/tricks/Tricks.hpp"

using namespace Hyprutils::Memory;
using namespace Hyprutils::Math;
using namespace Hyprtoolkit;

#define SP CSharedPointer
#define WP CWeakPointer
#define UP CUniquePointer

constexpr float                 SLIDER_HEIGHT = 10.F;

static SP<IElement> stretchLayout(std::string&& label, SP<IElement> control) {
    auto bg = CRectangleBuilder::begin()
                  ->size({CDynamicSize::HT_SIZE_PERCENT, CDynamicSize::HT_SIZE_AUTO, {1, 1}})
                  ->rounding(4)
                  ->commence();
    auto layoutE = CRowLayoutBuilder::begin()->size({CDynamicSize::HT_SIZE_PERCENT, CDynamicSize::HT_SIZE_AUTO, {1, 1}})->commence();
    auto labelE  = CTextBuilder::begin()->text(std::move(label))->commence();
    auto nullE   = CNullBuilder::begin()->commence();
    nullE->setGrow(true);

    auto container = CNullBuilder::begin()->size({CDynamicSize::HT_SIZE_AUTO, CDynamicSize::HT_SIZE_AUTO, {1, 1}})->commence();
    container->setMargin(4);

    bg->addChild(container);
    container->addChild(layoutE);

    layoutE->addChild(labelE);
    layoutE->addChild(nullE);
    layoutE->addChild(control);

    return bg;
}

int main(int argc, char** argv, char** envp) {
    Tests::Tricks::createBackendSupport();
    
    auto bg = CRectangleBuilder::begin()->commence();

    auto scroll = CScrollAreaBuilder::begin()->scrollY(true)->showScrollbar(true)->size({CDynamicSize::HT_SIZE_PERCENT, CDynamicSize::HT_SIZE_PERCENT, {1.F, 1.F}})->commence();

    auto mainLayout = CColumnLayoutBuilder::begin()->gap(3)->size({CDynamicSize::HT_SIZE_PERCENT, CDynamicSize::HT_SIZE_AUTO, {0.7F, 1.F}})->commence();

    mainLayout->setMargin(3);
    mainLayout->setPositionMode(Hyprtoolkit::IElement::HT_POSITION_ABSOLUTE);
    mainLayout->setPositionFlag(Hyprtoolkit::IElement::HT_POSITION_FLAG_HCENTER, true);

    bg->addChild(scroll);
    scroll->addChild(mainLayout);

    auto title = CTextBuilder::begin() //
                     ->text("Controls")
                     ->fontSize({CFontSize::HT_FONT_H2})
                     ->commence();
    title->setTooltip("Example tooltip!! Woo!");

    auto hr = CRectangleBuilder::begin() //
                  ->size({CDynamicSize::HT_SIZE_PERCENT, CDynamicSize::HT_SIZE_ABSOLUTE, {0.5F, 9.F}})
                  ->commence();

    hr->setMargin(4);

    auto button1 = CButtonBuilder::begin()
                       ->label("Secret")
                       ->size({CDynamicSize::HT_SIZE_AUTO, CDynamicSize::HT_SIZE_AUTO, {1, 1}})
                       ->commence();

    auto button2 = CButtonBuilder::begin()
                       ->label("Popup")
                       ->size({CDynamicSize::HT_SIZE_AUTO, CDynamicSize::HT_SIZE_AUTO, {1, 1}})
                       ->commence();

    auto button3 = CButtonBuilder::begin()
                       ->label("Select textbox")
                       ->size({CDynamicSize::HT_SIZE_AUTO, CDynamicSize::HT_SIZE_AUTO, {1, 1}})
                       ->commence();

    auto button4 = CButtonBuilder::begin()
                       ->label("Show text")
                       ->size({CDynamicSize::HT_SIZE_AUTO, CDynamicSize::HT_SIZE_AUTO, {1, 1}})
                       ->commence();

    auto checkbox  = stretchLayout("checkbox 1", CCheckboxBuilder::begin()->commence());
    auto checkbox2 = stretchLayout("checkbox 2", CCheckboxBuilder::begin()->commence());

    auto radio1 = CCheckboxBuilder::begin()->style(HT_CHECKBOX_STYLE_RADIO)->toggled(true)->commence();
    auto radio2 = CCheckboxBuilder::begin()->style(HT_CHECKBOX_STYLE_RADIO)->commence();
    auto radio3 = CCheckboxBuilder::begin()->style(HT_CHECKBOX_STYLE_RADIO)->commence();

    auto radioGroup = CRadioGroup::create();
    radioGroup->add(radio1);
    radioGroup->add(radio2);
    radioGroup->add(radio3);

    auto radioRow = CRowLayoutBuilder::begin()->gap(8)->size({CDynamicSize::HT_SIZE_PERCENT, CDynamicSize::HT_SIZE_AUTO, {1, 1}})->commence();
    radioRow->addChild(radio1);
    radioRow->addChild(CTextBuilder::begin()->text("first")->commence());
    radioRow->addChild(radio2);
    radioRow->addChild(CTextBuilder::begin()->text("second")->commence());
    radioRow->addChild(radio3);
    radioRow->addChild(CTextBuilder::begin()->text("third")->commence());

    auto spinbox = CSpinboxBuilder::begin()
                       ->label("Spinbox")
                       ->items({"Hello", "World", "Amongus"})
                       ->size({CDynamicSize::HT_SIZE_AUTO, CDynamicSize::HT_SIZE_AUTO, {1, 1}})
                       ->fill(true)
                       ->commence();

    auto slider = stretchLayout("Slider", CSliderBuilder::begin()->size({CDynamicSize::HT_SIZE_PERCENT, CDynamicSize::HT_SIZE_ABSOLUTE, {0.5F, SLIDER_HEIGHT}})->commence());

    auto slider2 = stretchLayout(
        "Big Slider", CSliderBuilder::begin()->max(10000)->val(2500)->size({CDynamicSize::HT_SIZE_PERCENT, CDynamicSize::HT_SIZE_ABSOLUTE, {0.5F, SLIDER_HEIGHT}})->commence());

    auto progress = stretchLayout(
        "Progress", CProgressBarBuilder::begin()->value(0.42F)->size({CDynamicSize::HT_SIZE_PERCENT, CDynamicSize::HT_SIZE_ABSOLUTE, {0.5F, 14.F}})->commence());

    auto combo = stretchLayout(
        "Combo",
        CComboboxBuilder::begin()
            ->items({"According", "to",  "all",  "known", "laws",   "of",    "aviation", "there",   "is",   "no",    "way",  "that", "a",      "bee",   "should", "be",
                     "able",      "to",  "fly.", "its",   "wings",  "are",   "too",      "small",   "to",   "get",   "its",  "fat",  "little", "body",  "off",    "the",
                     "ground.",   "the", "bee",  "of",    "course", "flies", "anyways,", "because", "bees", "don't", "care", "what", "humans", "think", "is",     "impossible."})
            ->size({CDynamicSize::HT_SIZE_PERCENT, CDynamicSize::HT_SIZE_ABSOLUTE, {0.3F, 25.F}})
            ->commence());

    auto textbox = CTextboxBuilder::begin()
                  ->defaultText("")
                  ->placeholder("placeholder")
                  ->size({CDynamicSize::HT_SIZE_ABSOLUTE, CDynamicSize::HT_SIZE_ABSOLUTE, {150.F, 60.F}})
                  ->multiline(true)
                  ->commence();

    auto textboxCont = stretchLayout("Textbox", textbox);

    auto text = CTextBuilder::begin()
                    ->text("This is a link test: <a href=\"https://hypr.land\">click me</a>! Test overflow as well woo woo woo woo woo woo woo woo I am vaxry")
                    ->commence();
    text->setTooltip("Example tooltip!");

    auto rowl = CRowLayoutBuilder::begin()->size({CDynamicSize::HT_SIZE_PERCENT, CDynamicSize::HT_SIZE_AUTO, {1, 1}})->commence();
    rowl->addChild(CTextBuilder::begin()->text("hello this will be ellipsized woooo oooo ooo oo oo oo oo o oo")->commence());

    mainLayout->addChild(title);
    mainLayout->addChild(hr);
    mainLayout->addChild(button1);
    mainLayout->addChild(button2);
    mainLayout->addChild(button3);
    mainLayout->addChild(button4);
    mainLayout->addChild(checkbox);
    mainLayout->addChild(checkbox2);
    mainLayout->addChild(radioRow);
    mainLayout->addChild(spinbox);
    mainLayout->addChild(slider);
    mainLayout->addChild(slider2);
    mainLayout->addChild(progress);
    mainLayout->addChild(combo);
    mainLayout->addChild(textboxCont);
    mainLayout->addChild(text);
    mainLayout->addChild(rowl);

    for (int x = 480; x < 1280; x += 10) {
        for (int y = 480; y < 720; y += 10) {
            bg->rebuild()->size({CDynamicSize::HT_SIZE_ABSOLUTE, CDynamicSize::HT_SIZE_ABSOLUTE, {x, y}})->commence();
            bg->forceReposition();
        }
    }

    return 0;
}
