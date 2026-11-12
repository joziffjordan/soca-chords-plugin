#pragma once
#include "PluginProcessor.h"

/** Rounded, colourful buttons and menus. */
struct CarnivalLookAndFeel : juce::LookAndFeel_V4
{
    CarnivalLookAndFeel();
    void drawButtonBackground (juce::Graphics&, juce::Button&, const juce::Colour&, bool, bool) override;
    juce::Font getTextButtonFont (juce::TextButton&, int buttonHeight) override;
    void drawComboBox (juce::Graphics&, int width, int height, bool isButtonDown,
                       int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox&) override;
};

class SocaChordsEditor : public juce::AudioProcessorEditor,
                         public juce::DragAndDropContainer,
                         private juce::Timer
{
public:
    explicit SocaChordsEditor (SocaChordsProcessor&);
    ~SocaChordsEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    bool keyPressed (const juce::KeyPress&) override;

private:
    /** The tile you drag from. Writes a .mid file and hands it to the DAW. */
    struct DragTile : juce::Component
    {
        explicit DragTile (SocaChordsEditor& e) : owner (e) { setMouseCursor (juce::MouseCursor::DraggingHandCursor); }
        void paint (juce::Graphics&) override;
        void mouseDown (const juce::MouseEvent&) override { dragging = false; }
        void mouseDrag (const juce::MouseEvent&) override;
        void mouseEnter (const juce::MouseEvent&) override { hover = true; repaint(); }
        void mouseExit (const juce::MouseEvent&) override { hover = false; repaint(); }
        SocaChordsEditor& owner;
        bool dragging = false, hover = false;
    };

    void timerCallback() override;
    void syncFromProcessor();
    void rebuildFilter();
    void step (int delta);
    void select (int libraryIndex);
    juce::File makeMidiFile() const;
    void styleCombo (juce::ComboBox&, juce::Label&, const juce::String& title);
    void renderBackdrop();
    void drawKeyboard (juce::Graphics&, juce::Rectangle<float>, const soca::Chord&, juce::Colour) const;

    SocaChordsProcessor& proc;
    CarnivalLookAndFeel lookAndFeel;
    std::vector<int> filtered;          // library indices that pass the filters
    int seenStateVersion = -1;
    float lastBeat = -2.0f;
    juce::Rectangle<int> displayArea;
    juce::Image backdrop;               // sky, feathers, powder and wood, painted once

    juce::ComboBox styleBox, modeBox, keyBox, rhythmBox, octaveBox;
    juce::Label styleLabel, modeLabel, keyLabel, rhythmLabel, octaveLabel, volumeLabel;
    juce::TextButton prevButton { "<" }, nextButton { ">" }, randomButton { "Random" }, playButton { "Play" };
    juce::ToggleButton bassToggle { "Bass note" }, soundToggle { "Preview sound" };
    juce::Slider volumeSlider;
    DragTile dragTile { *this };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SocaChordsEditor)
};
