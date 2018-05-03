/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#include <ctime>
#include <cstdlib>

class FypAudioProcessorEditor::ParameterSlider : public Slider
{
public:
	ParameterSlider (AudioProcessorParameter& p)
		: Slider (p.getName (256)), param (p)
	{
		updateSliderPos();
	}

	void valueChanged() override 
	{ 
		param.setValueNotifyingHost ((float) Slider::getValue());
	}

	//void timerCallback() override { updateSliderPos(); }

	void startedDragging() override { param.beginChangeGesture(); }
	void stoppedDragging() override { param.endChangeGesture(); }

	double getValueFromText(const String& text) override { return param.getValueForText(text); }
	String getTextFromValue(double value) override { return param.getText((float)value, 1024); }

	void updateSliderPos()
	{	
		const float newValue = param.getValue();
		if (newValue != (float) Slider::getValue() && !isMouseButtonDown())
			Slider::setValue (newValue, NotificationType::dontSendNotification);
	}

	AudioProcessorParameter& param;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterSlider)
};


class FypAudioProcessorEditor::MyParameterIntSlider : public Slider
{
public:
	MyParameterIntSlider (AudioParameterInt& p)
		: Slider(), param (p)
	{
		//setRange(0.0, 0.99, 0.1);
		updateSliderPos();
	}

	void valueChanged() override
	{
		if (param.name.equalsIgnoreCase("delay rate"))
		{
			param.setValueNotifyingHost((float) Slider::getValue() / 12000.0f);
		}
		else if (param.name.equalsIgnoreCase ("osc1 pitch") ||
			     param.name.equalsIgnoreCase ("osc2 pitch") ||
			     param.name.equalsIgnoreCase ("osc3 pitch"))
		{
			param.setValueNotifyingHost(((float) Slider::getValue() + 12) / 24.0f);
		}
		else
			param.setValueNotifyingHost(((float) Slider::getValue() + 100) / 200.0f);
		//param = limitRange (roundToInt ((v * (float) rangeOfValues) + minValue));
	} 

	//void timerCallback() override { updateSliderPos(); }

	void startedDragging() override { param.beginChangeGesture(); }
	void stoppedDragging() override { param.endChangeGesture(); }

	//double getValueFromText(const String& text) override { return param.getValueForText(text); }
	//String getTextFromValue(double value) override { return param.getText((float)value, 1024); }

	void updateSliderPos()
	{
		//param.getValue() = (limitRange (v) - minValue) / (float) rangeOfValues;
		const double newValue = (double) param.get();
		if (newValue != (float) Slider::getValue() && !isMouseButtonDown())
			Slider::setValue (newValue, NotificationType::dontSendNotification);
	}

	AudioParameterInt& param;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MyParameterIntSlider)
};


//==============================================================================
FypAudioProcessorEditor::FypAudioProcessorEditor (FypAudioProcessor& owner)
    : AudioProcessorEditor (owner), processor (owner),
	midiKeyboard (owner.keyboardState, MidiKeyboardComponent::horizontalKeyboard),
	gainLabel       (String(), "LEVEL"),
	delayLabel      (String(), "DELAY\nAMOUNT"),
	delayRateLabel  (String(), "DELAY\nRATE"),
	filterLabel     (String(), "FILTER\nCUTOFF"),
	attackLabel     (String(), "ATK"),
	decayLabel      (String(), "DEC"),
	sustainLabel    (String(), "SUS"),
	releaseLabel    (String(), "REL"),
	waveformLabel   (String(), "OSCILLATORS"),
	lfoLabel        (String(), "LFO SHAPE"),
	lfoTargetLabel  (String(), "LFO TARGET"),
	filterTypeLabel (String(), "FILTER TYPE"),
	lfoFreqLabel    (String(), "LFO RATE"),
	detuneLabel     (String(), "FINE"),
	pitchLabel      (String(), "PITCH"),
	mixLabel        (String(), "MIX"),
	lfoAmountLabel  (String(), "LFO AMOUNT")
{
	addAndMakeVisible (gainSlider = new ParameterSlider (*owner.gainParam));
	gainSlider->setSliderStyle (Slider::Rotary);
	gainSlider->setRange (0.0, 1.0, 0.005);
	gainSlider->setTextBoxStyle (Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);

	addAndMakeVisible (delaySlider = new ParameterSlider (*owner.delayParam));
	delaySlider->setSliderStyle (Slider::Rotary);
	delaySlider->setRange (0.0, 0.9, 0.005);
	delaySlider->setTextBoxStyle (Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);

	addAndMakeVisible (delayRateSlider = new MyParameterIntSlider (*owner.delayRateParam));
	delayRateSlider->setSliderStyle (Slider::Rotary);
	delayRateSlider->setRange (0.0, 12000.0, 1.0);
	delayRateSlider->setTextBoxStyle (Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);

	addAndMakeVisible (filterCutoffSlider = new ParameterSlider (*owner.filterCutoffParam));
	filterCutoffSlider->setSliderStyle (Slider::Rotary);
	filterCutoffSlider->setRange (0.0, 0.99, 0.001);
	filterCutoffSlider->setTextBoxStyle (Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);



	addAndMakeVisible (OSC1_pitchSlider = new MyParameterIntSlider (*owner.OSC1_pitchParam));
	OSC1_pitchSlider->setSliderStyle (Slider::IncDecButtons);
	OSC1_pitchSlider->setRange (-12.0, 12.0, 1.0);
	//OSC1_pitchSlider->setScrollWheelEnabled(true);

	addAndMakeVisible (OSC2_pitchSlider = new MyParameterIntSlider (*owner.OSC2_pitchParam));
	OSC2_pitchSlider->setSliderStyle (Slider::IncDecButtons);
	OSC2_pitchSlider->setRange (-12.0, 12.0, 1.0);
	OSC2_pitchSlider->setScrollWheelEnabled (true);

	addAndMakeVisible (OSC3_pitchSlider = new MyParameterIntSlider (*owner.OSC3_pitchParam));
	OSC3_pitchSlider->setSliderStyle (Slider::IncDecButtons);
	OSC3_pitchSlider->setRange (-12.0, 12.0, 1.0);
	OSC3_pitchSlider->setScrollWheelEnabled (true);

	addAndMakeVisible (OSC1_detuneSlider = new MyParameterIntSlider (*owner.OSC1_detuneParam));
	OSC1_detuneSlider->setSliderStyle (Slider::IncDecButtons);
	OSC1_detuneSlider->setRange (-100.0, 100.0, 1.0);
	//OSC1_detuneSlider->setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);

	addAndMakeVisible (OSC2_detuneSlider = new MyParameterIntSlider (*owner.OSC2_detuneParam));
	OSC2_detuneSlider->setSliderStyle (Slider::IncDecButtons);
	OSC2_detuneSlider->setRange (-100.0, 100.0, 1.0);
	//OSC2_detuneSlider->setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);

	addAndMakeVisible (OSC3_detuneSlider = new MyParameterIntSlider (*owner.OSC3_detuneParam));
	OSC3_detuneSlider->setSliderStyle (Slider::IncDecButtons);
	OSC3_detuneSlider->setRange (-100.0, 100.0, 1.0);
	//OSC3_detuneSlider->setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);

	addAndMakeVisible (OSC1_mixSlider = new ParameterSlider (*owner.OSC1_mixParam));
	OSC1_mixSlider->setSliderStyle (Slider::Rotary);
	OSC1_mixSlider->setRange (0.0, 1.0, 0.01);
	OSC1_mixSlider->setTextBoxStyle (Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);

	addAndMakeVisible (OSC2_mixSlider = new ParameterSlider (*owner.OSC2_mixParam));
	OSC2_mixSlider->setSliderStyle (Slider::Rotary);
	OSC2_mixSlider->setRange (0.0, 1.0, 0.01);
	OSC2_mixSlider->setTextBoxStyle (Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);

	addAndMakeVisible (OSC3_mixSlider = new ParameterSlider (*owner.OSC3_mixParam));
	OSC3_mixSlider->setSliderStyle (Slider::Rotary);
	OSC3_mixSlider->setRange (0.0, 1.0, 0.01);
	OSC3_mixSlider->setTextBoxStyle (Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);

	
	addAndMakeVisible (attackEnvSlider = new ParameterSlider (*owner.attackEnvParam));
	attackEnvSlider->setSliderStyle (Slider::LinearVertical);
	attackEnvSlider->setRange (0.0, 1.0, 0.0001);
	attackEnvSlider->setTextBoxStyle (Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
	
	addAndMakeVisible (decayEnvSlider = new ParameterSlider (*owner.decayEnvParam));
	decayEnvSlider->setSliderStyle (Slider::LinearVertical);
	decayEnvSlider->setRange (0.005, 1.0, 0.0001);
	decayEnvSlider->setTextBoxStyle (Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);

	addAndMakeVisible (sustainEnvSlider = new ParameterSlider (*owner.sustainEnvParam));
	sustainEnvSlider->setSliderStyle (Slider::LinearVertical);
	sustainEnvSlider->setRange (0.0, 1.0, 0.0001);
	sustainEnvSlider->setTextBoxStyle (Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);

	addAndMakeVisible (releaseEnvSlider = new ParameterSlider (*owner.releaseEnvParam));
	releaseEnvSlider->setSliderStyle (Slider::LinearVertical);
	releaseEnvSlider->setRange (0.0, 1.0, 0.0001);
	releaseEnvSlider->setTextBoxStyle (Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);


	addAndMakeVisible (lfoFreqSlider = new ParameterSlider (*owner.lfoFrequencyParam));
	lfoFreqSlider->setSliderStyle (Slider::Rotary);
	lfoFreqSlider->setRange (0.0, 1.0, 0.01);
	lfoFreqSlider->setTextBoxStyle (Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);

	addAndMakeVisible (lfoAmountSlider = new ParameterSlider (*owner.lfoAmountParam));
	lfoAmountSlider->setSliderStyle (Slider::Rotary);
	lfoAmountSlider->setRange (0.0, 1.0, 0.01);
	lfoAmountSlider->setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);


	//LFO TARGET SELECTOR
	addAndMakeVisible (lfoTargetBox);
	auto q = 1;
	for (auto choice : owner.lfoTargetParam->choices)
		lfoTargetBox.addItem (choice, q++);

	lfoTargetBox.addListener (this);
	lfoTargetBox.setSelectedId (owner.lfoTargetParam->getIndex() + 1);
	lfoTargetBox.setSize (30, 50);

	addAndMakeVisible (lfoTargetLabel);
	lfoTargetLabel.setJustificationType (Justification::centredLeft);
	lfoTargetLabel.attachToComponent (&lfoTargetBox, false);
	lfoTargetLabel.setFont (Font (11.0f));

	//FILTER TYPE SELECTOR====================================================
	addAndMakeVisible (filterBox);
	auto c = 1;
	for (auto choice : owner.filterTypeParam->choices)
		filterBox.addItem (choice, c++);

	filterBox.addListener (this);
	filterBox.setSelectedId (owner.filterTypeParam->getIndex() + 1);
	filterBox.setSize(3, 5); //30, 50 was huge for some reason

	addAndMakeVisible (filterTypeLabel);
	filterTypeLabel.setJustificationType (Justification::centredLeft);
	filterTypeLabel.attachToComponent (&filterBox, false);
	filterTypeLabel.setFont (Font (11.0f));

	//WAVEFORM 1 SELECTOR===================================================
	addAndMakeVisible (waveformBox);
	auto i = 1;
	for (auto choice : owner.waveformParam->choices)
		waveformBox.addItem (choice, i++);

	waveformBox.addListener (this);
	waveformBox.setSelectedId (owner.waveformParam->getIndex() + 1);
	waveformBox.setSize (30, 50);
	//waveformBox.setWantsKeyboardFocus(true);

	addAndMakeVisible (waveformLabel);
	//waveformLabel.setJustificationType(Justification::centredLeft);
	waveformLabel.attachToComponent (&waveformBox, false);
	waveformLabel.setFont (Font (14.0f));

	//WAVEFORM 2 SELECTOR===================================================
	addAndMakeVisible (waveform2Box);
	auto k = 1;
	for (auto choice : owner.waveform2Param->choices)
		waveform2Box.addItem (choice, k++);

	waveform2Box.addListener (this);
	waveform2Box.setSelectedId (owner.waveform2Param->getIndex() + 1);
	waveform2Box.setSize (30, 50);

	//addAndMakeVisible(waveform2Label);
	//waveform2Label.setJustificationType(Justification::centredLeft);
	//waveform2Label.attachToComponent(&waveform2Box, true);

	//WAVEFORM 3 SELECTOR===================================================
	addAndMakeVisible (waveform3Box);
	auto j = 1;
	for (auto choice : owner.waveform3Param->choices)
		waveform3Box.addItem (choice, j++);

	waveform3Box.addListener (this);
	waveform3Box.setSelectedId (owner.waveform3Param->getIndex() + 1);
	waveform3Box.setSize (30, 50);

	//addAndMakeVisible(waveform3Label);
	//waveform3Label.setJustificationType(Justification::centredLeft);
	//waveform3Label.attachToComponent(&waveform3Box, true);
	//====================================================================

	//LFO SHAPE SELECTOR===================================================
	addAndMakeVisible(lfoShapeBox);
	auto r = 1;
	for (auto choice : owner.lfoShapeParam->choices)
		lfoShapeBox.addItem (choice, r++);

	lfoShapeBox.addListener (this);
	lfoShapeBox.setSelectedId (owner.lfoShapeParam->getIndex() + 1);
	lfoShapeBox.setSize (30, 50);

	addAndMakeVisible (lfoLabel);
	lfoLabel.setJustificationType (Justification::centredLeft);
	lfoLabel.attachToComponent (&lfoShapeBox, false);
	lfoLabel.setFont (Font (11.0f));
	//====================================================================


	// add some labels for the sliders..
	gainLabel.attachToComponent (gainSlider, false);
	gainLabel.setFont (Font (11.0f));
	gainLabel.setJustificationType(Justification::centred);

	delayLabel.attachToComponent (delaySlider, false);
	delayLabel.setFont (Font (11.0f));
	delayLabel.setJustificationType(Justification::centred);

	delayRateLabel.attachToComponent (delayRateSlider, false);
	delayRateLabel.setFont (Font (11.0f));
	delayRateLabel.setJustificationType(Justification::centred);

	filterLabel.attachToComponent(filterCutoffSlider, false);
	filterLabel.setFont(Font(11.0f));
	filterLabel.setJustificationType(Justification::centred);

	attackLabel.attachToComponent(attackEnvSlider, false);
	attackLabel.setFont(Font(11.0f));
	attackLabel.setJustificationType(Justification::centred);

	decayLabel.attachToComponent(decayEnvSlider, false);
	decayLabel.setFont(Font(11.0f));
	decayLabel.setJustificationType(Justification::centred);

	sustainLabel.attachToComponent(sustainEnvSlider, false);
	sustainLabel.setFont(Font(11.0f));
	sustainLabel.setJustificationType(Justification::centred);

	releaseLabel.attachToComponent(releaseEnvSlider, false);
	releaseLabel.setFont(Font(11.0f));
	releaseLabel.setJustificationType(Justification::centred);

	lfoFreqLabel.attachToComponent (lfoFreqSlider, false);
	lfoFreqLabel.setFont (Font (11.0f));
	lfoFreqLabel.setJustificationType (Justification::centred);

	lfoAmountLabel.attachToComponent (lfoAmountSlider, false);
	lfoAmountLabel.setFont (Font (11.0f));
	lfoAmountLabel.setJustificationType (Justification::centred);

	pitchLabel.attachToComponent (OSC1_pitchSlider, false);
	pitchLabel.setFont (Font (14.0f));
	pitchLabel.setJustificationType (Justification::centred);

	detuneLabel.attachToComponent (OSC1_detuneSlider, false);
	detuneLabel.setFont (Font (14.0f));
	detuneLabel.setJustificationType (Justification::centred);

	mixLabel.attachToComponent (OSC1_mixSlider, false);
	mixLabel.setFont (Font (14.0f));
	mixLabel.setJustificationType (Justification::centred);

	mixLabel.attachToComponent(OSC1_mixSlider, false);
	mixLabel.setFont (Font (14.0f));
	mixLabel.setJustificationType(Justification::centred);



	// add the midi keyboard component..
	addAndMakeVisible(midiKeyboard);
	midiKeyboard.setAvailableRange(9, 96);
	midiKeyboard.setOctaveForMiddleC(6);
	midiKeyboard.setWantsKeyboardFocus(true);
	//midiKeyboard.clearKeyMappings(); //ensures that the PC keyboard mappings used are the ones set by the DAW
	
	

	setSize (owner.lastUIWidth, owner.lastUIHeight);

	updateTrackProperties();
}

FypAudioProcessorEditor::~FypAudioProcessorEditor()
{

}

//==============================================================================
void FypAudioProcessorEditor::paint (Graphics& g)
{
    g.setColour(backgroundColour);
	g.fillAll();
}

void FypAudioProcessorEditor::resized()
{
	Rectangle<int> r(getLocalBounds().reduced(8));

	midiKeyboard.setBounds(r.removeFromBottom(70));

	r.removeFromTop(30);
	Rectangle<int> sliderArea(r.removeFromTop(60));

	gainSlider->setBounds(sliderArea.removeFromLeft(jmin(180, sliderArea.getWidth() / 7) - 10));
	filterCutoffSlider->setBounds(sliderArea.removeFromLeft(jmin(180, sliderArea.getWidth() / 6) - 10));
	//filterCutoffSlider->setPopupDisplayEnabled(true, false, nullptr, 2000);
	filterBox.setBounds(sliderArea.removeFromLeft(jmin(180, sliderArea.getWidth() / 5)));

	lfoAmountSlider->setBounds(sliderArea.removeFromLeft(jmin(180, sliderArea.getWidth() / 4) - 10));
	lfoFreqSlider->setBounds(sliderArea.removeFromLeft(jmin(180, sliderArea.getWidth() / 3) - 10));

	lfoShapeBox.setBounds(sliderArea.removeFromLeft(jmin(180, sliderArea.getWidth() / 2)));
	lfoTargetBox.setBounds(sliderArea.removeFromLeft(jmin(180, sliderArea.getWidth())));

	delaySlider->setBounds(500, 132, 75, 75);
	delayRateSlider->setBounds(590, 132, 75, 75);

	filterBox.setSize(90, 30);
	lfoShapeBox.setSize(90, 30);
	lfoTargetBox.setSize(90, 30);

	Rectangle<int> OSCArea(r.removeFromTop(152)); //REVERT TO 200*****************************************************
	Rectangle<int> ADSRArea = OSCArea;
	ADSRArea.removeFromBottom(20);
	ADSRArea.removeFromRight(300);
	OSCArea.removeFromRight(570);
	OSCArea.removeFromTop(30);
	Rectangle<int> pitchArea = OSCArea;
	//Rectangle<int> mixArea = pitchArea;
	//mixArea.setX (mixArea.getX() + pitchArea.getWidth());
	//mixArea.setY (mixArea.getY());


	OSC1_detuneSlider->setBounds(200, 128, 74, 30);
	OSC2_detuneSlider->setBounds(200, 158, 74, 30);
	OSC3_detuneSlider->setBounds(200, 188, 74, 30);

	OSC1_mixSlider->setBounds(275, 122, 45, 42);
	OSC2_mixSlider->setBounds(275, 153, 45, 42);
	OSC3_mixSlider->setBounds(275, 184, 45, 42);

	waveformBox.setBounds(OSCArea.removeFromTop(30));
	waveform2Box.setBounds(OSCArea.removeFromTop(30));
	waveform3Box.setBounds(OSCArea.removeFromTop(30));

	ADSRArea.removeFromLeft(OSCArea.getWidth() + 110);
	ADSRArea.setX(ADSRArea.getX() + 100);

	Rectangle<int> attackEnvBounds = ADSRArea.removeFromLeft(ADSRArea.getWidth() / 4);
	attackEnvBounds.removeFromTop(30);

	Rectangle<int> decayEnvBounds = ADSRArea.removeFromLeft(ADSRArea.getWidth() / 3);
	decayEnvBounds.removeFromTop(30);

	Rectangle<int> sustainEnvBounds = ADSRArea.removeFromLeft(ADSRArea.getWidth() / 2);
	sustainEnvBounds.removeFromTop(30);

	Rectangle<int> releaseEnvBounds = ADSRArea.removeFromLeft(ADSRArea.getWidth());
	releaseEnvBounds.removeFromTop(30);

	//Rectangle<int> mixCopy(attackEnvBounds);
	//mixCopy.setX(mixCopy.getX() - 80);
	//mixCopy.setY(mixCopy.getY() - 40);

	attackEnvSlider->setBounds(attackEnvBounds);
	decayEnvSlider->setBounds(decayEnvBounds);
	sustainEnvSlider->setBounds(sustainEnvBounds);
	releaseEnvSlider->setBounds(releaseEnvBounds);

	/*
	filterEnvAtkSlider->setBounds(ADSRArea.removeFromLeft(ADSRArea.getWidth() / 5));
	filterEnvDecSlider->setBounds(ADSRArea.removeFromLeft(ADSRArea.getWidth() / 4));
	filterEnvSusSlider->setBounds(ADSRArea.removeFromLeft(ADSRArea.getWidth() / 3));
	filterEnvRelSlider->setBounds(ADSRArea.removeFromLeft(ADSRArea.getWidth() /2));
	filterEnvAmountSlider->setBounds(ADSRArea.removeFromLeft(ADSRArea.getWidth()));
	*/

	Point<int> pitchTopLeft = pitchArea.getPosition();
	pitchTopLeft.setX(pitchArea.getWidth());
	pitchArea.setPosition(pitchTopLeft);
	pitchArea.removeFromRight(30);


	OSC1_pitchSlider->setBounds(pitchArea.removeFromTop(30));
	OSC2_pitchSlider->setBounds(pitchArea.removeFromTop(30));
	OSC3_pitchSlider->setBounds(pitchArea.removeFromTop(30));



	OSC1_detuneSlider->setValue(*(getProcessor().OSC1_detuneParam), NotificationType::dontSendNotification);
	OSC2_detuneSlider->setValue(*(getProcessor().OSC2_detuneParam), NotificationType::dontSendNotification);
	OSC3_detuneSlider->setValue(*(getProcessor().OSC3_detuneParam), NotificationType::dontSendNotification);

	OSC1_pitchSlider->setValue(*(getProcessor().OSC1_pitchParam), NotificationType::dontSendNotification);
	OSC2_pitchSlider->setValue(*(getProcessor().OSC2_pitchParam), NotificationType::dontSendNotification);
	OSC3_pitchSlider->setValue(*(getProcessor().OSC3_pitchParam), NotificationType::dontSendNotification);

	gainSlider->setValue(*(getProcessor().gainParam), NotificationType::dontSendNotification);
	lfoAmountSlider->setValue(*(getProcessor().lfoAmountParam), NotificationType::dontSendNotification);
	filterCutoffSlider->setValue(*(getProcessor().filterCutoffParam), NotificationType::dontSendNotification);


	switch (*(getProcessor().filterTypeParam))
	{
		case 0:
			filterBox.setText("LPF");
			break;
		case 1:
			filterBox.setText("HPF");
			break;
		case 2:
			filterBox.setText("BPF");
			break;
		default:
			break;
	}

	switch (*(getProcessor().waveformParam))
	{
		case 0:
			waveformBox.setText("SINE");
			break;
		case 1:
			waveformBox.setText("TRIANGLE");
			break;
		case 2:
			waveformBox.setText("SQUARE");
			break;
		case 3:
			waveformBox.setText("SAW");
			break;
		default:
			break;
	}

	switch (*(getProcessor().waveform2Param))
	{
	case 0:
		waveform2Box.setText("SINE");
		break;
	case 1:
		waveform2Box.setText("TRIANGLE");
		break;
	case 2:
		waveform2Box.setText("SQUARE");
		break;
	case 3:
		waveform2Box.setText("SAW");
		break;
	default:
		break;
	}

	switch (*(getProcessor().waveform3Param))
	{
	case 0:
		waveform3Box.setText("SINE");
		break;
	case 1:
		waveform3Box.setText("TRIANGLE");
		break;
	case 2:
		waveform3Box.setText("SQUARE");
		break;
	case 3:
		waveform3Box.setText("SAW");
		break;
	default:
		break;
	}


	switch (*(getProcessor().lfoShapeParam))
	{
	case 0:
		lfoShapeBox.setText("SINE");
		break;
	case 1:
		lfoShapeBox.setText("TRIANGLE");
		break;
	case 2:
		lfoShapeBox.setText("SQUARE");
		break;
	case 3:
		lfoShapeBox.setText("SAW");
		break;
	default:
		break;
	}

	switch (*(getProcessor().lfoTargetParam))
	{
	case 0:
		lfoTargetBox.setText("FILTER");
		break;
	case 1:
		lfoTargetBox.setText("LEVEL");
		break;
	case 2:
		lfoTargetBox.setText("PITCH");
		break;
	default:
		break;
	}


	delaySlider->setValue        (*(getProcessor().delayParam),             NotificationType::dontSendNotification);
	delayRateSlider->setValue    (*(getProcessor().delayRateParam),         NotificationType::dontSendNotification);
	
	lfoFreqSlider->setValue      (*(getProcessor().lfoFrequencyParam) / 30, NotificationType::dontSendNotification);
	attackEnvSlider->setValue    (*(getProcessor().attackEnvParam) * 2,     NotificationType::dontSendNotification);
	decayEnvSlider->setValue     (*(getProcessor().decayEnvParam),          NotificationType::dontSendNotification);
	sustainEnvSlider->setValue   (*(getProcessor().sustainEnvParam),        NotificationType::dontSendNotification);
	releaseEnvSlider->setValue   (*(getProcessor().releaseEnvParam) / 6,    NotificationType::dontSendNotification);

	OSC1_mixSlider->setValue(*(getProcessor().OSC1_mixParam), NotificationType::dontSendNotification);
	OSC2_mixSlider->setValue(*(getProcessor().OSC2_mixParam), NotificationType::dontSendNotification);
	OSC3_mixSlider->setValue(*(getProcessor().OSC3_mixParam), NotificationType::dontSendNotification);



	pitchLabel.setTopLeftPosition (OSC1_pitchSlider->getX() - 12, 106);
	detuneLabel.setTopLeftPosition (OSC1_detuneSlider->getX() - 16, 106);
	mixLabel.setTopLeftPosition (OSC1_mixSlider->getX(), OSC1_mixSlider->getY() - 16);


	getProcessor().lastUIWidth = getWidth();
	getProcessor().lastUIHeight = getHeight();
}


void FypAudioProcessorEditor::hostMIDIControllerIsAvailable (bool controllerIsAvailable)
{
	midiKeyboard.setVisible (!controllerIsAvailable);
}


void FypAudioProcessorEditor::updateTrackProperties()
{
	auto trackColour = getProcessor().trackProperties.colour;
	auto& lf = getLookAndFeel();
	 
	backgroundColour = (trackColour == Colour() ? lf.findColour (ResizableWindow::backgroundColourId)
		: trackColour.withAlpha (1.0f).withBrightness (0.266f));

	repaint();
}


void FypAudioProcessorEditor::comboBoxChanged (ComboBox* box)
{
	auto index = box->getSelectedItemIndex();

	if (box == &waveformBox)
	{
		getProcessor().waveformParam->operator= (index);
	}
	else if (box == &waveform2Box)
	{
		getProcessor().waveform2Param->operator= (index);
	}
	else if (box == &waveform3Box)
	{
		getProcessor().waveform3Param->operator= (index);
	}
	else if (box == &filterBox)
	{
		getProcessor().filterTypeParam->operator= (index);
	}
	else if (box == &lfoShapeBox)
	{
		getProcessor().lfoShapeParam->operator= (index);
	}
	else if (box == &lfoTargetBox)
	{
		getProcessor().lfoTargetParam->operator= (index);
	}
	else {}
}