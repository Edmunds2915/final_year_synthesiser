/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "EnvelopeGenerator.h"
#include "LowFrequencyOscillator.h"

class FypAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    FypAudioProcessor();
    ~FypAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (AudioBuffer<float>&,  MidiBuffer&) override;
	void processBlock (AudioBuffer<double>&, MidiBuffer&) override;
	void reset() override;

	//==============================================================================
	bool hasEditor() const override;
	AudioProcessorEditor* createEditor() override;

	//==============================================================================
	const String getName() const override;

	bool acceptsMidi() const override;
	bool producesMidi() const override;
	bool isMidiEffect() const override;
	double getTailLengthSeconds() const override;

	//==============================================================================
	int getNumPrograms() override;
	int getCurrentProgram() override;
	void setCurrentProgram (int /*index*/) override;
	const String getProgramName (int /*index*/) override;
	void changeProgramName (int /*index*/, const String& /*name*/);

	//==============================================================================
	void getStateInformation (MemoryBlock&) override;
	void setStateInformation (const void* data, int sizeInBytes) override;

	//void updateTrackProperties (const TrackProperties& properties) override;
	//==============================================================================

	MidiKeyboardState keyboardState;

	int lastUIWidth = 700, lastUIHeight = 320;

	AudioParameterFloat* gainParam      = nullptr;
	AudioParameterFloat* delayParam     = nullptr;
	AudioParameterInt* delayRateParam   = nullptr;

	AudioParameterInt* OSC1_pitchParam  = nullptr;
	AudioParameterInt* OSC2_pitchParam  = nullptr;
	AudioParameterInt* OSC3_pitchParam  = nullptr;

	AudioParameterFloat* OSC1_mixParam = nullptr;
	AudioParameterFloat* OSC2_mixParam = nullptr;
	AudioParameterFloat* OSC3_mixParam = nullptr;

	AudioParameterInt* OSC1_detuneParam = nullptr;
	AudioParameterInt* OSC2_detuneParam = nullptr;
	AudioParameterInt* OSC3_detuneParam = nullptr;

	AudioParameterChoice* waveformParam  = nullptr;
	AudioParameterChoice* waveform2Param = nullptr;
	AudioParameterChoice* waveform3Param = nullptr;

	AudioParameterChoice* filterTypeParam = nullptr;

	AudioParameterChoice* lfoShapeParam = nullptr;

	AudioParameterChoice* lfoTargetParam = nullptr;

	AudioParameterFloat* lfoAmountParam;

	AudioParameterFloat* attackEnvParam  = nullptr;
	AudioParameterFloat* decayEnvParam   = nullptr;
	AudioParameterFloat* sustainEnvParam = nullptr;
	AudioParameterFloat* releaseEnvParam = nullptr;

	/*
	AudioParameterFloat* filterAttackParam = nullptr;
	AudioParameterFloat* filterDecayParam = nullptr;
	AudioParameterFloat* filterSustainParam = nullptr;
	AudioParameterFloat* filterReleaseParam = nullptr;
	AudioParameterFloat* filterEnvAmountParam = nullptr;
	*/

	AudioParameterFloat* filterCutoffParam = nullptr;

	AudioParameterFloat* lfoFrequencyParam = nullptr;

	TrackProperties trackProperties;

private:

	template <typename FloatType>
	void process (AudioBuffer<FloatType>& buffer, MidiBuffer& midiMessages, AudioBuffer<FloatType>& delayBuffer);
	template <typename FloatType>
	void applyGain (float newGain, AudioBuffer<FloatType>&, AudioBuffer<FloatType>& delayBuffer);
	template <typename FloatType>
	void applyDelay (AudioBuffer<FloatType>&, AudioBuffer<FloatType>& delayBuffer);

	AudioBuffer<float> delayBufferFloat;
	AudioBuffer<double> delayBufferDouble;

	int delayPosition = 0;

	const int numVoices = 8;
	Synthesiser synth1;
	Synthesiser synth2;
	Synthesiser synth3;

	void initialiseSynth();
	static BusesProperties getBusesProperties();

	ScopedPointer<XmlElement> tryThis = nullptr;

	LowFrequencyOscillator gainOSC;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FypAudioProcessor)
};
