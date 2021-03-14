// ---------------------------------------------------------------------------

#include <fmx.h>
#pragma hdrstop

#include "ufrmNodeActions.h"
#pragma package(smart_init)
#pragma resource "*.fmx"

TfrmNodeActions *frmNodeActions;

__fastcall TfrmNodeActions::TfrmNodeActions(TComponent* Owner) : TForm(Owner)
{
}

void __fastcall TfrmNodeActions::btnEditClick(TObject *Sender)
{
	// We pressed on the button, bring up the editor
	frmLicenceEditor->Tag = Tag;

	// Populate values in the LicenceEditor form
	frmLicenceEditor->edtLicenceName->Text = frmMain->PaletteLicences.at(Tag)->Name;
	frmLicenceEditor->edtLicenceName->SetFocus();
	frmLicenceEditor->PopulateLicences();

	// Show the licence editor
	#ifdef MSWINDOWS
	frmLicenceEditor->ShowModal();
	#else
	frmLicenceEditor->Show();
	#endif

	Close();
}

void __fastcall TfrmNodeActions::btnDeleteClick(TObject *Sender)
{
	// Search in sbContent to find TSelection with Tag and dispose of it
	for (int i = 0; i < frmMain->sbContent->Content->ControlsCount; i++)
		if (dynamic_cast<TSelection*>(frmMain->sbContent->Content->Controls->Items[i]))
			if (frmMain->sbContent->Content->Controls->Items[i]->Tag == Tag)
			{
				frmMain->sbContent->Content->Controls->Items[i]->DisposeOf();
				break;
			}

	Close();
}

void __fastcall TfrmNodeActions::btnCancelClick(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------

