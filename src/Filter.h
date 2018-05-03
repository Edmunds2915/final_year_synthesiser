#pragma once
#include "PluginProcessor.h"
#include "PluginEditor.h"

class Filter 
{
	public:
		enum FilterMode
		{
			FILTER_MODE_LOWPASS = 0,
			FILTER_MODE_HIGHPASS,
			FILTER_MODE_BANDPASS,
			kNumFilterModes
		};

		Filter() :
			cutoff (0.99),
			cutoffMod (0.0),
			resonance (0.0),
			mode (FILTER_MODE_LOWPASS),
			buf0 (0.0),
			buf1 (0.0)
		{
			//calculateFeedbackAmount();
		};

		double process (double inputValue);
		inline void setCutoff (double newCutoff) { cutoff = newCutoff; }; //calculateFeedbackAmount();

		inline double getCalculatedCutoff() const 
		{
			return jmax (jmin (cutoff + cutoffMod, 0.99), 0.01);
		};

		inline void setCutoffMod (double newCutoffMod)
		{
			cutoffMod = newCutoffMod;
			//calculateFeedbackAmount();
		}


		//inline void setResonance(double newResonance) { resonance = newResonance; calculateFeedbackAmount(); };
		inline void setFilterMode (int newMode)
		{
			if (newMode == 0)
				mode = FILTER_MODE_LOWPASS;
			else if (newMode == 1)
				mode = FILTER_MODE_HIGHPASS;
			else
				mode = FILTER_MODE_BANDPASS;
		}

	private:
		double cutoff;
		double cutoffMod;
		double resonance;
		FilterMode mode;
		double feedbackAmount;
		//inline void calculateFeedbackAmount() { feedbackAmount = resonance + resonance / (1.0 - getCalculatedCutoff()); }
		double buf0;
		double buf1;
};