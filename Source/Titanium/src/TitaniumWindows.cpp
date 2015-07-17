/**
 * Titanium for Windows
 *
 * Copyright (c) 2015 by Appcelerator, Inc. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#include "TitaniumWindows/TitaniumWindows.hpp"
#include "Titanium/Application.hpp"
#include "Titanium/ApplicationBuilder.hpp"
#include "TitaniumWindows/TiModule.hpp"
#include "TitaniumWindows/UIModule.hpp"
#include "TitaniumWindows/API.hpp"
#include "TitaniumWindows/AppModule.hpp"
#include "TitaniumWindows/Locale.hpp"
#include "TitaniumWindows/Platform.hpp"
#include "TitaniumWindows/Gesture.hpp"
#include "TitaniumWindows/Geolocation.hpp"
#include "TitaniumWindows/Accelerometer.hpp"
#include "TitaniumWindows/UI/View.hpp"
#include "TitaniumWindows/UI/TextArea.hpp"
#include "TitaniumWindows/UI/Notification.hpp"
#include "TitaniumWindows/UI/Switch.hpp"
#include "TitaniumWindows/Utils.hpp"
#include "TitaniumWindows/App/Properties.hpp"
#include "TitaniumWindows/UI/EmailDialog.hpp"
#include "TitaniumWindows/UI/Window.hpp"
#include "TitaniumWindows/UI/Button.hpp"
#include "TitaniumWindows/UI/AlertDialog.hpp"
#include "TitaniumWindows/UI/ImageView.hpp"
#include "TitaniumWindows/UI/Label.hpp"
#include "TitaniumWindows/UI/ScrollView.hpp"
#include "TitaniumWindows/UI/Slider.hpp"
#include "TitaniumWindows/UI/TextField.hpp"
#include "TitaniumWindows/UI/ListView.hpp"
#include "TitaniumWindows/Blob.hpp"
#include "TitaniumWindows/Filesystem.hpp"
#include "TitaniumWindows/File.hpp"
#include "TitaniumWindows/Network.hpp"
#include "TitaniumWindows/Network/HTTPClient.hpp"
#include "TitaniumWindows/UI/WebView.hpp"
#include "TitaniumWindows/Map/View.hpp"
#include "TitaniumWindows/Map/Annotation.hpp"
#include "TitaniumWindows/UI/Tab.hpp"
#include "TitaniumWindows/UI/TabGroup.hpp"
#include "TitaniumWindows/UI/TableView.hpp"
#include "TitaniumWindows/UI/TableViewRow.hpp"
#include "TitaniumWindows/UI/ActivityIndicator.hpp"
#include "TitaniumWindows/UI/OptionDialog.hpp"
#include "TitaniumWindows/UI/ProgressBar.hpp"
#include "TitaniumWindows/UI/ScrollableView.hpp"
#include "TitaniumWindows/Media.hpp"
#include "TitaniumWindows/GlobalObject.hpp"
#include "TitaniumWindows/GlobalString.hpp"
#include "TitaniumWindows/Utility.hpp"
#include "Titanium/detail/TiImpl.hpp"

#include <Windows.h>
#include <collection.h>

#define GET_TITANIUM_APP(VARNAME) \
  JSValue Titanium_property = js_context__.get_global_object().GetProperty("Titanium"); \
  TITANIUM_ASSERT(Titanium_property.IsObject()); \
  JSObject Titanium = static_cast<JSObject>(Titanium_property); \
  JSValue App_property = Titanium.GetProperty("App"); \
  TITANIUM_ASSERT(App_property.IsObject()); \
  std::shared_ptr<Titanium::AppModule> VARNAME = static_cast<JSObject>(App_property).GetPrivate<Titanium::AppModule>();

namespace TitaniumWindows
{
	using namespace HAL;

	Application::Application() : js_context__(js_context_group__.CreateContext(JSExport<TitaniumWindows::GlobalObject>::Class()))
	{
	}

	Application::Application(::Platform::String^ seed) : Application::Application()
	{
		auto js_global = js_context__.get_global_object();
		auto global = js_global.GetPrivate<TitaniumWindows::GlobalObject>();
		TITANIUM_ASSERT(global != nullptr);
		global->setSeed(seed);
	}

	Application::~Application()
	{
	}

	void Application::OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs ^ args)
	{
		application__ = std::make_shared<Titanium::Application>(Titanium::ApplicationBuilder(js_context__)
		                                                            .TiObject(js_context__.CreateObject(JSExport<TitaniumWindows::TiModule>::Class()))
		                                                            .GlobalString(js_context__.CreateObject(JSExport<TitaniumWindows::GlobalString>::Class()))
		                                                            .UIObject(js_context__.CreateObject(JSExport<TitaniumWindows::UIModule>::Class()))
		                                                            .APIObject(js_context__.CreateObject(JSExport<TitaniumWindows::API>::Class()))
		                                                            .AppObject(js_context__.CreateObject(JSExport<TitaniumWindows::AppModule>::Class()))
		                                                            .LocaleObject(js_context__.CreateObject(JSExport<TitaniumWindows::Locale>::Class()))
		                                                            .PlatformObject(js_context__.CreateObject(JSExport<TitaniumWindows::Platform>::Class()))
		                                                            .GestureObject(js_context__.CreateObject(JSExport<TitaniumWindows::Gesture>::Class()))
		                                                            .GeolocationObject(js_context__.CreateObject(JSExport<TitaniumWindows::Geolocation>::Class()))
		                                                            .AccelerometerObject(js_context__.CreateObject(JSExport<TitaniumWindows::Accelerometer>::Class()))
		                                                            .ViewObject(js_context__.CreateObject(JSExport<TitaniumWindows::UI::View>::Class()))
		                                                            .TextAreaObject(js_context__.CreateObject(JSExport<TitaniumWindows::UI::TextArea>::Class()))
		                                                            .NotificationObject(js_context__.CreateObject(JSExport<TitaniumWindows::UI::Notification>::Class()))
		                                                            .SwitchObject(js_context__.CreateObject(JSExport<TitaniumWindows::UI::Switch>::Class()))
		                                                            .UtilsObject(js_context__.CreateObject(JSExport<TitaniumWindows::Utils>::Class()))
		                                                            .PropertiesObject(js_context__.CreateObject(JSExport<TitaniumWindows::App::Properties>::Class()))
		                                                            .EmailDialogObject(js_context__.CreateObject(JSExport<TitaniumWindows::UI::EmailDialog>::Class()))
		                                                            .WindowObject(js_context__.CreateObject(JSExport<TitaniumWindows::UI::Window>::Class()))
		                                                            .ButtonObject(js_context__.CreateObject(JSExport<TitaniumWindows::UI::Button>::Class()))
		                                                            .AlertDialogObject(js_context__.CreateObject(JSExport<TitaniumWindows::UI::AlertDialog>::Class()))
		                                                            .ImageViewObject(js_context__.CreateObject(JSExport<TitaniumWindows::UI::ImageView>::Class()))
		                                                            .LabelObject(js_context__.CreateObject(JSExport<TitaniumWindows::UI::Label>::Class()))
		                                                            .ScrollViewObject(js_context__.CreateObject(JSExport<TitaniumWindows::UI::ScrollView>::Class()))
		                                                            .SliderObject(js_context__.CreateObject(JSExport<TitaniumWindows::UI::Slider>::Class()))
		                                                            .TextFieldObject(js_context__.CreateObject(JSExport<TitaniumWindows::UI::TextField>::Class()))
		                                                            .ListViewObject(js_context__.CreateObject(JSExport<TitaniumWindows::UI::ListView>::Class()))
		                                                            .BlobObject(js_context__.CreateObject(JSExport<TitaniumWindows::Blob>::Class()))
		                                                            .FilesystemObject(js_context__.CreateObject(JSExport<TitaniumWindows::FilesystemModule>::Class()))
		                                                            .FileObject(js_context__.CreateObject(JSExport<TitaniumWindows::Filesystem::File>::Class()))
		                                                            .NetworkObject(js_context__.CreateObject(JSExport<TitaniumWindows::NetworkModule>::Class()))
		                                                            .HTTPClientObject(js_context__.CreateObject(JSExport<TitaniumWindows::Network::HTTPClient>::Class()))
		                                                            .WebViewObject(js_context__.CreateObject(JSExport<TitaniumWindows::UI::WebView>::Class()))
		                                                            .MapViewObject(js_context__.CreateObject(JSExport<TitaniumWindows::Map::View>::Class()))
		                                                            .MapAnnotationObject(js_context__.CreateObject(JSExport<TitaniumWindows::Map::Annotation>::Class()))
		                                                            .Tab(js_context__.CreateObject(JSExport<TitaniumWindows::UI::Tab>::Class()))
		                                                            .TabGroup(js_context__.CreateObject(JSExport<TitaniumWindows::UI::TabGroup>::Class()))
		                                                            .TableViewObject(js_context__.CreateObject(JSExport<TitaniumWindows::UI::TableView>::Class()))
		                                                            .TableViewRowObject(js_context__.CreateObject(JSExport<TitaniumWindows::UI::TableViewRow>::Class()))
		                                                            .ActivityIndicatorObject(js_context__.CreateObject(JSExport<TitaniumWindows::UI::ActivityIndicator>::Class()))
		                                                            .OptionDialogObject(js_context__.CreateObject(JSExport<TitaniumWindows::UI::OptionDialog>::Class()))
		                                                            .ProgressBarObject(js_context__.CreateObject(JSExport<TitaniumWindows::UI::ProgressBar>::Class()))
		                                                            .ScrollableViewObject(js_context__.CreateObject(JSExport<TitaniumWindows::UI::ScrollableView>::Class()))
		                                                            .MediaObject(js_context__.CreateObject(JSExport<TitaniumWindows::MediaModule>::Class()))
		                                                            .build());

		const auto js_context_ref = reinterpret_cast<std::intptr_t>(static_cast<JSContextRef>(js_context__));
		auto preloaded = TitaniumModulePreload(js_context_ref);
		auto js_preloaded_modules = js_context__.CreateObject();
		if (preloaded) {
			js_preloaded_modules = JSObject(js_context__, reinterpret_cast<JSObjectRef>(TitaniumModulePreload(js_context_ref)));
		}

		// store all preloaded native modules
		std::unordered_map<std::string, JSValue> preloaded_modules;
		for (const auto& property_name : static_cast<std::vector<JSString>>(js_preloaded_modules.GetPropertyNames())) {
			preloaded_modules.emplace(property_name, js_preloaded_modules.GetProperty(property_name));
		}

		// store all supported native module names
		std::vector<std::string> native_module_names;
		const auto rt_native_module_names = TitaniumModuleNames(js_context_ref);
		if (rt_native_module_names) {
			for (auto v : rt_native_module_names) {
				native_module_names.push_back(TitaniumWindows::Utility::ConvertString(v));
			}
		}

		// registerNativeModuleRequireHook will be called for "non-preloaded" modules
		auto global = js_context__.get_global_object().GetPrivate<TitaniumWindows::GlobalObject>();
		global->registerNativeModuleRequireHook(native_module_names, preloaded_modules, [js_context_ref, this](const std::string& moduleId) {
			const auto js_value_ptr = TitaniumModuleRequire(js_context_ref, TitaniumWindows::Utility::ConvertString(moduleId));
			auto result = JSValue(js_context__, reinterpret_cast<JSValueRef>(js_value_ptr));
			return result;
		});

		Suspending += ref new Windows::UI::Xaml::SuspendingEventHandler(this, &Application::OnSuspending);
		Resuming += ref new Windows::Foundation::EventHandler<::Platform::Object^>(this, &Application::OnResuming);

		// #if _DEBUG
		//  if (IsDebuggerPresent()) {
		//    DebugSettings -> EnableFrameRateCounter = true;
		//  }
		// #endif

		auto rootFrame = dynamic_cast<Windows::UI::Xaml::Controls::Frame ^>(Windows::UI::Xaml::Window::Current->Content);
		// Do not repeat initialization when the Window already has content,
		// just ensure that the window is active.
		if (rootFrame == nullptr) {
			rootFrame = ref new Windows::UI::Xaml::Controls::Frame();

#if defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP)
			// This code is for Windows phone apps only. Remove the turnstile
			// navigation for startup.
			if (rootFrame->ContentTransitions != nullptr) {
				transitions__ = ref new Windows::UI::Xaml::Media::Animation::TransitionCollection();
				for (auto transition : rootFrame->ContentTransitions) {
					transitions__->Append(transition);
				}
			}

			rootFrame->ContentTransitions = nullptr;
			first_navigated_token__ = rootFrame->Navigated += ref new Windows::UI::Xaml::Navigation::NavigatedEventHandler(this, &Application::RootFrame_FirstNavigated);
#endif

			if (rootFrame->Content == nullptr) {
				TITANIUM_LOG_DEBUG("MDL: rootFrame -> Content == nullptr");
			}

			// Create a Frame to act as the navigation context and associate
			// it with a SuspensionManager key.

			// TODO: Change this value to a cache size that is appropriate for
			// your application.
			rootFrame->CacheSize = 1;

			if (args->PreviousExecutionState == Windows::ApplicationModel::Activation::ApplicationExecutionState::Terminated) {
				// TODO: Restore the saved session state only when appropriate,
				// scheduling the final launch steps after the restore is
				// complete.
			}

			// Place the frame in the current Window.
			Windows::UI::Xaml::Window::Current->Content = rootFrame;
			application__->Run("./app");
		}
		// Ensure the current Window is active.
		Windows::UI::Xaml::Window::Current->Activate();
	}

#if defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP)
	// This code is for Windows phone apps only. Restore the content
	// transitions after the application has launched.
	void Application::RootFrame_FirstNavigated(::Platform::Object ^ sender, Windows::UI::Xaml::Navigation::NavigationEventArgs ^ e)
	{
		auto rootFrame = safe_cast<Windows::UI::Xaml::Controls::Frame ^>(sender);

		Windows::UI::Xaml::Media::Animation::TransitionCollection ^ newTransitions;
		if (transitions__ == nullptr) {
			newTransitions = ref new Windows::UI::Xaml::Media::Animation::TransitionCollection();
			newTransitions->Append(ref new Windows::UI::Xaml::Media::Animation::NavigationThemeTransition());
		} else {
			newTransitions = transitions__;
		}

		rootFrame->ContentTransitions = newTransitions;
		rootFrame->Navigated -= first_navigated_token__;
	}
#endif

	void Application::OnResuming(Object ^sender, Object ^args) 
	{
		// Since we only have "resuming" event, we fires both resume and resumed event.
		GET_TITANIUM_APP(App);
		App->fireEvent("resume");
		App->fireEvent("resumed");
	}

	void Application::OnSuspending(Object ^ sender, Windows::ApplicationModel::SuspendingEventArgs ^ e)
	{
		auto deferral = e->SuspendingOperation->GetDeferral();

		js_context__.JSEvaluateScript("Ti.Analytics._receivedResponse = false");

		// Since we only have "suspending" event, we fires both pause and paused event.
		GET_TITANIUM_APP(App);
		App->fireEvent("pause");
		App->fireEvent("paused");

		// wait for Ti.Analytics to receive a response
		auto retry_count = 0;
		while (static_cast<bool>(js_context__.JSEvaluateScript("!Ti.Analytics._receivedResponse"))) {
			Sleep(200);
			retry_count++;

			// wait for a maximum of 3000ms
			if (retry_count > 15) {
				break;
			}
		}

		deferral->Complete();
	}

}  // namespace TitaniumWindows
