#pragma once

#include <vector>
#include <memory>
#include <string>
#include <condition_variable>
#include <mutex>
#include <vector>

#include "WinRTMidi.h"

namespace WinRT
{
    std::string PlatformStringToString(Platform::String^ s);
    ref class WinRTMidiPortWatcher;

    class WinRTMidiPortInfo
    {
    public:
        WinRTMidiPortInfo(std::string& name, Platform::String^ id)
            : mName(name)
            , mID(id)
        {
        };

        virtual ~WinRTMidiPortInfo() {
        };

        std::string mName;
        Platform::String^ mID;
    };

    // C++ Midi port changed callback
    typedef std::function<void(WinRTMidiPortWatcher^ watcher, WinRTMidiPortUpdateType update)> MidiPortChangedCallbackType;

    // WinRT Delegate
    delegate void MidiPortUpdateHandler(WinRTMidiPortWatcher^ sender, WinRTMidiPortUpdateType update);

    ref class WinRTMidiPortWatcher
    {
    internal:
        unsigned int GetPortCount();
        Platform::String^ GetPortId(unsigned int portNumber);

        void RemoveMidiPortChangedCallback() {
            mPortChangedCallback = nullptr;
        };

        WinRTMidiPortType GetPortType() { return mPortType; };
        event MidiPortUpdateHandler^ mMidiPortUpdateEventHander;
        void OnMidiPortUpdated(WinRTMidiPortUpdateType update);
        
        // needs to be internal as MidiPortChangedCallbackType is not a WinRT type
        void SetMidiPortChangedCallback(const MidiPortChangedCallback callback) {
            mPortChangedCallback = callback;
        };
       
        // Constructor needs to be internal as this is an unsealed ref base class
        WinRTMidiPortWatcher(WinRTMidiPortType type, MidiPortChangedCallback callback = nullptr);

        // needs to be internal as std::string is not a WinRT type
        const std::string& WinRTMidiPortWatcher::GetPortName(unsigned int portNumber);

    private:
        void OnDeviceAdded(Windows::Devices::Enumeration::DeviceWatcher^ sender, Windows::Devices::Enumeration::DeviceInformation^ args);
        void OnDeviceRemoved(Windows::Devices::Enumeration::DeviceWatcher^ sender, Windows::Devices::Enumeration::DeviceInformationUpdate^ args);
        void OnDeviceUpdated(Windows::Devices::Enumeration::DeviceWatcher^ sender, Windows::Devices::Enumeration::DeviceInformationUpdate^ args);
        void OnDeviceEnumerationCompleted(Windows::Devices::Enumeration::DeviceWatcher^ sender, Platform::Object^ args);

        void CheckForEnumeration();

        Windows::Devices::Enumeration::DeviceWatcher^ mPortWatcher;
        std::vector<std::unique_ptr<WinRTMidiPortInfo>> mPortInfo;
        std::mutex mEnumerationMutex;
        std::condition_variable mSleepCondition;

        MidiPortChangedCallback mPortChangedCallback;
        WinRTMidiPortType mPortType;
        bool mPortEnumerationComplete;
    };


    class MidiPortWatcherWrapper
    {
    public:
        MidiPortWatcherWrapper(WinRTMidiPortWatcher ^ watcher)
            : mWatcher(watcher)
        {
        }

        WinRTMidiPortWatcher^ GetWatcher() {
            return mWatcher;
        }

    private:
        WinRTMidiPortWatcher^ mWatcher;
    };
};




