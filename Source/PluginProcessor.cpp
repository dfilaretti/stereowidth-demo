/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
StereoWidthAudioProcessor::StereoWidthAudioProcessor() 
	: parameters(*this, nullptr, "StereoWidth", 
					{ std::make_unique<AudioParameterFloat>("WIDTH", "Width", NormalisableRange<float>(0.0f, 2.0f), 1.0f),
					  std::make_unique<AudioParameterFloat>("GAIN", "Gain", NormalisableRange<float>(-99.f, 12.f), 0.f) })
#ifndef JucePlugin_PreferredChannelConfigurations
    , AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
	widthParam = parameters.getRawParameterValue("WIDTH");
	gainParam  = parameters.getRawParameterValue("GAIN");
}

StereoWidthAudioProcessor::~StereoWidthAudioProcessor()
{
}

//==============================================================================
const String StereoWidthAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool StereoWidthAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool StereoWidthAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool StereoWidthAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double StereoWidthAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int StereoWidthAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int StereoWidthAudioProcessor::getCurrentProgram()
{
    return 0;
}

void StereoWidthAudioProcessor::setCurrentProgram (int /*index*/)
{
}

const String StereoWidthAudioProcessor::getProgramName (int /*index*/)
{
    return {};
}

void StereoWidthAudioProcessor::changeProgramName (int /*index*/, const String& /*newName*/)
{
}

//==============================================================================
void StereoWidthAudioProcessor::prepareToPlay (double /*sampleRate*/, int /*samplesPerBlock*/)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void StereoWidthAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool StereoWidthAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void StereoWidthAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& /*midiMessages*/)
{
	ScopedNoDenormals noDenormals;
	auto totalNumInputChannels = getTotalNumInputChannels();
	auto totalNumOutputChannels = getTotalNumOutputChannels();
	// -----------------------------------------------------------------------
	int numSamples = buffer.getNumSamples();
	float width = *widthParam;
	float gain = *gainParam;
	gain = pow(10, (gain / 20.0));
	AudioSampleBuffer midBuffer, sideBuffer;

	// In case we have more outputs than inputs, this code clears any output
	// channels that didn't contain input data, (because these aren't
	// guaranteed to be empty - they may contain garbage).
	// This is here to avoid people getting screaming feedback
	// when they first compile a plugin, but obviously you don't need to keep
	// this code if your algorithm always overwrites all the output channels.
	for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
		buffer.clear(i, 0, buffer.getNumSamples());

	// SAFETY CHECK :::: since some hosts will change buffer sizes without calling prepToPlay (Bitwig)
	// NB: seems not just BitWig related - if not present, it always crash
	if (midBuffer.getNumSamples() != numSamples)
	{
		midBuffer.setSize(2, numSamples, false, true, true);
		sideBuffer.setSize(2, numSamples, false, true, true);
	}

	// Get left and right channels separately
	float* left = buffer.getWritePointer(0);
	float* right = buffer.getWritePointer(1);

	// Create Mid
	// mid = 0.5 * (left + right)
	midBuffer.copyFrom(0, 0, left, numSamples);
	FloatVectorOperations::add(midBuffer.getWritePointer(0), right, numSamples);
	FloatVectorOperations::multiply(midBuffer.getWritePointer(0), 0.5f, numSamples);
	
	// Create Side
	// side = 0.5 * (left - right)
	sideBuffer.copyFrom(0, 0, left, numSamples);
	FloatVectorOperations::subtract(sideBuffer.getWritePointer(0), right, numSamples);
	FloatVectorOperations::multiply(sideBuffer.getWritePointer(0), 0.5f, numSamples);

	// Scale mid
	// mid' = (2 - width) * mid
	FloatVectorOperations::multiply(midBuffer.getWritePointer(0), 2 - width, numSamples);

	// Scale side
	// side' = width * side
	FloatVectorOperations::multiply(sideBuffer.getWritePointer(0), width, numSamples);

	// New left
	// left' = mid' + side'
	FloatVectorOperations::clear(left, numSamples);
	FloatVectorOperations::add(left, midBuffer.getWritePointer(0), numSamples);
	FloatVectorOperations::add(left, sideBuffer.getWritePointer(0), numSamples);

	// New right
	// right' = mid' - side'
	FloatVectorOperations::clear(right, numSamples);
	FloatVectorOperations::add(right, midBuffer.getWritePointer(0), numSamples);
	FloatVectorOperations::subtract(right, sideBuffer.getWritePointer(0), numSamples);

	// Apply gain
	FloatVectorOperations::multiply(right, gain, numSamples);
	FloatVectorOperations::multiply(left, gain, numSamples);

}

//==============================================================================
bool StereoWidthAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* StereoWidthAudioProcessor::createEditor()
{
    return new StereoWidthAudioProcessorEditor (*this, parameters);
}

//==============================================================================
void StereoWidthAudioProcessor::getStateInformation (MemoryBlock& destData)
{
	auto state = parameters.copyState();
	std::unique_ptr<XmlElement> xml(state.createXml());
	copyXmlToBinary(*xml, destData);
}

void StereoWidthAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
	std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

	if (xmlState.get() != nullptr)
		if (xmlState->hasTagName(parameters.state.getType()))
			parameters.replaceState(ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new StereoWidthAudioProcessor();
}
