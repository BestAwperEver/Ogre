#include "OgreConsoleM.h"
 
using namespace Ogre;
using namespace std;

template<> OgreConsoleM* Singleton<OgreConsoleM>::msSingleton = 0;
 
#define CONSOLE_LINE_LENGTH 100
#define CONSOLE_LINE_COUNT 15

OgreConsoleM::OgreConsoleM():
		m_pSceneManager(0),
		m_pRoot(0),
		m_pOverlay(0),
		m_pTextbox(0),
		m_pNode(0),
		m_pRect(0)
{
	m_nStartLine = 0;
}
 
OgreConsoleM::~OgreConsoleM(){

	Ogre::Root::getSingletonPtr()->removeFrameListener(this);
	Ogre::LogManager::getSingleton().getDefaultLog()->removeListener(this);

	shutdown();

	//if (m_pSceneManager) {
	//	delete m_pSceneManager;
	//	m_pSceneManager = 0;
	//}
	//if (m_pOverlay) {
	//	delete m_pOverlay;
	//	m_pOverlay = 0;
	//}
	//if (m_pTextbox) {
	//	delete m_pTextbox;
	//	m_pTextbox = 0;
	//}

	//if (m_pOverlay) {
	//	m_pOverlay->clear();
	//}

	//if (m_pRect) {
	//	delete m_pRect;
	//	m_pRect = 0;
	//}

	//if (m_pNode) {
	//	delete m_pNode;
	//	m_pNode = 0;
	//}
	//if (m_pRoot) {
	//	delete m_pRoot;
	//	m_pRoot = 0;
	//}
}
 
void OgreConsoleM::init(Ogre::Root *root){
	if(!root->getSceneManagerIterator().hasMoreElements())
		OGRE_EXCEPT( Exception::ERR_INTERNAL_ERROR, "No scene manager found!", "init" );
 
	this->m_pRoot=root;
	m_pSceneManager=m_pRoot->getSceneManagerIterator().getNext();
	m_pRoot->addFrameListener(this);
 
	m_fHeight=1;
 
//	 Create background rectangle covering the whole screen
//	m_pRect = m_pSceneManager->createMovableObject("myrect");
	m_pRect = new Rectangle2D(true);
	m_pRect->setCorners(-1, 1, 1, 1-m_fHeight);
	m_pRect->setMaterial("console/background");
	m_pRect->setRenderQueueGroup(RENDER_QUEUE_OVERLAY);
	m_pRect->setBoundingBox(AxisAlignedBox(-100000.0*Vector3::UNIT_SCALE, 100000.0*Vector3::UNIT_SCALE));
	m_pNode = m_pSceneManager->getRootSceneNode()->createChildSceneNode("#Console");
	m_pNode->attachObject(m_pRect);

	//m_pDecoration = m_pLayer->createRectangle(8,8, mScreen->getWidth() - 16, mGlyphData->mLineHeight );
	//m_pDecoration->background_gradient(Gorilla::Gradient_NorthSouth, Gorilla::rgb(128,128,128,128), Gorilla::rgb(64,64,64,128));
	//m_pDecoration->border(2, Gorilla::rgb(128,128,128,128));
 
	m_pTextbox=OverlayManager::getSingleton().createOverlayElement("TextArea","ConsoleText");
	m_pTextbox->setCaption("hello");
	m_pTextbox->setMetricsMode(GMM_RELATIVE);
	m_pTextbox->setPosition(0,0);
	m_pTextbox->setParameter("font_name","Console");
	m_pTextbox->setParameter("colour_top","1 1 1");
	m_pTextbox->setParameter("colour_bottom","1 1 1");
	m_pTextbox->setParameter("char_height","0.03");
 
	m_pOverlay=OverlayManager::getSingleton().create("Console");   
	m_pOverlay->add2D((OverlayContainer*)m_pTextbox);
	m_pOverlay->show();
	LogManager::getSingleton().getDefaultLog()->addListener(this);
}
 
void OgreConsoleM::shutdown(){
	if(!m_bInitialized)
		return;
	m_pOverlay->remove2D((OverlayContainer*)m_pTextbox);
	OverlayManager::getSingleton().destroyOverlayElement(m_pTextbox);
	OverlayManager::getSingleton().destroy(m_pOverlay);
	m_pNode->detachObject(m_pRect);
	delete m_pRect;
}

void OgreConsoleM::injectKeyDown(const OIS::KeyEvent &arg){
	if(!m_bVisible)
		return;
	if (arg.key == OIS::KC_RETURN)
	{
		//split the parameter list
		const char *str=m_sPrompt.c_str();
		std::vector<String> params;
		String param="";
		for(int c=0;c<m_sPrompt.length();c++){
			if(str[c]==' '){
				if(param.length())
				params.push_back(param);
				param="";
			}
			else
				param+=str[c];
		}
		if(param.length())
			params.push_back(param);
 
		//try to execute the command
		std::map<String,void(*)(std::vector<String>&)>::iterator i;
		for(i=commands.begin();i!=commands.end();i++){
			if((*i).first==params[0]){
				if((*i).second)
				(*i).second(params);
				break;
			}
		}
 
		print(m_sPrompt);
		m_sPrompt="";
	}
	if (arg.key == OIS::KC_BACK)
		m_sPrompt=m_sPrompt.substr(0,m_sPrompt.length()-1);
	if (arg.key == OIS::KC_PGUP)
	{
		if(m_nStartLine>0)
			m_nStartLine--;
	}
	if (arg.key == OIS::KC_PGDOWN)
	{
		if(m_nStartLine<m_Lines.size())
			m_nStartLine++;
	}
	else
	{
		char legalchars[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890+!\"#%&/()=?[]\\*-_.:,; ";
		for(int c=0;c<sizeof(legalchars);c++){
			if(legalchars[c]==arg.text){
				m_sPrompt+=arg.text;
				break;
			}
		}
	}
	m_bUpdateOverlay=true;
}

 void OgreConsoleM::print(const String &text){
    //subdivide it into m_Lines
    const char *str = text.c_str();
    int start = 0, count = 0;
    int len = text.length();

    String line;

    for (int c = 0; c < len; c++) {
       if (str[c] == '\n' || line.length() >= CONSOLE_LINE_LENGTH) {
          m_Lines.push_back(line);
          line = "";
       }
       if (str[c] != '\n')
          line += str[c];
    }

    if ( line.length() )
       m_Lines.push_back(line);

    if( m_Lines.size() > CONSOLE_LINE_COUNT)
       m_nStartLine = m_Lines.size() - CONSOLE_LINE_COUNT;
    else
       m_nStartLine = 0;

    m_bUpdateOverlay=true;
 }

 bool OgreConsoleM::frameStarted(const Ogre::FrameEvent &evt){
    if (m_bVisible && m_fHeight<1) {
       m_fHeight+=evt.timeSinceLastFrame*2;
       m_pTextbox->show();
       if(m_fHeight>=1){
          m_fHeight=1;
       }
    }
    else if (!m_bVisible && m_fHeight>0) {
       m_fHeight-=evt.timeSinceLastFrame*2;
       if(m_fHeight<=0){
          m_fHeight=0;
          m_pTextbox->hide();
       }
    }
 
    m_pTextbox->setPosition(0,(m_fHeight-1)*0.5);

	if (m_fHeight == 1) {
		bool ololo = false;
	}

    //m_pRect->setCorners(-1,m_fHeight,1,1-m_fHeight);
	//m_pRect->setCorners(-1, 1, 1, 1-m_fHeight);
 
    if(m_bUpdateOverlay){
       String text;
       std::list<String>::iterator i,start,end;
 
       //make sure is in range
       if(m_nStartLine>m_Lines.size())
          m_nStartLine=m_Lines.size();
 
       int lcount=0;
       start=m_Lines.begin();
       for(int c=0;c<m_nStartLine;c++)
          start++;
       end=start;
       for(int c=0;c<CONSOLE_LINE_COUNT;c++){
          if(end==m_Lines.end())
             break;
          end++;
       }
       for(i=start;i!=end;i++)
          text+=(*i)+"\n";
 
       //add the m_sPrompt
       text+="] "+m_sPrompt;
 
       m_pTextbox->setCaption(text);
       m_bUpdateOverlay=false;
    }
    return true;
 }
 
 bool OgreConsoleM::frameEnded(const Ogre::FrameEvent &evt){
 
    return true;
 }
 
void OgreConsoleM::setVisible(bool visible){
	this->m_bVisible = visible;
}
 
void OgreConsoleM::addCommand(const String &command, void (*func)(std::vector<String>&)){
	commands[command]=func;
}
 
void OgreConsoleM::removeCommand(const String &command){
	commands.erase(commands.find(command));
}

OgreConsoleM& OgreConsoleM::operator << (const Ogre::String& s) {
	this->print(s);
	return *this;
}

OgreConsoleM& OgreConsoleM::operator << (const GameMap& gm) {

	char* s;
	
	s = new char[2 * gm.getDimension() * gm.getDimension() + 1];

	for (int i = 0; i < gm.getDimension() /* - 1 */; i++) {
		for (int j = 0; j < gm.getDimension() - 1; j++) {
			s [2 * i * gm.getDimension() + 2 * j] = '0' + gm(i,j); 
			s [2 * i * gm.getDimension() + 2 * j + 1] = ' ';
		}
		s [2 * i * gm.getDimension() + 2 * (gm.getDimension() - 1)] = '0' + gm(i,gm.getDimension() - 1);
		s [2 * i * gm.getDimension() + 2 * (gm.getDimension() - 1) + 1] = '\n';
	}

	s[2 * gm.getDimension() * gm.getDimension()] = 0;

	this->print(s);

	delete s;

	//for (int j = 0; j < gm.getDimension() - 1; j++) {
	//	s += gm(gm.getDimension() - 1,gm.getDimension() - 1);
	//	s += ' ';
	//}
	//o << gm(gm.getDimension() - 1,gm.getDimension() - 1);

	return *this;
}