#pragma once

#include "PluginEditor.h"
#include "PluginProcessor.h"
#include "EnvelopeGenerator.h"
#include "LowFrequencyOscillator.h"
#include "Filter.h"


class FYPSound : public SynthesiserSound
{
public:
	FYPSound()
	{

	}

	bool appliesToNote(int /*midiNoteNumber*/) override { return true; }
	bool appliesToChannel(int /*midiChannel*/) override { return true; }
};

class FYPVoice : public SynthesiserVoice
{
public:
	FYPVoice()
		: currentAngle(0), angleDelta(0), level(0), tailOff(0), twoPi(double_Pi * 2),
		pitch(0), mode(4), lastOutput(0), mix(1)//, filterEnvelopeAmount(0.99)
	{

	}


	Filter filter;


	bool canPlaySound (SynthesiserSound* sound) override
	{
		return dynamic_cast<FYPSound*> (sound) != nullptr;
	}

	void startNote(int midiNoteNumber, float velocity, SynthesiserSound* /*sound*/,
		int /*currentPitchWheelPosition*/) override
	{
	
		midi = midiNoteNumber;
		currentAngle = 0.0;
		oscVel = velocity;

		int newPitch = midi + pitch;

		cyclesPerSecond = MidiMessage::getMidiNoteInHertz(newPitch);

		angleDelta = cyclesPerSecond * 2.0 * double_Pi;
		
		envelopeGenerator.enterStage (EnvelopeGenerator::ENVELOPE_STAGE_ATTACK);
		//filterEnvelopeGenerator.enterStage(EnvelopeGenerator::ENVELOPE_STAGE_ATTACK);
	}

	void stopNote (float /*velocity*/, bool /*allowTailOff*/) override
	{
		envelopeGenerator.enterStage (EnvelopeGenerator::ENVELOPE_STAGE_RELEASE);
		//filterEnvelopeGenerator.enterStage(EnvelopeGenerator::ENVELOPE_STAGE_RELEASE);
	}


	void setLFOMode (int mode)
	{
		lfo.setMode (mode);
	}

	void setLFOFreq (double hz)
	{
		lfo.setFrequency (hz);
	}

	void setLFOTarget (int index)
	{
		if (index == 0)
			lfoTarget = LfoTarget::LFO_TARGET_CUTOFF;
		else if (index == 1)
			lfoTarget = LfoTarget::LFO_TARGET_LEVEL;
		else //if (index == 2)
			lfoTarget = LfoTarget::LFO_TARGET_PITCH;
		//else
			//lfoTarget = LfoTarget::LFO_TARGET_NONE;
	}


	void setMode (int modeParam)
	{
		mode = modeParam;
	}

	void setPitch (int note)
	{
		pitch = note;
	}

	void setCents (int detuneAmount)
	{
		cents = detuneAmount;
	}

	void setMix (float mixParam)
	{
		mix = mixParam;
	}

	void setLevel (float levelParam)
	{
		level = levelParam;
	}

	void setLfoAmount (float lfoAmountParam)
	{
		lfo.setAmount (lfoAmountParam);
	}

	void setADSR (float atk, float dec, float sus, float rel)
	{
		if (atk < 0.009)
			envelopeGenerator.setStageValue (EnvelopeGenerator::ENVELOPE_STAGE_ATTACK, 0.009);
		else
			envelopeGenerator.setStageValue(EnvelopeGenerator::ENVELOPE_STAGE_ATTACK, atk);

		envelopeGenerator.setStageValue (EnvelopeGenerator::ENVELOPE_STAGE_DECAY, dec);
		envelopeGenerator.setStageValue (EnvelopeGenerator::ENVELOPE_STAGE_SUSTAIN, sus);
		if (rel < 0.05)
			envelopeGenerator.setStageValue (EnvelopeGenerator::ENVELOPE_STAGE_RELEASE, 0.05);
		else
			envelopeGenerator.setStageValue (EnvelopeGenerator::ENVELOPE_STAGE_RELEASE, rel);
	}


	void renderNextBlock (AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override
	{
		if (angleDelta != 0.0)
		{
			while (--numSamples >= 0)
			{
				if (envelopeGenerator.getCurrentStage() == EnvelopeGenerator::ENVELOPE_STAGE_OFF)
				{
					clearCurrentNote();
					angleDelta = 0.0;
					lastOutput = 0.0;
					lfo.resetCurrentAngle();
					break;
				}

				double newCyclesPerSample = 0;

				int newPitch = midi + pitch;

				cyclesPerSecond = MidiMessage::getMidiNoteInHertz (newPitch);
				//double cyclesPerSample = cyclesPerSecond / getSampleRate();

				////i.e. the angular frequency = 2(pi)f, the change of the angle every sample, fixed for a given note
				//angleDelta = cyclesPerSample * 2.0 * double_Pi; //a fraction of 2pi


				if (lfoTarget == LfoTarget::LFO_TARGET_PITCH)
				{
					double centsMod = cents + lfo.nextSample() * 500;
					newCyclesPerSample = (cyclesPerSecond * pow (2.0, centsMod / 1200.0)) / getSampleRate();
				}
				else
				{
					newCyclesPerSample = (cyclesPerSecond * pow (2.0, cents / 1200.0)) / getSampleRate();
				}

				//i.e. the angular frequency = 2(pi)f, the change of the angle every sample, fixed for a given note
				angleDelta = newCyclesPerSample * 2.0 * double_Pi; //a fraction of 2pi
			

				double currentSample = 0;
				double t = currentAngle / twoPi;

				double lfoMod = 0;

				currentSample = nextSampleWithAntialiasing (mode, t);

				currentSample *= envelopeGenerator.nextSample();

				double volume = (double) (level * mix * oscVel);
				float lfoModTransformation = 0.0f;
				
				switch (lfoTarget)
				{
				case LFO_TARGET_CUTOFF:
					lfoMod = lfo.nextSample();
					filter.setCutoffMod (lfoMod);
					currentSample = filter.process (currentSample);
					currentSample *= volume;
					break;
				case LFO_TARGET_LEVEL:
					lfoMod = lfo.nextSample();
					filter.setCutoffMod (0.0);
					currentSample = filter.process (currentSample);
					lfoModTransformation = (lfoMod + 1.0f) / 2;
					currentSample *= /*jmax (jmin (*/volume * lfoModTransformation;//, volume), 0.0);
					break;
				case LFO_TARGET_PITCH:
					filter.setCutoffMod(0.0);
					currentSample = filter.process (currentSample);
					currentSample *= volume;
					break;
				/*case LFO_TARGET_NONE:
					filter.setCutoffMod (0.0);
					currentSample = filter.process (currentSample);
					currentSample *= volume;
					break;*/
				default:
					break;
				}
			
				

				for (auto i = outputBuffer.getNumChannels(); --i >= 0;) 
				{
					outputBuffer.addSample (i, startSample, (float) currentSample);
				}


				currentAngle += angleDelta;
				++startSample;
				while (currentAngle >= twoPi)
				{
					currentAngle -= twoPi;
				}
			}
		}
	}


	void pitchWheelMoved(int /*newValue*/) override
	{
	}

	void controllerMoved(int /*controllerNumber*/, int /*newValue*/) override
	{
	}

private:

	double currentAngle, angleDelta, level, tailOff, twoPi, lastOutput, cyclesPerSecond;
	int mode, pitch;

	float oscVel;
	float mix;
	int cents;
	int midi;

	enum LfoTarget 
	{
		LFO_TARGET_CUTOFF = 0,
		LFO_TARGET_LEVEL,
		LFO_TARGET_PITCH//,
		//LFO_TARGET_NONE
	};

	LfoTarget lfoTarget;

	EnvelopeGenerator envelopeGenerator;

	double filterEnvelopeAmount;

	LowFrequencyOscillator lfo;

	//Author: Martin Finke @ martin-finke.de
	//Credit: http://www.kvraudio.com/forum/viewtopic.php?t=375517
	double polyBLEP (double t)
	{
		double dt = angleDelta / twoPi;
		// 0 <= t < 1
		if (t < dt)
		{
			t /= dt;
			return t + t - t * t - 1.0;
		}
		// -1 < t < 0
		else if (t > 1.0 - dt)
		{
			t = (t - 1.0) / dt;
			return t * t + t + t + 1.0;
		}
		// 0 otherwise
		else return 0.0;
	}


	//Author: Martin Finke martin-finke.de
	double nextSampleWithAntialiasing (int chosenWaveform, double t)
	{
		double nextSample = 0;

		switch (chosenWaveform)
		{
		case 0: //sine wave
			nextSample = sin (currentAngle);
			break;
		case 1: //triangle wave
			if (currentAngle <= double_Pi)
			{
				nextSample = 1.0;
			}
			else 
			{
				nextSample = -1.0;
			}

			nextSample += polyBLEP (t);
			nextSample -= polyBLEP (fmod (t + 0.5, 1.0));
			nextSample = angleDelta * nextSample + (1 - angleDelta) * lastOutput;
			lastOutput = nextSample;

			break;
		case 2: //square wave
			if (currentAngle <= double_Pi)
			{
				nextSample = 1.0;
			}
			else
			{
				nextSample = -1.0;
			}
			nextSample += polyBLEP (t);
			nextSample -= polyBLEP (fmod (t + 0.5, 1.0));
			break;
		case 3: //saw wave
			nextSample = (2.0 * currentAngle / twoPi) - 1.0;
			nextSample -= polyBLEP (t);
			break;
		case 4: //none
			break;
		default:
			break;
		}

		return nextSample;

	}
};