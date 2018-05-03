#pragma once

class LowFrequencyOscillator
{
public:
	LowFrequencyOscillator()
		: currentAngle (0), cyclesPerSample (0.0),
			twoPi (double_Pi * 2), mode (1), frequency (6.0), amount (0.0)
	{
		cyclesPerSample = frequency / 44100.0;
		angleDelta = cyclesPerSample * 2.0 * double_Pi;
	}

	~LowFrequencyOscillator()
	{
	}

	void setAmount (double amountParam)
	{
		amount = amountParam;
	}


	void setMode (int modeParam)
	{
		mode = modeParam;
	}


	void setFrequency (double freqParam)
	{
		frequency = freqParam;
		cyclesPerSample = frequency / 44100.0;
		angleDelta = cyclesPerSample * 2.0 * double_Pi;
	}


	void resetCurrentAngle()
	{
		currentAngle = 0.0;
	}


	double nextSample()
	{
		double currentSample = 0.0;

		if (angleDelta != 0.0) 
		{	
			currentSample = nextSampleNoAntialiasing (mode);
			currentSample *= amount;

			currentAngle += angleDelta;
			while (currentAngle >= twoPi)
			{
				currentAngle -= twoPi;
			}
		}

		return currentSample;
	}


	double frequency;

private:

	double currentAngle, angleDelta, cyclesPerSample;
	int mode;

	const double twoPi;

	double amount;


	//Author: Martin Finke martin-finke.de
	double nextSampleNoAntialiasing (int chosenWaveform)
	{
		double nextSample = 0.0;

		switch (chosenWaveform)
		{
			case 0: //sine wave
				nextSample = sin (currentAngle);
				break;
			case 1: //triangle wave		
				nextSample = -1.0 + (2.0 * currentAngle / twoPi);
				nextSample = 2.0 * (fabs (nextSample) - 0.5);
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
				break;
			case 3: //saw wave
				nextSample = 1.0 + (2.0 * currentAngle / twoPi);
				break;
			default:
				break;
		}

		return nextSample;
	}
};