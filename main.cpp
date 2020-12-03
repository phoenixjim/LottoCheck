#include "LottoCheck.h"
void MyData::Jsonize ( JsonIO& json )

{
	json
		("betsFile", betsFile)
		("winnersFile", winnersFile)
		;
}

void Configs::SelectBets()
{
	selectdbwin.Type(t_("CSV file"), "*.csv");
	if(!selectdbwin.ExecuteOpen(t_("Select Bets File")))
	{
	    return;
	}
	data.betsFile = selectdbwin.Get();
	SaveSettings();
}

void Configs::SelectWinners()
{
	selectdbwin.Type(t_("CSV file"), "*.csv");
	if(!selectdbwin.ExecuteOpen(t_("Select Previous Winners File")))
	{
	    return;
	}
	data.winnersFile = selectdbwin.Get();
	SaveSettings();
}

void Configs::Populate()
{
	if ( !FileExists ( ConfigFile(GetExeTitle() + ".json") ) )
	{
		SelectBets();
		SelectWinners();
	}
}

Configs::Configs()
{
	Populate();
	LoadFromJsonFile ( data );
}

void Configs::SaveSettings()
{
	StoreAsJsonFile(data);
}

LottoCheck::LottoCheck()
{
	CtrlLayout(*this, "Check your NYLotto bets.");
	
	EditInt one, two, three, four, five, six;
	btnCheck << [=] { Check(); };
	btnSave << [=] { Save(); };
	btnLast10 << [=] { Last(); };
	btnClose << [=] { Close(); };
	
	arrBets.AddColumn("First").Edit(one);
	arrBets.AddColumn("Second").Edit(two);
	arrBets.AddColumn("Third").Edit(three);
	arrBets.AddColumn("Fourth").Edit(four);
	arrBets.AddColumn("Fifth").Edit(five);
	arrBets.AddColumn("Sixth").Edit(six);
	arrBets.Appending().Removing();

	FileIn bets(myConfig.data.betsFile);
	
	auto v = GetCsvLine(bets, ',', CHARSET_DEFAULT);
	while (v) {
		arrBets.Add(StrInt(v[0]), StrInt(v[1]), StrInt(v[2]), StrInt(v[3]), StrInt(v[4]), StrInt(v[5]));
		v = GetCsvLine(bets, ',', CHARSET_DEFAULT);
	}
	bets.Close();
}

void	LottoCheck::Check()
{
	if (IsNull(txtFirst) || IsNull(txtSecond) || IsNull(txtThird) || IsNull(txtFourth) || IsNull(txtFifth) || IsNull(txtSixth) || IsNull(txtBonus))
	{
		Exclamation("Must fill in all values first");
		return;
	}
	int newwinners[7] = {txtFirst.GetData(), txtSecond.GetData(), txtThird.GetData(), txtFourth.GetData(), txtFifth.GetData(), txtSixth.GetData(), txtBonus.GetData()};
    int n;
    int matches = 0;
    bool Winner = false;
    
	for (int x = 0; x < arrBets.GetCount(); x++) // each row of bets
	{
		for (int y = 0; y < 6; y++) // each member of bets
		{
			for (int z = 0; z < 7; z++) // each member of newwinners
			{
				if ((int)arrBets.GetColumn(x, y) == newwinners[z]) {
					matches += 1;
					break;
				}
			}
		}
		if (matches >= 3)
		{
			Winner = true;
			if (matches > 5) {
				// for (int c = 0; c < 6; c++) arrBets.Set(x, c, Red); //.GridColor(Red);
				PromptOK("It's HUGE!");
			}
			else if (matches == 5) {
				// for (int c = 0; c < 6; c++) arrBets.Set(x, c, Yellow); //.GridColor(Red);
				PromptOK("Looks like a Big winner!");
			}
			else {
				// for (int c = 0; c < 6; c++)	arrBets.Set(x, c, Color(Green)); //.GridColor(Red);
				PromptOK("Looks like a winner :)");
			}
		}
		matches = 0;
	}
	if (!Winner) PromptOK("Better luck next time, Bub");
}
	
void	LottoCheck::Save()
{
	if (IsNull(txtFirst) || IsNull(txtSecond) || IsNull(txtThird) || IsNull(txtFourth) || IsNull(txtFifth) || IsNull(txtSixth) || IsNull(txtBonus))
	{
		Exclamation("Must fill in all values first");
		return;
	}
	Date betdate = --Date(GetSysDate());

	String data = Format(betdate) << "," << txtFirst.GetData() << "," << txtSecond.GetData() << "," << txtThird.GetData() << "," << txtFourth.GetData() << "," << txtFifth.GetData() << "," << txtSixth.GetData() << "," << txtBonus.GetData();
	String tmpCsv = GetFileDirectory(myConfig.data.betsFile) + "tmpcsv.csf";
	FileOut tmpcsv(tmpCsv);
	tmpcsv.PutLine(data);
    FileIn oldwinners(myConfig.data.winnersFile);
    
    auto w = GetCsvLine(oldwinners, ',', CHARSET_DEFAULT);

	while (w) {
		String olddata = w[0] << "," << w[1] << "," << w[2] << "," << w[3] << "," << w[4] << "," << w[5] << "," << w[6] << "," << w[7];
		tmpcsv.PutLine(olddata);
		w = GetCsvLine(oldwinners, ',', CHARSET_DEFAULT);
	}
	oldwinners.Close();
	tmpcsv.Close();
	if ( FileDelete(myConfig.data.winnersFile) ) FileMove( tmpCsv, myConfig.data.winnersFile );
}

void	LottoCheck::Last()
{
    FileIn oldwinners(myConfig.data.winnersFile);
    
    auto w = GetCsvLine(oldwinners, ',', CHARSET_DEFAULT);
    String date = w[0];
    int matches = 0, bonusmatches = 0;
    bool Winner = false;
    int winners[10][7], i = 0;
    while (i < 10) {
		for (int x = 0; x < arrBets.GetCount(); x++) // each row of bets
		{
			for (int y = 0; y < 6; y++) // each member of bets
			{
				for (int z = 1; z < 8; z++) // each member of newwinners
				{
					if ((int)arrBets.GetColumn(x, y) == StrInt(w[z])) {
						if (z < 7)	matches += 1;
						else bonusmatches = 1;
						break;
					}
				}
			}
			if (matches >= 3)
			{
				Winner = true;
				if (matches > 5 || (matches == 5 && bonusmatches == 1)) {
					// for (int c = 0; c < 6; c++) arrBets.Set(x, c, Red); //.GridColor(Red);
					PromptOK("It's HUGE!");
				}
				else if (matches == 5) {
					// for (int c = 0; c < 6; c++) arrBets.Set(x, c, Yellow); //.GridColor(Red);
					PromptOK("Looks like a Big winner!");
				}
				else {
					// for (int c = 0; c < 6; c++)	arrBets.Set(x, c, Color(Green)); //.GridColor(Red);
					PromptOK("Looks like a winner :)");
				}
			}
			matches = bonusmatches = 0;
		}
	w = GetCsvLine(oldwinners, ',', CHARSET_DEFAULT);
	i++;
	}
	if (!Winner) PromptOK("Better luck next time, Bub");
}

void	LottoCheck::Close()
{
	Close();
}

GUI_APP_MAIN
{
	LottoCheck().Run();
}
