/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class FypAudioProcessorEditor : public AudioProcessorEditor,
								private ComboBox::Listener
{
public:
	//The editor constructor receives a reference to the processor.
    FypAudioProcessorEditor (FypAudioProcessor&);
    ~FypAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
	void hostMIDIControllerIsAvailable(bool) override;
	void updateTrackProperties();

private:

	void comboBoxChanged(ComboBox*) override;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    FypAudioProcessor& processor;
	FypAudioProcessor& getProcessor() const {
		return static_cast<FypAudioProcessor&> (processor);
	}

	MidiKeyboardComponent midiKeyboard;

	class ParameterSlider;
	class MyParameterIntSlider;

	ScopedPointer<ParameterSlider> gainSlider;

	ScopedPointer<ParameterSlider> delaySlider;
	ScopedPointer<MyParameterIntSlider> delayRateSlider;

	ScopedPointer<MyParameterIntSlider> OSC1_pitchSlider;
	ScopedPointer<MyParameterIntSlider> OSC2_pitchSlider;
	ScopedPointer<MyParameterIntSlider> OSC3_pitchSlider;

	ScopedPointer<ParameterSlider> OSC1_mixSlider;
	ScopedPointer<ParameterSlider> OSC2_mixSlider;
	ScopedPointer<ParameterSlider> OSC3_mixSlider;

	ScopedPointer<MyParameterIntSlider> OSC1_detuneSlider;
	ScopedPointer<MyParameterIntSlider> OSC2_detuneSlider;
	ScopedPointer<MyParameterIntSlider> OSC3_detuneSlider;

	ScopedPointer<ParameterSlider> attackEnvSlider;
	ScopedPointer<ParameterSlider> decayEnvSlider;
	ScopedPointer<ParameterSlider> sustainEnvSlider;
	ScopedPointer<ParameterSlider> releaseEnvSlider;

	ScopedPointer<ParameterSlider> lfoFreqSlider;
	ScopedPointer<ParameterSlider> lfoAmountSlider;

	ScopedPointer<ParameterSlider> filterCutoffSlider;
	
	ComboBox waveformBox, waveform2Box, waveform3Box;
	Label waveformLabel, waveform2Label, waveform3Label;

	ComboBox filterBox;
	Label filterLabel;

	Label lfoLabel;
	Label lfoTargetLabel;
	Label lfoFreqLabel;
	Label lfoAmountLabel;

	Label pitchLabel;
	Label mixLabel;
	Label detuneLabel;



	Label filterTypeLabel;

	ComboBox lfoShapeBox;
	ComboBox lfoTargetBox;

	Label gainLabel, delayLabel, delayRateLabel;

	Label attackLabel, decayLabel, sustainLabel, releaseLabel;

	
	Colour backgroundColour;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FypAudioProcessorEditor)
};