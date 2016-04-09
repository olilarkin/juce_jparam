/* 

juce_jparam.cpp
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

#include "juce_jparam.h"

#define MAX_PARAM_DISPLAY_PRECISION 6

JParam::JParam()
: mType(kTypeNone)
, mValue(0.)
, mMin(0.)
, mMax(1.)
, mStep(1.)
, mShape(1.)
, mDefault(0.)
, mDisplayPrecision(0)
, mNegateDisplay(false)
, mCanAutomate(true)
, mIsReadOnly(false)
, mName(String::empty)
, mLabel(String::empty)
, mParamGroup(String::empty)
{
}

JParam::~JParam() {}

void JParam::setValue(float newValue)
{
  mValue = FromNormalizedParam(newValue, GetMin(), GetMax(), GetShape());
}

float JParam::getValueForText(const String& text) const
{
  return text.getFloatValue();
}

String JParam::getText(float value, int maximumStringLength) const
{
  return GetDisplayForHost(value, true);
}

#pragma mark Setters

void JParam::InitBool(const String& name, bool defaultVal, const String& group, bool readonly)
{
  if (mType == kTypeNone) mType = kTypeBool;
  
  InitEnum(name, (defaultVal ? 1 : 0), 2, group, readonly);
  
  SetDisplayText(0, "off");
  SetDisplayText(1, "on");
}

void JParam::InitEnum(const String& name, int defaultVal, int nEnums, const String& group, bool readonly)
{
  if (mType == kTypeNone) mType = kTypeEnum;
  
  InitInt(name, defaultVal, 0, nEnums - 1, "", group, readonly);
}

void JParam::InitInt(const String& name, int defaultVal, int minVal, int maxVal, const String& label, const String& group, bool readonly)
{
  if (mType == kTypeNone) mType = kTypeInt;
  
  InitFloat(name, (float) defaultVal, (float) minVal, (float) maxVal, 1.0, label, group, readonly, 1.);
}

void JParam::InitFloat(const String& name, float defaultVal, float minVal, float maxVal, float step, const String& label, const String& group, bool readonly, float shape, std::function<String (float)> displayTextFunction)
{
  if (mType == kTypeNone) mType = kTypeFloat;
  
  mName = name;
  mLabel = label;
  mParamGroup = group;
  mValue = defaultVal;
  mMin = minVal;
  mMax = jmax(maxVal, minVal + step);
  mStep = step;
  mDefault = defaultVal;
  mIsReadOnly = readonly;
  mDisplayTextFunction = displayTextFunction;
  
  if (mIsReadOnly)
    mCanAutomate = false;
  
  for (mDisplayPrecision = 0;
       mDisplayPrecision < MAX_PARAM_DISPLAY_PRECISION && step != floor(step);
       ++mDisplayPrecision, step *= 10.0)
  {
    ;
  }
  
  SetShape(shape);
}

void JParam::Set(float value)
{
  mValue = jlimit(mMin, mMax, value);
}

void JParam::SetDisplayText(int value, const String& text)
{
  mDisplayTexts.add(new DisplayText(value, text));
}

void JParam::SetCanAutomate(bool canAutomate)
{
  mCanAutomate = canAutomate;
}

void JParam::SetShape(float shape)
{
  if(shape != 0.0)
    mShape = shape;
}

void JParam::SetToDefault()
{
  mValue = mDefault;
}

void JParam::SetNegateDisplay()
{
  mNegateDisplay = true;
}

void JParam::SignDisplay()
{
  mSignDisplay = true;
}

void JParam::SetNormalized(float normalizedValue)
{
  mValue = FromNormalizedParam(normalizedValue, mMin, mMax, mShape);
  
  if (mType != kTypeFloat)
    mValue = floor(0.5 + mValue / mStep) * mStep;
  
  mValue = jmin(mValue, mMax);
}


#pragma mark Getters
JParam::EParamType JParam::GetType() const
{
  return mType;
}

float JParam::GetShape() const
{
  return mShape;
}

float JParam::GetStep() const
{
  return mStep;
}

float JParam::GetDefault() const
{
  return mDefault;
}

float JParam::GetDefaultNormalized() const
{
  return ToNormalizedParam(mDefault, mMin, mMax, mShape);
}

bool JParam::GetDisplayIsNegated() const
{
  return mNegateDisplay;
}

float JParam::GetValue() const
{
  return mValue;
}

bool JParam::GetBool() const
{
  return (mValue >= 0.5);
}

int JParam::GetInt() const
{
  return int(mValue);
}

const String JParam::GetDisplayForHost(float value, bool normalized, bool withDisplayText) const
{
  if (normalized)
    value = FromNormalizedParam(value, mMin, mMax, mShape);
  
  if (mDisplayTextFunction != nullptr)
    return mDisplayTextFunction(value);
  
  if (withDisplayText)
  {
    const String& str = GetDisplayText( (int) value);
    
    if (!str.isEmpty())
      return str;
  }
  
  float displayValue = value;
  
  if (mNegateDisplay)
    displayValue = -displayValue;
  
  if (mDisplayPrecision == 0)
  {
    return String(int(displayValue));
  }
  else if(mSignDisplay)
  {
    return String::formatted(String::formatted("%%+.%df", mDisplayPrecision), displayValue);
  }
  else
  {
    return String(displayValue, mDisplayPrecision);
  }
}

const String JParam::GetDisplayForHost(bool withDisplayText) const
{
  return GetDisplayForHost(mValue, false, withDisplayText);
}

float JParam::GetMin() const
{
  return mMin;
}

float JParam::GetMax() const
{
  return mMax;
}

float JParam::GetRange() const
{
  return mMax - mMin;
}

int JParam::GetPrecision() const
{
  return mDisplayPrecision;
}

bool JParam::GetCanAutomate() const
{
  return mCanAutomate;
}

bool JParam::GetIsReadOnly() const
{
  return mIsReadOnly;
}

float JParam::GetDBToAmp() const
{
  return Decibels::decibelsToGain(mValue);
}

float JParam::GetNormalized() const
{
  return GetNormalized(mValue);
}

float JParam::GetNormalized(float nonNormalizedValue) const
{
  nonNormalizedValue = jlimit(mMin, mMax, nonNormalizedValue);
  return ToNormalizedParam(nonNormalizedValue, mMin, mMax, mShape);
}

const String JParam::GetNameForHost() const
{
  return mName;
}

const String JParam::GetLabelForHost() const
{
  const String displayText = GetDisplayText((int) mValue);
  return (!displayText.isEmpty()) ? String::empty : mLabel;
}

const String JParam::GetParamGroupForHost() const
{
 return mParamGroup;
}

int JParam::GetNDisplayTexts() const
{
  return mDisplayTexts.size();
}

const String JParam::GetDisplayText(int value) const
{
  int n = mDisplayTexts.size();
  
  if (n)
  {
    for (int i = 0; i < n; i++)
    {
      if (value == mDisplayTexts.getUnchecked(i)->mValue)
      {
        return mDisplayTexts.getUnchecked(i)->mText;
      }
    }
  }
  return "";  
}

const String JParam::GetDisplayTextAtIdx(int idx, int* value) const
{
  DisplayText* pDT = mDisplayTexts[idx];
  
  if (value)
  {
    *value = pDT->mValue;
  }
  
  return pDT->mText;
}

bool JParam::MapDisplayText(String& str, int& pValue) const
{
  if (mDisplayTexts.size())
  {
    for (int i = 0; i < mDisplayTexts.size(); i++)
    {
      DisplayText* pDT = mDisplayTexts[i];
      
      if (str == pDT->mText)
      {
        pValue = pDT->mValue;
        return true;
      }
    }
  }
  
  return false;
}

void JParam::GetBounds(float& pMin, float& pMax) const
{
  pMin = mMin;
  pMax = mMax;
}
