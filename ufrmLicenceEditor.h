#ifndef ufrmLicenceEditorH
#define ufrmLicenceEditorH

#include <System.Classes.hpp>
#include <FMX.Controls.hpp>
#include <FMX.Forms.hpp>
#include <FMX.Controls.Presentation.hpp>
#include <FMX.Edit.hpp>
#include <FMX.Layouts.hpp>
#include <FMX.ListView.Adapters.Base.hpp>
#include <FMX.ListView.Appearances.hpp>
#include <FMX.ListView.hpp>
#include <FMX.ListView.Types.hpp>
#include <FMX.StdCtrls.hpp>
#include <FMX.Types.hpp>
#include "ufrmMain.h"

class TfrmLicenceEditor : public TForm
{
__published:	// IDE-managed Components
	TLayout *LicenceNameLayout;
	TLabel *lblLicenceName;
	TEdit *edtLicenceName;
	TGridPanelLayout *GridPanelLayout;
	TListView *lvCompatibleWith;
	TListView *lvIncompatibleWith;
	TLayout *ButtonsLayout;
	TButton *btnOK;
	TButton *btnCancel;
	TButton *btnDelete;
	void __fastcall btnOKClick(TObject *Sender);
	void __fastcall btnCancelClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall btnDeleteClick(TObject *Sender);
private:
public:
	__fastcall TfrmLicenceEditor(TComponent* Owner);
  void PopulateLicences();
};

extern PACKAGE TfrmLicenceEditor *frmLicenceEditor;

#endif
