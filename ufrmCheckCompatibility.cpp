#include <fmx.h>
#pragma hdrstop

#include "ufrmCheckCompatibility.h"
#pragma package(smart_init)
#pragma resource "*.fmx"

TfrmCheckCompatibility *frmCheckCompatibility;
TLicence *LicenceA, *LicenceB;
std::vector<TLicence*>LicencesVisited;

__fastcall TfrmCheckCompatibility::TfrmCheckCompatibility(TComponent* Owner) : TForm(Owner)
{

}

void TfrmCheckCompatibility::PopulateLicences()
{
	LicenceA = NULL;
	LicenceB = NULL;

	TLicence *Licence;
	TRadioButton *RadioButton;

	lblCompatibility->Text = "Select licences to check";

	// First dispose of any radion buttons that have been previously populated
	while (sbLicencesA->Content->ControlsCount > 0)
		sbLicencesA->Content->Controls->Items[0]->DisposeOf();
	while (sbLicencesB->Content->ControlsCount > 0)
		sbLicencesB->Content->Controls->Items[0]->DisposeOf();

	// Get all licences from frmMain's sbContent into the two scroll boxes
	// as radio buttons
	float LastBottom = 0;
	// for (int i = 0; i < frmMain->PaletteLicences.size(); i++)
	for (int i = 0; i < frmMain->sbContent->Content->ControlsCount; i++)
		if (dynamic_cast<TSelection*>(frmMain->sbContent->Content->Controls->Items[i]))
		{
			// Licence = frmMain->PaletteLicences.at(i);
			Licence = frmMain->PaletteLicences.at(frmMain->sbContent->Content->Controls->Items[i]->Tag);

			RadioButton = new TRadioButton(NULL);
			// RadioButton->Tag = i;
			RadioButton->Tag = frmMain->sbContent->Content->Controls->Items[i]->Tag;
			RadioButton->Text = Licence->Name;
			RadioButton->Height = 36;
			RadioButton->Align = TAlignLayout::Top;
			RadioButton->GroupName = "LicA";
			RadioButton->OnClick = RadioButtonLicenceAClick;
			RadioButton->Parent = sbLicencesA;
			RadioButton->Position->Y = LastBottom; // Make it go down the list

			RadioButton = new TRadioButton(NULL);
			RadioButton->Tag = frmMain->sbContent->Content->Controls->Items[i]->Tag;
			RadioButton->Text = Licence->Name;
			RadioButton->Height = 36;
			RadioButton->Align = TAlignLayout::Top;
			RadioButton->GroupName = "LicB";
			RadioButton->OnClick = RadioButtonLicenceBClick;
			RadioButton->Parent = sbLicencesB;
			RadioButton->Position->Y = LastBottom; // Make it go down the list

			LastBottom = RadioButton->Position->Y + RadioButton->Height;
		}
}

void __fastcall TfrmCheckCompatibility::RadioButtonLicenceAClick(TObject *Sender)
{
	LicenceA = frmMain->PaletteLicences.at(reinterpret_cast<TRadioButton*>(Sender)->Tag);
	CalcCompatibility();
}

void __fastcall TfrmCheckCompatibility::RadioButtonLicenceBClick(TObject *Sender)
{
	LicenceB = frmMain->PaletteLicences.at(reinterpret_cast<TRadioButton*>(Sender)->Tag);
	CalcCompatibility();
}

void TfrmCheckCompatibility::CalcCompatibility()
{
	// If both licences are selected
	if (LicenceA && LicenceB)
	{
		if (LicenceA == LicenceB)
			lblCompatibility->Text = "A licence is compatible to itself";
		else
		{
			// Check if there is a direct incompatibility from LicenceA to LicenceB
			for (int i = 0; i < LicenceA->IncompatibleWith.size(); i++)
				if (LicenceA->IncompatibleWith.at(i) == LicenceB)
				{
					lblCompatibility->Text = "Incompatible licences: Direct incompatibility";
					return;
				}

			if (FindCompatibilityRoute())
				lblCompatibility->Text = "Licences are compatible";
			else
				lblCompatibility->Text = "Incompatible Licences: No compatibility route found";
		}
	}
}

bool FindCompatibilityRouteRecursive(TLicence *aLicence)
{
	bool Result;

	// Remember we have visited this licence
	LicencesVisited.push_back(aLicence);

	if (aLicence == LicenceB)
		Result = true;
	else
	{
		int i, j;
		bool AlreadyVisited;

		Result = false;
		// Call ourselves for every CompatibleWith element
		for (i = 0; i < aLicence->CompatibleWith.size(); i++)
		{
			// Check if we have already visited aLicence->CompatibleWith.at(i)
			AlreadyVisited = false;
			for (j = 0; j < LicencesVisited.size(); j++)
				if (LicencesVisited.at(j) == aLicence->CompatibleWith.at(i))
				{
					AlreadyVisited = true;
					break;
				}

			if (!AlreadyVisited)
				if (FindCompatibilityRouteRecursive(aLicence->CompatibleWith.at(i)))
				{
					Result = true;
					break;
				}
		}
	}

	return Result;
}

bool TfrmCheckCompatibility::FindCompatibilityRoute()
{
	// We will recursivelly traverse all possible routes from LicenceA to LicenceB
	// Since we don't want to end up in infinite recursion, we will note what nodes
	// we have visited in LicencesVisited array. LicencesVisited is initially empty
	LicencesVisited.clear();

	return FindCompatibilityRouteRecursive(LicenceA);
}

void __fastcall TfrmCheckCompatibility::btnOKClick(TObject *Sender)
{
	Close();
}
