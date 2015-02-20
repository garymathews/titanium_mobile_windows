/**
 * TitaniumKit
 *
 * Copyright (c) 2014 by Appcelerator, Inc. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#include "Titanium/Titanium.hpp"
#include "NativeListViewExample.hpp"
#include "Titanium/UI/listview_js.hpp"
#include "gtest/gtest.h"

#define XCTAssertEqual ASSERT_EQ
#define XCTAssertNotEqual ASSERT_NE
#define XCTAssertTrue ASSERT_TRUE
#define XCTAssertFalse ASSERT_FALSE
#define XCTAssertNoThrow ASSERT_NO_THROW

using namespace Titanium;
using namespace HAL;

class ListViewTests : public testing::Test
{
protected:
	virtual void SetUp()
	{
	}

	virtual void TearDown()
	{
	}

	JSContextGroup js_context_group;

	// Titanium mock functions
	std::string TI_INIT_SCRIPT = R"js(
		this.Ti = {};
		this.Ti.API = {};
		this.Ti.API.warn = function(E) {throw E;};
		this.Ti.UI  = {};
		this.Ti.UI.createView = function(option) {
			return {
				test:'Ti.UI.View',
				option:option,
				add:function(view) {
					this.view_count = this.view_count || 0;
					this.view_count++;
				},
				applyProperties: function(option) {}
			};
		};
		this.Ti.UI.createImageView = function(option) {
			return {
				test:'Ti.UI.ImageView',
				option:option,
				applyProperties: function(option) {}
			};
		};
		this.Ti.UI.createLabel = function(option) {
			return {
				test:'Ti.UI.Label',
				option:option,
				applyProperties: function(option) {}
			};
		};

		this.Ti.UI.LIST_ITEM_TEMPLATE_DEFAULT = '1';
	)js";

	// default template
	std::string LISTVIEW_LIST_ITEM_TEMPLATE_DEFAULT = R"js(
		var LISTVIEW_LIST_ITEM_TEMPLATE_DEFAULT = {
			childTemplates: [
				{
					type: 'Ti.UI.ImageView',
					bindId: 'image',
					properties: {
						left: 0, top: 0
					}
				},
				{
					type: 'Ti.UI.Label', 
					bindId: 'text',
					properties: {
						color: 'black',
						left: 0, top: 0,
					}
				}
			]
		}; 
		LISTVIEW_LIST_ITEM_TEMPLATE_DEFAULT;
	)js";

	// custom template
	std::string LISTVIEW_LIST_ITEM_TEMPLATE_CUSTOM = R"js(
		var LISTVIEW_LIST_ITEM_TEMPLATE_CUSTOM = {
			childTemplates: [
				{                            // Image justified left
					type: 'Ti.UI.ImageView', // Use an image view for the image
					bindId: 'pic',           // Maps to a custom pic property of the item data
					properties: {            // Sets the image view  properties
						width: '50dp', height: '50dp', left: 0
					}
				},
				{                            // Title 
					type: 'Ti.UI.Label',     // Use a label for the title 
					bindId: 'info',          // Maps to a custom info property of the item data
					properties: {            // Sets the label properties
						color: 'black',
						font: { fontFamily:'Arial', fontSize: '20dp', fontWeight:'bold' },
						left: '60dp', top: 0,
					}
				},
				{                            // Subtitle
					type: 'Ti.UI.Label',     // Use a label for the subtitle
					bindId: 'es_info',       // Maps to a custom es_info property of the item data
					properties: {            // Sets the label properties
						color: 'gray',
						font: { fontFamily:'Arial', fontSize: '14dp' },
						left: '60dp', top: '25dp',
					}
				}
			]
		};
		LISTVIEW_LIST_ITEM_TEMPLATE_CUSTOM;
	)js";
};

TEST_F(ListViewTests, basic_functionality)
{
	JSContext js_context = js_context_group.CreateContext(JSExport<Titanium::GlobalObject>::Class());
	auto global_object = js_context.get_global_object();

	XCTAssertFalse(global_object.HasProperty("Titanium"));
	auto Titanium = js_context.CreateObject();
	global_object.SetProperty("Titanium", Titanium, {JSPropertyAttribute::ReadOnly, JSPropertyAttribute::DontDelete});
	XCTAssertTrue(global_object.HasProperty("Titanium"));

	// Make the alias "Ti" for the "Titanium" property.
	XCTAssertFalse(global_object.HasProperty("Ti"));
	global_object.SetProperty("Ti", Titanium, {JSPropertyAttribute::ReadOnly, JSPropertyAttribute::DontDelete});
	XCTAssertTrue(global_object.HasProperty("Ti"));

	XCTAssertFalse(Titanium.HasProperty("UI"));
	auto UI = js_context.CreateObject(JSExport<Titanium::UIModule>::Class());
	Titanium.SetProperty("UI", UI, {JSPropertyAttribute::ReadOnly, JSPropertyAttribute::DontDelete});
	XCTAssertTrue(Titanium.HasProperty("UI"));

	XCTAssertTrue(UI.HasProperty("createListView"));
	XCTAssertTrue(UI.HasProperty("createListSection"));

	auto View = js_context.CreateObject(JSExport<NativeListViewExample>::Class());
	auto view_ptr = View.GetPrivate<NativeListViewExample>();
	XCTAssertNotEqual(nullptr, view_ptr);

	UI.SetProperty("ListView", View);
	UI.SetProperty("ListSection", js_context.CreateObject(JSExport<Titanium::UI::ListSection>::Class()));

	XCTAssertTrue(UI.HasProperty("ListView"));
	XCTAssertTrue(UI.HasProperty("ListSection"));

	JSValue result = js_context.CreateNull();
	XCTAssertNoThrow(result = js_context.JSEvaluateScript("Ti.UI.createListView();"));
	XCTAssertTrue(result.IsObject());
	JSObject view = static_cast<JSObject>(result);

	XCTAssertTrue(view.HasProperty("sections"));
	XCTAssertTrue(view.HasProperty("footerTitle"));
	XCTAssertTrue(view.HasProperty("headerTitle"));
	XCTAssertTrue(view.HasProperty("searchText"));
	XCTAssertTrue(view.HasProperty("footerView"));
	XCTAssertTrue(view.HasProperty("headerView"));
	XCTAssertTrue(view.HasProperty("searchView"));
	XCTAssertTrue(view.HasProperty("sectionCount"));
	XCTAssertTrue(view.HasProperty("caseInsensitiveSearch"));
	XCTAssertTrue(view.HasProperty("showVerticalScrollIndicator"));
	XCTAssertTrue(view.HasProperty("separatorColor"));
	XCTAssertTrue(view.HasProperty("defaultItemTemplate"));

	XCTAssertTrue(view.HasProperty("scrollToItem"));
	XCTAssertTrue(view.HasProperty("appendSection"));
	XCTAssertTrue(view.HasProperty("deleteSectionAt"));
	XCTAssertTrue(view.HasProperty("insertSectionAt"));
	XCTAssertTrue(view.HasProperty("replaceSectionAt"));
	XCTAssertTrue(view.HasProperty("setMarker"));

	XCTAssertNoThrow(result = js_context.JSEvaluateScript("Ti.UI.createListSection();"));
	XCTAssertTrue(result.IsObject());
	JSObject section = static_cast<JSObject>(result);

	XCTAssertTrue(section.HasProperty("footerTitle"));
	XCTAssertTrue(section.HasProperty("footerView"));
	XCTAssertTrue(section.HasProperty("headerTitle"));
	XCTAssertTrue(section.HasProperty("headerView"));
	XCTAssertTrue(section.HasProperty("items"));
	XCTAssertTrue(section.HasProperty("appendItems"));
	XCTAssertTrue(section.HasProperty("deleteItemsAt"));
	XCTAssertTrue(section.HasProperty("insertItemsAt"));
	XCTAssertTrue(section.HasProperty("replaceItemsAt"));
	XCTAssertTrue(section.HasProperty("updateItemAt"));
	XCTAssertTrue(section.HasProperty("getItemAt"));

}

TEST_F(ListViewTests, resource_listview_js_default)
{
	JSContext js_context = js_context_group.CreateContext(JSExport<Titanium::GlobalObject>::Class());
	auto global_object = js_context.get_global_object();

	js_context.JSEvaluateScript(TI_INIT_SCRIPT, global_object);
	XCTAssertTrue(js_context.JSEvaluateScript("Ti.UI").IsObject());
	XCTAssertTrue(js_context.JSEvaluateScript("Ti.UI.createView").IsObject());
	XCTAssertTrue(js_context.JSEvaluateScript("Ti.UI.createImageView").IsObject());
	XCTAssertTrue(js_context.JSEvaluateScript("Ti.UI.createLabel").IsObject());

	auto export_object = js_context.CreateObject();
	export_object.SetProperty("global", global_object);

	js_context.JSEvaluateScript(listview_js, export_object);

	XCTAssertTrue(export_object.HasProperty("exports"));
	auto js_exports = static_cast<JSObject>(export_object.GetProperty("exports"));

	auto js_func = js_exports.GetProperty("processChildTemplates");
	XCTAssertTrue(js_func.IsObject());
	auto func = static_cast<JSObject>(js_func);
	XCTAssertTrue(func.IsFunction());

	JSValue js_listview_template = js_context.CreateNull();
	XCTAssertNoThrow(js_listview_template = js_context.JSEvaluateScript(LISTVIEW_LIST_ITEM_TEMPLATE_DEFAULT));
	XCTAssertTrue(js_listview_template.IsObject());

	global_object.SetProperty("TEST_TEMPLATE", js_listview_template);

	XCTAssertNoThrow(func({js_listview_template}, func));

	XCTAssertTrue(js_context.JSEvaluateScript("TEST_TEMPLATE").IsObject());
	XCTAssertTrue(js_context.JSEvaluateScript("TEST_TEMPLATE.childTemplates").IsObject());
	XCTAssertTrue(js_context.JSEvaluateScript("TEST_TEMPLATE.childTemplates[0].createView").IsObject());

	// Test if UI constructor functions are looked up
	XCTAssertTrue(static_cast<bool>(js_context.JSEvaluateScript("TEST_TEMPLATE.childTemplates[0].createView === Ti.UI.createImageView")));
	XCTAssertTrue(static_cast<bool>(js_context.JSEvaluateScript("TEST_TEMPLATE.childTemplates[1].createView === Ti.UI.createLabel")));

	// Test if UI constructor properties are set
	XCTAssertTrue(static_cast<bool>(js_context.JSEvaluateScript("TEST_TEMPLATE.childTemplates[0].createView.properties.left === 0")));
	XCTAssertTrue(static_cast<bool>(js_context.JSEvaluateScript("TEST_TEMPLATE.childTemplates[1].createView.properties.left === 0")));
}

TEST_F(ListViewTests, resource_listview_js_custom)
{
	JSContext js_context = js_context_group.CreateContext(JSExport<Titanium::GlobalObject>::Class());
	auto global_object = js_context.get_global_object();

	js_context.JSEvaluateScript(TI_INIT_SCRIPT, global_object);
	XCTAssertTrue(js_context.JSEvaluateScript("Ti.UI").IsObject());
	XCTAssertTrue(js_context.JSEvaluateScript("Ti.UI.createView").IsObject());
	XCTAssertTrue(js_context.JSEvaluateScript("Ti.UI.createImageView").IsObject());
	XCTAssertTrue(js_context.JSEvaluateScript("Ti.UI.createLabel").IsObject());

	auto export_object = js_context.CreateObject();
	export_object.SetProperty("global", global_object);

	js_context.JSEvaluateScript(listview_js, export_object);

	XCTAssertTrue(export_object.HasProperty("exports"));
	auto js_exports = static_cast<JSObject>(export_object.GetProperty("exports"));

	auto js_func = js_exports.GetProperty("processChildTemplates");
	XCTAssertTrue(js_func.IsObject());
	auto func = static_cast<JSObject>(js_func);
	XCTAssertTrue(func.IsFunction());

	JSValue js_listview_template = js_context.CreateNull();
	XCTAssertNoThrow(js_listview_template = js_context.JSEvaluateScript(LISTVIEW_LIST_ITEM_TEMPLATE_CUSTOM));
	XCTAssertTrue(js_listview_template.IsObject());

	global_object.SetProperty("TEST_TEMPLATE", js_listview_template);

	XCTAssertNoThrow(func({js_listview_template}, func));

	XCTAssertTrue(js_context.JSEvaluateScript("TEST_TEMPLATE").IsObject());
	XCTAssertTrue(js_context.JSEvaluateScript("TEST_TEMPLATE.childTemplates").IsObject());
	XCTAssertTrue(js_context.JSEvaluateScript("TEST_TEMPLATE.childTemplates[0].createView").IsObject());

	// Test if UI constructor functions are looked up
	XCTAssertTrue(static_cast<bool>(js_context.JSEvaluateScript("TEST_TEMPLATE.childTemplates[0].createView === Ti.UI.createImageView")));
	XCTAssertTrue(static_cast<bool>(js_context.JSEvaluateScript("TEST_TEMPLATE.childTemplates[1].createView === Ti.UI.createLabel")));
	XCTAssertTrue(static_cast<bool>(js_context.JSEvaluateScript("TEST_TEMPLATE.childTemplates[2].createView === Ti.UI.createLabel")));

	// Test if UI constructor properties are set
	XCTAssertTrue(static_cast<bool>(js_context.JSEvaluateScript("TEST_TEMPLATE.childTemplates[0].createView.properties.width == '50dp'")));
	XCTAssertTrue(static_cast<bool>(js_context.JSEvaluateScript("TEST_TEMPLATE.childTemplates[1].createView.properties.left  == '60dp'")));
	XCTAssertTrue(static_cast<bool>(js_context.JSEvaluateScript("TEST_TEMPLATE.childTemplates[2].createView.properties.top   == '25dp'")));
}

TEST_F(ListViewTests, createSectionView_resource_listview_js_default)
{
	JSContext js_context = js_context_group.CreateContext(JSExport<Titanium::GlobalObject>::Class());
	auto global_object = js_context.get_global_object();

	js_context.JSEvaluateScript(TI_INIT_SCRIPT, global_object);
	XCTAssertTrue(js_context.JSEvaluateScript("Ti.UI").IsObject());
	XCTAssertTrue(js_context.JSEvaluateScript("Ti.UI.createView").IsObject());
	XCTAssertTrue(js_context.JSEvaluateScript("Ti.UI.createImageView").IsObject());
	XCTAssertTrue(js_context.JSEvaluateScript("Ti.UI.createLabel").IsObject());

	auto export_object = js_context.CreateObject();
	export_object.SetProperty("global", global_object);

	js_context.JSEvaluateScript(listview_js, export_object);

	XCTAssertTrue(export_object.HasProperty("exports"));
	auto js_exports = static_cast<JSObject>(export_object.GetProperty("exports"));

	auto js_func = js_exports.GetProperty("createSectionView");
	XCTAssertTrue(js_func.IsObject());
	auto func = static_cast<JSObject>(js_func);
	XCTAssertTrue(func.IsFunction());

	// Ti.UI.ListView
	std::string js_listview_script = R"js(
		var listview = {
			defaultItemTemplate: Ti.UI.LIST_ITEM_TEMPLATE_DEFAULT
		};
		listview;
	)js";

	// Ti.UI.ListSection
	std::string js_section_script = R"js(
		var section = {
			items: [
				{properties: { title: 'Carrots'}},
				{properties: { title: 'Potatoes'}}
			]
		};
		section;
	)js";

	JSValue result = js_context.CreateNull();
	auto js_listview = static_cast<JSObject>(js_context.JSEvaluateScript(js_listview_script));
	auto js_section  = static_cast<JSObject>(js_context.JSEvaluateScript(js_section_script));

	XCTAssertTrue(js_listview.HasProperty("defaultItemTemplate"));
	XCTAssertTrue(js_section.HasProperty("items"));

	// Run createSectionView
	const std::vector<JSValue> args {js_listview, js_section};
	result = func(args, js_exports);

	XCTAssertTrue(result.IsObject());
	auto js_result = static_cast<JSObject>(result);

	XCTAssertTrue(js_result.IsArray());
	XCTAssertTrue(static_cast<uint32_t>(js_result.GetProperty("length"))==2);

	auto view1 = static_cast<JSObject>(js_result.GetProperty(0));
	auto view2 = static_cast<JSObject>(js_result.GetProperty(1));

	XCTAssertEqual("Ti.UI.View", static_cast<std::string>(view1.GetProperty("test")));
	XCTAssertEqual("Ti.UI.View", static_cast<std::string>(view2.GetProperty("test")));

	XCTAssertTrue(static_cast<uint32_t>(view1.GetProperty("view_count"))==1);
	XCTAssertTrue(static_cast<uint32_t>(view2.GetProperty("view_count"))==1);
}

TEST_F(ListViewTests, createSectionView_resource_listview_js_custom)
{
	JSContext js_context = js_context_group.CreateContext(JSExport<Titanium::GlobalObject>::Class());
	auto global_object = js_context.get_global_object();

	js_context.JSEvaluateScript(TI_INIT_SCRIPT, global_object);
	XCTAssertTrue(js_context.JSEvaluateScript("Ti.UI").IsObject());
	XCTAssertTrue(js_context.JSEvaluateScript("Ti.UI.createView").IsObject());
	XCTAssertTrue(js_context.JSEvaluateScript("Ti.UI.createImageView").IsObject());
	XCTAssertTrue(js_context.JSEvaluateScript("Ti.UI.createLabel").IsObject());

	auto export_object = js_context.CreateObject();
	export_object.SetProperty("global", global_object);

	js_context.JSEvaluateScript(listview_js, export_object);

	XCTAssertTrue(export_object.HasProperty("exports"));
	auto js_exports = static_cast<JSObject>(export_object.GetProperty("exports"));

	auto js_func = js_exports.GetProperty("createSectionView");
	XCTAssertTrue(js_func.IsObject());
	auto func = static_cast<JSObject>(js_func);
	XCTAssertTrue(func.IsFunction());

	// Ti.UI.ListView
	std::string js_listview_script = R"js(
		var listview = {
			templates: {
				TEST: LISTVIEW_LIST_ITEM_TEMPLATE_CUSTOM
			},
			defaultItemTemplate: 'TEST'
		};
		listview;
	)js";

	// Ti.UI.ListSection
	std::string js_section_script = R"js(
		var section = {
			items: [
				{ info: {text: 'Carrot'}, es_info: {text: 'Zanahoria'}, pic: {image: 'carrot.png'}},
				{ info: {text: 'Potato'}, es_info: {text: 'Patata'}, pic: {image: 'potato.png'}}
			]
		};
		section;
	)js";

	JSValue result = js_context.CreateNull();
	auto js_listview_custom_template = js_context.JSEvaluateScript(LISTVIEW_LIST_ITEM_TEMPLATE_CUSTOM);
	XCTAssertTrue(js_listview_custom_template.IsObject());
	global_object.SetProperty("LISTVIEW_LIST_ITEM_TEMPLATE_CUSTOM", js_listview_custom_template);

	auto js_listview = static_cast<JSObject>(js_context.JSEvaluateScript(js_listview_script));
	auto js_section  = static_cast<JSObject>(js_context.JSEvaluateScript(js_section_script));

	XCTAssertTrue(js_listview.HasProperty("defaultItemTemplate"));
	XCTAssertTrue(js_listview.HasProperty("templates"));
	auto js_listview_templates = js_listview.GetProperty("templates");
	XCTAssertTrue(js_listview_templates.IsObject());
	XCTAssertTrue(static_cast<JSObject>(js_listview_templates).HasProperty("TEST"));
	XCTAssertTrue(js_section.HasProperty("items"));

	// Run createSectionView
	const std::vector<JSValue> args {js_listview, js_section};
	result = func(args, js_exports);

	XCTAssertTrue(result.IsObject());
	auto js_result = static_cast<JSObject>(result);

	XCTAssertTrue(js_result.IsArray());
	XCTAssertTrue(static_cast<uint32_t>(js_result.GetProperty("length"))==2);

	auto view1 = static_cast<JSObject>(js_result.GetProperty(0));
	auto view2 = static_cast<JSObject>(js_result.GetProperty(1));

	XCTAssertEqual("Ti.UI.View", static_cast<std::string>(view1.GetProperty("test")));
	XCTAssertEqual("Ti.UI.View", static_cast<std::string>(view2.GetProperty("test")));

	XCTAssertEqual(3, static_cast<uint32_t>(view1.GetProperty("view_count")));
	XCTAssertEqual(3, static_cast<uint32_t>(view2.GetProperty("view_count")));
}
