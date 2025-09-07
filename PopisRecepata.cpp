
// *******************************************************************************************************************************************
// //
//
//                                                             XML Data Binding
//
//         Generated on: 01/09/2025 14:01:05
//       Generated from: C:\Users\MobiS PC\Desktop\SCHOOL\4.SEMESTAR\NAPREDNE
//       TEHNIKE PROGRAMIRANJA\&PROJEKT\MojReceptar\PopisRecepata.xml
//   Settings stored in: C:\Users\MobiS PC\Desktop\SCHOOL\4.SEMESTAR\NAPREDNE
//   TEHNIKE PROGRAMIRANJA\&PROJEKT\MojReceptar\PopisRecepata.xdb
//
// *******************************************************************************************************************************************
// //

#include <System.hpp>
#pragma hdrstop

#include "PopisRecepata.h"

// Global Functions

_di_IXMLrecipesType __fastcall Getrecipes(Xml::Xmlintf::_di_IXMLDocument Doc) {
  return (_di_IXMLrecipesType)Doc->GetDocBinding(
      "recipes", __classid(TXMLrecipesType), TargetNamespace);
};

_di_IXMLrecipesType __fastcall Getrecipes(Xml::Xmldoc::TXMLDocument* Doc) {
  Xml::Xmlintf::_di_IXMLDocument DocIntf;
  Doc->GetInterface(DocIntf);
  return Getrecipes(DocIntf);
};

_di_IXMLrecipesType __fastcall Loadrecipes(
    const System::UnicodeString& FileName) {
  return (_di_IXMLrecipesType)Xml::Xmldoc::LoadXMLDocument(FileName)
      ->GetDocBinding("recipes", __classid(TXMLrecipesType), TargetNamespace);
};

_di_IXMLrecipesType __fastcall Newrecipes() {
  return (_di_IXMLrecipesType)Xml::Xmldoc::NewXMLDocument()->GetDocBinding(
      "recipes", __classid(TXMLrecipesType), TargetNamespace);
};

// TXMLrecipesType

void __fastcall TXMLrecipesType::AfterConstruction(void) {
  RegisterChildNode(System::UnicodeString("recipe"), __classid(TXMLrecipeType));
  ItemTag = "recipe";
  ItemInterface = __uuidof(IXMLrecipeType);
  Xml::Xmldoc::TXMLNodeCollection::AfterConstruction();
};

_di_IXMLrecipeType __fastcall TXMLrecipesType::Get_recipe(const int Index) {
  return (_di_IXMLrecipeType)List->Nodes[Index];
};

_di_IXMLrecipeType __fastcall TXMLrecipesType::Add() {
  return (_di_IXMLrecipeType)AddItem(-1);
};

_di_IXMLrecipeType __fastcall TXMLrecipesType::Insert(const int Index) {
  return (_di_IXMLrecipeType)AddItem(Index);
};

// TXMLrecipeType

void __fastcall TXMLrecipeType::AfterConstruction(void) {
  RegisterChildNode(System::UnicodeString("ingredient"),
                    __classid(TXMLingredientType));
  ItemTag = "ingredient";
  ItemInterface = __uuidof(IXMLingredientType);
  Xml::Xmldoc::TXMLNodeCollection::AfterConstruction();
};

System::UnicodeString __fastcall TXMLrecipeType::Get_name() {
  return GetAttributeNodes()->Nodes[System::UnicodeString("name")]->Text;
};

void __fastcall TXMLrecipeType::Set_name(const System::UnicodeString Value) {
  SetAttribute(System::UnicodeString("name"), Value);
};

int __fastcall TXMLrecipeType::Get_difficulty() {
  return XmlStrToInt(
      GetAttributeNodes()->Nodes[System::UnicodeString("difficulty")]->Text);
};

void __fastcall TXMLrecipeType::Set_difficulty(const int Value) {
  SetAttribute(System::UnicodeString("difficulty"), Value);
};

_di_IXMLingredientType __fastcall TXMLrecipeType::Get_ingredient(
    const int Index) {
  return (_di_IXMLingredientType)List->Nodes[Index];
};

_di_IXMLingredientType __fastcall TXMLrecipeType::Add() {
  return (_di_IXMLingredientType)AddItem(-1);
};

_di_IXMLingredientType __fastcall TXMLrecipeType::Insert(const int Index) {
  return (_di_IXMLingredientType)AddItem(Index);
};

// TXMLingredientType

int __fastcall TXMLingredientType::Get_qty() {
  return XmlStrToInt(
      GetAttributeNodes()->Nodes[System::UnicodeString("qty")]->Text);
};

void __fastcall TXMLingredientType::Set_qty(const int Value) {
  SetAttribute(System::UnicodeString("qty"), Value);
};

