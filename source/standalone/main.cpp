/* Copyright 2013-2019 Matt Tytel
 *
 * vital is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * vital is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with vital.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "border_bounds_constrainer.h"
#include "FullInterface.h"

#include "startup.h"
#include "synth_editor.h"

#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_core/juce_core.h>

void handleBitklavierCrash(void* data) {
  //LoadSave::writeCrashLog(SystemStats::getStackBacktrace());
}
namespace ProjectInfo
{
  const char* const  projectName    = "bitKlavier";
  const char* const  companyName    = "Many Arrows Music";
  const char* const  versionString  = "3.4a";
  const int          versionNumber  = 0x30400;
}
String getArgumentValue(const StringArray& args, const String& flag, const String& full_flag) {
  int index = 0;
  for (String arg : args) {
    index++;
    if (arg == flag || arg == full_flag)
      return args[index];
  }

  return "";
}

int loadAudioFile(AudioSampleBuffer& destination, InputStream* audio_stream) {
  AudioFormatManager format_manager;
  format_manager.registerBasicFormats();
  
  audio_stream->setPosition(0);
  std::unique_ptr<AudioFormatReader> format_reader(
      format_manager.createReaderFor(std::unique_ptr<InputStream>(audio_stream)));
  if (format_reader == nullptr)
    return 0;

  int num_samples = static_cast<int>(format_reader->lengthInSamples);
  destination.setSize(format_reader->numChannels, num_samples);
  format_reader->read(&destination, 0, num_samples, 0, true, true);
  return format_reader->sampleRate;
}


class SynthApplication : public JUCEApplication {
  public:
    class MainWindow : public DocumentWindow, public ApplicationCommandTarget, private AsyncUpdater {
      public:
        enum PresetCommand {
          kSave = 0x5001,
          kSaveAs,
          kOpen,
          kToggleVideo,
        };

        MainWindow(const String& name, bool visible) :
            DocumentWindow(name, Colours::lightgrey, DocumentWindow::allButtons, visible), editor_(nullptr) {
          if (!Startup::isComputerCompatible()) {
            String error = String(ProjectInfo::projectName) +
                           " requires SSE2, NEON or AVX2 compatible processor. Exiting.";
            //AlertWindow::showNativeDialogBox("Computer not supported", error, false);
            quit();
          }

          SystemStats::setApplicationCrashHandler(handleBitklavierCrash);
              
          if (visible) {
            setUsingNativeTitleBar(true);
            setResizable(true, true);
          }

          editor_ = new SynthEditor(visible);
          constrainer_.setGui(editor_->getGui());
          if (visible) {
            editor_->animate(true);
            setContentOwned(editor_, true);

            constrainer_.setMinimumSize(bitklavier::kMinWindowWidth, bitklavier::kMinWindowHeight);
            constrainer_.setBorder(getPeer()->getFrameSize());
            float ratio = (1.0f * bitklavier::kDefaultWindowWidth) / bitklavier::kDefaultWindowHeight;

            constrainer_.setFixedAspectRatio(ratio);
            setConstrainer(&constrainer_);

            centreWithSize(getWidth(), getHeight());
            setVisible(visible);
            triggerAsyncUpdate();
          }
          else
            editor_->animate(false);
        }

        void closeButtonPressed() override {
          JUCEApplication::getInstance()->systemRequestedQuit();
        }

        void loadFile(const File& file) {
          file_to_load_ = file;
          triggerAsyncUpdate();
        }

        void shutdownAudio() {
          editor_->shutdownAudio();
        }

        ApplicationCommandTarget* getNextCommandTarget() override {
          return findFirstTargetParentComponent();
        }

        void getAllCommands(Array<CommandID>& commands) override {
          commands.add(kSave);
          commands.add(kSaveAs);
          commands.add(kOpen);
        }

        void getCommandInfo(const CommandID commandID, ApplicationCommandInfo& result) override {
          if (commandID == kSave) {
            result.setInfo(TRANS("Save"), TRANS("Save the current preset"), "Application", 0);
            result.defaultKeypresses.add(KeyPress('s', ModifierKeys::commandModifier, 0));
          }
          else if (commandID == kSaveAs) {
            result.setInfo(TRANS("Save As"), TRANS("Save preset to a new file"), "Application", 0);
            ModifierKeys modifier = ModifierKeys::commandModifier | ModifierKeys::shiftModifier;
            result.defaultKeypresses.add(KeyPress('s', modifier, 0));
          }
          else if (commandID == kOpen) {
            result.setInfo(TRANS("Open"), TRANS("Open a preset"), "Application", 0);
            result.defaultKeypresses.add(KeyPress('o', ModifierKeys::commandModifier, 0));
          }
          else if (commandID == kToggleVideo) {
            result.setInfo(TRANS("Toggle Zoom"), TRANS("Toggle zoom for recording"), "Application", 0);
            ModifierKeys modifier = ModifierKeys::commandModifier | ModifierKeys::shiftModifier;
            result.defaultKeypresses.add(KeyPress('t', modifier, 0));
          }
        }

        bool perform(const InvocationInfo& info) override {
          if (info.commandID == kSave) {
//            if (!editor_->saveToActiveFile())
//              editor_->openSaveDialog();
//            else {
//              grabKeyboardFocus();
//              editor_->setFocus();
//            }
            return true;
          }
          else if (info.commandID == kSaveAs) {
//            File active_file = editor_->getActiveFile();
//            FileChooser save_box("Export Preset", File(), String("*.") + vital::kPresetExtension);
//            if (save_box.browseForFileToSave(true))
//              editor_->saveToFile(save_box.getResult().withFileExtension(vital::kPresetExtension));
//            grabKeyboardFocus();
//            editor_->setFocus();
            return true;
          }
          else if (info.commandID == kOpen) {
            //File active_file = editor_->getActiveFile();
//            FileChooser open_box("Open Preset", active_file, String("*.") + vital::kPresetExtension);
//            if (!open_box.browseForFileToOpen())
//              return true;

//            File choice = open_box.getResult();
//            if (!choice.exists())
//              return true;
//
//            std::string error;
//            if (!editor_->loadFromFile(choice, error)) {
//              error = "There was an error open the preset. " + error;
//              AlertWindow::showNativeDialogBox("Error opening preset", error, false);
//            }
//            else
//              editor_->externalPresetLoaded(choice);
//            grabKeyboardFocus();
//            editor_->setFocus();
            return true;
          }
          else if (info.commandID == kToggleVideo)
//            editor_->getGui()->toggleFilter1Zoom();

          return false;
        }

        void handleAsyncUpdate() override {
          if (command_manager_ == nullptr) {
            command_manager_ = std::make_unique<ApplicationCommandManager>();
            command_manager_->registerAllCommandsForTarget(JUCEApplication::getInstance());
            command_manager_->registerAllCommandsForTarget(this);
            addKeyListener(command_manager_->getKeyMappings());
          }
          
          if (file_to_load_.exists()) {
            loadFileAsyncUpdate();
            file_to_load_ = File();
          }
          
          editor_->setFocus();
        }

      private:
        void loadFileAsyncUpdate() {
//          std::string error;
//          bool success = editor_->loadFromFile(file_to_load_, error);
//          if (success)
//            editor_->externalPresetLoaded(file_to_load_);
        }
      
        File file_to_load_;
        SynthEditor* editor_;
        std::unique_ptr<ApplicationCommandManager> command_manager_;
        BorderBoundsConstrainer constrainer_;
      
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
    };

    SynthApplication() = default;

    const String getApplicationName() override { return ProjectInfo::projectName; }
    const String getApplicationVersion() override { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override { return true; }

    void initialise(const String& command_line) override {
      String command = " " + command_line + " ";
      if (command.contains(" --version ") || command.contains(" -v ")) {
        std::cout << getApplicationName() << " " << getApplicationVersion() << newLine;
        quit();
      }
      else if (command.contains(" --help ") || command.contains(" -h ")) {
        std::cout << "Usage:" << newLine;
        std::cout << "  " << getApplicationName().toLowerCase() << " [OPTION...]" << newLine << newLine;
        std::cout << getApplicationName() << " polyphonic wavetable synthesizer." << newLine << newLine;
        std::cout << "Help Options:" << newLine;
        std::cout << "  -h, --help                          Show help options" << newLine << newLine;
        std::cout << "Application Options:" << newLine;
        std::cout << "  -v, --version                       Show version information and exit" << newLine;
        std::cout << "  --headless                          Run without graphical interface." << newLine;
        std::cout << "  --tabletowav                        Converts a vitaltable to wav file." << newLine;
        std::cout << "  --tableimages                       Renders an image for the table." << newLine;
        std::cout << "  --render                            Render to an audio file." << newLine;
        std::cout << "  -m, --midi                          Note to play (with --render)." << newLine;
        std::cout << "  -l, --length                        Not length to play (with --render)." << newLine;
        std::cout << "  -b, --bpm                           BPM to play (with --render)." << newLine;
        std::cout << "  --images                            Render oscilloscope images (with --render)." << newLine << newLine;
        quit();
      }
      else if (command.contains(" --tabletowav ")) {






      }
      else if (command.contains(" --tableimages ")) {

      }
      else if (command.contains(" --render ")) {

      }
      else {
        bool visible = !command.contains(" --headless ");
        main_window_ = std::make_unique<MainWindow>(getApplicationName(), visible);

        StringArray args = getCommandLineParameterArray();
        bool last_arg_was_option = false;
        for (const String& arg : args) {
          if (arg != "" && arg[0] != '-' && !last_arg_was_option && loadFromCommandLine(arg))
            break;

          last_arg_was_option = arg[0] == '-' && arg != "--headless";
        }
      }
    }

    bool loadFromCommandLine(const String& command_line) {
      String file_path = command_line;
      if (file_path[0] == '"' && file_path[file_path.length() - 1] == '"')
        file_path = command_line.substring(1, command_line.length() - 1);
      File file = File::getCurrentWorkingDirectory().getChildFile(file_path);
      if (!file.exists())
        return false;

      main_window_->loadFile(file);
      return true;
    }

    void shutdown() override {
      main_window_ = nullptr;
    }

    void systemRequestedQuit() override {
      quit();
    }

    void anotherInstanceStarted(const String& command_line) override {
      loadFromCommandLine(command_line);
    }

  private:
    std::unique_ptr<MainWindow> main_window_;
};

START_JUCE_APPLICATION(SynthApplication)
