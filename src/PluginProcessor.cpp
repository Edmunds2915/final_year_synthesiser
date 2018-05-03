#pragma once

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "FYPOscillator.h"
#include "LowFrequencyOscillator.h"

AudioProcessor* JUCE_CALLTYPE createPluginFilter();

//==============================================================================
FypAudioProcessor::FypAudioProcessor() : AudioProcessor (getBusesProperties())
{
	addParameter (gainParam  = new AudioParameterFloat ("gain", "Gain", 0.0f, 1.0f, 0.5f));

	addParameter (delayParam = new AudioParameterFloat ("delay", "Delay Feedback", 0.0f, 1.0f, 0.0f));
	addParameter (delayRateParam = new AudioParameterInt ("delayrate", "Delay Rate", 0, 12000, 2560, "samples"));

	addParameter (OSC1_pitchParam = new AudioParameterInt ("osc1pitch", "OSC1 Pitch", -12, 12, 0, "semitones"));
	addParameter (OSC2_pitchParam = new AudioParameterInt ("osc2pitch", "OSC2 Pitch", -12, 12, 0, "semitones"));
	addParameter (OSC3_pitchParam = new AudioParameterInt ("osc3pitch", "OSC3 Pitch", -12, 12, 0, "semitones"));

	addParameter(OSC1_mixParam = new AudioParameterFloat ("osc1mix", "OSC1 Amount", 0.0f, 1.0f, 1.0f));
	addParameter(OSC2_mixParam = new AudioParameterFloat ("osc2mix", "OSC2 Amount", 0.0f, 1.0f, 0.0f));
	addParameter(OSC3_mixParam = new AudioParameterFloat ("osc3mix", "OSC3 Amount", 0.0f, 1.0f, 0.0f));

	addParameter(OSC1_detuneParam = new AudioParameterInt ("osc1detune", "OSC1 Detune", -100, 100, 0, "cents"));
	addParameter(OSC2_detuneParam = new AudioParameterInt ("osc2detune", "OSC2 Detune", -100, 100, 0, "cents"));
	addParameter(OSC3_detuneParam = new AudioParameterInt ("osc3detune", "OSC3 Detune", -100, 100, 0, "cents"));

	addParameter (waveformParam  = new AudioParameterChoice ("waveform1", "Waveform 1", { "SINE", "TRIANGLE", "SQUARE", "SAW" }, 3));
	addParameter (waveform2Param = new AudioParameterChoice ("waveform2", "Waveform 2", { "SINE", "TRIANGLE", "SQUARE", "SAW" }, 3));
	addParameter (waveform3Param = new AudioParameterChoice ("waveform3", "Waveform 3", { "SINE", "TRIANGLE", "SQUARE", "SAW" }, 3));

	addParameter (filterTypeParam = new AudioParameterChoice ("filtertype", "Filter type", { "LPF", "HPF", "BPF" }, 0));

	addParameter (lfoTargetParam = new AudioParameterChoice ("lfotarget", "LFO Target", { "FILTER", "LEVEL", "PITCH" }, 0));
	addParameter (lfoShapeParam = new AudioParameterChoice ("lfoshape", "LFO Shape", { "SINE", "TRIANGLE", "SQUARE", "SAW" }, 0));
	addParameter (lfoAmountParam = new AudioParameterFloat ("lfoamount", "LFO Amount", 0.0f, 1.0f, 0.0f));

	addParameter (attackEnvParam  = new AudioParameterFloat ("attack",  "Attack",  0.0f, 0.5f, 0.0001f));
	addParameter (decayEnvParam   = new AudioParameterFloat ("decay",   "Decay",   0.0f, 1.0f, 1.0f));
	addParameter (sustainEnvParam = new AudioParameterFloat ("sustain", "Sustain", 0.0f, 1.0f, 1.0f));
	addParameter (releaseEnvParam = new AudioParameterFloat ("release", "Release", 0.0f, 6.0f, 0.05f));

	addParameter (lfoFrequencyParam = new AudioParameterFloat ("lfofreq", "LFO Frequency (Hz)", 0.0f, 30.0f, 0.1f));


	addParameter (filterCutoffParam = new AudioParameterFloat ("cutoff", "Filter Cutoff", 0.0f, 0.99f, 0.99f));

	initialiseSynth();


	tryThis = new XmlElement ("FYPPLUGINSETTINGS");
}

FypAudioProcessor::~FypAudioProcessor()
{
}


AudioProcessor::BusesProperties FypAudioProcessor::getBusesProperties()
{
	return BusesProperties()
		.withInput ("Input", AudioChannelSet::stereo(), true)
		.withOutput ("Output", AudioChannelSet::stereo(), true);
}


void FypAudioProcessor::prepareToPlay (double newSampleRate, int samplesPerBlock)
{
	synth1.setCurrentPlaybackSampleRate (newSampleRate);
	synth2.setCurrentPlaybackSampleRate (newSampleRate);
	synth3.setCurrentPlaybackSampleRate (newSampleRate);

	keyboardState.reset();

	if (isUsingDoublePrecision())
	{
		delayBufferDouble.setSize (2, 12000);
		delayBufferFloat.setSize (1, 1);
	}
	else
	{
		delayBufferFloat.setSize (2, 12000);
		delayBufferDouble.setSize (1, 1);
	}

	reset();
}


bool FypAudioProcessor::hasEditor() const
{
	return true;
}


AudioProcessorEditor* FypAudioProcessor::createEditor()
{
	return new FypAudioProcessorEditor (*this);
}


void FypAudioProcessor::initialiseSynth()
{
	for (int i = numVoices; --i >= 0;)
	{
		synth1.addVoice (new FYPVoice());
		synth2.addVoice (new FYPVoice());
		synth3.addVoice (new FYPVoice());
	}

	synth1.addSound (new FYPSound());
	synth2.addSound (new FYPSound());
	synth3.addSound (new FYPSound());

	/*
	const void* data;
	int sizeInBytes = 10;
	data = &sizeInBytes;
	setStateInformation(data, 10);
	*/
}


template <typename FloatType>
void FypAudioProcessor::process (AudioBuffer<FloatType>& buffer, MidiBuffer& midiMessages, AudioBuffer<FloatType>& delayBuffer)
{
	const int numSamples = buffer.getNumSamples();

	for (int i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
		buffer.clear (i, 0, numSamples);

	keyboardState.processNextMidiBuffer (midiMessages, 0, numSamples, true);
		
	//SET OSC1 PARAMETERS=============================================================================
	for (int i = 0; i < numVoices; ++i)
	{
		dynamic_cast<FYPVoice*> (synth1.getVoice (i))
			->filter.setFilterMode (filterTypeParam->getIndex());

		dynamic_cast<FYPVoice*> (synth1.getVoice (i))->filter.setCutoff (*filterCutoffParam);
			
		dynamic_cast<FYPVoice*> (synth1.getVoice (i))->setMode (waveformParam->getIndex());

		dynamic_cast<FYPVoice*> (synth1.getVoice (i))
			->setADSR (*attackEnvParam, *decayEnvParam, *sustainEnvParam, *releaseEnvParam);

		dynamic_cast<FYPVoice*> (synth1.getVoice (i))->setPitch (*OSC1_pitchParam);

		dynamic_cast<FYPVoice*> (synth1.getVoice (i))->setLFOMode (*lfoShapeParam);

		dynamic_cast<FYPVoice*> (synth1.getVoice (i))->setLFOFreq (*lfoFrequencyParam);

		dynamic_cast<FYPVoice*> (synth1.getVoice (i))->setLFOTarget (*lfoTargetParam);

		dynamic_cast<FYPVoice*> (synth1.getVoice (i))->setCents (*OSC1_detuneParam);

		dynamic_cast<FYPVoice*> (synth1.getVoice (i))->setMix (*OSC1_mixParam);

		dynamic_cast<FYPVoice*> (synth1.getVoice (i))->setLevel (*gainParam);

		dynamic_cast<FYPVoice*> (synth1.getVoice (i))->setLfoAmount (*lfoAmountParam);
	}
	
	
	//SET OSC2 PARAMETERS============================================================================
	for (int i = 0; i < numVoices; ++i)
	{	
		dynamic_cast<FYPVoice*> (synth2.getVoice (i))
			->filter.setFilterMode (filterTypeParam->getIndex());

		dynamic_cast<FYPVoice*> (synth2.getVoice (i))->filter.setCutoff (*filterCutoffParam);

		dynamic_cast<FYPVoice*> (synth2.getVoice (i))->setMode (waveform2Param->getIndex());

		dynamic_cast<FYPVoice*> (synth2.getVoice (i))
			->setADSR (*attackEnvParam, *decayEnvParam, *sustainEnvParam, *releaseEnvParam);

		dynamic_cast<FYPVoice*> (synth2.getVoice (i))->setPitch (*OSC2_pitchParam);

		dynamic_cast<FYPVoice*> (synth2.getVoice (i))->setLFOMode (*lfoShapeParam);

		dynamic_cast<FYPVoice*> (synth2.getVoice (i))->setLFOFreq (*lfoFrequencyParam);

		dynamic_cast<FYPVoice*> (synth2.getVoice (i))->setLFOTarget (*lfoTargetParam);

		dynamic_cast<FYPVoice*> (synth2.getVoice (i))->setCents (*OSC2_detuneParam);

		dynamic_cast<FYPVoice*> (synth2.getVoice (i))->setMix (*OSC2_mixParam);

		dynamic_cast<FYPVoice*> (synth2.getVoice (i))->setLevel (*gainParam);

		dynamic_cast<FYPVoice*> (synth2.getVoice (i))->setLfoAmount (*lfoAmountParam);
	}
	

	//SET OSC3 PARAMETERS==========================================================================
	for (int i = 0; i < numVoices; ++i)
	{
		dynamic_cast<FYPVoice*> (synth3.getVoice (i))
			->filter.setFilterMode (filterTypeParam->getIndex());

		dynamic_cast<FYPVoice*> (synth3.getVoice (i))->filter.setCutoff (*filterCutoffParam);

		dynamic_cast<FYPVoice*> (synth3.getVoice (i))->setMode (waveform3Param->getIndex());

		dynamic_cast<FYPVoice*> (synth3.getVoice (i))
			->setADSR (*attackEnvParam, *decayEnvParam, *sustainEnvParam, *releaseEnvParam);

		dynamic_cast<FYPVoice*> (synth3.getVoice (i))->setPitch (*OSC3_pitchParam);

		dynamic_cast<FYPVoice*> (synth3.getVoice (i))->setLFOMode (*lfoShapeParam);

		dynamic_cast<FYPVoice*> (synth3.getVoice (i))->setLFOFreq (*lfoFrequencyParam);

		dynamic_cast<FYPVoice*> (synth3.getVoice (i))->setLFOTarget (*lfoTargetParam);

		dynamic_cast<FYPVoice*> (synth3.getVoice (i))->setCents (*OSC3_detuneParam);

		dynamic_cast<FYPVoice*> (synth3.getVoice (i))->setMix (*OSC3_mixParam);

		dynamic_cast<FYPVoice*> (synth3.getVoice (i))->setLevel (*gainParam);

		dynamic_cast<FYPVoice*> (synth3.getVoice (i))->setLfoAmount (*lfoAmountParam);
	}
	
	synth1.renderNextBlock (buffer, midiMessages, 0, numSamples);
	synth2.renderNextBlock (buffer, midiMessages, 0, numSamples);
	synth3.renderNextBlock (buffer, midiMessages, 0, numSamples);
	
	applyDelay (buffer, delayBuffer);
}


template <typename FloatType>
void FypAudioProcessor::applyGain (float newGain, AudioBuffer<FloatType>& buffer, AudioBuffer<FloatType>& delayBuffer)
{
	ignoreUnused (delayBuffer);

	for (int channel = 0; channel < getTotalNumOutputChannels(); ++channel)
		buffer.applyGain (channel, 0, buffer.getNumSamples(), newGain);
}




template <typename FloatType>
void FypAudioProcessor::applyDelay (AudioBuffer<FloatType>& buffer, AudioBuffer<FloatType>& delayBuffer)
{
	const int numSamples = buffer.getNumSamples();
	const float delayLevel = *delayParam;
	const int theRate = *delayRateParam;

	int delayPos = 0;

	for (int channel = 0; channel < getTotalNumOutputChannels(); ++channel)
	{
		auto channelData = buffer.getWritePointer (channel);
		auto delayData = delayBuffer.getWritePointer (jmin (channel, delayBuffer.getNumChannels() - 1));
		delayPos = delayPosition;

		for (int i = 0; i < numSamples; ++i)
		{
			auto in = channelData[i];

			channelData[i] += delayData[delayPos];

			delayData[delayPos] = (delayData[delayPos] + in) * delayLevel;

			if (++delayPos >= theRate)
				delayPos = 0;
		}
	}

	delayPosition = delayPos;
}



void FypAudioProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
	jassert (!isUsingDoublePrecision());
	process (buffer, midiMessages, delayBufferFloat);
}


void FypAudioProcessor::processBlock (AudioBuffer<double>& buffer, MidiBuffer& midiMessages)
{
	jassert (isUsingDoublePrecision());
	process (buffer, midiMessages, delayBufferDouble);
}

//==============================================================================


bool FypAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
	// Only mono/stereo and input/output must have same layout
	const AudioChannelSet& mainOutput = layouts.getMainOutputChannelSet();
	const AudioChannelSet& mainInput = layouts.getMainInputChannelSet();

	// input and output layout must either be the same or the input must be disabled altogether
	if (!mainInput.isDisabled() && mainInput != mainOutput)
		return false;

	// do not allow disabling the main buses
	if (mainOutput.isDisabled())
		return false;

	// only allow stereo and mono
	if (mainOutput.size() > 2)
		return false;

	return true;
}


void FypAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
	keyboardState.reset();
}


void FypAudioProcessor::reset()
{
	// Use this method as the place to clear any delay lines, buffers, etc, as it
	// means there's been a break in the audio's continuity.
	delayBufferFloat.clear();
	delayBufferDouble.clear();
}


//==============================================================================
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new FypAudioProcessor();
}


const String FypAudioProcessor::getName() const
{
	return JucePlugin_Name;
}


bool FypAudioProcessor::acceptsMidi() const
{
	return true;
}


bool FypAudioProcessor::producesMidi() const
{
	return false;
}


double FypAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}


int FypAudioProcessor::getNumPrograms()
{
	return 1;
}


int FypAudioProcessor::getCurrentProgram()
{
	return 0;
}

bool FypAudioProcessor::isMidiEffect() const
{
	return false;
}


void FypAudioProcessor::setCurrentProgram (int /*index*/)
{
}


const String FypAudioProcessor::getProgramName (int /*index*/)
{
	return String();
}


void FypAudioProcessor::changeProgramName (int /*index*/, const String& /*newName*/)
{
}

/*
void FypAudioProcessor::updateTrackProperties (const TrackProperties& properties)
{
	trackProperties = properties;

	if (auto* editor = dynamic_cast<FypAudioProcessorEditor*> (getActiveEditor()))
		editor->updateTrackProperties();
}
*/


void FypAudioProcessor::getStateInformation (MemoryBlock& destData)
{
	// You should use this method to store your parameters in the memory block.
	// Here's an example of how you can use XML to make it easy and more robust:

	// Create an outer XML element..
	//XmlElement xml ("FYPPLUGINSETTINGS");

	// add some attributes to it..
	//xml.setAttribute("uiWidth", lastUIWidth);
	//xml.setAttribute("uiHeight", lastUIHeight);

	// Store the values of all our parameters, using their param ID as the XML attribute
	for (auto* param : getParameters())
		if (auto* p = dynamic_cast<AudioProcessorParameterWithID*> (param))
			tryThis->setAttribute (p->paramID, p->getValue());

	// then use this helper function to stuff it into the binary blob and return it..
	copyXmlToBinary (*tryThis, destData);
}

void FypAudioProcessor::setStateInformation (const void* data, int sizeInBytes) 
{
	// You should use this method to restore your parameters from this memory block,
	// whose contents will have been created by the getStateInformation() call.

	// This getXmlFromBinary() helper function retrieves our XML from the binary blob..
	tryThis = getXmlFromBinary (data, sizeInBytes);

	if (tryThis != nullptr)
	{
		// make sure that it's actually our type of XML object..
		if (tryThis->hasTagName ("FYPPLUGINSETTINGS"))
		{
			// Now reload our parameters..
			for (auto param : getParameters())
				if (auto* p = dynamic_cast<AudioProcessorParameterWithID*> (param))
				{
					p->setValue ((float) tryThis->getDoubleAttribute (p->paramID, p->getValue()));
				}
		}
	}

	createEditorIfNeeded();
	getActiveEditor()->resized();
}
