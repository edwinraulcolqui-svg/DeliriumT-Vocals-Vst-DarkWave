#include "PluginEditor.h"

static void styleSlider(juce::Slider& s)
{
    s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 68, 18);
    s.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xffa56bd4));
    s.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff332044));
    s.setColour(juce::Slider::thumbColourId, juce::Colour(0xffeee3ff));
    s.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    s.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff17101f));
    s.setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xff51366a));
}
DeliriumTVocalsAudioProcessorEditor::DeliriumTVocalsAudioProcessorEditor(DeliriumTVocalsAudioProcessor& p)
: AudioProcessorEditor(&p), processor(p)
{
    setSize(1320, 790);
    title.setText("DXTXM", juce::dontSendNotification);
    title.setFont(juce::Font(juce::FontOptions(66.0f, juce::Font::bold)));
    title.setColour(juce::Label::textColourId, juce::Colours::white);
    title.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(title);
    subtitle.setText("DELIRIUMT VOCALS  /  DARKWAVE SIGNAL PROCESSOR", juce::dontSendNotification);
    subtitle.setFont(juce::Font(juce::FontOptions(17.0f, juce::Font::bold)));
    subtitle.setColour(juce::Label::textColourId, juce::Colour(0xffc49be8));
    subtitle.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(subtitle);
    footer.setText("DARK GRAVE  •  INDEPENDENT DARK ELECTRONIC AUDIO TOOLS", juce::dontSendNotification);
    footer.setFont(juce::Font(juce::FontOptions(12.0f)));
    footer.setColour(juce::Label::textColourId, juce::Colour(0xff9b78b9));
    footer.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(footer);

    presets.addItem("GOTHIC VOICE",1); presets.addItem("COLD DARKWAVE",2);
    presets.addItem("ETHEREAL VOICE",3); presets.addItem("DEATHROCK EDGE",4);
    presets.addItem("POST-PUNK VOICE",5);
    presets.setSelectedId(1);
    presets.onChange = [this] { processor.loadPreset(presets.getSelectedId()-1); };
    presets.setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff1a1025));
    presets.setColour(juce::ComboBox::textColourId, juce::Colours::white);
    presets.setColour(juce::ComboBox::outlineColourId, juce::Colour(0xff8654b3));
    addAndMakeVisible(presets);

    styleSlider(input); input.setName("INPUT"); addAndMakeVisible(input);
    styleSlider(output); output.setName("OUTPUT"); addAndMakeVisible(output);
    inputAttachment = std::make_unique<SliderAttachment>(processor.parameters,"input",input);
    outputAttachment = std::make_unique<SliderAttachment>(processor.parameters,"output",output);
    bypass.setClickingTogglesState(true);
    bypassAttachment = std::make_unique<ButtonAttachment>(processor.parameters,"masterBypass",bypass);
    bypass.setColour(juce::TextButton::buttonColourId,juce::Colour(0xff39234c));
    bypass.setColour(juce::TextButton::buttonOnColourId,juce::Colour(0xff8d52bd));
    addAndMakeVisible(bypass);

    addModule("01  /  EQUALIZER","eqOn",{{"low","LOW"},{"mid","MID"},{"high","HIGH"},{"midFreq","MID FREQ"}});
    addModule("02  /  COMPRESSOR","compOn",{{"threshold","THRESH"},{"ratio","RATIO"},{"attack","ATTACK"},{"release","RELEASE"},{"makeup","MAKEUP"}});
    addModule("03  /  CHORUS","chorusOn",{{"chorusRate","RATE"},{"chorusDepth","DEPTH"},{"chorusMix","MIX"}});
    addModule("04  /  DELAY","delayOn",{{"delayTime","TIME"},{"feedback","FEEDBACK"},{"delayMix","MIX"},{"stereo","STEREO"}});
    addModule("05  /  REVERB","reverbOn",{{"decay","DECAY"},{"room","SIZE"},{"damping","DAMPING"},{"predelay","PRE-DELAY"},{"reverbMix","MIX"}});
    addModule("06  /  SATURATION","satOn",{{"drive","DRIVE"},{"tone","TONE"},{"satMix","MIX"}});
}

void DeliriumTVocalsAudioProcessorEditor::addModule(const juce::String& name, const juce::String& toggleId,
    std::initializer_list<std::pair<const char*, const char*>> params)
{
    auto m = std::make_unique<Module>(); m->name = name;
    m->toggle.setButtonText("ON");
    m->toggle.setClickingTogglesState(true);
    m->toggle.setColour(juce::TextButton::buttonColourId,juce::Colour(0xff261632));
    m->toggle.setColour(juce::TextButton::buttonOnColourId,juce::Colour(0xff8c54b9));
    addAndMakeVisible(m->toggle);
    m->buttonAttachment = std::make_unique<ButtonAttachment>(processor.parameters,toggleId,m->toggle);
    for (auto [id, labelText] : params)
    {
        auto s = std::make_unique<juce::Slider>(); styleSlider(*s);
        auto l = std::make_unique<juce::Label>(); l->setText(labelText,juce::dontSendNotification);
        l->setFont(juce::Font(juce::FontOptions(10.0f,juce::Font::bold)));
        l->setColour(juce::Label::textColourId,juce::Colour(0xffc6a5df));
        l->setJustificationType(juce::Justification::centred);
        addAndMakeVisible(*s); addAndMakeVisible(*l);
        m->attachments.push_back(std::make_unique<SliderAttachment>(processor.parameters,id,*s));
        m->sliders.push_back(std::move(s)); m->labels.push_back(std::move(l));
    }
    modules.push_back(std::move(m));
}

void DeliriumTVocalsAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff05040a));
    // Pixel-sunset and skyline motif, drawn natively so no external artwork is required.
    g.setColour(juce::Colour(0xff26143a));
    for (int i=0;i<13;++i) {
        int w=150-i*10, y=125+i*13;
        g.fillRect(getWidth()/2-w/2,y,w,5);
    }
    g.setColour(juce::Colour(0xff100b18));
    juce::Random r(73);
    for (int x=0;x<getWidth();x+=24) {
        int h=35+r.nextInt(95);
        g.fillRect(x,getHeight()-h-12,20,h);
        g.setColour(juce::Colour(0xff50316a));
        for(int wy=getHeight()-h+5;wy<getHeight()-20;wy+=13)
            g.fillRect(x+5,wy,3,3);
        g.setColour(juce::Colour(0xff100b18));
    }
    g.setColour(juce::Colour(0xff8f5bb8).withAlpha(0.24f));
    g.drawRect(12,12,getWidth()-24,getHeight()-24,2);
    g.setColour(juce::Colour(0xffa16acb));
    g.drawLine(28,182,getWidth()-28,182,1.0f);
    g.drawLine(28,getHeight()-48,getWidth()-28,getHeight()-48,1.0f);
    // Module bays
    for (int i=0;i<6;++i) {
        int col=i%3, row=i/3;
        juce::Rectangle<int> area(24+col*432,210+row*255,416,238);
        g.setColour(juce::Colour(0xff120d1b).withAlpha(.96f)); g.fillRoundedRectangle(area.toFloat(),7.0f);
        g.setColour(juce::Colour(0xff71478e)); g.drawRoundedRectangle(area.toFloat(),7.0f,1.2f);
        g.setColour(juce::Colour(0xffb88bdb)); g.setFont(juce::Font(juce::FontOptions(14.0f,juce::Font::bold)));
        g.drawText(modules[(size_t)i]->name,area.removeFromTop(28).reduced(12,0),juce::Justification::centredLeft);
    }
}
void DeliriumTVocalsAudioProcessorEditor::resized()
{
    title.setBounds(20,10,getWidth()-40,75);
    subtitle.setBounds(20,82,getWidth()-40,25);
    presets.setBounds(28,145,210,28);
    input.setBounds(260,130,100,65); output.setBounds(375,130,100,65);
    bypass.setBounds(getWidth()-210,145,180,30);
    footer.setBounds(20,getHeight()-42,getWidth()-40,24);
    for (size_t i=0;i<modules.size();++i) {
        int col=(int)i%3, row=(int)i/3;
        auto x=24+col*432, y=210+row*255;
        modules[i]->toggle.setBounds(x+340,y+8,64,23);
        int count=(int)modules[i]->sliders.size();
        int available=398, cell=available/juce::jmax(1,count);
        for(int j=0;j<count;++j) {
            int sx=x+10+j*cell;
            modules[i]->labels[(size_t)j]->setBounds(sx,y+48,cell-4,20);
            modules[i]->sliders[(size_t)j]->setBounds(sx,y+67,cell-4,145);
        }
    }
}
