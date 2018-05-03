#include "Filter.h"

// Author: Martin Finke @ http://www.martin-finke.de/blog/articles/audio-plugins-013-filter/
// Algorithm by Paul Kellett http://www.musicdsp.org/showone.php?id=29


double Filter::process (double inputValue) 
{

	if (inputValue == 0.0) return inputValue;
	double calculatedCutoff = getCalculatedCutoff();
	buf0 += calculatedCutoff * (inputValue - buf0);
	buf1 += calculatedCutoff * (buf0 - buf1);

	switch (mode)
	{
		case FILTER_MODE_LOWPASS:
			return buf1;
		case FILTER_MODE_HIGHPASS:
			return inputValue - buf0;
		case FILTER_MODE_BANDPASS:
			return buf0 - buf1;
		default:
			return 0.0;
	}
}

