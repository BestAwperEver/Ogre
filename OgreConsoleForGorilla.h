/*
  Description:

   This is a port of the OgreConsole code presented by PixL in the Ogre Forums then later added
   to the Ogre Wiki[1].

   This is a straight port replacing all the Overlay code with Gorilla code, some changes have
   been added but they are minor and do not add to the actual functionality of the class.

  Port Author:

   Betajaen.

  Usage:



  References:

   [1] http://www.ogre3d.org/tikiwiki/OgreConsole&structure=Cookbook

*/

#pragma once
#ifndef _Ogre_Console_FG_h
#define _Ogre_Console_FG_h

#include "Gorilla.h"
#include "GameMap.h"
#include "array2.h"
#include "Logger.h"
//class _OgreConsoleEmpty_ {};

template <class callbackClass>// = _OgreConsoleEmpty_>
class OgreConsole : public /*Ogre::Singleton<OgreConsole<callbackClass>>,*/ Ogre::FrameListener, Ogre::LogListener
{

	bool m_bLCtrlPressed;

public:

	OgreConsole (callbackClass* callbackObject, int lineLength, int lineCount = 15);

	~OgreConsole();

	void   init (Gorilla::Screen* screen_to_use);
	void   shutdown();

	void	setVisible (bool Visible);
	bool	isVisible() {return mIsVisible;}

	bool	performCommand (Ogre::StringVector& params);
	void	print (const Ogre::String &text);
	OgreConsole& operator << (const Ogre::String &);
	OgreConsole& operator << (int);
	OgreConsole& operator << (Ogre::Real);
	OgreConsole& operator << (char);

	virtual bool frameStarted (const Ogre::FrameEvent &evt);
	virtual bool frameEnded (const Ogre::FrameEvent &evt);

	void onKeyReleased (const OIS::KeyEvent &arg);
	void onKeyPressed (const OIS::KeyEvent &arg);
	bool mouseMoved (const OIS::MouseEvent &arg);

	void deleteSymbol();
	void stopDeleting();

	int getLineCount() const {return m_nLineCount;};
	void setLineCount(unsigned int);
	void forceUpdateConsole();

	typedef void (*OgreConsoleFunctionPtr) (Ogre::StringVector&);
	//
	typedef void (callbackClass::*OgreConsoleMemberFunctionPtr) (Ogre::StringVector&);

	//void addCommand(const Ogre::String &command, OgreConsoleFunctionPtr);
	//
	void	addCommand (Ogre::String command, OgreConsoleMemberFunctionPtr); //, callbackClass* callbackObject);
	void	removeCommand (Ogre::String command);
	void	showCommands();
	bool	isSuchCommand(const Ogre::String&) const;
	void	addAlias (Ogre::String alias, Ogre::String for_what);
	void	removeAlias (Ogre::String alias);
	void	showAliases();
	bool	isSuchAlias(const Ogre::String&) const;

	//log
#if OGRE_VERSION_MINOR < 8 && OGRE_VERSION_MAJOR < 2
	void messageLogged ( const Ogre::String& message, Ogre::LogMessageLevel lml, bool maskDebug, const Ogre::String &logName );
#else
	// "bool& skip" added in Ogre 1.8
	void messageLogged ( const Ogre::String& message, Ogre::LogMessageLevel lml, bool maskDebug, const Ogre::String &logName, bool &skip );
#endif
private:

	void  updateConsole();
	void  updatePrompt();

	int					m_nLineLength;
	int					m_nLineCount;
	bool				mIsVisible;
	bool				mIsInitialised;
	bool				m_bDeleteSymbols;

	Ogre::Real			m_fDeleteTime;
	Ogre::Real			m_fDeleteCooldown;

	Ogre::String		line;

	Gorilla::Screen*		mScreen;
	Gorilla::Layer*			mLayer;
	Gorilla::MarkupText* /*Caption**/		mPromptText; // интересно, это я исправил?
	Gorilla::MarkupText*	mConsoleText;
	Gorilla::Rectangle*		mDecoration;
	Gorilla::GlyphData*		mGlyphData;

	bool                 mUpdateConsole;
	bool                 mUpdatePrompt;

	int					mStartline;
	std::list<Ogre::String>		lines;
	std::list<Ogre::String>		executed_commands;
	Ogre::String				prompt;

	std::list<Ogre::String>::reverse_iterator	last_command;
	std::list<Ogre::String>::iterator			first_command;

	//std::map<Ogre::String, OgreConsoleFunctionPtr>  commands;
	//
	callbackClass*											m_pCallbackObject;
	std::map<Ogre::String, OgreConsoleMemberFunctionPtr>	commands;
	std::map<Ogre::String, Ogre::String>					aliases;

	boost::mutex m_Mutex;

public:

	int getLineLength() const {
		return m_nLineLength;
	}
	void setLineLength (int Length) {
		m_nLineLength = Length;
	}
};

/*
  Description:

   This is a port of the OgreConsole code presented by PixL in the Ogre Forums then later added
   to the Ogre Wiki.

   This is a straight port replacing all the Overlay code with Gorilla code, some changes have
   been added but they are minor and do not add to the actual functionality of the class.


*/

// не смог сделать синглетоном, хз почему ошибку выдает
//#if OGRE_VERSION < 67584 // 1.8.0
//template<> OgreConsole* Ogre::Singleton<OgreConsole>::ms_Singleton=0;
//#else
//template <class T> OgreConsole<T>* Ogre::Singleton<OgreConsole<T>>::msSingleton=0;
//#endif

#define CONSOLE_FONT_INDEX 9
#define MAX_LINE_COUNT 35
#define CONSOLE_ADDITIONAL_HEIGHT 0//(-4)
#define CONSOLE_WRITING_SYMBOL ""
#define CONSOLE_LINE_LENGTH 85
#define CONSOLE_LINE_COUNT 15
#define CONSOLE_SYMBOL_DELETING_TIME 60.0f
#define CONSOLE_SYMBOL_DELETING_COOLDOWN 0.5f
static const unsigned char legalchars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890+!\"'#%&/()=?[]\\*-_.:,; ";

template<class callbackClass> OgreConsole<callbackClass>::OgreConsole (callbackClass* _callbackObject, int LineLength, int lineCount)
	: mIsVisible (true), mIsInitialised (false), mScreen (0), mUpdateConsole (false), mUpdatePrompt (false), mStartline (0),
	  m_bDeleteSymbols (0), m_fDeleteTime (0), m_fDeleteCooldown (0), m_nLineLength (LineLength), m_pCallbackObject (_callbackObject),
	  m_nLineCount(lineCount)
{
}

template<class callbackClass> OgreConsole<callbackClass>::~OgreConsole()
{
	if (mIsInitialised)
		shutdown();
}

template<class callbackClass> void OgreConsole<callbackClass>::init (Gorilla::Screen* screen)
{

	if (mIsInitialised)
		shutdown();

	m_bLCtrlPressed = false;

	Ogre::Root::getSingletonPtr()->addFrameListener (this);
	Ogre::LogManager::getSingleton().getDefaultLog()->addListener (this);

	// Create gorilla things here.
	mScreen = screen;
	mLayer = mScreen->createLayer (15);

	mGlyphData = mLayer->_getGlyphData (CONSOLE_FONT_INDEX); // Font.CONSOLE_FONT_INDEX

	mConsoleText = mLayer->createMarkupText (CONSOLE_FONT_INDEX,  10, 10, Ogre::StringUtil::BLANK);
	mConsoleText->width (mScreen->getWidth() - 10);

	std::stringstream nulltext;

	nulltext << "> " << CONSOLE_WRITING_SYMBOL;

	mPromptText =  mLayer->createMarkupText/*mLayer->createCaption*/ (CONSOLE_FONT_INDEX,  10, 10, nulltext.str());

	mDecoration = mLayer->createRectangle (8, 8, mScreen->getWidth() - 16, mGlyphData->mLineHeight );
	mDecoration->background_gradient (Gorilla::Gradient_NorthSouth, Gorilla::rgb (128, 128, 128, 128), Gorilla::rgb (64, 64, 64, 128) );
	mDecoration->border (2, Gorilla::rgb (128, 128, 128, 128) );


	mIsInitialised = true;

	//print("%5Ogre%R%6Console%0 Activated. Press F1 to show/hide.%R");
}

template<class callbackClass> void OgreConsole<callbackClass>::shutdown()
{
	if (!mIsInitialised)
		return;

	mIsInitialised = false;

	Ogre::Root::getSingletonPtr()->removeFrameListener (this);
	Ogre::LogManager::getSingleton().getDefaultLog()->removeListener (this);

	mScreen->destroy (mLayer);

}

//namespace Ogre {
//	namespace StringUtil {
//		void deleteNullSymbols(Ogre::String& str) {
//
//		}
//	}
//}

template<class callbackClass> bool OgreConsole<callbackClass>::performCommand (Ogre::StringVector& params) {
	std::map<Ogre::String, OgreConsoleMemberFunctionPtr>::iterator i;

	for ( i = commands.begin(); i != commands.end(); i++) {
		Ogre::String command (params[0]);

//		/*Ogre::StringUtil::*/deleteNullSymbols(command);
		// удаляем лишние нули в начале (хз откуда они там берутся)
		int k;
		for (k = 0; !command[k]; ++k) {}
		if (k) command = command.substr(k, command.length() - k);

		//if (command.length() > 1) {
		//	while (command[0] == 0) {
		//		command = command.substr (1, command.length() - 1);
		//	}
		//}

		Ogre::StringUtil::toLowerCase (command);

		if ( (*i).first == command ) {
			if ( (*i).second )
				(m_pCallbackObject->* ( (*i).second) ) (params);

			return true;
		}
	}

	return false;
}

template<class callbackClass> void OgreConsole<callbackClass>::onKeyPressed (const OIS::KeyEvent &arg)
{

	if (!mIsVisible)
		return;

	if (arg.key == OIS::KC_RETURN || arg.key == OIS::KC_NUMPADENTER)
	{

		if (prompt.empty()) {
			return;
		}

		_LOG_COMMAND("> " + prompt);
		print ("%3> " + prompt + "%R");

		if (isSuchAlias(prompt)) {
			prompt = aliases[prompt];
		}

		//split the parameter list
		Ogre::StringVector params = Ogre::StringUtil::split (prompt, " ");

		if (params.size() ) {
			//std::map<Ogre::String, OgreConsoleMemberFunctionPtr>::iterator i;
			//for( i = commands.begin(); i != commands.end(); i++) {
			//	if ( (*i).first == params[0] ) {
			//		if ( (*i).second )
			//			(m_pCallbackObject->*((*i).second))(params);
			//		break;
			//	}
			//}

			if (!performCommand(params)) {
				_LOG("Unrecognized command: " + params[0]);
			}

			auto it = std::find(executed_commands.begin(), executed_commands.end(), prompt);
			if (it != executed_commands.end()) {
				executed_commands.erase(it);
			}
			executed_commands.push_back(prompt);

			last_command = executed_commands.rbegin();
			first_command = executed_commands.begin();

			prompt.clear();
			mUpdateConsole = true;
			mUpdatePrompt = true;
		}
	}

	else if (arg.key == OIS::KC_BACK)
	{
		if (prompt.size() ) {
			prompt.erase (prompt.end() - 1); //=prompt.substr(0,prompt.length()-1);
			mUpdatePrompt = true;
		}

		//mTimer.AddTimer(100, &OgreConsole::makeDeleteSymbolTimer, this, 10, false, false);
		m_bDeleteSymbols = true;
	}

	else if (arg.key == OIS::KC_UP) {
		if (executed_commands.size()) {
			if (first_command != executed_commands.begin()) {
				if ( --first_command != executed_commands.begin() ) {
					prompt = *(--first_command);
					last_command = executed_commands.rbegin();
				}
				else if (last_command != executed_commands.rend()) {
					prompt = *(last_command++);
				}
				++first_command;
			}
			else if (last_command != executed_commands.rend()) {
				prompt = *(last_command++);
				//++last_command;
			}
			mUpdatePrompt = true;
		}
	}

	else if (arg.key == OIS::KC_DOWN) {
		if (executed_commands.size()) {
			if (last_command != executed_commands.rbegin()) {
				if ( --last_command != executed_commands.rbegin() ) {
					prompt = *(--last_command);
					first_command = executed_commands.begin();
				}
				else if (first_command != executed_commands.end()) {
					prompt = *(first_command++);
				}
				++last_command;
			}
			else if (first_command != executed_commands.end()) {
				prompt = *(first_command++);
				//++first_command;
			}
			mUpdatePrompt = true;
		}
	}

	else if (arg.key == OIS::KC_PGUP)
	{
		if (m_nLineCount > 0) {
			m_nLineCount--;
			mStartline++;
		}

		mUpdateConsole = true;
	}

	else if (arg.key == OIS::KC_PGDOWN)
	{
		if (m_nLineCount < MAX_LINE_COUNT) {
			m_nLineCount++;
			mStartline--;
		}

		mUpdateConsole = true;
	}

	else if (arg.key == OIS::KC_LCONTROL)
	{
		m_bLCtrlPressed = true;
	}

	else if (arg.key == OIS::KC_V && m_bLCtrlPressed)
	{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		//CString fromClipboard;
		if ( OpenClipboard(NULL) )						//открываем буфер обмена
		{
			HANDLE hData = GetClipboardData(CF_TEXT);	//извлекаем текст из буфера обмена
			char* chBuffer = (char*)GlobalLock(hData);	//блокируем память
			//fromClipboard = chBuffer;
			if (*chBuffer) {
				while (*chBuffer) {
					for (unsigned int c = 0; c < sizeof (legalchars) - 1; c++) {
						if (legalchars[c] == *chBuffer) {
							prompt += *chBuffer;
							break;
						}
					}
					++chBuffer;
				}
				mUpdatePrompt = true;
			}
			GlobalUnlock(hData);						//разблокируем память
			CloseClipboard();							//закрываем буфер обмена
		}
#endif
	}

	else
	{
		for (unsigned int c = 0; c < sizeof (legalchars) - 1; c++) {
			if (legalchars[c] == arg.text) {
				prompt += arg.text;
				break;
			}
		}

		mUpdatePrompt = true;
	}

}

template<class callbackClass> void OgreConsole<callbackClass>::onKeyReleased (const OIS::KeyEvent &arg) {
	if (arg.key == OIS::KC_LCONTROL) {
		m_bLCtrlPressed = false;
	}
}

template<class callbackClass> bool OgreConsole<callbackClass>::mouseMoved( const OIS::MouseEvent &arg )
{
	
	if (arg.state.Z.rel) {
		int n = arg.state.Z.rel / 120.0f;
		mStartline -= n;

		if (mStartline < 0)
			mStartline = 0;

		if (mStartline > lines.size() - m_nLineCount )
			mStartline = lines.size() - m_nLineCount;


		mUpdateConsole = true;
	}

	return true;
}

template<class callbackClass> bool OgreConsole<callbackClass>::frameStarted (const Ogre::FrameEvent &evt)
{
	if (mUpdateConsole)
		updateConsole();

	if (mUpdatePrompt)
		updatePrompt();

	if (m_bDeleteSymbols) {
		if (m_fDeleteCooldown > CONSOLE_SYMBOL_DELETING_COOLDOWN) {
			if (m_fDeleteTime > CONSOLE_SYMBOL_DELETING_TIME / 1000) {
				deleteSymbol();
				m_fDeleteTime = 0;
			}
			else {
				m_fDeleteTime += evt.timeSinceLastFrame;
			}
		}
		else {
			m_fDeleteCooldown += evt.timeSinceLastFrame;
		}
	}

	return true;
}

template<class callbackClass> void OgreConsole<callbackClass>::deleteSymbol() {
	if (prompt.size() ) {
		prompt.erase (prompt.end() - 1); //=prompt.substr(0,prompt.length()-1);
		mUpdatePrompt = true;
	}
}

template<class callbackClass> void OgreConsole<callbackClass>::stopDeleting() {
	m_fDeleteTime = 0;
	m_fDeleteCooldown = 0;
	m_bDeleteSymbols = false;
}

template<class callbackClass> void OgreConsole<callbackClass>::updateConsole()
{

	boost::mutex::scoped_lock lk(m_Mutex);

	mUpdateConsole = false;

	std::stringstream text;
	std::list<Ogre::String>::iterator i, start, end;

//make sure is in range
	if (mStartline > lines.size() )
		mStartline = lines.size();

	int lcount = 0;
	start = lines.begin();

	for (unsigned int c = 0; c < mStartline; ++c, ++start);

	end = start;

	for (unsigned int c = 0; c < m_nLineCount; ++c) {
		if (end == lines.end() )
			break;

		++end;
	}

	for (i = start; i != end; ++i)
	{
		++lcount;
		text << (*i) << "\n";
	}

	mConsoleText->text (text.str() );

// Move prompt downwards.
	mPromptText->top (10 + (lcount * mGlyphData->mLineHeight) );

// Change background height so it covers the text and prompt
	mDecoration->height ( ( (lcount + 1) * mGlyphData->mLineHeight) + CONSOLE_ADDITIONAL_HEIGHT);

	mConsoleText->width (mScreen->getWidth() - 20);
	mDecoration->width (mScreen->getWidth() - 16);
	mPromptText->width (mScreen->getWidth() - 20);

}

template<class callbackClass> void OgreConsole<callbackClass>::forceUpdateConsole() {
	mUpdateConsole = true;
}

template<class callbackClass> void OgreConsole<callbackClass>::updatePrompt()
{
	mUpdatePrompt = false;
	std::stringstream text;
	text << "> " << prompt << CONSOLE_WRITING_SYMBOL;
	mPromptText->text (text.str() );
}

template<class callbackClass> void OgreConsole<callbackClass>::print (const Ogre::String &text)
{
	//subdivide it into lines
	const char *str = text.c_str();
	int len = text.length();
	Ogre::String line;

	for (int c = 0; c < len; c++) {
		if (str[c] == '\n' || line.length() >= m_nLineLength) {
			lines.push_back (line);
			line = "";
		}

		if (str[c] != '\n')
			line += str[c];
	}

	{ boost::mutex::scoped_lock lk(m_Mutex);
	if (line.length() )
		lines.push_back (line);

	if (lines.size() > m_nLineCount)
		mStartline = lines.size() - m_nLineCount;
	else
		mStartline = 0;

	mUpdateConsole = true;
	}
}

template <class T> void OgreConsole<T>::setLineCount(unsigned int _Count) {
	if (_Count > 35) _Count = 35;

	mStartline -= _Count - m_nLineCount;

	m_nLineCount = _Count;

	if (mStartline < 0)
		mStartline = 0;

	if (mStartline > lines.size() - m_nLineCount )
		mStartline = lines.size() - m_nLineCount;

	mUpdateConsole = true;
}

template<class callbackClass> OgreConsole<callbackClass>& OgreConsole<callbackClass>::operator << (const Ogre::String &text) {

	const char *str = text.c_str();
	int len = text.length();

	for (int c = 0; c < len; c++) {
		if (str[c] == '\n' || line.length() >= this->m_nLineLength) {
			this->lines.push_back (line);
			line = "";
		}
		if (str[c] != '\n')
			line += str[c];
	}

	{ boost::mutex::scoped_lock lk(m_Mutex);
	if (this->lines.size() > m_nLineCount)
		this->mStartline = this->lines.size() - m_nLineCount;
	else
		this->mStartline = 0;

	this->mUpdateConsole = true;
	}

	return *this;
}

template<class callbackClass> OgreConsole<callbackClass>& OgreConsole<callbackClass>::operator << (int in) {

	Ogre::String text = Ogre::StringConverter::toString(in);

	line += text;

	if (line.length() >= m_nLineLength) {
		lines.push_back (line);
		line = "";
	}

	if (lines.size() > m_nLineCount)
		mStartline = lines.size() - m_nLineCount;
	else
		mStartline = 0;

	mUpdateConsole = true;

	return *this;
}

template<class callbackClass> OgreConsole<callbackClass>& OgreConsole<callbackClass>::operator << (Ogre::Real in) {

	Ogre::String text = Ogre::StringConverter::toString(in);

	line += text;

	if (line.length() >= m_nLineLength) {
		lines.push_back (line);
		line = "";
	}

	if (lines.size() > m_nLineCount)
		mStartline = lines.size() - m_nLineCount;
	else
		mStartline = 0;

	mUpdateConsole = true;

	return *this;
}

template<class callbackClass> OgreConsole<callbackClass>& OgreConsole<callbackClass>::operator << (char in) {

	if (in == '\n' || line.length() >= m_nLineLength) {
		lines.push_back (line);
		line = "";
	}
	else line += in;

	if (lines.size() > m_nLineCount)
		mStartline = this->lines.size() - m_nLineCount;
	else
		mStartline = 0;

	mUpdateConsole = true;

	return *this;
}

template<class callbackClass> bool OgreConsole<callbackClass>::frameEnded (const Ogre::FrameEvent &evt)
{
	return true;
}

template<class callbackClass> void OgreConsole<callbackClass>::setVisible (bool isVisible)
{
	mIsVisible = isVisible;
	mLayer->setVisible (mIsVisible);
}

template<class callbackClass> void OgreConsole<callbackClass>::addAlias (Ogre::String alias, Ogre::String exec) {
	Ogre::StringUtil::toLowerCase (alias);
	aliases[alias] = exec;
}

template<class callbackClass> void OgreConsole<callbackClass>::addCommand (Ogre::String command, OgreConsoleMemberFunctionPtr func)
{
	Ogre::StringUtil::toLowerCase (command);
	commands[command] = func;
}

template<class callbackClass> void OgreConsole<callbackClass>::removeAlias (Ogre::String alias)
{
	Ogre::StringUtil::toLowerCase (alias);
	aliases.erase (aliases.find (alias) );
}

template<class callbackClass> void OgreConsole<callbackClass>::removeCommand (Ogre::String command)
{
	Ogre::StringUtil::toLowerCase (command);
	commands.erase (commands.find (command) );
}

#if OGRE_VERSION_MINOR < 8 && OGRE_VERSION_MAJOR < 2
template<class callbackClass> void OgreConsole<callbackClass>::messageLogged ( const Ogre::String& message, Ogre::LogMessageLevel lml, bool maskDebug, const Ogre::String &logName )
#else
// "bool& skip" added in Ogre 1.8
template<class callbackClass> void OgreConsole<callbackClass>::messageLogged ( const Ogre::String& message, Ogre::LogMessageLevel lml, bool maskDebug, const Ogre::String &logName, bool &skip )
#endif
{
	print (message);
}

template<class callbackClass> void OgreConsole<callbackClass>::showAliases() {
	for (std::map<Ogre::String, Ogre::String>::iterator it = aliases.begin(); it != aliases.end(); ++it) {
		print('\"' + it->first + "\" \"" + it->second + '\"');
	}
}

template<class callbackClass> void OgreConsole<callbackClass>::showCommands() {
	for (std::map<Ogre::String, OgreConsoleMemberFunctionPtr>::iterator it = commands.begin(); it != commands.end(); ++it) {
		if (it->first != "why") print(it->first);
	}
}

template<class T> bool OgreConsole<T>::isSuchAlias(const Ogre::String& alias) const {
	if (aliases.find(alias) == aliases.end()) {
		return false;
	}
	return true;
}

template<class T> bool OgreConsole<T>::isSuchCommand(const Ogre::String& command) const {
	if (commands.find(command) == commands.end()) {
		return false;
	}
	return true;
}

//std::ostream& operator << (std::ostream&, const GameMap&);
template <class T, class Y> OgreConsole<T>& operator << (OgreConsole<T>& o, const array2<Y>& gm) {
	for (int i = 0; i < gm.getM() - 1; i++) {
		for (int j = 0; j < gm.getN() - 1; j++) {
			Ogre::String sym = Ogre::StringConverter::toString((int)gm(i,j));
			while (sym.length() < 4) {
				sym += ' ';
			}
			o << sym;
		}
		o << (int)gm(i, gm.getN() - 1) << '\n';
	}
	for (int j = 0; j < gm.getM() - 1; j++) {
		Ogre::String sym = Ogre::StringConverter::toString((int)gm(gm.getM() - 1, j));
		while (sym.length() < 4) {
			sym += ' ';
		}
		o << sym;
	}
	o << (int)gm(gm.getM() - 1,gm.getN() - 1);

	return o;
}
template <class T> OgreConsole<T>& operator << (OgreConsole<T>& o, const MapContainer& gm) {
	for (int i = 0; i < gm.getHeight() - 1; i++) {
		for (int j = 0; j < gm.getWidth() - 1; j++) {
			Ogre::String sym = Ogre::StringConverter::toString((int)gm(i,j));
			while (sym.length() < 4) {
				sym += ' ';
			}
			o << sym;
		}
		o << (int)gm(i, gm.getWidth() - 1) << '\n';
	}
	for (int j = 0; j < gm.getHeight() - 1; j++) {
		Ogre::String sym = Ogre::StringConverter::toString((int)gm(gm.getHeight() - 1, j));
		while (sym.length() < 4) {
			sym += ' ';
		}
		o << sym;
	}
	o << (int)gm(gm.getHeight() - 1,gm.getWidth() - 1);

	return o;
}
template <class T> OgreConsole<T>& operator << (OgreConsole<T>& o, GameMap& gm) {
	//for (int i = 0; i < gm.getHeight() - 1; i++) {
	//	for (int j = 0; j < gm.getWidth() - 1; j++) {

	//		Ogre::String sym;

	//		if (curFov(i,j) > 0)
	//			sym = " " + Ogre::StringConverter::toString((int)curFov(i,j));
	//		else
	//			sym = Ogre::StringConverter::toString((int)gm(i,j));

	//		const int length_of_tab = 4; 

	//		while (sym.length() < length_of_tab) {
	//			sym += ' ';
	//		}

	//		o << sym;
	//	}
	//	if (curFov(i, gm.getWidth() - 1) > 0)
	//		o << " " << (int)curFov(i, gm.getWidth() - 1) << '\n';
	//	else
	//		o << (int)gm(i, gm.getWidth() - 1) << '\n';
	//}
	//for (int j = 0; j < gm.getHeight() - 1; j++) {

	//	Ogre::String sym;

	//	if (curFov(gm.getHeight() - 1, j) > 0)
	//		sym = " " + Ogre::StringConverter::toString((int)curFov(gm.getHeight() - 1, j));
	//	else
	//		sym = Ogre::StringConverter::toString((int)gm(gm.getHeight() - 1, j));

	//	//const int length_of_tab = curFov(gm.getHeight() - 1, j) > 0 ? 5 : 4; 
	//	const int length_of_tab = 4; 

	//	while (sym.length() < length_of_tab) {
	//		sym += ' ';
	//	}

	//	o << sym;
	//}

	//if (curFov(gm.getHeight() - 1, gm.getWidth() - 1) > 0)
	//	o << " " << (int)curFov(gm.getHeight() - 1, gm.getWidth() - 1);
	//else
	//	o << (int)gm(gm.getHeight() - 1,gm.getWidth() - 1);

	
	//auto curFov = *gm.getCurFOV();

	//for (int i = 0; i < gm.getHeight(); i++) {
	//	Ogre::String sym(2*gm.getWidth(), 0);
	//	for (int j = 0; j < gm.getWidth(); ++j) {
	//		sym[2*j] = curFov(i,j) > 0 ? '0' + curFov(i,j) : '0';
	//		sym[2*j+1] = ' ';
	//	}
	//	if (i < gm.getHeight()-1)
	//		o << sym << '\n';
	//	else
	//		o << sym;
	//}

	//return o;
	return o << gm.getMapContainerConstRef();
}
template <class T> OgreConsole<T>& operator << (OgreConsole<T>& o, GameMap* _gm) {
	//for (int i = 0; i < gm.getDimension() - 1; i++) {
	//	for (int j = 0; j < gm.getDimension() - 1; j++) 
	//		o << (int)gm(i,j) << "  ";
	//	o << (int)gm(i,gm.getDimension() - 1) << '\n';
	//}
	//for (int j = 0; j < gm.getDimension() - 1; j++) 
	//	o << (int)gm(gm.getDimension() - 1,gm.getDimension() - 1) << "  ";
	//o << (int)gm(gm.getDimension() - 1,gm.getDimension() - 1);

	//return o;
	return o << _gm->getMapContainerConstRef();
	//return o << *_gm;
}

#endif