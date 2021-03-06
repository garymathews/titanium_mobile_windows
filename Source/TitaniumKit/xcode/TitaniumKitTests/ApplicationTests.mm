/**
 * Titanium
 *
 * Copyright (c) 2014 by Appcelerator, Inc. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#include "Titanium/Titanium.hpp"
#include "NativeGlobalObjectExample.hpp"
#include "NativeAPIExample.hpp"
#include "NativeViewExample.hpp"
#include "NativeWindowExample.hpp"
#include "NativeButtonExample.hpp"
#import <XCTest/XCTest.h>

using namespace HAL;

@interface ApplicationTests : XCTestCase
@end

@implementation ApplicationTests {
  JSContextGroup js_context_group;
}

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)testApplication {
  std::string app_js = R"js(
  'use strict';
  
  var button = Ti.UI.createButton({
  top: 20,
  bottom: 20,
  width: 200,
  height: Ti.UI.SIZE
  });
  
  button.title = 'Neeraj Says...';
  
  button.addEventListener('click', function(e) {
    Ti.API.warn('Goals without a timeline are just a dream.');
  });
  
  var view = Ti.UI.createView({
  bottom: 50,
  right: 50,
  width: 300,
  height: Ti.UI.SIZE
  });
  
  view.add(button);
  
  var window = Ti.UI.createWindow();
  window.add(view);
  
  window.open();
  
  Ti.API.info('ng.js running...');
  )js";
  
  JSContext js_context = js_context_group.CreateContext(JSExport<NativeGlobalObjectExample>::Class());
  Titanium::Application app = Titanium::ApplicationBuilder(js_context)
  .APIObject(js_context.CreateObject<NativeAPIExample>())
  .ViewObject(js_context.CreateObject<NativeViewExample>())
  .WindowObject(js_context.CreateObject<NativeWindowExample>())
  .ButtonObject(js_context.CreateObject<NativeButtonExample>())
  .build();
  
  auto global_object_ptr = app.get_context().get_global_object().GetPrivate<NativeGlobalObjectExample>();
  global_object_ptr -> add_require("/app.js", app_js);
  
  JSValue reslut = app.Run("app.js");
}

- (void)xtestCrash {
  std::string app_js = R"js(
  'use strict';
  
  for (var i = 0; i < 1000; i++) {
    var b = Ti.UI.createButton();
    Ti.API.info("MDL: i = " + i);
    b = null;
  }
  )js";
  
  JSContext js_context = js_context_group.CreateContext(JSExport<NativeGlobalObjectExample>::Class());
  Titanium::Application app = Titanium::ApplicationBuilder(js_context)
  .APIObject(js_context.CreateObject<NativeAPIExample>())
  .ViewObject(js_context.CreateObject<NativeViewExample>())
  .WindowObject(js_context.CreateObject<NativeWindowExample>())
  .ButtonObject(js_context.CreateObject<NativeButtonExample>())
  .build();
  
  auto global_object_ptr = app.get_context().get_global_object().GetPrivate<NativeGlobalObjectExample>();
  global_object_ptr -> add_require("/app.js", app_js);
  
  JSValue reslut = app.Run("app.js");
}

@end
