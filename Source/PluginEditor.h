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
class StereoWidthAudioProcessorEditor  : public AudioProcessorEditor
{
public:

	typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
	
	enum
	{
		paramControlHeight = 40,
		paramLabelWidth = 60,
		paramSliderWidth = 300
	};

    StereoWidthAudioProcessorEditor (StereoWidthAudioProcessor&, AudioProcessorValueTreeState&);
    ~StereoWidthAudioProcessorEditor();

	//==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
	AudioProcessorValueTreeState& valueTreeState;

	// sliders
	Label widthLabel, gainLabel;
	
	Slider widthSlider, gainSlider;
	
	std::unique_ptr<SliderAttachment> widthSliderAttachment, gainSliderAttachment;
	
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    StereoWidthAudioProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StereoWidthAudioProcessorEditor)
};
