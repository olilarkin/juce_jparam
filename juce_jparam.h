/* 

juce_jparam.h
Enhanced JUCE AudioProcessorParameter version of Cockos IPlug's IParam parameter class
Oli Larkin 2016 
 
---------------------------------------------

Cockos WDL License

Copyright (C) 2005 and later Cockos Incorporated

Portions copyright other contributors, see each source file for more information

This software is provided 'as-is', without any express or implied warranty.  In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
1. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
1. This notice may not be removed or altered from any source distribution.

---------------------------------------------

*/

/*******************************************************************************

 BEGIN_JUCE_MODULE_DECLARATION

  ID:               juce_jparam
  vendor:           olilarkin
  version:          0.1
  name:             JParam
  description:      Enhanced JUCE AudioProcessorParameter version of Cockos IPlug's IParam parameter class
  website:          https://github.com/olilarkin/jparam
  license:          Cockos WDL License

  dependencies:     juce_core, juce_audio_processors

 END_JUCE_MODULE_DECLARATION

*******************************************************************************/

#ifndef __JUCE_JPARAM_H__
#define __JUCE_JPARAM_H__

#include "JuceHeader.h"

using namespace juce;

class JParam : public AudioProcessorParameter
{
public:
  JParam();
  ~JParam();
  
  enum EParamType { kTypeNone, kTypeBool, kTypeInt, kTypeEnum, kTypeFloat };
  
  static inline float ToNormalizedParam(float nonNormalizedValue, float min, float max, float shape)
  {
    return powf((nonNormalizedValue - min) / (max - min), 1.0 / shape);
  }
  
  static inline float FromNormalizedParam(float normalizedValue, float min, float max, float shape)
  {
    return min + powf((float) normalizedValue, shape) * (max - min);
  }
  
  //AudioProcessorParameter
  //==============================================================================
  float getValue() const override { return GetNormalized(); }
  void setValue(float newValue) override;
  float getDefaultValue() const override { return GetDefaultNormalized(); }
  String getName(int maximumStringLength) const override { return GetNameForHost(); }
  String getLabel() const override { return GetLabelForHost(); }
  float getValueForText(const String& text) const override;
  int getNumSteps() const override { return int ((GetMin()-GetMax()) / GetStep() ); }
  String getText (float value, int maximumStringLength) const override;
//TODO: bool isOrientationInverted() const override; { return false; } 
  bool isAutomatable() const override { return GetIsReadOnly(); }
//TODO: bool isMetaParameter() const override { return false; } 
  
  //JParam
  //==============================================================================
  void InitBool(const String& name, bool defaultVal, const String& group = "", bool readonly = false);
  void InitEnum(const String& name, int defaultVal, int nEnums, const String& group = "", bool readonly = false);
  void InitInt(const String& name, int defaultVal, int minVal, int maxVal, const String& label = "", const String& group = "", bool readonly = false);
  void InitFloat(const String& name, float defaultVal, float minVal, float maxVal, float step, const String& label = "", const String& group = "", bool readonly = false, float shape = 1., std::function<String (float)> displayTextFunction = nullptr);
  
  void Set(float value);
  void SetDisplayText(int value, const String& text);
  void SetCanAutomate(bool canAutomate);
  void SetShape(float shape);
  void SetToDefault();
  void SetNegateDisplay();
  void SignDisplay();
  void SetNormalized(float normalizedValue);
  
  EParamType GetType() const;
  bool GetDisplayIsNegated() const;
  float GetValue() const;
  bool GetBool() const;
  int GetInt() const;
  float GetDBToAmp() const;
  
  /** Gets the normalized equivalent of mValue */
  float GetNormalized() const;
  
  /** Gets the normalized equivalent of nonNormalizedValue */
  float GetNormalized(float nonNormalizedValue) const;
  
  /** Gets the readout for mValue */
  const String GetDisplayForHost(bool withDisplayText = true) const;
  
  /** Gets the readout for value */
  const String GetDisplayForHost(float value, bool normalized, bool withDisplayText = true) const;
  
  const String GetNameForHost() const;
  const String GetLabelForHost() const;
  const String GetParamGroupForHost() const;
  
  int GetNDisplayTexts() const;
  const String GetDisplayText(int value) const;
  const String GetDisplayTextAtIdx(int idx, int* value = nullptr) const;
  bool MapDisplayText(String& str, int& pValue) const;

  void GetBounds(float& pMin, float& pMax) const;
  float GetShape() const;
  float GetStep() const;
  float GetDefault() const;
  float GetDefaultNormalized() const;
  float GetMin() const;
  float GetMax() const;
  float GetRange() const;
  int GetPrecision() const;
  bool GetCanAutomate() const; //TODO: do we need this as well as ReadOnly? see VST3
  bool GetIsReadOnly() const;
  
private:
  EParamType mType;
  float mValue; // non normalized value
  float mMin;
  float mMax;
  float mStep;
  float mShape;
  float mDefault;
  int mDisplayPrecision;
  bool mNegateDisplay;
  bool mSignDisplay;
  bool mCanAutomate;
  bool mIsReadOnly;
  
  std::function<String (float)> mDisplayTextFunction;
  
  String mName;
  String mLabel;
  String mParamGroup;
  
  struct DisplayText
  {
    int mValue;
    const String mText;
    
    DisplayText(int value, const String& text)
    : mValue(value)
    , mText(text)
    {
    }
  };
  
  OwnedArray<DisplayText> mDisplayTexts;
};

#endif //__JUCE_JPARAM_H__