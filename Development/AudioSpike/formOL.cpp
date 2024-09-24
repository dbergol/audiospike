//---------------------------------------------------------------------------

#include <vcl.h>
#include <clipbrd.hpp>
#pragma hdrstop
#include <typeinfo>

#include "formOL.h"

#include "SoundDllProLic.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmOL *frmOL;

//---------------------------------------------------------------------------
__fastcall TfrmOL::TfrmOL(TComponent* Owner)
   : TForm(Owner), m_pIni(NULL)
{
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
///
//---------------------------------------------------------------------------
__fastcall TfrmOL::~TfrmOL()
{
   TRYDELETENULL(m_pIni);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
///
//---------------------------------------------------------------------------
void TfrmOL::UpdateGUI(bool bEnable)
{
   if (!m_pIni)
      return;

   btnActivate->Enabled    =  bEnable
                           && edtProductKey->Tag == HT_PK_OK;
   btnActivate->Tag        = m_pIni->ReadInteger("License", "Activated", 0);
   btnActivate->Caption    = btnActivate->Tag ? "Deactivate" : "Activate";

   edtProductKey->Enabled  = bEnable;
   edtProductKey->ReadOnly = btnActivate->Tag;

   btnOK->Enabled          = bEnable;
   Application->ProcessMessages();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
///
//---------------------------------------------------------------------------
void TfrmOL::EditOnlineL(UnicodeString usIniFile)
{
   TRYDELETENULL(m_pIni);
   m_pIni = new TIniFile(usIniFile);
   edtProductKey->Text     = m_pIni->ReadString("License", "ProductKey", "");
   edtHostId->Text         = m_pIni->ReadString("License", "LicServerHostId", "");
   UpdateGUI(true);
   ShowModal();

}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
///
//---------------------------------------------------------------------------
#pragma argsused
void __fastcall TfrmOL::btnActivateClick(TObject *Sender)
{
   // store in form's tag, that online licnse was changed somehow
   Tag = 1;
   // NOTE currently button's Tag 0 means: not activated, do activate it
   // and vice versa!
   UpdateGUI(false);
   try
      {
      m_pIni->WriteString("License", "ProductKey", edtProductKey->Text);
      SoundMexProL smpl;
      UnicodeString usInfo;
      bool b = smpl.ActivateOnlineL(m_pIni->FileName, usInfo, btnActivate->Tag == 0);
      if (!b)
         {
         ShowError("Error activating/deactivating license", usInfo);
         }
      else
         {
         edtHostId->Text = usInfo;
         m_pIni->WriteBool("License", "Activated", !btnActivate->Tag);
         m_pIni->WriteString("License", "LicServerHostId", edtHostId->Text);
         }
      }
   __finally
      {
      UpdateGUI(true);
      }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
///
//---------------------------------------------------------------------------
void TfrmOL::ShowError(UnicodeString usAction, UnicodeString usError)
{
   usAction = usAction + ": " + usError;
   MessageBoxW(0, usAction.w_str(), L"Error", MB_ICONERROR);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
///
//---------------------------------------------------------------------------
#pragma argsused
void __fastcall TfrmOL::edtProductKeyChange(TObject *Sender)
{
   UnicodeString us;
   edtProductKey->Tag = THtLicProductKey::GetProductKeyStatus(edtProductKey->Text, us);
   edtProductKey->Font->Color = edtProductKey->Tag == HT_PK_OK ? clBlack : clRed;
   UpdateGUI(true);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
///
//---------------------------------------------------------------------------
#pragma argsused
void __fastcall TfrmOL::btnClipboardClick(TObject *Sender)
{
   TStringList* psl = new TStringList();
   try
      {
      psl->Add("ProductKey: " + edtProductKey->Text);
      psl->Add("LicServerHostId: " + edtHostId->Text);
      Clipboard()->AsText = psl->Text;
      }
   __finally
      {
      TRYDELETENULL(psl);
      }
}
//---------------------------------------------------------------------------

