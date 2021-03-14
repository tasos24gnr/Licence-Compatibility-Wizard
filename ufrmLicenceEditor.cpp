#include <fmx.h>
#pragma hdrstop

#include "ufrmLicenceEditor.h"
#pragma package(smart_init)
#pragma resource "*.fmx"

TfrmLicenceEditor *frmLicenceEditor;

// We are using Tag do denote the usage of this form
// If Tag is -1 then we are adding a new Licence
// If Tag is 0 or more then we are editing a Licence

// ---------------------------------------------------------------------------
// Form creation/destruction

// Constructor
__fastcall TfrmLicenceEditor::TfrmLicenceEditor(TComponent* Owner) : TForm(Owner)
{
	// Does nothing
	return;
}

void __fastcall TfrmLicenceEditor::FormShow(TObject *Sender)
{
	btnDelete->Visible = (Tag >= 0);
}

void TfrmLicenceEditor::PopulateLicences()
{
	TListViewItem *lvItem;
	TLicence *ParentLicence, *Licence;

	if (Tag != -1)
		ParentLicence = frmMain->PaletteLicences.at(Tag);
	else
		ParentLicence = NULL;

	// Set them in edit mode so that check boxes appear next to the items
	lvCompatibleWith->EditMode = true;
	lvIncompatibleWith->EditMode = true;

	lvCompatibleWith->BeginUpdate(); // Do not draw listview until we are done inserting new elements
	lvIncompatibleWith->BeginUpdate();
	try
	{
		lvCompatibleWith->Items->Clear();
		lvIncompatibleWith->Items->Clear();

		// Create header for CompatibleWith list view
		lvItem = lvCompatibleWith->Items->Add();
		lvItem->Purpose = TListItemPurpose::Header;
		lvItem->Text = "Compatible with";
		lvItem->Height = 46;

		// Create header for IncompatibleWith list view
		lvItem = lvIncompatibleWith->Items->Add();
		lvItem->Purpose = TListItemPurpose::Header;
		lvItem->Text = "Incompatible with";
		lvItem->Height = 46;

		// Get all licences from frmMain's PaletteLicences into the two list views
		for (int i = 0; i < frmMain->PaletteLicences.size(); i++)
		{
			Licence = frmMain->PaletteLicences.at(i);

			if (i != Tag) // We will not add to the list the licence we are editing
			{
				lvItem = lvCompatibleWith->Items->Add();
				lvItem->Tag = i;
				lvItem->Text = Licence->Name;
				lvItem->Height = 36;
				// Check to see if licence exists in compatibility list
				if (Tag != -1)
					for (int j = 0; j < ParentLicence->CompatibleWith.size(); j++)
						if (ParentLicence->CompatibleWith.at(j) == Licence)
							lvItem->Checked = true;

				lvItem = lvIncompatibleWith->Items->Add();
				lvItem->Tag = i;
				lvItem->Text = Licence->Name;
				lvItem->Height = 36;
				// Check to see if licence exists in incompatibility list
				if (Tag != -1)
					for (int j = 0; j < ParentLicence->IncompatibleWith.size(); j++)
						if (ParentLicence->IncompatibleWith.at(j) == Licence)
							lvItem->Checked = true;
			}
		}
	}
	__finally
	{
		lvIncompatibleWith->EndUpdate();
		lvCompatibleWith->EndUpdate();
	}
}

// ---------------------------------------------------------------------------
// Button actions

void __fastcall TfrmLicenceEditor::btnOKClick(TObject *Sender)
{
	int i;
	TLicence *Licence;
	TListViewItem *lvItem1, *lvItem2;

	if (Tag == -1)
	{
		// If Tag is -1 we are adding a new Licence to the palette
		Licence = new TLicence;
		Licence->Name = edtLicenceName->Text;

		if (frmMain->AddNewPaletteLicence(Licence))
		{
			// We have added this Licence to frmMain->PaletteLicences
			// Now set the compatibility/incompatibility arrays

			Licence->CompatibleWith.clear();
			Licence->IncompatibleWith.clear();
			for (i = 0; i < lvCompatibleWith->Items->Count; i++)
			{
				lvItem1 = dynamic_cast<TListViewItem*>(lvCompatibleWith->Items->Item[i]);
				lvItem2 = dynamic_cast<TListViewItem*>(lvIncompatibleWith->Items->Item[i]);

				if (lvItem1->Checked)
					Licence->CompatibleWith.push_back(frmMain->PaletteLicences.at(lvItem1->Tag));
				if (lvItem2->Checked)
					Licence->IncompatibleWith.push_back(frmMain->PaletteLicences.at(lvItem2->Tag));
			}

			Close(); // We have finished, close this form
		}
		else
			delete Licence; // Licence could not be added, delete allocated memory
	}
	else
	{
		// Tag is >= 0, which means we are editing a licence

		// Check to see the new name in edtLicenceName is not the same as any other
		// licence name
		for (i = 0; i < frmMain->PaletteLicences.size(); i++)
			if (i != Tag)
			{
				Licence = frmMain->PaletteLicences.at(i);
				if (SameText(Licence->Name, edtLicenceName->Text))
				{
					TDialogService::MessageDialog(

						"You cannot use this licence name, it already exists",

						TMsgDlgType::mtError,

						TMsgDlgButtons() << TMsgDlgBtn::mbOK,

						TMsgDlgBtn::mbOK,

						0,

						(Fmx::Dialogs::TInputCloseDialogEvent) NULL);

					edtLicenceName->SetFocus();

					return;
				}
			}

		// Now check the user has not checked both the same licence as compatible and
		// incompatible in the list views
		for (i = 0; i < lvCompatibleWith->Items->Count; i++)
		{
			lvItem1 = dynamic_cast<TListViewItem*>(lvCompatibleWith->Items->Item[i]);
			lvItem2 = dynamic_cast<TListViewItem*>(lvIncompatibleWith->Items->Item[i]);
			if (lvItem1->Checked && lvItem2->Checked)
			{
				TDialogService::MessageDialog(

					"A licence cannot be both compatible and incompatible to another licence",

					TMsgDlgType::mtError,

					TMsgDlgButtons() << TMsgDlgBtn::mbOK,

					TMsgDlgBtn::mbOK,

					0,

					(Fmx::Dialogs::TInputCloseDialogEvent) NULL);

				lvCompatibleWith->ItemIndex = i;
				lvIncompatibleWith->ItemIndex = i;
				lvCompatibleWith->SetFocus();
				return;
			}
		}

		// Set the name of the licence and the compatibility / incompatibility arrays
		Licence = frmMain->PaletteLicences.at(Tag);
		Licence->Name = edtLicenceName->Text;
		Licence->CompatibleWith.clear();
		Licence->IncompatibleWith.clear();
		for (i = 0; i < lvCompatibleWith->Items->Count; i++)
		{
			lvItem1 = dynamic_cast<TListViewItem*>(lvCompatibleWith->Items->Item[i]);
			lvItem2 = dynamic_cast<TListViewItem*>(lvIncompatibleWith->Items->Item[i]);

			if (lvItem1->Checked)
				Licence->CompatibleWith.push_back(frmMain->PaletteLicences.at(lvItem1->Tag));
			if (lvItem2->Checked)
				Licence->IncompatibleWith.push_back(frmMain->PaletteLicences.at(lvItem2->Tag));
		}
		frmMain->RepopulateLicencePalette();

		// Now check if any sbContent licences exist with our Tag and set the name
		TSelection *Selection;
		for (i = 0; i < frmMain->sbContent->Content->ControlsCount; i++)
			if (dynamic_cast<TSelection*>(frmMain->sbContent->Content->Controls->Items[i]))
				if (frmMain->sbContent->Content->Controls->Items[i]->Tag == Tag)
				{
					// Find the label
					Selection = reinterpret_cast<TSelection*>(frmMain->sbContent->Content->Controls->Items[i]);
					// Controls gives the direct children of the component. Components gives all
					// components, even the children of the children
					for (int j = 0; j < Selection->ComponentCount; j++)
						if (dynamic_cast<TLabel*>(Selection->Components[j]))
						{
							reinterpret_cast<TLabel*>(Selection->Components[j])->Text = edtLicenceName->Text;
							break;
						}
					break;
				}

		Close(); // We are finished, close this form
	}

	return;
}

void __fastcall TfrmLicenceEditor::btnDeleteClick(TObject * Sender)
{
	// Delete the Licence denoted by Tag
	if (Tag != -1) // This should never be -1 as btnDelete is only visible during edit mode
	{
		TLicence *Licence;
		int i, j;

		// First check to see if the Licence is contained in sbContent
		for (i = 0; i < frmMain->sbContent->Content->ControlsCount; i++)
			if (dynamic_cast<TSelection*>(frmMain->sbContent->Content->Controls->Items[i]))
			{
				if (frmMain->sbContent->Content->Controls->Items[i]->Tag == Tag)
				{
					frmMain->sbContent->Content->Controls->Items[i]->DisposeOf();
					i--; // Revisit same index as it will now contain a new item
				}
				else if (frmMain->sbContent->Content->Controls->Items[i]->Tag > Tag)
					frmMain->sbContent->Content->Controls->Items[i]->Tag--;
			}

		// Delete the Licence from any CompatibleWith/IncompatibleWith arrays in PaletteLicences
		for (i = 0; i < frmMain->PaletteLicences.size(); i++)
		{
			Licence = frmMain->PaletteLicences.at(i);
			for (j = 0; j < Licence->CompatibleWith.size(); j++)
				if (Licence->CompatibleWith.at(j) == frmMain->PaletteLicences.at(Tag))
				{
					Licence->CompatibleWith.erase(Licence->CompatibleWith.begin() + j);
					break;
				}
			for (j = 0; j < Licence->IncompatibleWith.size(); j++)
				if (Licence->IncompatibleWith.at(j) == frmMain->PaletteLicences.at(Tag))
				{
					Licence->IncompatibleWith.erase(Licence->IncompatibleWith.begin() + j);
					break;
				}
		}
		// Now, delete the item from frmMain->PaletteLicences array
		Licence = frmMain->PaletteLicences.at(Tag);
		frmMain->PaletteLicences.erase(frmMain->PaletteLicences.begin() + Tag);
		delete Licence;
		// and also free its memory

		frmMain->RepopulateLicencePalette(); // reflect changes in the interface
		Close(); // close this form, we are done
	}
}

void __fastcall TfrmLicenceEditor::btnCancelClick(TObject * Sender)
{
	// Just set this form to not visible and do nothing else
	Close();

	return;
}
