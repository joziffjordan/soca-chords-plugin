#include "PluginEditor.h"

namespace
{
// Carnival palette: sky, feathers, J'ouvert powder, piano wood.
const juce::Colour kAsphalt  { 0xff17151c };
const juce::Colour kAsphalt2 { 0xff241f2d };
const juce::Colour kControl  { 0xff2f2839 };
const juce::Colour kSky      { 0xff18b6c9 };
const juce::Colour kOrange   { 0xfff36a1f };
const juce::Colour kYellow   { 0xffffcf1a };
const juce::Colour kMagenta  { 0xffe0257c };
const juce::Colour kGreen    { 0xff12a07a };
const juce::Colour kPurple   { 0xff7b3fd1 };
const juce::Colour kIvory    { 0xfff7efdc };
const juce::Colour kDim      { 0xffc9bda8 };
const juce::Colour kWood     { 0xffa9642a };
const juce::Colour kWoodDark { 0xff6b3a17 };

const juce::Colour kCarnival[] = { kOrange, kYellow, kSky, kMagenta, kGreen, kPurple };
const int kNumCarnival = 6;

juce::Font font (float size, bool bold = false)
{
    return juce::Font (juce::FontOptions (size, bold ? juce::Font::bold : juce::Font::plain));
}

juce::Colour textOn (juce::Colour background)
{
    return background.getPerceivedBrightness() > 0.62f ? kAsphalt : juce::Colours::white;
}

/** One feather, growing from `origin` in direction `angle` (radians, clockwise from pointing right). */
void drawFeather (juce::Graphics& g, juce::Point<float> origin, float angle, float length, float width,
                  juce::Colour colour, bool striped)
{
    juce::Graphics::ScopedSaveState state (g);
    g.addTransform (juce::AffineTransform::rotation (angle).translated (origin.x, origin.y));

    const float start = length * 0.16f;
    juce::Path vane;
    vane.startNewSubPath (start, 0.0f);
    vane.quadraticTo (length * 0.55f, -width, length, 0.0f);
    vane.quadraticTo (length * 0.55f, width, start, 0.0f);
    vane.closeSubPath();

    g.setGradientFill ({ colour.darker (0.3f), start, 0.0f, colour.brighter (0.2f), length, 0.0f, false });
    g.fillPath (vane);

    {
        juce::Graphics::ScopedSaveState clip (g);
        g.reduceClipRegion (vane);
        if (striped)
        {
            g.setColour (juce::Colour (0xff2b1708).withAlpha (0.8f));
            for (float x = start; x < length; x += 10.0f)
                g.fillRect (x, -width, 3.5f, width * 2.0f);
        }
        else
        {
            g.setColour (juce::Colours::black.withAlpha (0.13f));
            for (float x = start; x < length; x += 5.0f)
            {
                g.drawLine (x, 0.0f, x + 7.0f, -width, 1.0f);
                g.drawLine (x, 0.0f, x + 7.0f, width, 1.0f);
            }
        }
    }
    g.setColour (kIvory.withAlpha (0.75f));
    g.drawLine (0.0f, 0.0f, length, 0.0f, 1.2f);
}

/** A burst of thrown powder paint. */
void drawPowder (juce::Graphics& g, juce::Random& r, juce::Point<float> centre, float spread, juce::Colour colour, float strength)
{
    g.setGradientFill ({ colour.withAlpha (0.30f * strength), centre.x, centre.y,
                         colour.withAlpha (0.0f), centre.x + spread * 2.2f, centre.y, true });
    g.fillEllipse (centre.x - spread * 2.2f, centre.y - spread * 2.2f, spread * 4.4f, spread * 4.4f);

    for (int i = 0; i < 170; ++i)
    {
        // two uniform randoms -> roughly bell-shaped scatter
        const float dx = (r.nextFloat() + r.nextFloat() + r.nextFloat() - 1.5f) * spread * 1.6f;
        const float dy = (r.nextFloat() + r.nextFloat() + r.nextFloat() - 1.5f) * spread * 1.6f;
        const float size = 0.7f + r.nextFloat() * r.nextFloat() * 4.2f;
        g.setColour (colour.withAlpha ((0.25f + r.nextFloat() * 0.6f) * strength));
        g.fillEllipse (centre.x + dx, centre.y + dy, size, size);
    }
}
}

// ---------------------------------------------------------------------- look and feel
CarnivalLookAndFeel::CarnivalLookAndFeel()
{
    setColour (juce::ComboBox::backgroundColourId, kControl);
    setColour (juce::ComboBox::outlineColourId, juce::Colours::transparentBlack);
    setColour (juce::ComboBox::textColourId, kIvory);
    setColour (juce::ComboBox::arrowColourId, kYellow);
    setColour (juce::PopupMenu::backgroundColourId, kControl);
    setColour (juce::PopupMenu::textColourId, kIvory);
    setColour (juce::PopupMenu::highlightedBackgroundColourId, kMagenta);
    setColour (juce::PopupMenu::highlightedTextColourId, juce::Colours::white);
    setColour (juce::TextButton::buttonColourId, kPurple);
    setColour (juce::TextButton::textColourOffId, juce::Colours::white);
    setColour (juce::TextButton::textColourOnId, kAsphalt);
    setColour (juce::ToggleButton::textColourId, kIvory);
    setColour (juce::ToggleButton::tickColourId, kYellow);
    setColour (juce::ToggleButton::tickDisabledColourId, kDim);
    setColour (juce::Slider::thumbColourId, kYellow);
    setColour (juce::Slider::trackColourId, kMagenta);
    setColour (juce::Slider::backgroundColourId, kControl);
}

void CarnivalLookAndFeel::drawButtonBackground (juce::Graphics& g, juce::Button& b, const juce::Colour& colour, bool over, bool down)
{
    auto r = b.getLocalBounds().toFloat().reduced (1.0f);
    auto c = down ? colour.darker (0.25f) : over ? colour.brighter (0.12f) : colour;
    g.setColour (juce::Colours::black.withAlpha (0.35f));
    g.fillRoundedRectangle (r.translated (0.0f, 2.0f), 11.0f);
    g.setGradientFill ({ c.brighter (0.15f), 0.0f, r.getY(), c.darker (0.2f), 0.0f, r.getBottom(), false });
    g.fillRoundedRectangle (r.withTrimmedBottom (2.0f), 11.0f);
}

juce::Font CarnivalLookAndFeel::getTextButtonFont (juce::TextButton&, int buttonHeight)
{
    return font (juce::jmin (17.0f, (float) buttonHeight * 0.5f), true);
}

void CarnivalLookAndFeel::drawComboBox (juce::Graphics& g, int width, int height, bool, int, int, int, int, juce::ComboBox& box)
{
    auto r = juce::Rectangle<float> (0.0f, 0.0f, (float) width, (float) height);
    g.setColour (box.findColour (juce::ComboBox::backgroundColourId));
    g.fillRoundedRectangle (r, 9.0f);
    g.setColour (kIvory.withAlpha (0.12f));
    g.drawRoundedRectangle (r.reduced (0.5f), 9.0f, 1.0f);

    juce::Path arrow;
    const float cx = (float) width - 18.0f, cy = (float) height * 0.5f;
    arrow.addTriangle (cx - 5.0f, cy - 2.5f, cx + 5.0f, cy - 2.5f, cx, cy + 4.0f);
    g.setColour (box.findColour (juce::ComboBox::arrowColourId));
    g.fillPath (arrow);
}

// ---------------------------------------------------------------------- editor
static const int kHeaderHeight = 96;

SocaChordsEditor::SocaChordsEditor (SocaChordsProcessor& p) : AudioProcessorEditor (&p), proc (p)
{
    setLookAndFeel (&lookAndFeel);

    styleCombo (styleBox, styleLabel, "STYLE");
    styleBox.addItem ("All styles", 1);
    for (int i = 0; i < soca::kNumStyles; ++i) styleBox.addItem (soca::kStyles[i].name, i + 2);
    styleBox.onChange = [this] { proc.styleFilter = styleBox.getSelectedId() - 1; rebuildFilter(); };

    styleCombo (modeBox, modeLabel, "SCALE");
    modeBox.addItem ("Major + minor", 1);
    modeBox.addItem ("Major", 2);
    modeBox.addItem ("Minor", 3);
    modeBox.onChange = [this] { proc.modeFilter = modeBox.getSelectedId() - 1; rebuildFilter(); };

    styleCombo (keyBox, keyLabel, "KEY");
    for (int i = 0; i < 12; ++i) keyBox.addItem (soca::keyName (i), i + 1);
    keyBox.onChange = [this] { proc.key = keyBox.getSelectedId() - 1; proc.notesChanged(); repaint(); };

    styleCombo (rhythmBox, rhythmLabel, "RHYTHM");
    for (int i = 0; i < soca::kNumRhythms; ++i) rhythmBox.addItem (soca::kRhythms[i].name, i + 1);
    rhythmBox.onChange = [this] { proc.rhythm = rhythmBox.getSelectedId() - 1; proc.notesChanged(); };

    styleCombo (octaveBox, octaveLabel, "OCTAVE");
    octaveBox.addItem ("Low", 1);
    octaveBox.addItem ("Middle", 2);
    octaveBox.addItem ("High", 3);
    octaveBox.onChange = [this] { proc.octave = octaveBox.getSelectedId() - 2; proc.notesChanged(); repaint(); };

    for (auto* b : { &prevButton, &nextButton, &randomButton, &playButton })
    {
        addAndMakeVisible (b);
        b->setWantsKeyboardFocus (false);
    }
    prevButton.onClick = [this] { step (-1); };
    nextButton.onClick = [this] { step (1); };
    randomButton.onClick = [this]
    {
        if (filtered.size() > 1)
        {
            int pick = proc.progression.load();
            while (pick == proc.progression.load())
                pick = filtered[(size_t) juce::Random::getSystemRandom().nextInt ((int) filtered.size())];
            select (pick);
        }
    };
    playButton.setColour (juce::TextButton::buttonColourId, kOrange);
    playButton.setColour (juce::TextButton::buttonOnColourId, kSky);
    randomButton.setColour (juce::TextButton::buttonColourId, kGreen);
    prevButton.setColour (juce::TextButton::buttonColourId, kMagenta);
    nextButton.setColour (juce::TextButton::buttonColourId, kMagenta);
    keyBox.setColour (juce::ComboBox::arrowColourId, kSky);
    modeBox.setColour (juce::ComboBox::arrowColourId, kOrange);
    rhythmBox.setColour (juce::ComboBox::arrowColourId, kMagenta);
    playButton.setClickingTogglesState (true);
    playButton.onClick = [this]
    {
        proc.playing = playButton.getToggleState();
        playButton.setButtonText (playButton.getToggleState() ? "Stop" : "Play");
    };

    addAndMakeVisible (bassToggle);
    bassToggle.onClick = [this] { proc.bass = bassToggle.getToggleState(); proc.notesChanged(); };
    addAndMakeVisible (soundToggle);
    soundToggle.onClick = [this] { proc.soundOn = soundToggle.getToggleState(); };

    addAndMakeVisible (volumeSlider);
    volumeSlider.setSliderStyle (juce::Slider::LinearHorizontal);
    volumeSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    volumeSlider.setRange (0.0, 1.0);
    volumeSlider.onValueChange = [this] { proc.volume = (float) volumeSlider.getValue(); };
    addAndMakeVisible (volumeLabel);
    volumeLabel.setText ("VOLUME", juce::dontSendNotification);
    volumeLabel.setFont (font (11.0f, true));
    volumeLabel.setColour (juce::Label::textColourId, kDim);

    addAndMakeVisible (dragTile);

    setWantsKeyboardFocus (true);
    setSize (780, 600);
    syncFromProcessor();
    startTimerHz (30);
}

SocaChordsEditor::~SocaChordsEditor() { stopTimer(); setLookAndFeel (nullptr); }

void SocaChordsEditor::styleCombo (juce::ComboBox& box, juce::Label& label, const juce::String& title)
{
    addAndMakeVisible (box);
    box.setWantsKeyboardFocus (false);
    addAndMakeVisible (label);
    label.setText (title, juce::dontSendNotification);
    label.setFont (font (11.0f, true));
    label.setColour (juce::Label::textColourId, kDim);
}

void SocaChordsEditor::syncFromProcessor()
{
    seenStateVersion = proc.stateVersion.load();
    styleBox.setSelectedId (proc.styleFilter.load() + 1, juce::dontSendNotification);
    modeBox.setSelectedId (proc.modeFilter.load() + 1, juce::dontSendNotification);
    keyBox.setSelectedId (proc.key.load() + 1, juce::dontSendNotification);
    rhythmBox.setSelectedId (proc.rhythm.load() + 1, juce::dontSendNotification);
    octaveBox.setSelectedId (proc.octave.load() + 2, juce::dontSendNotification);
    bassToggle.setToggleState (proc.bass.load(), juce::dontSendNotification);
    soundToggle.setToggleState (proc.soundOn.load(), juce::dontSendNotification);
    volumeSlider.setValue (proc.volume.load(), juce::dontSendNotification);
    playButton.setToggleState (proc.playing.load(), juce::dontSendNotification);
    playButton.setButtonText (proc.playing.load() ? "Stop" : "Play");

    // Keep the saved progression even if the saved filters would hide it.
    const int current = proc.progression.load();
    rebuildFilter();
    if (std::find (filtered.begin(), filtered.end(), current) == filtered.end())
    {
        proc.styleFilter = 0;
        proc.modeFilter = 0;
        styleBox.setSelectedId (1, juce::dontSendNotification);
        modeBox.setSelectedId (1, juce::dontSendNotification);
        proc.progression = current;
        rebuildFilter();
    }
    select (current);
}

void SocaChordsEditor::rebuildFilter()
{
    filtered.clear();
    const int style = proc.styleFilter.load();
    const int mode = proc.modeFilter.load();
    const auto& lib = soca::library();
    for (int i = 0; i < soca::kLibrarySize; ++i)
    {
        const auto& def = soca::kLibrary[i];
        if (! lib[(size_t) i].valid) continue;
        if (style > 0 && (def.styles & soca::kStyles[style - 1].flag) == 0) continue;
        if (mode == 1 && def.mode != soca::Maj) continue;
        if (mode == 2 && def.mode != soca::Min) continue;
        filtered.push_back (i);
    }
    if (! filtered.empty()
        && std::find (filtered.begin(), filtered.end(), proc.progression.load()) == filtered.end())
        select (filtered.front());
    repaint();
}

void SocaChordsEditor::select (int libraryIndex)
{
    proc.progression = juce::jlimit (0, soca::kLibrarySize - 1, libraryIndex);
    proc.notesChanged();
    repaint();
}

void SocaChordsEditor::step (int delta)
{
    if (filtered.empty()) return;
    auto it = std::find (filtered.begin(), filtered.end(), proc.progression.load());
    int pos = it == filtered.end() ? 0 : (int) std::distance (filtered.begin(), it);
    const int n = (int) filtered.size();
    select (filtered[(size_t) (((pos + delta) % n + n) % n)]);
}

bool SocaChordsEditor::keyPressed (const juce::KeyPress& k)
{
    if (k == juce::KeyPress::rightKey || k == juce::KeyPress::downKey) { step (1);  return true; }
    if (k == juce::KeyPress::leftKey  || k == juce::KeyPress::upKey)   { step (-1); return true; }
    return false;
}

juce::File SocaChordsEditor::makeMidiFile() const
{
    return soca::writeMidiFile (proc.progression.load(), proc.key.load(), proc.rhythm.load(),
                                proc.octave.load(), proc.bass.load(), proc.currentBpm.load());
}

void SocaChordsEditor::timerCallback()
{
    if (proc.stateVersion.load() != seenStateVersion)
        syncFromProcessor();

    const float beat = proc.playheadBeat.load();
    if (std::abs (beat - lastBeat) > 0.001f)
    {
        lastBeat = beat;
        repaint (displayArea);
        repaint (0, 0, getWidth(), kHeaderHeight);
    }
}

// ---------------------------------------------------------------------- layout

void SocaChordsEditor::resized()
{
    auto area = getLocalBounds();
    area.removeFromTop (kHeaderHeight + 12);
    area = area.reduced (18, 0);
    area.removeFromBottom (18);

    auto filters = area.removeFromTop (52);
    auto place = [] (juce::Rectangle<int> cell, juce::Label& label, juce::Component& control)
    {
        label.setBounds (cell.removeFromTop (16));
        control.setBounds (cell.reduced (0, 2));
    };
    const int gap = 10;
    const int w = (filters.getWidth() - gap * 3) / 4;
    place (filters.removeFromLeft (w), styleLabel, styleBox);   filters.removeFromLeft (gap);
    place (filters.removeFromLeft (w), modeLabel, modeBox);     filters.removeFromLeft (gap);
    place (filters.removeFromLeft (w), keyLabel, keyBox);       filters.removeFromLeft (gap);
    place (filters, rhythmLabel, rhythmBox);

    area.removeFromTop (12);
    auto bottom = area.removeFromBottom (112);
    area.removeFromBottom (14);

    // the wooden "piano" display, with prev / next either side
    auto main = area;
    prevButton.setBounds (main.removeFromLeft (44).withSizeKeepingCentre (44, 90));
    nextButton.setBounds (main.removeFromRight (44).withSizeKeepingCentre (44, 90));
    displayArea = main.reduced (8, 0);

    // bottom: controls on the left, drag tile on the right
    dragTile.setBounds (bottom.removeFromRight (250));
    bottom.removeFromRight (14);
    auto row1 = bottom.removeFromTop (50);
    playButton.setBounds (row1.removeFromLeft (110).reduced (0, 4));
    row1.removeFromLeft (10);
    randomButton.setBounds (row1.removeFromLeft (96).reduced (0, 4));
    row1.removeFromLeft (14);
    place (row1.removeFromLeft (110), octaveLabel, octaveBox);
    row1.removeFromLeft (14);
    bassToggle.setBounds (row1.withTrimmedTop (14));

    bottom.removeFromTop (8);
    auto row2 = bottom;
    soundToggle.setBounds (row2.removeFromLeft (150).withSizeKeepingCentre (150, 26));
    row2.removeFromLeft (10);
    volumeLabel.setBounds (row2.removeFromLeft (62).withSizeKeepingCentre (62, 20));
    volumeSlider.setBounds (row2.withSizeKeepingCentre (row2.getWidth(), 26));

    renderBackdrop();
}

/** Everything that never changes is painted once into an image. */
void SocaChordsEditor::renderBackdrop()
{
    const int W = getWidth(), H = getHeight();
    if (W <= 0 || H <= 0) return;
    backdrop = juce::Image (juce::Image::ARGB, W, H, true);
    juce::Graphics g (backdrop);
    juce::Random r (2016);

    // the road
    g.setGradientFill ({ kAsphalt2, 0.0f, (float) kHeaderHeight, kAsphalt, 0.0f, (float) H, false });
    g.fillAll();

    // J'ouvert powder, heavier towards the edges so the controls stay readable
    for (int i = 0; i < 44; ++i)
    {
        juce::Point<float> c (r.nextFloat() * (float) W, (float) kHeaderHeight + r.nextFloat() * (float) (H - kHeaderHeight));
        const float edge = juce::jmin (c.x, (float) W - c.x, (float) H - c.y) / ((float) W * 0.5f);
        const float strength = juce::jlimit (0.5f, 1.0f, 1.15f - edge * 1.6f);
        drawPowder (g, r, c, 16.0f + r.nextFloat() * 46.0f, kCarnival[r.nextInt (kNumCarnival)], strength);
    }

    // confetti on the road
    for (int i = 0; i < 70; ++i)
    {
        juce::Graphics::ScopedSaveState state (g);
        const float x = r.nextFloat() * (float) W, y = (float) kHeaderHeight + r.nextFloat() * (float) (H - kHeaderHeight);
        g.addTransform (juce::AffineTransform::rotation (r.nextFloat() * 3.14f, x, y));
        g.setColour (kCarnival[r.nextInt (kNumCarnival)].withAlpha (0.55f));
        g.fillRect (x, y, 5.0f, 2.5f);
    }

    // the sky
    auto sky = juce::Rectangle<float> (0.0f, 0.0f, (float) W, (float) kHeaderHeight);
    g.setGradientFill ({ juce::Colour (0xff0f93b8), 0.0f, 0.0f, juce::Colour (0xff3fd2d4), 0.0f, sky.getBottom(), false });
    g.fillRect (sky);
    for (int i = 0; i < 16; ++i)   // soft clouds
    {
        const float cx = 230.0f + r.nextFloat() * 240.0f, cy = 14.0f + r.nextFloat() * 38.0f, rad = 18.0f + r.nextFloat() * 26.0f;
        g.setGradientFill ({ juce::Colours::white.withAlpha (0.32f), cx, cy, juce::Colours::white.withAlpha (0.0f), cx + rad * 1.6f, cy, true });
        g.fillEllipse (cx - rad * 1.6f, cy - rad, rad * 3.2f, rad * 2.0f);
    }

    // the feather fan, sweeping in from the top-right corner of the sky
    {
        juce::Graphics::ScopedSaveState featherClip (g);
        g.reduceClipRegion (0, 0, W, kHeaderHeight);
        const juce::Point<float> origin ((float) W - 24.0f, -26.0f);
        const float deg = juce::MathConstants<float>::pi / 180.0f;
        for (int i = 0; i < 13; ++i)      // long striped pheasant quills at the back
            drawFeather (g, origin, (96.0f + (float) i * 6.6f + r.nextFloat() * 3.0f) * deg,
                         270.0f + r.nextFloat() * 150.0f, 7.0f + r.nextFloat() * 2.5f,
                         juce::Colour (i % 2 == 0 ? 0xffe9a24a : 0xfff0c27a), true);
        const juce::Colour layers[] = { kGreen, kMagenta, kOrange, kYellow };
        const float lengths[] = { 250.0f, 205.0f, 165.0f, 120.0f };
        for (int layer = 0; layer < 4; ++layer)
            for (int i = 0; i < 12; ++i)
                drawFeather (g, origin, (92.0f + (float) i * 7.6f + (float) layer * 3.0f + r.nextFloat() * 4.0f) * deg,
                             lengths[layer] * (0.85f + r.nextFloat() * 0.3f), 15.0f + r.nextFloat() * 7.0f,
                             layers[layer].withMultipliedBrightness (0.9f + r.nextFloat() * 0.2f), false);
        for (int i = 0; i < 6; ++i)       // a few purple plumes on top
            drawFeather (g, origin, (98.0f + (float) i * 15.0f + r.nextFloat() * 6.0f) * deg,
                         95.0f + r.nextFloat() * 30.0f, 9.0f, kPurple.brighter (0.2f), false);
    }

    // a ribbon and shadow under the sky
    g.setGradientFill ({ kMagenta, 0.0f, 0.0f, kYellow, (float) W, 0.0f, false });
    g.fillRect (0.0f, sky.getBottom() - 4.0f, (float) W, 4.0f);
    g.setGradientFill ({ juce::Colours::black.withAlpha (0.35f), 0.0f, sky.getBottom(), juce::Colours::transparentBlack, 0.0f, sky.getBottom() + 10.0f, false });
    g.fillRect (0.0f, sky.getBottom(), (float) W, 10.0f);

    // the piano: a wooden cabinet
    auto wood = displayArea.toFloat();
    g.setColour (juce::Colours::black.withAlpha (0.45f));
    g.fillRoundedRectangle (wood.translated (0.0f, 4.0f), 14.0f);
    {
        juce::Graphics::ScopedSaveState state (g);
        juce::Path shape;
        shape.addRoundedRectangle (wood, 14.0f);
        g.reduceClipRegion (shape);
        g.setGradientFill ({ kWood.brighter (0.08f), 0.0f, wood.getY(), kWoodDark, 0.0f, wood.getBottom(), false });
        g.fillRect (wood);
        for (int i = 0; i < 60; ++i)   // grain
        {
            juce::Path grain;
            const float y = wood.getY() + r.nextFloat() * wood.getHeight();
            const float wobble = 1.5f + r.nextFloat() * 4.0f, phase = r.nextFloat() * 6.0f;
            grain.startNewSubPath (wood.getX(), y);
            for (float x = wood.getX(); x <= wood.getRight(); x += 24.0f)
                grain.lineTo (x, y + std::sin (x * 0.012f + phase) * wobble);
            g.setColour ((r.nextBool() ? juce::Colour (0xff3d1f0a) : juce::Colour (0xffe0a060)).withAlpha (0.10f + r.nextFloat() * 0.12f));
            g.strokePath (grain, juce::PathStrokeType (0.8f + r.nextFloat() * 1.4f));
        }
    }
    g.setColour (juce::Colour (0xff3d1f0a).withAlpha (0.8f));
    g.drawRoundedRectangle (wood.reduced (1.0f), 14.0f, 2.0f);
    g.setColour (juce::Colour (0xffffd9a0).withAlpha (0.25f));
    g.drawRoundedRectangle (wood.reduced (3.5f), 11.0f, 1.0f);
}

void SocaChordsEditor::drawKeyboard (juce::Graphics& g, juce::Rectangle<float> area, const soca::Chord& chord, juce::Colour colour) const
{
    const int firstNote = 48 + 12 * proc.octave.load();     // three octaves from C
    const int numWhite = 21;
    int lit[6];
    soca::chordPitches (chord, proc.key.load(), proc.octave.load(), lit);
    auto isLit = [&] (int note)
    {
        for (int i = 0; i < chord.numIntervals; ++i) if (lit[i] == note) return true;
        return false;
    };

    g.setColour (juce::Colour (0xff1a0d05));
    g.fillRoundedRectangle (area.expanded (3.0f), 5.0f);

    static const int whiteSteps[] = { 0, 2, 4, 5, 7, 9, 11 };
    const float keyW = area.getWidth() / (float) numWhite;
    for (int i = 0; i < numWhite; ++i)
    {
        const int note = firstNote + (i / 7) * 12 + whiteSteps[i % 7];
        auto key = juce::Rectangle<float> (area.getX() + (float) i * keyW, area.getY(), keyW, area.getHeight()).reduced (0.8f, 0.0f);
        const bool on = isLit (note);
        g.setGradientFill ({ on ? colour.brighter (0.25f) : kIvory, 0.0f, key.getY(),
                             on ? colour : juce::Colour (0xffd9cfb8), 0.0f, key.getBottom(), false });
        g.fillRoundedRectangle (key, 2.5f);
    }
    static const int blackAfter[] = { 0, 1, 3, 4, 5 };      // white-key index a black key follows
    static const int blackSteps[] = { 1, 3, 6, 8, 10 };
    for (int oct = 0; oct < 3; ++oct)
        for (int b = 0; b < 5; ++b)
        {
            const int note = firstNote + oct * 12 + blackSteps[b];
            const float x = area.getX() + ((float) (oct * 7 + blackAfter[b]) + 1.0f) * keyW;
            auto key = juce::Rectangle<float> (x - keyW * 0.3f, area.getY(), keyW * 0.6f, area.getHeight() * 0.62f);
            const bool on = isLit (note);
            g.setColour (on ? colour.darker (0.1f) : juce::Colour (0xff18141a));
            g.fillRoundedRectangle (key, 2.0f);
            if (on)
            {
                g.setColour (kIvory);
                g.drawRoundedRectangle (key, 2.0f, 1.2f);
            }
        }
}

void SocaChordsEditor::paint (juce::Graphics& g)
{
    g.drawImageAt (backdrop, 0, 0);

    // title, sitting in the sky
    auto title = juce::Rectangle<int> (22, 14, 420, 48);
    g.setFont (font (40.0f, true));
    g.setColour (juce::Colour (0xff083a4d).withAlpha (0.55f));
    g.drawText ("SOCA CHORDS", title.translated (2, 3), juce::Justification::centredLeft);
    const int socaWidth = juce::GlyphArrangement::getStringWidthInt (g.getCurrentFont(), "SOCA ");
    g.setColour (juce::Colours::white);
    g.drawText ("SOCA", title, juce::Justification::centredLeft);
    g.setColour (kYellow);
    g.drawText ("CHORDS", title.withTrimmedLeft (socaWidth), juce::Justification::centredLeft);
    g.setColour (juce::Colour (0xff063544));
    g.setFont (font (13.5f, true));
    g.drawText ("MODERN SOCA PROGRESSIONS  |  " + juce::String (juce::roundToInt (proc.currentBpm.load())) + " BPM",
                juce::Rectangle<int> (24, 61, 420, 20), juce::Justification::centredLeft);

    if (filtered.empty())
    {
        g.setColour (kIvory);
        g.setFont (font (16.0f, true));
        g.drawText ("No progressions match these filters", displayArea, juce::Justification::centred);
        return;
    }

    const int index = proc.progression.load();
    const auto& def = soca::kLibrary[index];
    const auto& prog = soca::library()[(size_t) index];
    const int key = proc.key.load();

    auto inner = displayArea.reduced (18, 14);
    auto top = inner.removeFromTop (30);

    auto it = std::find (filtered.begin(), filtered.end(), index);
    const int pos = it == filtered.end() ? 0 : (int) std::distance (filtered.begin(), it);
    g.setColour (kIvory.withAlpha (0.85f));
    g.setFont (font (13.5f, true));
    g.drawText (juce::String (pos + 1) + " / " + juce::String ((int) filtered.size()), top, juce::Justification::centredRight);

    g.setColour (juce::Colour (0xff2b1408).withAlpha (0.6f));
    g.setFont (font (24.0f, true));
    g.drawText (def.name, top.translated (1, 2), juce::Justification::centredLeft);
    g.setColour (kIvory);
    g.drawText (def.name, top, juce::Justification::centredLeft);

    // subtitle: key + style tags
    juce::StringArray tags;
    for (int i = 0; i < soca::kNumStyles; ++i)
        if (def.styles & soca::kStyles[i].flag) tags.add (soca::kStyles[i].name);
    g.setColour (kYellow);
    g.setFont (font (13.5f, true));
    g.drawText (soca::noteName (key, soca::keyUsesSharps (key, def.mode))
                    + (def.mode == soca::Min ? " minor" : " major") + "   |   " + tags.joinIntoString (", "),
                inner.removeFromTop (20), juce::Justification::centredLeft);
    inner.removeFromTop (10);

    auto keyboard = inner.removeFromBottom (54).toFloat().reduced (4.0f, 0.0f);
    inner.removeFromBottom (12);

    // chord tiles, width in proportion to length, each its own carnival colour
    const float beat = proc.playheadBeat.load();
    const bool isPlaying = beat >= 0.0f;
    const float total = juce::jmax (1.0f, prog.totalBeats);
    auto blocks = inner.toFloat();
    float x = blocks.getX(), t = 0.0f;
    int shown = 0;
    juce::Colour shownColour = kCarnival[0];

    for (size_t i = 0; i < prog.chords.size(); ++i)
    {
        const auto& c = prog.chords[i];
        const float w = blocks.getWidth() * c.beats / total;
        auto r = juce::Rectangle<float> (x, blocks.getY(), w, blocks.getHeight()).reduced (3.0f, 0.0f);
        const bool active = isPlaying && beat >= t && beat < t + c.beats;
        const auto base = kCarnival[(i + (size_t) index) % (size_t) kNumCarnival];
        const auto colour = (isPlaying && ! active) ? base.withMultipliedSaturation (0.75f).darker (0.55f) : base;
        if (active) { shown = (int) i; shownColour = base; }
        if (! isPlaying && i == 0) shownColour = base;

        g.setColour (juce::Colours::black.withAlpha (0.35f));
        g.fillRoundedRectangle (r.translated (0.0f, 3.0f), 10.0f);
        g.setGradientFill ({ colour.brighter (0.18f), 0.0f, r.getY(), colour.darker (0.15f), 0.0f, r.getBottom(), false });
        g.fillRoundedRectangle (r, 10.0f);
        if (active)
        {
            g.setColour (juce::Colours::white);
            g.drawRoundedRectangle (r.reduced (1.5f), 9.0f, 3.0f);
        }

        const float nameSize = juce::jlimit (13.0f, 32.0f, r.getWidth() / 3.2f);
        g.setColour (textOn (colour));
        g.setFont (font (nameSize, true));
        g.drawText (soca::chordName (c, key, def.mode), r.withTrimmedBottom (r.getHeight() * 0.25f),
                    juce::Justification::centred, false);
        g.setColour (textOn (colour).withAlpha (0.72f));
        g.setFont (font (juce::jmin (15.0f, nameSize * 0.7f), true));
        g.drawText (juce::String (c.roman), r.withTrimmedTop (r.getHeight() * 0.58f),
                    juce::Justification::centred, false);
        x += w;
        t += c.beats;
    }

    drawKeyboard (g, keyboard, prog.chords[(size_t) shown], shownColour);
}

// ---------------------------------------------------------------------- drag tile
void SocaChordsEditor::DragTile::paint (juce::Graphics& g)
{
    auto r = getLocalBounds().toFloat().reduced (2.0f);
    g.setColour (juce::Colours::black.withAlpha (0.4f));
    g.fillRoundedRectangle (r.translated (0.0f, 3.0f), 14.0f);
    g.setGradientFill ({ hover ? kYellow : kYellow.darker (0.05f), r.getX(), r.getY(),
                         hover ? kOrange.brighter (0.1f) : kOrange, r.getRight(), r.getBottom(), false });
    g.fillRoundedRectangle (r, 14.0f);

    juce::Path outline;
    outline.addRoundedRectangle (r.reduced (6.0f), 9.0f);
    const float dashes[] = { 7.0f, 5.0f };
    juce::Path dashed;
    juce::PathStrokeType (2.0f).createDashedStroke (dashed, outline, dashes, 2);
    g.setColour (kAsphalt.withAlpha (0.75f));
    g.fillPath (dashed);

    g.setColour (kAsphalt);
    g.setFont (font (23.0f, true));
    g.drawText ("DRAG MIDI", getLocalBounds().withTrimmedBottom (26), juce::Justification::centred);
    g.setColour (kAsphalt.withAlpha (0.8f));
    g.setFont (font (12.5f, true));
    g.drawText ("drag from here onto a track", getLocalBounds().withTrimmedTop (46), juce::Justification::centred);
}

void SocaChordsEditor::DragTile::mouseDrag (const juce::MouseEvent& e)
{
    if (dragging || e.getDistanceFromDragStart() < 6) return;
    dragging = true;
    const auto file = owner.makeMidiFile();
    if (file.existsAsFile())
        juce::DragAndDropContainer::performExternalDragDropOfFiles ({ file.getFullPathName() }, false, this);
}
