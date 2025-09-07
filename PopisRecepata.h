
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

#ifndef PopisRecepataH
#define PopisRecepataH

#include <XMLNodeImp.h>

#include <System.SysUtils.hpp>
#include <System.Variants.hpp>
#include <System.hpp>
#include <Xml.XMLDoc.hpp>
#include <Xml.XMLIntf.hpp>
#include <Xml.Xmldom.hpp>
#include <Xml.xmlutil.hpp>

// Forward Decls

__interface IXMLrecipesType;
typedef System::DelphiInterface<IXMLrecipesType> _di_IXMLrecipesType;
__interface IXMLrecipeType;
typedef System::DelphiInterface<IXMLrecipeType> _di_IXMLrecipeType;
__interface IXMLingredientType;
typedef System::DelphiInterface<IXMLingredientType> _di_IXMLingredientType;

// IXMLrecipesType

__interface INTERFACE_UUID("{025312D7-F231-4963-B05B-9597872310F7}")
    IXMLrecipesType : public Xml::Xmlintf::IXMLNodeCollection {
 public:
 public:
  // Property Accessors
  virtual _di_IXMLrecipeType __fastcall Get_recipe(const int Index) = 0;
  // Methods & Properties
  virtual _di_IXMLrecipeType __fastcall Add() = 0;
  virtual _di_IXMLrecipeType __fastcall Insert(const int Index) = 0;
  __property _di_IXMLrecipeType recipe[const int Index] = {
      read = Get_recipe}; /* default */
};

// IXMLrecipeType

__interface INTERFACE_UUID("{F9E63CFA-7539-40E0-A81F-31FA35588005}")
    IXMLrecipeType : public Xml::Xmlintf::IXMLNodeCollection {
 public:
 public:
  // Property Accessors
  virtual System::UnicodeString __fastcall Get_name() = 0;
  virtual int __fastcall Get_difficulty() = 0;
  virtual _di_IXMLingredientType __fastcall Get_ingredient(const int Index) = 0;
  virtual void __fastcall Set_name(const System::UnicodeString Value) = 0;
  virtual void __fastcall Set_difficulty(const int Value) = 0;
  // Methods & Properties
  virtual _di_IXMLingredientType __fastcall Add() = 0;
  virtual _di_IXMLingredientType __fastcall Insert(const int Index) = 0;
  __property System::UnicodeString name = {read = Get_name, write = Set_name};
  __property int difficulty = {read = Get_difficulty, write = Set_difficulty};
  __property _di_IXMLingredientType ingredient[const int Index] = {
      read = Get_ingredient}; /* default */
};

// IXMLingredientType

__interface INTERFACE_UUID("{5B07E452-E355-40DB-81AD-8DB1ABFADD6C}")
    IXMLingredientType : public Xml::Xmlintf::IXMLNode {
 public:
  // Property Accessors
  virtual int __fastcall Get_qty() = 0;
  virtual void __fastcall Set_qty(const int Value) = 0;
  // Methods & Properties
  __property int qty = {read = Get_qty, write = Set_qty};
};

// Forward Decls

class TXMLrecipesType;
class TXMLrecipeType;
class TXMLingredientType;

// TXMLrecipesType

class TXMLrecipesType : public Xml::Xmldoc::TXMLNodeCollection,
                        public IXMLrecipesType {
  __IXMLNODECOLLECTION_IMPL__
 protected:
  // IXMLrecipesType
  virtual _di_IXMLrecipeType __fastcall Get_recipe(const int Index);
  virtual _di_IXMLrecipeType __fastcall Add();
  virtual _di_IXMLrecipeType __fastcall Insert(const int Index);

 public:
  virtual void __fastcall AfterConstruction(void);
};

// TXMLrecipeType

class TXMLrecipeType : public Xml::Xmldoc::TXMLNodeCollection,
                       public IXMLrecipeType {
  __IXMLNODECOLLECTION_IMPL__
 protected:
  // IXMLrecipeType
  virtual System::UnicodeString __fastcall Get_name();
  virtual int __fastcall Get_difficulty();
  virtual _di_IXMLingredientType __fastcall Get_ingredient(const int Index);
  virtual void __fastcall Set_name(const System::UnicodeString Value);
  virtual void __fastcall Set_difficulty(const int Value);
  virtual _di_IXMLingredientType __fastcall Add();
  virtual _di_IXMLingredientType __fastcall Insert(const int Index);

 public:
  virtual void __fastcall AfterConstruction(void);
};

// TXMLingredientType

class TXMLingredientType : public Xml::Xmldoc::TXMLNode,
                           public IXMLingredientType {
  __IXMLNODE_IMPL__
 protected:
  // IXMLingredientType
  virtual int __fastcall Get_qty();
  virtual void __fastcall Set_qty(const int Value);
};

// Global Functions

_di_IXMLrecipesType __fastcall Getrecipes(Xml::Xmlintf::_di_IXMLDocument Doc);
_di_IXMLrecipesType __fastcall Getrecipes(Xml::Xmldoc::TXMLDocument* Doc);
_di_IXMLrecipesType __fastcall Loadrecipes(
    const System::UnicodeString& FileName);
_di_IXMLrecipesType __fastcall Newrecipes();

#define TargetNamespace ""

#endif
