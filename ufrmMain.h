#ifndef ufrmMainH
#define ufrmMainH

#include <FMX.Controls.hpp>
#include <FMX.Controls.Presentation.hpp>
#include <FMX.Layouts.hpp>
#include <FMX.StdCtrls.hpp>
#include <FMX.Types.hpp>
#include <System.Classes.hpp>
#include <FMX.Objects.hpp>
#include <FMX.Controls.hpp>
#include <FMX.Controls.Presentation.hpp>
#include <FMX.Layouts.hpp>
#include <FMX.StdCtrls.hpp>
#include <FMX.Types.hpp>
#include <FMX.Controls.hpp>
#include <FMX.Forms.hpp>
#include <FMX.Controls.Presentation.hpp>
#include <FMX.Layouts.hpp>
#include <FMX.StdCtrls.hpp>
#include <FMX.Types.hpp>
#include <FMX.Objects.hpp>
#include <FMX.MultiView.hpp>
#include <iostream>
#include <vector>
#include <System.IniFiles.hpp>
#include <System.IOUtils.hpp>
#include <FMX.DialogService.hpp>
#include "ufrmLicenceEditor.h"
#include "ufrmNodeActions.h"
#include "ufrmCheckCompatibility.h"

// ---------------------------------------------------------------------------
// A Licence has a name and two lists: one for which it's compatible with
// and another for which it's incompatible with.
// For a vector,
// push_back inserts an element at the end
// size() returns the number of elements
// and at(aIndex) returns an element at position aIndex

struct TLicence
{
	String Name;

	std::vector<TLicence*>CompatibleWith;
	std::vector<TLicence*>IncompatibleWith;
};

// ---------------------------------------------------------------------------
// Main Form

class TfrmMain : public TForm
{
__published: // IDE-managed Components
	TToolBar *ToolBar;
	TButton *btnAddNewLicence;
	TButton *btnFindCompatibility;
	TVertScrollBox *sbLicencePalette;
	TScrollBox *sbContent;
	TRectangle *NodeBackground;
	TLabel *NodeDescription;
	TSelection *NodeTemplate;
	TStyleBook *StyleBook;
	TToolBar *tbLicencePalette;
	TMultiView *MultiView;
	TButton *btnMaster;

	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall btnAddNewLicenceClick(TObject *Sender);
	void __fastcall sbContentPainting(TObject *Sender, TCanvas *Canvas, const TRectF &ARect);
	void __fastcall btnFindCompatibilityClick(TObject *Sender);
private:
	bool PaletteButtonMouseIsDown;
	TPointF PaletteButtonMouseDownPos;
public:
	std::vector<TLicence*>PaletteLicences;
	TMemIniFile *DataFile;

	__fastcall TfrmMain(TComponent* Owner);

	void SaveToFile();
	void LoadFromFile();

	bool AddNewPaletteLicence(TLicence *aLicence);
	void RepopulateLicencePalette();

	void __fastcall NodeTemplateMouseEnter(TObject *Sender);
	void __fastcall NodeTemplateMouseLeave(TObject *Sender);
	void __fastcall NodeTemplateDblClick(TObject *Sender);
  void __fastcall NodeTemplateResize(TObject *Sender);
	void __fastcall NodeTemplateGesture(TObject *Sender, const TGestureEventInfo &EventInfo, bool &Handled);

	void __fastcall PaletteButtonOnMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, float X, float Y);
	void __fastcall PaletteButtonOnMouseMove(TObject *Sender, TShiftState Shift, float X, float Y);
	void __fastcall PaletteButtonOnMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, float X, float Y);
};

extern PACKAGE TfrmMain *frmMain;

#endif
