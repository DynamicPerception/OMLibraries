

#include "MenuEngine.h"

/**
 * Test class for MenuEngine
 * */
class MenuEngineMock: public MenuEngine
{
public:
	MenuEngineMock(LiquidCrystal& lcd, Commander& cmd): MenuEngine(lcd, cmd){};


	void InitialiseDisplay(void) {MenuEngine::InitialiseDisplay();}

	unsigned char mSecTimeup (void *data1,void *data2);
	void mSecInitialise(void *data1, void *data2, unsigned long timerValue);

	void ProcessUI(void) {MenuEngine::ProcessUI();}

	void PopStack (void) {MenuEngine::PopStack();}
	void FlattenStack (void) {MenuEngine::FlattenStack();}
	void PushStack (void){MenuEngine::PushStack();}

	void OverallProcess(void){MenuEngine::OverallProcess();}
	uint8_t Keyboard(void){return MenuEngine::Keyboard();}
	void DisplayInterfaceManager(void){MenuEngine::DisplayInterfaceManager();}
	void TimeoutNoKeyEntry (void){MenuEngine::TimeoutNoKeyEntry();}

	void DisplaySelectedItem(void){MenuEngine::DisplaySelectedItem();}

	void UpdateDisplay(void){HorizMenu::UpdateView();}
	int WriteLine (const uint8_t *bufferStart, uint8_t startLine, uint8_t lineOffset)
	  {return HorizMenu::WriteLine (bufferStart, startLine, lineOffset);}
	void TransferMsg(unsigned char MessageNumber, void *Destination)
	{Resources::readMsgToBuf(MessageNumber, Destination);}

	void CreateDisplay(uint8_t disp) {HorizMenu::CreateMenu(disp);}
	void TransferDisplay() {HorizMenu::TransferMenu();}
	void OpenParam(uint8_t idx) {status.openParameter(idx);}
	void CloseParam() {status.closeParameter(true);}
	void IncParam() {HorizMenu::IncreaseParameter();}
	void DecParam() {HorizMenu::DecreaseParameter();}

	//getters
	//uint8_t getEnginePhase() const {return iEnginePhase;};
	//uint8_t getInterPhase() const {return iInterPhase;};
	int getKeyboardReads()  {return keyboard.GetSample();};
    //StackType& getStack() {return ProcessStack;};
    MenuContext& getContext() {return status;}

};
