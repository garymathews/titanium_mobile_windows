/**
 * Copyright (c) 2014 by Appcelerator, Inc. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#ifndef _TITANIUMWINDOWS_GESTURE_HPP_
#define _TITANIUMWINDOWS_GESTURE_HPP_

#include "Titanium/Gesture.hpp"
#include "TitaniumWindows_Sensors_EXPORT.h"
#include "TitaniumWindows/ShakeGestureHelper.hpp"

namespace TitaniumWindows
{
	using namespace HAL;

	/*!
	  @class Gesture
	  @ingroup Titanium.Gesture

	  @discussion This is the Titanium.Gesture implementation for Windows.
    */
	class TITANIUMWINDOWS_SENSORS_EXPORT Gesture final : public Titanium::Gesture, public JSExport<Gesture>
	{
	public:
		Gesture(const JSContext&) TITANIUM_NOEXCEPT;

		virtual ~Gesture();
		Gesture(const Gesture&) = default;
		Gesture& operator=(const Gesture&) = default;
#ifdef TITANIUM_MOVE_CTOR_AND_ASSIGN_DEFAULT_ENABLE
		Gesture(Gesture&&) = default;
		Gesture& operator=(Gesture&&) = default;
#endif

		static void JSExportInitialize();

		virtual void enableEvent(const std::string& event_name) TITANIUM_NOEXCEPT override;
		virtual void disableEvent(const std::string& event_name) TITANIUM_NOEXCEPT override;

		virtual Titanium::UI::ORIENTATION get_orientation() const TITANIUM_NOEXCEPT override;

	protected:
	private:
		void setupOrientationListener();
		bool orientationEventSet_{false};
		Titanium::UI::ORIENTATION updateOrientation();

		Windows::Foundation::DateTime previous_acceleromter_time_;
		Windows::Devices::Sensors::Accelerometer^ accelerometer_;

		Titanium::UI::ORIENTATION orientation_ = Titanium::UI::ORIENTATION::UNKNOWN;

		Windows::Foundation::EventRegistrationToken shake_event_;
		Windows::Foundation::EventRegistrationToken orientationchange_event_;

#pragma warning(push)
#pragma warning(disable : 4251)
		TitaniumWindows::Gesture::ShakeGestureHelper shakeGestureHelper;
#pragma warning(pop)
	};

}  // namespace TitaniumWindows

#endif  // _TITANIUMWINDOWS_GESTURE_HPP_
