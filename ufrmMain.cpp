#include <fmx.h>
#pragma hdrstop

#include "ufrmMain.h"
#pragma package(smart_init)
#pragma resource "*.fmx"

TfrmMain *frmMain;

#define Pi 3.1415926535897932384626433832795

// ---------------------------------------------------------------------------
// Form creation/destruction

// Constructor
__fastcall TfrmMain::TfrmMain(TComponent* Owner) : TForm(Owner)
{
	PaletteButtonMouseIsDown = false;

	MultiView->Mode = TMultiViewMode::Drawer;

	String DataFileName;

#ifdef MSWINDOWS

	// On windows make the file reside where the application is

	DataFileName =

		IncludeTrailingPathDelimiter(ExtractFilePath(ParamStr(0))) +

		"LicenceCompatibilityData.ini";

#else

	// On Andoid, iOS, MacOS, make it reside in the home path

	DataFileName =

		IncludeTrailingPathDelimiter(System::Ioutils::TPath::GetDocumentsPath()) +

		"LicenceCompatibilityData.ini";

#endif
	// Open our data file
	DataFile = new TMemIniFile(DataFileName);
	// Load data from file into UI
	LoadFromFile();
}

// Destruction of form
void __fastcall TfrmMain::FormDestroy(TObject *Sender)
{
	// Save our data
	SaveToFile();
	// Destroy data file object
	DataFile->Free();
}

void __fastcall TfrmMain::FormShow(TObject *Sender)
{
	//
}

void __fastcall TfrmMain::FormClose(TObject *Sender, TCloseAction &Action)
{
	//
}

// ---------------------------------------------------------------------------
// File Management

void TfrmMain::SaveToFile()
{
	int i, j;
	String Str;
	TLicence *Licence;
	TStringList *SL;
	TSelection *Selection;
	bool VisibleInGraph;

	// Save all palette licences
	SL = new TStringList;
	try
	{
		// Write how many palette licences we have
		DataFile->WriteInteger("Palette", "Count", PaletteLicences.size());
		for (i = 0; i < PaletteLicences.size(); i++)
		{
			Licence = PaletteLicences.at(i);
			// Write name of licence
			Str = "Lic" + String(i);
			DataFile->WriteString(Str, "Name", Licence->Name);

			// Write CompatibleWith array
			SL->Clear();
			for (j = 0; j < Licence->CompatibleWith.size(); j++)
				SL->Add(Licence->CompatibleWith.at(j)->Name);
			DataFile->WriteString(Str, "CompatibleWith", SL->CommaText);

			// Write CompatibleWith array
			SL->Clear();
			for (j = 0; j < Licence->IncompatibleWith.size(); j++)
				SL->Add(Licence->IncompatibleWith.at(j)->Name);
			DataFile->WriteString(Str, "IncompatibleWith", SL->CommaText);

			// Look in sbContents to find if it is also there
			VisibleInGraph = false;
			for (j = 0; j < sbContent->Content->ControlsCount; j++)
				if (dynamic_cast<TSelection*>(sbContent->Content->Controls->Items[j]))
					if (sbContent->Content->Controls->Items[j]->Tag == i)
					{
						VisibleInGraph = true;
						Selection = reinterpret_cast<TSelection*>(sbContent->Content->Controls->Items[j]);

						DataFile->WriteBool(Str, "VisibleInGraph", true);
						DataFile->WriteFloat(Str, "GraphPosLeft", Selection->Position->X);
						DataFile->WriteFloat(Str, "GraphPosTop", Selection->Position->Y);
						DataFile->WriteFloat(Str, "GraphPosWidth", Selection->Width);
						DataFile->WriteFloat(Str, "GraphPosHeight", Selection->Height);

						break;
					}
			if (!VisibleInGraph)
				DataFile->WriteBool(Str, "VisibleInGraph", false);
		}

		DataFile->UpdateFile(); // Update file contents immediately
	}
	__finally
	{
		SL->Free();
	}
}

void TfrmMain::LoadFromFile()
{
	int i, j, k;
	String Str;
	TStringList *SL;
	TLicence *Licence;
	TSelection* NewNode;

	SL = new TStringList;
	try
	{
		// Read how many palette licences we have
		int PaletteCount = DataFile->ReadInteger("Palette", "Count", 0);

		// Populate LicencePalette array and assign names
		for (i = 0; i < PaletteCount; i++)
		{
			Licence = new TLicence;
			PaletteLicences.push_back(Licence);

			Str = "Lic" + String(i);
			// Read name of licence
			Licence->Name = DataFile->ReadString(Str, "Name", Licence->Name);
		}

		// Build CompatibleWith arrays
		for (i = 0; i < PaletteCount; i++)
		{
			Licence = PaletteLicences.at(i);
			Str = "Lic" + String(i);

			SL->CommaText = DataFile->ReadString(Str, "CompatibleWith", "");
			// Find the licence and add it to the CompatibleWith list
			for (j = 0; j < SL->Count; j++)
				for (k = 0; k < PaletteLicences.size(); k++)
					if (SameText(PaletteLicences.at(k)->Name, SL->Strings[j]))
					{
						Licence->CompatibleWith.push_back(PaletteLicences.at(k));
						break;
					}

			SL->CommaText = DataFile->ReadString(Str, "IncompatibleWith", "");
			// Find the licence and add it to the IncompatibleWith list
			for (j = 0; j < SL->Count; j++)
				for (k = 0; k < PaletteLicences.size(); k++)
					if (SameText(PaletteLicences.at(k)->Name, SL->Strings[j]))
					{
						Licence->IncompatibleWith.push_back(PaletteLicences.at(k));
						break;
					}
		}

		RepopulateLicencePalette();

		// Now populate the sbContent
		for (i = 0; i < PaletteCount; i++)
		{
			Licence = PaletteLicences.at(i);
			Str = "Lic" + String(i);
			if (DataFile->ReadBool(Str, "VisibleInGraph", false))
			{
				NodeTemplate->Tag = i;
				NodeDescription->Text = Licence->Name;

				// Make a clone of NodeTemplate and put it in sbContent
				NewNode = dynamic_cast<TSelection*>(NodeTemplate->Clone(NULL));
				NewNode->Position->Point = PointF(

					DataFile->ReadFloat(Str, "GraphPosLeft", 0),

					DataFile->ReadFloat(Str, "GraphPosTop", 0));
				NewNode->Width = DataFile->ReadFloat(Str, "GraphPosWidth", 0);
				NewNode->Height = DataFile->ReadFloat(Str, "GraphPosHeight", 0);

				NewNode->OnMouseEnter = NodeTemplateMouseEnter;
				NewNode->OnMouseLeave = NodeTemplateMouseLeave;
				NewNode->OnDblClick = NodeTemplateDblClick;
				NewNode->OnGesture = NodeTemplateGesture;
				NewNode->OnResize = NodeTemplateResize;
				NewNode->Visible = true;
				NewNode->Parent = sbContent;

			}
		}
	}
	__finally
	{
		SL->Free();
	}
}

// ---------------------------------------------------------------------------
// Licence Management

bool TfrmMain::AddNewPaletteLicence(TLicence *aLicence)
{
	// First check we don't have already a licence with the same name
	TLicence * Lic;
	for (int i = 0; i < PaletteLicences.size(); i++)
	{
		Lic = PaletteLicences.at(i);
		// SameText is NOT case sensitive (in English). However, to make it non-case
		// sensitive also in Greek we would need to create a new function, which
		// is beyond the scope of my implementation
		if (SameText(Lic->Name, aLicence->Name))
		{
			TDialogService::MessageDialog(

				"Cannot create a licence with the same name",

				TMsgDlgType::mtError,

				TMsgDlgButtons() << TMsgDlgBtn::mbOK,

				TMsgDlgBtn::mbOK,

				0,

				(Fmx::Dialogs::TInputCloseDialogEvent) NULL);

			return false;
		}
	}

	// Add licence to our vector
	PaletteLicences.push_back(aLicence);
	// And rebuild the UI
	RepopulateLicencePalette();

	return true;
}

void TfrmMain::RepopulateLicencePalette()
{
	// Dispose of any buttons in the sbLicencePalette scroll box
	while (sbLicencePalette->Content->ControlsCount > 0)
		sbLicencePalette->Content->Controls->Items[0]->DisposeOf();

	// Now explore PaletteLicences and for each element add a button
	// in sbLicencePalette scroll box
	TLicence *Lic;
	float LastButtonBottom = 0;
	for (int i = 0; i < PaletteLicences.size(); i++)
	{
		Lic = PaletteLicences.at(i);

		TButton *LicButton;
		LicButton = new TButton(NULL);

		// Set the button's Tag to point at the vector's item
		LicButton->Tag = i;

		// Set Text, Height, Align and Margins
		LicButton->Text = Lic->Name;
		LicButton->TextSettings->WordWrap = true;
		LicButton->Align = TAlignLayout::Top;
		LicButton->Height = 42;
		LicButton->Margins->Rect = Rect(2, 2, 2, 2);

		// Set AutoCapture to true. This means the button receives mouse events
		// if the button was pressed on it, even when the mouse leaves its client area
		LicButton->AutoCapture = true;

		// Assign its OnMouseDown, OnMouseMove, OnMouseUp event handlers
		LicButton->OnMouseDown = PaletteButtonOnMouseDown;
		LicButton->OnMouseMove = PaletteButtonOnMouseMove;
		LicButton->OnMouseUp = PaletteButtonOnMouseUp;

		// Make it visible in sbLicences
		LicButton->Parent = sbLicencePalette->Content;

		// Make it go under the last button added
		LicButton->Position->Y = LastButtonBottom;
		LastButtonBottom = LicButton->BoundsRect.Bottom + LicButton->Margins->Top + LicButton->Margins->Bottom;
	}

	// Call also a repaint on sbContent as we want to reflect the new relationships
	// if they have been edited
	sbContent->Repaint();
}

// ---------------------------------------------------------------------------
// Button actions

void __fastcall TfrmMain::btnAddNewLicenceClick(TObject *Sender)
{
	frmLicenceEditor->Visible = false;
	frmLicenceEditor->Tag = -1; // Tag of -1 means add, will be used by ok button

	// Populate fields to the values they should have
	frmLicenceEditor->edtLicenceName->Text = "";
#ifdef MSWINDOWS
	frmLicenceEditor->edtLicenceName->SetFocus();
#endif
	frmLicenceEditor->PopulateLicences();

	// Show the licence editor
#ifdef MSWINDOWS
	frmLicenceEditor->ShowModal();
#else
	frmLicenceEditor->Show();
#endif
}

void __fastcall TfrmMain::PaletteButtonOnMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, float X, float Y)
{
	frmLicenceEditor->Visible = false;
	TButton* SenderButton = reinterpret_cast<TButton*>(Sender);

	if (Shift.Contains(ssLeft))
	{
		PaletteButtonMouseIsDown = true;
		PaletteButtonMouseDownPos = PointF(X, Y);
		NodeDescription->Text = SenderButton->Text;
		NodeTemplate->Tag = SenderButton->Tag;
	}
}

void __fastcall TfrmMain::PaletteButtonOnMouseMove(TObject *Sender, TShiftState Shift, float X, float Y)
{
	if (PaletteButtonMouseIsDown)
	{
		TButton* SenderButton = reinterpret_cast<TButton*>(Sender);

		// If we have dragged the mouse more than 10 pixels distance since mouse down,
		// show the NodeTemplate
		if (!NodeTemplate->Visible)
			if (PaletteButtonMouseDownPos.Distance(PointF(X, Y)) > 10)
				NodeTemplate->Visible = true;

		// Make NodeTemplate follow the mouse. NodeTemplate's parent is the form so its
		// positioning is in absolute coordinates, so we calculate the absolute X, Y
		// from the sender button
		NodeTemplate->Position->Point =

			SenderButton->LocalToAbsolute(PointF(X, Y)) -

			PointF(NodeTemplate->Width / 2, NodeTemplate->Height / 2);
	}
}

void __fastcall TfrmMain::PaletteButtonOnMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, float X, float Y)
{
	PaletteButtonMouseIsDown = false;
	TButton* SenderButton = reinterpret_cast<TButton*>(Sender);

	if (NodeTemplate->Visible)
	{
		// We were dragging a node, create it in the sbContent
		NodeTemplate->Visible = false;

		TPointF AbsPt = SenderButton->LocalToAbsolute(PointF(X, Y));
		TPointF ContentLocalPt = sbContent->AbsoluteToLocal(AbsPt);
		TPointF PaletteLocalPt = sbLicencePalette->AbsoluteToLocal(AbsPt);

		// if user has released button within the sbLicencePalette, that means
		// he was scrolling the list with the touch interface, so do nothing
		if ((PaletteLocalPt.x >= 0) && (PaletteLocalPt.y >= 0) && (PaletteLocalPt.x < sbLicencePalette->Width) && (PaletteLocalPt.y < sbLicencePalette->Height))
			return;

		if ((ContentLocalPt.x >= 0) && (ContentLocalPt.y >= 0) && (ContentLocalPt.x < sbContent->Width) && (ContentLocalPt.y < sbContent->Height))
		{
			// First search to see if licence is not already added to sbContent
			for (int i = 0; i < sbContent->Content->ControlsCount; i++)
				if (dynamic_cast<TSelection*>(sbContent->Content->Controls->Items[i]))
					if (sbContent->Content->Controls->Items[i]->Tag == SenderButton->Tag)
					{
						TDialogService::MessageDialog(

							"This licence has already been added to the graph",

							TMsgDlgType::mtError,

							TMsgDlgButtons() << TMsgDlgBtn::mbOK,

							TMsgDlgBtn::mbOK,

							0,

							(Fmx::Dialogs::TInputCloseDialogEvent) NULL);

						return;
					}

			// Make a clone of NodeTemplate and put it in sbContent
			TSelection* NewNode;

			NewNode = dynamic_cast<TSelection*>(NodeTemplate->Clone(NULL));
			NewNode->Position->Point = ContentLocalPt - PointF(NewNode->Width / 2, NewNode->Height / 2) + sbContent->ViewportPosition;
			NewNode->OnMouseEnter = NodeTemplateMouseEnter;
			NewNode->OnMouseLeave = NodeTemplateMouseLeave;
			NewNode->OnDblClick = NodeTemplateDblClick;
			NewNode->OnGesture = NodeTemplateGesture;
			NewNode->OnResize = NodeTemplateResize;
			NewNode->Visible = true;
			NewNode->Parent = sbContent;
		}
	}
	else
	{
		// We pressed on the button, bring up the editor
		frmLicenceEditor->Tag = SenderButton->Tag;

		// Populate values in the LicenceEditor form
		frmLicenceEditor->edtLicenceName->Text = SenderButton->Text;
#ifdef MSWINDOWS
		frmLicenceEditor->edtLicenceName->SetFocus();
#endif
		frmLicenceEditor->PopulateLicences();

		// Show the licence editor
#ifdef MSWINDOWS
		frmLicenceEditor->ShowModal();
#else
		frmLicenceEditor->Show();
#endif
	}
}

// ---------------------------------------------------------------------------
// Visual management

void __fastcall TfrmMain::NodeTemplateMouseEnter(TObject * Sender)
{
	reinterpret_cast<TSelection*>(Sender)->HideSelection = false;
	sbContent->Touch->InteractiveGestures = sbContent->Touch->InteractiveGestures >> Fmx::Types::TInteractiveGesture::Pan;
}

void __fastcall TfrmMain::NodeTemplateMouseLeave(TObject * Sender)
{
	sbContent->Touch->InteractiveGestures = sbContent->Touch->InteractiveGestures << Fmx::Types::TInteractiveGesture::Pan;
	reinterpret_cast<TSelection*>(Sender)->HideSelection = true;
}

void __fastcall TfrmMain::NodeTemplateDblClick(TObject *Sender)
{
	frmNodeActions->Left = Screen->MousePos().X - frmNodeActions->Width / 2;
	frmNodeActions->Top = Screen->MousePos().Y - frmNodeActions->Height / 2;
	frmNodeActions->Tag = reinterpret_cast<TSelection*>(Sender)->Tag;
	frmNodeActions->Show();
}

void __fastcall TfrmMain::NodeTemplateGesture(TObject *Sender, const TGestureEventInfo &EventInfo, bool &Handled)
{
	if (EventInfo.GestureID == (Fmx::Types::TGestureID) igiLongTap)
		NodeTemplateDblClick(Sender);
}

void __fastcall TfrmMain::NodeTemplateResize(TObject *Sender)
{
	// We call a repaint on sbContent as resizing the node will affect the
	// relationship line positioning and they must be thus updated
	sbContent->Repaint();
}

void __fastcall TfrmMain::sbContentPainting(TObject * Sender, TCanvas * Canvas, const TRectF & ARect)
{
	// We use OnPainting as it is done before the children control are painted
	// That means that whatever we draw here is drawn underneath the nodes...
	// If we wanted to paint on top of the nodes, we would utilise the OnPaint
	// event handler

	// Search for all TSelection components on this form
	// and draw lines if CompatibleWith and IncompatibleWith lists are not empty
	int i, j, k;
	TLicence *Licence, *Licence2;
	TSelection *Selection, *Selection2;
	TPointF SelectionMid, Selection2Mid, PtA, PtB, PtMid;
	TPathData *Arrow;
	float SelectionAngle, LineAngle;

	Arrow = new TPathData;
	try
	{
		for (i = 0; i < sbContent->Content->ControlsCount; i++)
			if (dynamic_cast<TSelection*>(sbContent->Content->Controls->Items[i]))
			{
				Selection = reinterpret_cast<TSelection*>(sbContent->Content->Controls->Items[i]);
				Licence = PaletteLicences.at(Selection->Tag);

				// Now look at all other controls in sbContent and see if they are
				// in the Licence.CompatibleWith and Licence.IncpomatibleWith arrays,
				// in which cases we must draw a line
				for (j = 0; j < sbContent->Content->ControlsCount; j++)
					if (dynamic_cast<TSelection*>(sbContent->Content->Controls->Items[j]))
						if (j != i)
						{
							Selection2 = reinterpret_cast<TSelection*>(sbContent->Content->Controls->Items[j]);
							Licence2 = PaletteLicences.at(Selection2->Tag);

							// Enter GPU drawing mode
							Canvas->BeginScene();
							try
							{
								Canvas->Stroke->Kind = TBrushKind::Solid;
								Canvas->Stroke->Thickness = 3;
								Canvas->Stroke->Color = 0xFF000000; // Set black colour (ARGB)

								Canvas->Fill->Kind = TBrushKind::Solid;
								Canvas->Fill->Color = 0xFF000000; // Set black colour (ARGB)

								SelectionMid = Selection->Position->Point + PointF(Selection->Width / 2, Selection->Height / 2);
								Selection2Mid = Selection2->Position->Point + PointF(Selection2->Width / 2, Selection2->Height / 2);
								SelectionAngle = ArcTan2(Selection2Mid.y - SelectionMid.y, Selection2Mid.x - SelectionMid.x);
								if (SelectionAngle < 0)
									SelectionAngle += 2 * Pi;

								// Search compatible with
								for (k = 0; k < Licence->CompatibleWith.size(); k++)
									if (Licence->CompatibleWith.at(k) == Licence2)
									{
										// Licence2 is contained in Licence's CompatibleWith list
										// Draw a black line between the two
										// Draw the line

										// Depending on angle, arrange end points to be at the edge or the rectangle
										if ((SelectionAngle > Pi / 4) && (SelectionAngle <= 3 * Pi / 4))
										{
											// Coming from top

											PtA = PointF(

												SelectionMid.x - Selection->Width / 6,

												Selection->Position->Y + Selection->Height - Selection->Padding->Bottom

												);

											PtB = PointF(

												Selection2Mid.x - Selection2->Width / 6,

												Selection2->Position->Y + Selection2->Padding->Top

												);
										}
										else if ((SelectionAngle > 3 * Pi / 4) && (SelectionAngle <= 5 * Pi / 4))
										{
											// Coming from right

											PtA = PointF(

												Selection->Position->X + Selection->Padding->Left,

												SelectionMid.y - Selection->Height / 6

												);

											PtB = PointF(

												Selection2->Position->X + Selection2->Width - Selection2->Padding->Right,

												Selection2Mid.y - Selection2->Height / 6

												);
										}
										else if ((SelectionAngle > 5 * Pi / 4) && (SelectionAngle <= 7 * Pi / 4))
										{
											// Coming from bottom

											PtA = PointF(

												SelectionMid.x - Selection->Width / 6,

												Selection->Position->Y + Selection->Padding->Top

												);

											PtB = PointF(

												Selection2Mid.x - Selection2->Width / 6,

												Selection2->Position->Y + Selection2->Height - Selection2->Padding->Bottom

												);
										}
										else
										{
											// Coming from left

											PtA = PointF(

												Selection->Position->X + Selection->Width - Selection->Padding->Right,

												SelectionMid.y - Selection->Height / 6

												);

											PtB = PointF(

												Selection2->Position->X + Selection2->Padding->Left,

												Selection2Mid.y - Selection2->Height / 6

												);
										}

										PtA = PtA - sbContent->ViewportPosition;
										PtB = PtB - sbContent->ViewportPosition;
										LineAngle = ArcTan2(PtB.y - PtA.y, PtB.x - PtA.x);

										Canvas->DrawLine(

											PtA,

											PtB - PointF(20, 0).Rotate(LineAngle), // Make it stop before the arrow begins

											1);

										// Draw an arrow at the end of the line

										Arrow->Clear();
										Arrow->MoveTo(PtB + PointF(Canvas->Stroke->Thickness + 1, 0).Rotate(LineAngle));
										Arrow->LineTo(PtB - PointF(20, 0).Rotate(LineAngle - Pi / 10));
										Arrow->LineTo(PtB - PointF(20, 0).Rotate(LineAngle + Pi / 10));
										Arrow->ClosePath();

										Canvas->FillPath(Arrow, 0.75); // Draw arrow with a bit of transparency

										break;
									}

								Canvas->Stroke->Kind = TBrushKind::Solid;
								Canvas->Stroke->Thickness = 3;
								Canvas->Stroke->Color = 0xFFFF0000; // Set red colour (ARGB)

								Canvas->Fill->Kind = TBrushKind::Solid;
								Canvas->Fill->Color = 0xFFFF0000; // Set red colour (ARBG)

								// Search incompatible with
								for (k = 0; k < Licence->IncompatibleWith.size(); k++)
									if (Licence->IncompatibleWith.at(k) == Licence2)
									{
										// Licence2 is contained in Licence's IncompatibleWith list
										// Draw a red line between the two
										// Draw the line

										// Depending on angle, arrange end points to be at the edge or the rectangle
										if ((SelectionAngle > Pi / 4) && (SelectionAngle <= 3 * Pi / 4))
										{
											// Coming from top

											PtA = PointF(

												SelectionMid.x + Selection->Width / 6,

												Selection->Position->Y + Selection->Height - Selection->Padding->Bottom

												);

											PtB = PointF(

												Selection2Mid.x + Selection2->Width / 6,

												Selection2->Position->Y + Selection2->Padding->Top

												);
										}
										else if ((SelectionAngle > 3 * Pi / 4) && (SelectionAngle <= 5 * Pi / 4))
										{
											// Coming from right

											PtA = PointF(

												Selection->Position->X + Selection->Padding->Left,

												SelectionMid.y + Selection->Height / 6

												);

											PtB = PointF(

												Selection2->Position->X + Selection2->Width - Selection2->Padding->Right,

												Selection2Mid.y + Selection2->Height / 6

												);
										}
										else if ((SelectionAngle > 5 * Pi / 4) && (SelectionAngle <= 7 * Pi / 4))
										{
											// Coming from bottom

											PtA = PointF(

												SelectionMid.x + Selection->Width / 6,

												Selection->Position->Y + Selection->Padding->Top

												);

											PtB = PointF(

												Selection2Mid.x + Selection2->Width / 6,

												Selection2->Position->Y + Selection2->Height - Selection2->Padding->Bottom

												);
										}
										else
										{
											// Coming from left

											PtA = PointF(

												Selection->Position->X + Selection->Width - Selection->Padding->Right,

												SelectionMid.y + Selection->Height / 6

												);

											PtB = PointF(

												Selection2->Position->X + Selection2->Padding->Left,

												Selection2Mid.y + Selection2->Height / 6

												);
										}

										PtA = PtA - sbContent->ViewportPosition;
										PtB = PtB - sbContent->ViewportPosition;
										LineAngle = ArcTan2(PtB.y - PtA.y, PtB.x - PtA.x);

										Canvas->DrawLine(

											PtA,

											PtB - PointF(20, 0).Rotate(LineAngle), // Make it stop before the arrow begins

											1);

										// Draw an arrow at the end of the line, slightly off centre

										Arrow->Clear();
										Arrow->MoveTo(PtB + PointF(Canvas->Stroke->Thickness + 1, 0).Rotate(LineAngle));
										Arrow->LineTo(PtB - PointF(20, 0).Rotate(LineAngle - Pi / 10));
										Arrow->LineTo(PtB - PointF(20, 0).Rotate(LineAngle + Pi / 10));
										Arrow->ClosePath();

										Canvas->FillPath(Arrow, 0.75); // Draw arrow with a bit of transparency

										// Draw also an X, with a bit of transparency
										PtMid = (PtA + PtB) / 2;

										Arrow->Clear();
										Arrow->MoveTo(PtMid + PointF(10, 0).Rotate(LineAngle - Pi / 4));
										Arrow->LineTo(PtMid - PointF(10, 0).Rotate(LineAngle - Pi / 4));
										Arrow->MoveTo(PtMid + PointF(10, 0).Rotate(LineAngle + Pi / 4));
										Arrow->LineTo(PtMid - PointF(10, 0).Rotate(LineAngle + Pi / 4));
										Arrow->MoveTo(PtMid); // Android GPU canvas needs this so that last line gets drawn

										Canvas->DrawPath(Arrow, 0.75);

										break;
									}
							}
							__finally
							{
								Canvas->EndScene();
							}
						}
			}
	}
	__finally
	{
		delete Arrow;
	}
}

void __fastcall TfrmMain::btnFindCompatibilityClick(TObject *Sender)
{
	frmCheckCompatibility->PopulateLicences();
#ifdef MSWINDOWS
	frmCheckCompatibility->ShowModal();
#else
	frmCheckCompatibility->Show();
#endif
}
