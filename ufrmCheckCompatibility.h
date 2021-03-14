#ifndef ufrmCheckCompatibilityH
#define ufrmCheckCompatibilityH

#include <System.Classes.hpp>
#include <FMX.Controls.hpp>
#include <FMX.Forms.hpp>
#include <FMX.Controls.Presentation.hpp>
#include <FMX.Layouts.hpp>
#include <FMX.StdCtrls.hpp>
#include <FMX.Types.hpp>
#include <iostream>
#include <vector>
#include "ufrmMain.h"

class TfrmCheckCompatibility : public TForm
{
__published: // IDE-managed Components
	TGridPanelLayout *GridPanelLayout1;
	TToolBar *tbMain;
	TLayout *ButtonsLayout;
	TButton *btnOK;
	TGroupBox *gbLicenceA;
	TGroupBox *gbLicenceB;
	TLabel *lblCompatibility;
	TVertScrollBox *sbLicencesA;
	TVertScrollBox *sbLicencesB;

	void __fastcall btnOKClick(TObject *Sender);
private:
	void CalcCompatibility();
  bool FindCompatibilityRoute();
public:
	__fastcall TfrmCheckCompatibility(TComponent* Owner);
	void PopulateLicences();
	void __fastcall RadioButtonLicenceAClick(TObject *Sender);
	void __fastcall RadioButtonLicenceBClick(TObject *Sender);
};

extern PACKAGE TfrmCheckCompatibility *frmCheckCompatibility;

#endif
