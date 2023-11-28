/* Copyright 2013-2019 Matt Tytel
 *
 * vital is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * vital is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with vital.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once
#include "juce_audio_basics/juce_audio_basics.h"
#include <juce_audio_devices/juce_audio_devices.h>
#include "juce_core/juce_core.h"
#include "juce_events/juce_events.h"
#include "common.h"

#include <map>

#include <string>
#if !defined(JUCE_AUDIO_DEVICES_H_INCLUDED)

class MidiInput {};

class MidiInputCallback {
  public:
    virtual ~MidiInputCallback() = default;
    virtual void handleIncomingMidiMessage(MidiInput *source, const juce::MidiMessage &midi_message) { }
    virtual void 	handlePartialSysexMessage (MidiInput *source, const uint *messageData, int numBytesSoFar, double timestamp) { }
};

class MidiMessageCollector {
  public:
    void reset(int sample_rate) { }
    void removeNextBlockOfMessages(juce::MidiBuffer& buffer, int num_samples) { }
    void addMessageToQueue(const juce::MidiMessage &midi_message) { }
};

#endif

class SynthBase;

namespace bitklavier {
  class SoundEngine;
  struct ValueDetails;
} // namespace bitklavier
using namespace juce;
class MidiManager : public MidiInputCallback {
  public:
    typedef std::map<int, std::map<std::string, const bitklavier::ValueDetails*>> midi_map;

    enum MidiMainType {
      kNoteOff = 0x80,
      kNoteOn = 0x90,
      kAftertouch = 0xa0,
      kController = 0xb0,
      kProgramChange = 0xc0,
      kChannelPressure = 0xd0,
      kPitchWheel = 0xe0,
    };

    enum MidiSecondaryType {
      kBankSelect = 0x00,
      kModWheel = 0x01,
      kFolderSelect = 0x20,
      kSustainPedal = 0x40,
      kSostenutoPedal = 0x42,
      kSoftPedalOn = 0x43,
      kSlide = 0x4a,
      kLsbPressure = 0x66,
      kLsbSlide = 0x6a,
      kAllSoundsOff = 0x78,
      kAllControllersOff = 0x79,
      kAllNotesOff = 0x7b,
    };

    class Listener {
      public:
        virtual ~Listener() { }
        virtual void valueChangedThroughMidi(const std::string& name, bitklavier::mono_float value) = 0;
        virtual void pitchWheelMidiChanged(bitklavier::mono_float value) = 0;
        virtual void modWheelMidiChanged(bitklavier::mono_float value) = 0;
        virtual void presetChangedThroughMidi(juce::File preset) = 0;
    };

    MidiManager(SynthBase* synth, juce::MidiKeyboardState* keyboard_state,
                std::map<std::string, juce::String>* gui_state, Listener* listener = nullptr);
    virtual ~MidiManager() override;

    void armMidiLearn(std::string name);
    void cancelMidiLearn();
    void clearMidiLearn(const std::string& name);
    void midiInput(int control, bitklavier::mono_float value);
    void processMidiMessage(const juce::MidiMessage &midi_message, int sample_position = 0);
    bool isMidiMapped(const std::string& name) const;

    void setSampleRate(double sample_rate);
    void removeNextBlockOfMessages(juce::MidiBuffer& buffer, int num_samples);
    void replaceKeyboardMessages(juce::MidiBuffer& buffer, int num_samples);

    void processAllNotesOff(const juce::MidiMessage& midi_message, int sample_position, int channel);
    void processAllSoundsOff();
    void processSustain(const juce::MidiMessage& midi_message, int sample_position, int channel);
    void processSostenuto(const juce::MidiMessage& midi_message, int sample_position, int channel);
    void processPitchBend(const juce::MidiMessage& midi_message, int sample_position, int channel);
    void processPressure(const juce::MidiMessage& midi_message, int sample_position, int channel);
    void processSlide(const juce::MidiMessage& midi_message, int sample_position, int channel);

    bool isMpeChannelMasterLowerZone(int channel);
    bool isMpeChannelMasterUpperZone(int channel);

    force_inline int lowerZoneStartChannel() { return mpe_zone_layout_.getLowerZone().getFirstMemberChannel() - 1; }
    force_inline int upperZoneStartChannel() { return mpe_zone_layout_.getUpperZone().getLastMemberChannel() - 1; }
    force_inline int lowerZoneEndChannel() { return mpe_zone_layout_.getLowerZone().getLastMemberChannel() - 1; }
    force_inline int upperZoneEndChannel() { return mpe_zone_layout_.getUpperZone().getFirstMemberChannel() - 1; }
    force_inline int lowerMasterChannel() { return mpe_zone_layout_.getLowerZone().getMasterChannel() - 1; }
    force_inline int upperMasterChannel() { return mpe_zone_layout_.getUpperZone().getMasterChannel() - 1; }

    void setMpeEnabled(bool enabled) { mpe_enabled_ = enabled; }

    midi_map getMidiLearnMap() { return midi_learn_map_; }
    void setMidiLearnMap(const midi_map& midi_learn_map) { midi_learn_map_ = midi_learn_map; }

    // MidiInputCallback
    void handleIncomingMidiMessage(MidiInput *source, const juce::MidiMessage &midi_message) override;

    struct PresetLoadedCallback : public juce::CallbackMessage {
      PresetLoadedCallback(Listener* lis, juce::File pre) : listener(lis), preset(std::move(pre)) { }

      void messageCallback() override {
        if (listener)
          listener->presetChangedThroughMidi(preset);
      }

      Listener* listener;
      juce::File preset;
    };

  protected:
    void readMpeMessage(const juce::MidiMessage& message);

    SynthBase* synth_;
    bitklavier::SoundEngine* engine_;
    juce::MidiKeyboardState* keyboard_state_;
    juce::MidiMessageCollector midi_collector_;
    std::map<std::string, juce::String>* gui_state_;
    Listener* listener_;
    int current_bank_;
    int current_folder_;
    int current_preset_;

    const bitklavier::ValueDetails* armed_value_;
    midi_map midi_learn_map_;

    int msb_pressure_values_[bitklavier::kNumMidiChannels];
    int lsb_pressure_values_[bitklavier::kNumMidiChannels];
    int msb_slide_values_[bitklavier::kNumMidiChannels];
    int lsb_slide_values_[bitklavier::kNumMidiChannels];

    bool mpe_enabled_;
    juce::MPEZoneLayout mpe_zone_layout_;
    juce::MidiRPNDetector rpn_detector_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiManager)
};

