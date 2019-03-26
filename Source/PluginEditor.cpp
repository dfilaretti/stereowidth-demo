/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
StereoWidthAudioProcessorEditor::StereoWidthAudioProcessorEditor (StereoWidthAudioProcessor& p, AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), processor (p), valueTreeState(vts)
{
	getLookAndFeel().setColour(Slider::trackColourId, Colours::white);
	getLookAndFeel().setColour(Slider::thumbColourId, Colours::orange);

	widthLabel.setText("Width", dontSendNotification);
	gainLabel.setText("Gain", dontSendNotification);

	addAndMakeVisible(widthLabel);
	addAndMakeVisible(gainLabel);
	addAndMakeVisible(widthSlider);
	addAndMakeVisible(gainSlider);

	gainSliderAttachment.reset(new SliderAttachment(valueTreeState, "GAIN", gainSlider));
	widthSliderAttachment.reset(new SliderAttachment(valueTreeState, "WIDTH", widthSlider));

	setSize(400, 100);
}

StereoWidthAudioProcessorEditor::~StereoWidthAudioProcessorEditor()
{
}

//==============================================================================
void StereoWidthAudioProcessorEditor::paint (Graphics& g)
{
	g.fillAll(Colours::black);
}

void StereoWidthAudioProcessorEditor::resized()
{
	auto r = getLocalBounds();

	auto widthRect = r.removeFromTop(paramControlHeight);
	widthLabel.setBounds(widthRect.removeFromLeft(paramLabelWidth));
	widthSlider.setBounds(widthRect);

	auto gainRect = r.removeFromTop(paramControlHeight);
	gainLabel.setBounds(gainRect.removeFromLeft(paramLabelWidth));
	gainSlider.setBounds(gainRect);
}