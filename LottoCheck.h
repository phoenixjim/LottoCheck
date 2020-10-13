#ifndef _LottoCheck_LottoCheck_h
#define _LottoCheck_LottoCheck_h

#include <CtrlLib/CtrlLib.h>

using namespace Upp;

#define LAYOUTFILE <LottoCheck/LottoCheck.lay>
#include <CtrlCore/lay.h>

struct MyData {
	String	betsFile, winnersFile;
	void	Jsonize(JsonIO& json);
    // void	Xmlize(XmlIO& xio)           { XmlizeByJsonize(xio, *this); }
};

struct Configs {
	
public:
	MyData	data;
			Configs();

	void 	Populate();
	void	SelectBets();
	void	SelectWinners();
	void	SaveSettings();
	void	CreateSample();
	
	FileSel	selectdbwin;

};

class LottoCheck : public WithLottoCheckLayout<TopWindow> {
	Configs myConfig;
	void	Check();
	void	Save();
	void	Last();
	void	Close();
	
public:
	typedef LottoCheck CLASSNAME;
	LottoCheck();
	void	Paint(Draw& w) {
				w.DrawRect(GetSize(), Color(85, 32, 155)); // <= enter your background color here
	}
};

#endif
