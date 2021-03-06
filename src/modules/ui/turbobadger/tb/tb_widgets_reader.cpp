/**
 * @file
 */

#include "core/StringUtil.h"
#include "tb_widgets_reader.h"
#include "image/tb_image_widget.h"
#include "tb_editfield.h"
#include "tb_font_renderer.h"
#include "tb_inline_select.h"
#include "tb_node_tree.h"
#include "tb_scroll_container.h"
#include "tb_select.h"
#include "tb_system.h"
#include "tb_tab_container.h"
#include "tb_toggle_container.h"
#include "tb_widgets_common.h"

namespace tb {

TB_WIDGET_FACTORY(TBWidget, TBValue::TYPE_NULL, WIDGET_Z_TOP) {
}
void TBWidget::onInflate(const INFLATE_INFO &info) {
	TBWidgetsReader::setIDFromNode(getID(), info.node->getNode("id"));

	TBWidgetsReader::setIDFromNode(getGroupID(), info.node->getNode("group-id"));

	if (info.sync_type == TBValue::TYPE_FLOAT) {
		setValueDouble(info.node->getValueFloat("value", 0));
	} else {
		setValue(info.node->getValueInt("value", 0));
	}

	if (TBNode *data_node = info.node->getNode("data")) {
		data.copy(data_node->getValue());
	}

	setIsGroupRoot(info.node->getValueInt("is-group-root", static_cast<int>(getIsGroupRoot())) != 0);

	setIsFocusable(info.node->getValueInt("is-focusable", static_cast<int>(getIsFocusable())) != 0);

	setWantLongClick(info.node->getValueInt("want-long-click", static_cast<int>(getWantLongClick())) != 0);

	setIgnoreInput(info.node->getValueInt("ignore-input", static_cast<int>(getIgnoreInput())) != 0);

	setOpacity(info.node->getValueFloat("opacity", getOpacity()));

	if (const char *text = info.node->getValueString("text", nullptr)) {
		setText(text);
	}

	if (const char *connection = info.node->getValueStringRaw("connection", nullptr)) {
		// If we already have a widget value with this name, just connect to it and the widget will
		// adjust its value to it. Otherwise create a new widget value, and give it the value we
		// got from the resource.
		if (TBWidgetValue *value = g_value_group.getValue(connection)) {
			connect(value);
		} else if (TBWidgetValue *value = g_value_group.createValueIfNeeded(connection, info.sync_type)) {
			value->setFromWidget(this);
			connect(value);
		}
	}
	if (const char *axis = info.node->getValueString("axis", nullptr)) {
		setAxis(*axis == 'x' ? AXIS_X : AXIS_Y);
	}
	if (const char *gravity = info.node->getValueString("gravity", nullptr)) {
		WIDGET_GRAVITY g = WIDGET_GRAVITY_NONE;
		if (SDL_strstr(gravity, "left") != nullptr) {
			g |= WIDGET_GRAVITY_LEFT;
		}
		if (SDL_strstr(gravity, "top") != nullptr) {
			g |= WIDGET_GRAVITY_TOP;
		}
		if (SDL_strstr(gravity, "right") != nullptr) {
			g |= WIDGET_GRAVITY_RIGHT;
		}
		if (SDL_strstr(gravity, "bottom") != nullptr) {
			g |= WIDGET_GRAVITY_BOTTOM;
		}
		if (SDL_strstr(gravity, "all") != nullptr) {
			g |= WIDGET_GRAVITY_ALL;
		}
		if ((g & WIDGET_GRAVITY_LEFT_RIGHT) == 0U) {
			g |= WIDGET_GRAVITY_LEFT;
		}
		if ((g & WIDGET_GRAVITY_TOP_BOTTOM) == 0U) {
			g |= WIDGET_GRAVITY_TOP;
		}
		setGravity(g);
	}
	if (const char *visibility = info.node->getValueString("visibility", nullptr)) {
		if (SDL_strcmp(visibility, "visible") == 0) {
			setVisibility(WIDGET_VISIBILITY_VISIBLE);
		} else if (SDL_strcmp(visibility, "invisible") == 0) {
			setVisibility(WIDGET_VISIBILITY_INVISIBLE);
		} else if (SDL_strcmp(visibility, "gone") == 0) {
			setVisibility(WIDGET_VISIBILITY_GONE);
		}
	}
	if (const char *state = info.node->getValueString("state", nullptr)) {
		if (SDL_strstr(state, "disabled") != nullptr) {
			setState(WIDGET_STATE_DISABLED, true);
		}
	}
	if (const char *skin = info.node->getValueString("skin", nullptr)) {
		setSkinBg(skin);
	}
	if (TBNode *lp = info.node->getNode("lp")) {
		LayoutParams layout_params;
		if (getLayoutParams() != nullptr) {
			layout_params = *getLayoutParams();
		}
		const TBDimensionConverter *dc = g_tb_skin->getDimensionConverter();
		if (const char *str = lp->getValueString("width", nullptr)) {
			layout_params.setWidth(dc->getPxFromString(str, LayoutParams::UNSPECIFIED));
		}
		if (const char *str = lp->getValueString("height", nullptr)) {
			layout_params.setHeight(dc->getPxFromString(str, LayoutParams::UNSPECIFIED));
		}
		if (const char *str = lp->getValueString("min-width", nullptr)) {
			layout_params.min_w = dc->getPxFromString(str, LayoutParams::UNSPECIFIED);
		}
		if (const char *str = lp->getValueString("max-width", nullptr)) {
			layout_params.max_w = dc->getPxFromString(str, LayoutParams::UNSPECIFIED);
		}
		if (const char *str = lp->getValueString("pref-width", nullptr)) {
			layout_params.pref_w = dc->getPxFromString(str, LayoutParams::UNSPECIFIED);
		}
		if (const char *str = lp->getValueString("min-height", nullptr)) {
			layout_params.min_h = dc->getPxFromString(str, LayoutParams::UNSPECIFIED);
		}
		if (const char *str = lp->getValueString("max-height", nullptr)) {
			layout_params.max_h = dc->getPxFromString(str, LayoutParams::UNSPECIFIED);
		}
		if (const char *str = lp->getValueString("pref-height", nullptr)) {
			layout_params.pref_h = dc->getPxFromString(str, LayoutParams::UNSPECIFIED);
		}
		setLayoutParams(layout_params);
	}

	// Add the new widget to the hiearchy if not already added.
	if (getParent() == nullptr) {
		info.target->addChild(this, info.target->getZInflate());
	}

	// Read the font now when the widget is in the hiearchy so inheritance works.
	if (TBNode *font = info.node->getNode("font")) {
		TBFontDescription fd = getCalculatedFontDescription();
		if (const char *size = font->getValueString("size", nullptr)) {
			int new_size = g_tb_skin->getDimensionConverter()->getPxFromString(size, fd.getSize());
			fd.setSize(new_size);
		}
		if (const char *name = font->getValueString("name", nullptr)) {
			fd.setID(name);
		}
		setFontDescription(fd);
	}

	info.target->onInflateChild(this);

	if (TBNode *rect_node = info.node->getNode("rect")) {
		const TBDimensionConverter *dc = g_tb_skin->getDimensionConverter();
		TBValue &val = rect_node->getValue();
		if (val.getArrayLength() == 4) {
			setRect(TBRect(dc->getPxFromValue(val.getArray()->getValue(0), 0),
						   dc->getPxFromValue(val.getArray()->getValue(1), 0),
						   dc->getPxFromValue(val.getArray()->getValue(2), 0),
						   dc->getPxFromValue(val.getArray()->getValue(3), 0)));
		}
	}
}

TB_WIDGET_FACTORY(TBWindow, TBValue::TYPE_NULL, WIDGET_Z_TOP) {
}

TB_WIDGET_FACTORY(TBButton, TBValue::TYPE_NULL, WIDGET_Z_BOTTOM) {
}
void TBButton::onInflate(const INFLATE_INFO &info) {
	setSqueezable(info.node->getValueInt("squeezable", static_cast<int>(getSqueezable())) != 0);
	setAutoRepeat(info.node->getValueInt("auto-repeat", static_cast<int>(getAutoRepeat())) != 0);
	setToggleMode(info.node->getValueInt("toggle-mode", static_cast<int>(getToggleMode())) != 0);
	TBWidget::onInflate(info);
	if (const char *command = info.node->getValueString("command", nullptr)) {
		_command.set(command);
	}
	if (const char *varname = info.node->getValueString("varref", nullptr)) {
		_var = core::Var::get(varname, 0);
		setValue(_var->intVal());
	}
}

void TBInlineSelectBase::onInflate(const INFLATE_INFO &info) {
	Super::onInflate(info);
	if (const char *command = info.node->getValueString("command", nullptr)) {
		_command.set(command);
	}
}

TB_WIDGET_FACTORY(TBInlineSelect, TBValue::TYPE_INT, WIDGET_Z_TOP) {
}
void TBInlineSelect::onInflate(const INFLATE_INFO &info) {
	Super::onInflate(info);
	int min = info.node->getValueInt("min", getMinValue());
	int max = info.node->getValueInt("max", getMaxValue());
	setLimits(min, max);
	if (const char *varname = info.node->getValueString("varref", nullptr)) {
		_var = core::Var::get(varname, m_value);
		setValue(_var->intVal());
	}
}

TB_WIDGET_FACTORY(TBInlineSelectDouble, TBValue::TYPE_FLOAT, WIDGET_Z_TOP) {
}
void TBInlineSelectDouble::onInflate(const INFLATE_INFO &info) {
	Super::onInflate(info);
	double min = info.node->getValueFloat("min", getMinValue());
	double max = info.node->getValueFloat("max", getMaxValue());
	setLimits(min, max);
	if (const char *varname = info.node->getValueString("varref", nullptr)) {
		char buf[32];
		core::string::formatBuf(buf, sizeof(buf), "%f", (float)m_value);
		_var = core::Var::get(varname, buf);
		setValue(_var->intVal());
	}
}

TB_WIDGET_FACTORY(TBClickLabel, TBValue::TYPE_STRING, WIDGET_Z_BOTTOM) {
}

TB_WIDGET_FACTORY(TBMover, TBValue::TYPE_NULL, WIDGET_Z_TOP) {
}

TB_WIDGET_FACTORY(TBEditField, TBValue::TYPE_STRING, WIDGET_Z_TOP) {
}
void TBEditField::onInflate(const INFLATE_INFO &info) {
	setMultiline(info.node->getValueInt("multiline", 0) != 0);
	setStyling(info.node->getValueInt("styling", 0) != 0);
	setReadOnly(info.node->getValueInt("readonly", 0) != 0);
	setWrapping(info.node->getValueInt("wrap", static_cast<int>(getWrapping())) != 0);
	setAdaptToContentSize(info.node->getValueInt("adapt-to-content", static_cast<int>(getAdaptToContentSize())) != 0);
	if (const char *virtual_width = info.node->getValueString("virtual-width", nullptr)) {
		setVirtualWidth(g_tb_skin->getDimensionConverter()->getPxFromString(virtual_width, getVirtualWidth()));
	}
	if (const char *text = info.node->getValueString("placeholder", nullptr)) {
		setPlaceholderText(text);
	}
	if (const char *text_align = info.node->getValueString("text-align", nullptr)) {
		if (SDL_strcmp(text_align, "left") == 0) {
			setTextAlign(TB_TEXT_ALIGN_LEFT);
		} else if (SDL_strcmp(text_align, "center") == 0) {
			setTextAlign(TB_TEXT_ALIGN_CENTER);
		} else if (SDL_strcmp(text_align, "right") == 0) {
			setTextAlign(TB_TEXT_ALIGN_RIGHT);
		}
	}
	if (const char *type = info.node->getValueString("type", nullptr)) {
		if (stristr(type, "text") != nullptr) {
			setEditType(EDIT_TYPE_TEXT);
		} else if (stristr(type, "search") != nullptr) {
			setEditType(EDIT_TYPE_SEARCH);
		} else if (stristr(type, "password") != nullptr) {
			setEditType(EDIT_TYPE_PASSWORD);
		} else if (stristr(type, "email") != nullptr) {
			setEditType(EDIT_TYPE_EMAIL);
		} else if (stristr(type, "phone") != nullptr) {
			setEditType(EDIT_TYPE_PHONE);
		} else if (stristr(type, "url") != nullptr) {
			setEditType(EDIT_TYPE_URL);
		} else if (stristr(type, "number") != nullptr) {
			setEditType(EDIT_TYPE_NUMBER);
		}
	}
	TBWidget::onInflate(info);
	if (const char *varname = info.node->getValueString("varref", nullptr)) {
		_var = core::Var::get(varname, getText().c_str());
		setText(_var->strVal().c_str());
	}
}

TB_WIDGET_FACTORY(TBLayout, TBValue::TYPE_NULL, WIDGET_Z_TOP) {
}
void TBLayout::onInflate(const INFLATE_INFO &info) {
	if (const char *spacing = info.node->getValueString("spacing", nullptr)) {
		setSpacing(g_tb_skin->getDimensionConverter()->getPxFromString(spacing, SPACING_FROM_SKIN));
	}
	setGravity(WIDGET_GRAVITY_ALL);
	if (const char *size = info.node->getValueString("size", nullptr)) {
		LAYOUT_SIZE ls = LAYOUT_SIZE_PREFERRED;
		if (SDL_strstr(size, "available") != nullptr) {
			ls = LAYOUT_SIZE_AVAILABLE;
		} else if (SDL_strstr(size, "gravity") != nullptr) {
			ls = LAYOUT_SIZE_GRAVITY;
		} else if (SDL_strstr(size, "preferred") != nullptr) {
		} else {
			Log::debug("TBLayout: Unknown size '%s'", size);
		}
		setLayoutSize(ls);
	}
	if (const char *pos = info.node->getValueString("position", nullptr)) {
		LAYOUT_POSITION lp = LAYOUT_POSITION_CENTER;
		if ((SDL_strstr(pos, "left") != nullptr) || (SDL_strstr(pos, "top") != nullptr)) {
			lp = LAYOUT_POSITION_LEFT_TOP;
		} else if ((SDL_strstr(pos, "right") != nullptr) || (SDL_strstr(pos, "bottom") != nullptr)) {
			lp = LAYOUT_POSITION_RIGHT_BOTTOM;
		} else if (SDL_strstr(pos, "gravity") != nullptr) {
			lp = LAYOUT_POSITION_GRAVITY;
		} else if (SDL_strcmp(pos, "center") == 0) {
		} else {
			Log::debug("TBLayout: Unknown position '%s'", pos);
		}
		setLayoutPosition(lp);
	}
	if (const char *pos = info.node->getValueString("overflow", nullptr)) {
		LAYOUT_OVERFLOW lo = LAYOUT_OVERFLOW_CLIP;
		if (SDL_strstr(pos, "scroll") != nullptr) {
			lo = LAYOUT_OVERFLOW_SCROLL;
		} else if (SDL_strstr(pos, "clip") != nullptr) {
		} else {
			Log::debug("TBLayout: Unknown overflow '%s'", pos);
		}
		setLayoutOverflow(lo);
	}
	if (const char *dist = info.node->getValueString("distribution", nullptr)) {
		LAYOUT_DISTRIBUTION ld = LAYOUT_DISTRIBUTION_PREFERRED;
		if (SDL_strstr(dist, "available") != nullptr) {
			ld = LAYOUT_DISTRIBUTION_AVAILABLE;
		} else if (SDL_strstr(dist, "gravity") != nullptr) {
			ld = LAYOUT_DISTRIBUTION_GRAVITY;
		} else if (SDL_strstr(dist, "preferred") != nullptr) {
		} else {
			Log::debug("TBLayout: Unknown distribution '%s'", dist);
		}
		setLayoutDistribution(ld);
	}
	if (const char *dist = info.node->getValueString("distribution-position", nullptr)) {
		LAYOUT_DISTRIBUTION_POSITION ld = LAYOUT_DISTRIBUTION_POSITION_CENTER;
		if ((SDL_strstr(dist, "left") != nullptr) || (SDL_strstr(dist, "top") != nullptr)) {
			ld = LAYOUT_DISTRIBUTION_POSITION_LEFT_TOP;
		} else if ((SDL_strstr(dist, "right") != nullptr) || (SDL_strstr(dist, "bottom") != nullptr)) {
			ld = LAYOUT_DISTRIBUTION_POSITION_RIGHT_BOTTOM;
		}
		setLayoutDistributionPosition(ld);
	}
	TBWidget::onInflate(info);
}

TB_WIDGET_FACTORY(TBScrollContainer, TBValue::TYPE_NULL, WIDGET_Z_TOP) {
}
void TBScrollContainer::onInflate(const INFLATE_INFO &info) {
	setGravity(WIDGET_GRAVITY_ALL);
	setAdaptContentSize(info.node->getValueInt("adapt-content", static_cast<int>(getAdaptContentSize())) != 0);
	setAdaptToContentSize(info.node->getValueInt("adapt-to-content", static_cast<int>(getAdaptToContentSize())) != 0);
	if (const char *mode = info.node->getValueString("scroll-mode", nullptr)) {
		if (SDL_strcmp(mode, "xy") == 0) {
			setScrollMode(SCROLL_MODE_X_Y);
		} else if (SDL_strcmp(mode, "y") == 0) {
			setScrollMode(SCROLL_MODE_Y);
		} else if (SDL_strcmp(mode, "y-auto") == 0) {
			setScrollMode(SCROLL_MODE_Y_AUTO);
		} else if (SDL_strcmp(mode, "auto") == 0) {
			setScrollMode(SCROLL_MODE_X_AUTO_Y_AUTO);
		} else if (SDL_strcmp(mode, "off") == 0) {
			setScrollMode(SCROLL_MODE_OFF);
		} else {
			Log::debug("TBScrollContainer: Unknown scroll-mode '%s'", mode);
		}
	}
	TBWidget::onInflate(info);
}

TB_WIDGET_FACTORY(TBTabContainer, TBValue::TYPE_NULL, WIDGET_Z_TOP) {
}
void TBTabContainer::onInflate(const INFLATE_INFO &info) {
	TBWidget::onInflate(info);

	if (const char *align = info.node->getValueString("align", nullptr)) {
		if (SDL_strcmp(align, "left") == 0) {
			setAlignment(TB_ALIGN_LEFT);
		} else if (SDL_strcmp(align, "top") == 0) {
			setAlignment(TB_ALIGN_TOP);
		} else if (SDL_strcmp(align, "right") == 0) {
			setAlignment(TB_ALIGN_RIGHT);
		} else if (SDL_strcmp(align, "bottom") == 0) {
			setAlignment(TB_ALIGN_BOTTOM);
		} else {
			Log::debug("TBTabContainer: Unknown align '%s'", align);
		}
	}
	// Allow additional attributes to be specified for the "tabs", "content" and "root" layouts by
	// calling onInflate.
	if (TBNode *tabs = info.node->getNode("tabs")) {
		// Inflate the tabs widgets into the tab layout.
		TBLayout *tab_layout = getTabLayout();
		info.reader->loadNodeTree(tab_layout, tabs);

		INFLATE_INFO inflate_info(info.reader, tab_layout->getContentRoot(), tabs, TBValue::TYPE_NULL);
		tab_layout->onInflate(inflate_info);
	}
	if (TBNode *tabs = info.node->getNode("content")) {
		INFLATE_INFO inflate_info(info.reader, getContentRoot(), tabs, TBValue::TYPE_NULL);
		getContentRoot()->onInflate(inflate_info);
	}
	if (TBNode *tabs = info.node->getNode("root")) {
		INFLATE_INFO inflate_info(info.reader, &m_root_layout, tabs, TBValue::TYPE_NULL);
		m_root_layout.onInflate(inflate_info);
	}
}

TB_WIDGET_FACTORY(TBScrollBar, TBValue::TYPE_FLOAT, WIDGET_Z_TOP) {
}
void TBScrollBar::onInflate(const INFLATE_INFO &info) {
	const char *axis = info.node->getValueString("axis", "x");
	setAxis(*axis == 'x' ? AXIS_X : AXIS_Y);
	setGravity(*axis == 'x' ? WIDGET_GRAVITY_LEFT_RIGHT : WIDGET_GRAVITY_TOP_BOTTOM);
	TBWidget::onInflate(info);
}

TB_WIDGET_FACTORY(TBSlider, TBValue::TYPE_FLOAT, WIDGET_Z_TOP) {
}
void TBSlider::onInflate(const INFLATE_INFO &info) {
	const char *axis = info.node->getValueString("axis", "x");
	setAxis(*axis == 'x' ? AXIS_X : AXIS_Y);
	setGravity(*axis == 'x' ? WIDGET_GRAVITY_LEFT_RIGHT : WIDGET_GRAVITY_TOP_BOTTOM);
	double min = (double)info.node->getValueFloat("min", (float)getMinValue());
	double max = (double)info.node->getValueFloat("max", (float)getMaxValue());
	setLimits(min, max);
	TBWidget::onInflate(info);
	if (const char *command = info.node->getValueString("command", nullptr)) {
		_command.set(command);
	}
	if (const char *varname = info.node->getValueString("varref", nullptr)) {
		_var = core::Var::get(varname, m_value);
		setValueDouble(_var->floatVal());
	}
}

void readItems(TBNode *node, TBGenericStringItemSource *targetSource) {
	// If there is a items node, loop through all its children and add
	// items to the target item source.
	if (TBNode *items = node->getNode("items")) {
		for (TBNode *n = items->getFirstChild(); n != nullptr; n = n->getNext()) {
			if (SDL_strcmp(n->getName(), "item") != 0) {
				continue;
			}
			const char *item_str = n->getValueString("text", "");
			TBID item_id;
			if (TBNode *n_id = n->getNode("id")) {
				TBWidgetsReader::setIDFromNode(item_id, n_id);
			}

			TBGenericStringItem *item = new TBGenericStringItem(item_str, item_id);
			if ((item == nullptr) || !targetSource->addItem(item)) {
				// Out of memory
				delete item;
				break;
			}
		}
	}
}

TB_WIDGET_FACTORY(TBSelectList, TBValue::TYPE_INT, WIDGET_Z_TOP) {
}
void TBSelectList::onInflate(const INFLATE_INFO &info) {
	// Read items (if there is any) into the default source
	readItems(info.node, getDefaultSource());
	TBWidget::onInflate(info);
}

TB_WIDGET_FACTORY(TBSelectDropdown, TBValue::TYPE_INT, WIDGET_Z_TOP) {
}
void TBSelectDropdown::onInflate(const INFLATE_INFO &info) {
	// Read items (if there is any) into the default source
	readItems(info.node, getDefaultSource());
	TBWidget::onInflate(info);
}

void TBRadioCheckBox::onInflate(const INFLATE_INFO &info) {
	TBWidget::onInflate(info);
	if (const char *command = info.node->getValueString("command", nullptr)) {
		_command.set(command);
	}
	if (const char *varname = info.node->getValueString("varref", nullptr)) {
		_var = core::Var::get(varname, m_value);
		setValue(_var->intVal());
	}
}

TB_WIDGET_FACTORY(TBCheckBox, TBValue::TYPE_INT, WIDGET_Z_TOP) {
}
TB_WIDGET_FACTORY(TBRadioButton, TBValue::TYPE_INT, WIDGET_Z_TOP) {
}

TB_WIDGET_FACTORY(TBTextField, TBValue::TYPE_STRING, WIDGET_Z_TOP) {
}
void TBTextField::onInflate(const INFLATE_INFO &info) {
	if (const char *text_align = info.node->getValueString("text-align", nullptr)) {
		if (SDL_strcmp(text_align, "left") == 0) {
			setTextAlign(TB_TEXT_ALIGN_LEFT);
		} else if (SDL_strcmp(text_align, "center") == 0) {
			setTextAlign(TB_TEXT_ALIGN_CENTER);
		} else if (SDL_strcmp(text_align, "right") == 0) {
			setTextAlign(TB_TEXT_ALIGN_RIGHT);
		}
	}
	TBWidget::onInflate(info);
}

TB_WIDGET_FACTORY(TBSkinImage, TBValue::TYPE_NULL, WIDGET_Z_TOP) {
}
TB_WIDGET_FACTORY(TBSeparator, TBValue::TYPE_NULL, WIDGET_Z_TOP) {
}
TB_WIDGET_FACTORY(TBProgressSpinner, TBValue::TYPE_INT, WIDGET_Z_TOP) {
}
TB_WIDGET_FACTORY(TBContainer, TBValue::TYPE_NULL, WIDGET_Z_TOP) {
}
TB_WIDGET_FACTORY(TBSectionHeader, TBValue::TYPE_INT, WIDGET_Z_TOP) {
}
TB_WIDGET_FACTORY(TBSection, TBValue::TYPE_INT, WIDGET_Z_TOP) {
}

TB_WIDGET_FACTORY(TBToggleContainer, TBValue::TYPE_INT, WIDGET_Z_TOP) {
}
void TBToggleContainer::onInflate(const INFLATE_INFO &info) {
	if (const char *toggle = info.node->getValueString("toggle", nullptr)) {
		if (stristr(toggle, "enabled") != nullptr) {
			setToggle(TBToggleContainer::TOGGLE_ENABLED);
		} else if (stristr(toggle, "opacity") != nullptr) {
			setToggle(TBToggleContainer::TOGGLE_OPACITY);
		} else if (stristr(toggle, "expanded") != nullptr) {
			setToggle(TBToggleContainer::TOGGLE_EXPANDED);
		}
	}
	setInvert(info.node->getValueInt("invert", static_cast<int>(getInvert())) != 0);
	TBWidget::onInflate(info);
}

TB_WIDGET_FACTORY(TBImageWidget, TBValue::TYPE_NULL, WIDGET_Z_TOP) {
}
void TBImageWidget::onInflate(const INFLATE_INFO &info) {
	if (const char *filename = info.node->getValueString("filename", nullptr)) {
		setImage(filename);
	}
	TBWidget::onInflate(info);
}

// We can't use a linked list object since we don't know if its constructor
// would run before of after any widget factory constructor that add itself
// to it. Using a manual one way link list is very simple.
TBWidgetFactory *g_registered_factories = nullptr;

TBWidgetFactory::TBWidgetFactory(const char *name, TBValue::TYPE syncType)
	: name(name), sync_type(syncType), next_registered_wf(nullptr) {
}

void TBWidgetFactory::doRegister() {
	next_registered_wf = g_registered_factories;
	g_registered_factories = this;
}

// == TBWidgetsReader ===================================

TBWidgetsReader *TBWidgetsReader::create() {
	TBWidgetsReader *w_reader = new TBWidgetsReader;
	if ((w_reader == nullptr) || !w_reader->init()) {
		delete w_reader;
		return nullptr;
	}
	return w_reader;
}

bool TBWidgetsReader::init() {
	for (TBWidgetFactory *wf = g_registered_factories; wf != nullptr; wf = wf->next_registered_wf) {
		if (!addFactory(wf)) {
			return false;
		}
	}
	return true;
}

TBWidgetsReader::~TBWidgetsReader() {
}

bool TBWidgetsReader::loadFile(TBWidget *target, const char *filename) {
	TBNode node;
	if (!node.readFile(filename)) {
		return false;
	}
	loadNodeTree(target, &node);
	return true;
}

bool TBWidgetsReader::loadData(TBWidget *target, const char *data) {
	TBNode node;
	node.readData(data);
	loadNodeTree(target, &node);
	return true;
}

bool TBWidgetsReader::loadData(TBWidget *target, const char *data, int dataLen) {
	TBNode node;
	node.readData(data, dataLen);
	loadNodeTree(target, &node);
	return true;
}

void TBWidgetsReader::loadNodeTree(TBWidget *target, TBNode *node) {
	// Iterate through all nodes and create widgets
	for (TBNode *child = node->getFirstChild(); child != nullptr; child = child->getNext()) {
		createWidget(target, child);
	}
}

void TBWidgetsReader::setIDFromNode(TBID &id, TBNode *node) {
	if (node == nullptr) {
		return;
	}
	if (node->getValue().isString()) {
		id.set(node->getValue().getString());
	} else {
		id.set(node->getValue().getInt());
	}
}

bool TBWidgetsReader::createWidget(TBWidget *target, TBNode *node) {
	// Find a widget creator from the node name
	TBWidgetFactory *wc = nullptr;
	for (wc = factories.getFirst(); wc != nullptr; wc = wc->getNext()) {
		if (SDL_strcmp(node->getName(), wc->name) == 0) {
			break;
		}
	}
	if (wc == nullptr) {
		return false;
	}

	// Create the widget
	INFLATE_INFO info(this, target->getContentRoot(), node, wc->sync_type);
	TBWidget *new_widget = wc->create(&info);
	if (new_widget == nullptr) {
		return false;
	}

	// Read properties and add i to the hierarchy.
	new_widget->onInflate(info);

	// If this core_assert is trigged, you probably forgot to call TBWidget::onInflate from an overridden version.
	core_assert(new_widget->getParent());

	// Iterate through all nodes and create widgets
	for (TBNode *n = node->getFirstChild(); n != nullptr; n = n->getNext()) {
		createWidget(new_widget, n);
	}

	if (node->getValueInt("autofocus", 0) != 0) {
		new_widget->setFocus(WIDGET_FOCUS_REASON_UNKNOWN);
	}

	return true;
}

} // namespace tb
